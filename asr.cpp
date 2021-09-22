#include <random>
#include <chrono>
#include <openmha/mha_plugin.hh>
#include "asr_calibrate_simple.h"
#include "asr_process_simple.h"
#include "asr_terminate.h"
// Function Declarations
static coder::array<double, 2U> argInit_1xUnbounded_real_T(unsigned, std::optional<double>);
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T(unsigned, unsigned, std::optional<double>);
static void argInit_asr_state_t(asr_state_t *result);
static boolean_T argInit_boolean_T();
static double argInit_real_T();
// Function Definitions
//
// Arguments    : void
// Return Type  : coder::array<double, 2U>
//
static coder::array<double, 2U> argInit_1xUnbounded_real_T(unsigned M=10,std::optional<double> val=std::nullopt)
{
  coder::array<double, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, M);

  // Loop over the array to initialize each element.
  for (int idx0 = 0; idx0 < 1; idx0++) {
    for (int idx1 = 0; idx1 < result.size(1); idx1++) {
        if(val)
            result[idx1] = *val;
        else
            result[idx1] = argInit_real_T();
    }
  }
  return result;
}

//
// Arguments    : void
// Return Type  : coder::array<double, 2U>
//
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T(unsigned N=20, unsigned M=20000, std::optional<double> val=std::nullopt)
{
  coder::array<double, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(N,M);

  // Loop over the array to initialize each element.
  for (int idx0 = 0; idx0 < result.size(0); idx0++) {
    for (int idx1 = 0; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      if(val)
        result[idx0 + result.size(0) * idx1] = *val;
      else
        result[idx0 + result.size(0) * idx1] = argInit_real_T();
    }
  }

  return result;
}
//
// Arguments    : asr_state_t *result
// Return Type  : void
//
static void argInit_asr_state_t(asr_state_t *result)
{
  coder::array<double, 2U> b_result_tmp;
  coder::array<double, 2U> result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result->M = argInit_UnboundedxUnbounded_real_T(20,20,0);
  result->T = argInit_UnboundedxUnbounded_real_T(20,20,0);
  result->B=argInit_1xUnbounded_real_T(9,0);
  result->A=argInit_1xUnbounded_real_T(9,0);
  result->cov = argInit_1xUnbounded_real_T(1,0);
  result->carry = argInit_1xUnbounded_real_T(1,0);
  result->iir = argInit_1xUnbounded_real_T(8,20);
  result->last_R =  argInit_1xUnbounded_real_T(1,0);
  result->last_trivial = argInit_boolean_T();
}

//
// Arguments    : void
// Return Type  : boolean_T
//
static boolean_T argInit_boolean_T()
{
  return false;
}

//
// Arguments    : void
// Return Type  : double
//
static std::random_device rd{};
static std::mt19937 gen{rd()};
static std::normal_distribution<> d{0,1};
static double argInit_real_T()
{
    return d(gen);
}

class asr_t : public MHAPlugin::plugin_t<int> {
public:
 asr_t(algo_comm_t iac, const std::string & configured_name)
      : MHAPlugin::plugin_t<int>("",iac)
  {(void)configured_name;/* ignore 2nd parameter */}

  /** Release may be empty */
  void release(void)
    {asr_terminate();
        std::cout<<nproc<<"\n";
    }



 void prepare(mhaconfig_t & )
  {
      //TODO: get calibration data and srate from somewhere
      // Initialize function 'asr_calibrate_simple' input arguments.
      // Initialize function input argument 'X'.
      auto X = argInit_UnboundedxUnbounded_real_T(20,50000);
      auto M = argInit_UnboundedxUnbounded_real_T(20,20,0);
      auto T = argInit_UnboundedxUnbounded_real_T(20,20,0);
      double B[9];
      double A[9];


      argInit_asr_state_t(&instate);
      auto iirstate = argInit_UnboundedxUnbounded_real_T(8,20,0);
      asr_calibrate_simple(X, 100, M,T,B,A,instate.iirstate);
      // Copy calibration to initial instate
      instate.M=M;
      instate.T=T;
      for(int i=0;i<9;i++){
          instate.A[i]=A[i];
          instate.B[i]=B[i];
      }

      // Prepare fake indata
      for(int i=0;i<2000;i++)
          indata[i]=argInit_UnboundedxUnbounded_real_T(20,50);

      // Initialize outdata to zero
      outdata=argInit_UnboundedxUnbounded_real_T(20,50,0);
      for(int i=0;i<2000;i++)
          indata[i]=argInit_UnboundedxUnbounded_real_T(20,50);
      argInit_asr_state_t(&outstate);

  }


    mha_wave_t * process(mha_wave_t * signal)
    {
        auto tic = std::chrono::system_clock::now().time_since_epoch();
        //TODO get real data from somewhere, preferably LSL, maybe via lsl2ac plugin
        asr_process_simple(indata[k++ % 2000], 100, &instate, outdata, &outstate);
        auto toc = std::chrono::system_clock::now().time_since_epoch();
        dt+=std::chrono::duration_cast<std::chrono::microseconds>(toc).count()-std::chrono::duration_cast<std::chrono::microseconds>(tic).count();
        instate=outstate;
        nproc++;
        if(nproc % 100 == 0){
            std::cerr<<nproc<<" "<<dt/100<<"us"<<"\n";
            dt=0;
        }
        return signal;
    }
    
private:
    asr_state_t instate;
    asr_state_t outstate;
    coder::array<double, 2U> indata[2000];
    coder::array<double, 2U> outdata;
    unsigned long k=0;
    std::size_t nproc=0;
    unsigned dt;

};

MHAPLUGIN_CALLBACKS(asr,asr_t,wave,wave)

MHAPLUGIN_DOCUMENTATION\
(asr,"","")

// Local Variables:
// compile-command: "make"
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// End:
