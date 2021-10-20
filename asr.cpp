#include "asr.hh"
#include <thread>
#include <chrono>
#include <openmha/mha_algo_comm.h>
#include <openmha/mha_plugin.hh>
#include <openmha/mha_fifo.h>
class asr_cfg_t {
public:
    asr_cfg_t(algo_comm_t ac,
              const std::string& ac_name,
              const std::string& calib_file_name,
              float WindowLength,
              float SamplingRate);
    virtual ~asr_cfg_t();
    void process();
    void exit_request();
private:
    algo_comm_t ac;
    std::string ac_name;

    /// Number of channels in the EEG signal
    std::size_t NumChannels;
    /// Window length used in the asr algorithm. Right now, it's fixed in asr_process and this must be the same
    float WindowLength;
    /// Sampling rate of the EEG signal
    float SamplingRate;

    /// Exit request flag. The asr thread loops until this is set to true
    std::atomic<bool> close_session;
    //asr_state_t state;
    /// Input state of the ASR algorithm. Must be initialized by a call to asr_process_simple
    asr_state_t instate;
    /// Output state of asr_process_simple. Not needed because the instate is changed in-place.
    asr_state_t outstate;
    /// Input data for asr_process_simple
    coder::array<double, 2U> indata;
    /// Output of asr_process_simple
    coder::array<double, 2U> outdata;
    /// Main thread of the asr algorithm.
    std::thread asr_thread;
    /// Main asr loop. Waits for enough samples in the fifo, copies the input signal into its buffers and then calls asr_process_simple
    void asr();
    /// Lockfree FIFO to copy AC variable content to ASR thread safely.
    std::unique_ptr<mha_fifo_lf_t<float>> fifo_in;
    /// Lockfree FIFO for ASR output.
    std::unique_ptr<mha_fifo_lf_t<float>> fifo_out;
    /// Temporary buffer to store FIFO elements of one asr call. Needed because the internal buffer of outdata is not available
    std::unique_ptr<float[]> buf;
    /// Temporary buffer to store FIFO elements of the output. Needed because the internal buffer outdata is not available
    std::unique_ptr<float[]> buf_out;
    /// AC variable for asr output
    std::unique_ptr<MHA_AC::waveform_t> wave_out;
};

asr_cfg_t::asr_cfg_t(algo_comm_t ac, const std::string& ac_name, const std::string& calib_file_name, float WindowLength, float SamplingRate):
    ac(ac),
    ac_name(ac_name),
    WindowLength(WindowLength),
    SamplingRate(SamplingRate){


    auto X=(to_array(rapidcsv::Document(calib_file_name.c_str(),rapidcsv::LabelParams(-1,-1))));
    // Fix number of channels number of channels in calibration data
    NumChannels=X.size(0);

    auto M = argInit_UnboundedxUnbounded_real_T(NumChannels,NumChannels,0);
    auto T = argInit_UnboundedxUnbounded_real_T(NumChannels,NumChannels,0);
    double B[9];
    double A[9];
    argInit_asr_state_t(&instate);
    auto iirstate = argInit_UnboundedxUnbounded_real_T(8,NumChannels,0);

    asr_calibrate_simple(X,SamplingRate, M,T,B,A,instate.iir);

    // Copy calibration to initial instate
    instate.M=M;
    instate.T=T;
    for(int i=0;i<9;i++){
        instate.A[i]=A[i];
        instate.B[i]=B[i];
    }

    // Initialize input data array
    indata=argInit_UnboundedxUnbounded_real_T(X.size(0),SamplingRate*WindowLength,0);

    // Initialize outdata to zero
    outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),SamplingRate*WindowLength,0);
    argInit_asr_state_t(&outstate);


    // Initialize fifo and buffers
    fifo_in=std::make_unique<mha_fifo_lf_t<float>>(SamplingRate*WindowLength*NumChannels*20000);
    fifo_out=std::make_unique<mha_fifo_lf_t<float>>(SamplingRate*WindowLength*NumChannels*20000);
    buf=std::make_unique<float[]>(SamplingRate*WindowLength*NumChannels);
    buf_out=std::make_unique<float[]>(SamplingRate*WindowLength*NumChannels);
    wave_out=std::make_unique<MHA_AC::waveform_t>(MHA_AC::waveform_t(ac,"asr_out",SamplingRate*WindowLength,NumChannels,false));
    // Start asr thread
    asr_thread=std::thread(&asr_cfg_t::asr, this);
}

asr_cfg_t::~asr_cfg_t(){
    exit_request();
}

void asr_cfg_t::asr(){

    // Loop until told to exit
    while(!close_session){

        // Wait until enough samples are available
        if(fifo_in->get_fill_count()<NumChannels*SamplingRate*WindowLength){
            // Rate limit busy loop
            mha_msleep(1);
            continue;
        }

        // Reset input and output arrays to correct size
        indata.set_size(NumChannels,SamplingRate*WindowLength);
        outdata.set_size(NumChannels,SamplingRate*WindowLength);

        // Read data from fifo, copy into array
        fifo_in->read(buf.get(),indata.numel());
        for(int idx=0;idx<indata.numel();++idx)
            indata[idx]=buf[idx];

        // Call asr
        asr_process_simple(indata, SamplingRate, &instate, outdata, &outstate);

        // Send output to audio thread
        for(int idx=0;idx<outdata.numel();++idx){
            outdata[idx]=buf_out[idx];
        }
        fifo_out->write(buf_out.get(),outdata.numel());
    }
}

void asr_cfg_t::exit_request(){

    // Signal asr thread to stop, wait for it to complete last loop
    if(!close_session.load()){
        close_session.store(true);
        asr_thread.join();
    }
    return;
}

void asr_cfg_t::process(){

    // Get input signal, check dimensions
    mha_wave_t signal_in=MHA_AC::get_var_waveform(ac,ac_name.c_str());
    if(NumChannels!=signal_in.num_channels)
        throw MHA_Error(__FILE__,__LINE__,
                        "Number of channels in EEG signal does not fit calibration."
                        " Expected: %zu. Actual %u.",
                        NumChannels,signal_in.num_channels);
    // Send input data to asr thread
    fifo_in->write(signal_in.buf,signal_in.num_channels*signal_in.num_frames);

    // Skip copying output data when none is available, but publish AC variable anyway (with old data)
    // because the framework expects us to
    if(fifo_out->get_fill_count()<wave_out->num_frames*wave_out->num_channels){
        wave_out->insert()
        return;
    }
    // Copy output data to AC variable and publish AC variable
    fifo_out->read(wave_out->buf,wave_out->num_frames*wave_out->num_channels);
    wave_out->insert();
}

class asr_t : public MHAPlugin::plugin_t<asr_cfg_t> {
public:
    asr_t(algo_comm_t iac, const std::string & configured_name)
        : MHAPlugin::plugin_t<asr_cfg_t>("",iac)
    {(void)configured_name;/* ignore 2nd parameter */
        insert_member(SamplingRate);
        insert_member(WindowLength);
        insert_member(VarName);
        insert_member(CalibrationFileName);
    }

    /** Release may be empty */
    void release(void)
    {
        // If there's a config, send an exit request and terminate
        auto latest_cfg=peek_config();
        if(latest_cfg)
            latest_cfg->exit_request();
        asr_terminate();
    }

    void prepare(mhaconfig_t & )
    {
        push_config(new asr_cfg_t(ac,VarName.data,CalibrationFileName.data,WindowLength.data,SamplingRate.data));
    }

    mha_wave_t * process(mha_wave_t * signal)
    {
        poll_config()->process();
        return signal;
    }

private:
    /// Sampling rate of incoming EEG signal
    MHAParser::float_t SamplingRate{"Nominal sampling rate of input data in Hz.","100"};
    /// Window length of asr implementation must be the same as the currently fixed window length in asr_process_simple
    MHAParser::float_t WindowLength{"Length of the statistcs window, in seconds (e.g., 0.5). This should not be much"
        " longer than the time scale over which artifacts persist, but the number of samples"
        " in the window should not be smaller than 1.5x the number of channels.","0.5"};
    /// Name of the AC variable containing the input data
    MHAParser::string_t VarName{"Name of input AC variable.",""};
    /// Filename of the calibration csv
    MHAParser::string_t CalibrationFileName{"Filename of calibration file, ",""};
    MHAEvents::patchbay_t<asr_t> patchbay;
};

MHAPLUGIN_CALLBACKS(asr,asr_t,wave,wave)

MHAPLUGIN_DOCUMENTATION\
(asr,"","Experimental implementation of artifact subspace reconstruction. The asr code is generated by the matlab coder from"
 " a simplified version of the asr reference implementation. The ASR state needs to be calibrated, the calibration file must be a csv file containing"
 " the calibration data, each row representing a channel. The actual EEG data is then received via AC variable. The number of channels must stay the"
 " same during processing, otherwise the processing will be aborted. The input AC variable is expected to always contain the newest samples or now samples,"
 " stale data will cause frames to be double counted. The output of the asr is available in the AC variable named asr_out"
 ")

// Local Variables:
// compile-command: "make"
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// End:
