#include "mha_plugin.hh"
#include "asr_calibrate_simple.h"
#include "asr_process_simple.h"
#include "asr_terminate.h"
// Function Declarations
static coder::array<double, 2U> argInit_1xUnbounded_real_T();
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T();
static void argInit_asr_state_t(asr_state_t *result);
static boolean_T argInit_boolean_T();
static double argInit_real_T();
// Function Definitions
//
// Arguments    : void
// Return Type  : coder::array<double, 2U>
//
static coder::array<double, 2U> argInit_1xUnbounded_real_T()
{
  coder::array<double, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int idx0 = 0; idx0 < 1; idx0++) {
    for (int idx1 = 0; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_real_T();
    }
  }

  return result;
}

//
// Arguments    : void
// Return Type  : coder::array<double, 2U>
//
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T()
{
  coder::array<double, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(20, 2000);

  // Loop over the array to initialize each element.
  for (int idx0 = 0; idx0 < result.size(0); idx0++) {
    for (int idx1 = 0; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx0 + result.size(0) * idx1] = (float)rand()/(float)RAND_MAX;
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
  result_tmp = argInit_UnboundedxUnbounded_real_T();
  result->M = result_tmp;
  result->T = result_tmp;
  b_result_tmp = argInit_1xUnbounded_real_T();
  result->B = b_result_tmp;
  result->A = b_result_tmp;
  result->cov = result_tmp;
  result->carry = result_tmp;
  result->iir = result_tmp;
  result->last_R = result_tmp;
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
static double argInit_real_T()
{
  return 0.0;
}



class asr_t : public MHAPlugin::plugin_t<int> {
public:
 asr_t(algo_comm_t iac, const std::string & configured_name)
      : MHAPlugin::plugin_t<int>("",iac)
  {(void)configured_name;/* ignore 2nd parameter */}

  /** Release may be empty */
  void release(void)
    {asr_terminate();}



 void prepare(mhaconfig_t & )
  {
      //coder::array<double, 2U> X;
      //TODO: get calibration data and srate from somewhere
      
      // Initialize function 'asr_calibrate_simple' input arguments.
      // Initialize function input argument 'X'.
      auto X = argInit_UnboundedxUnbounded_real_T();
      auto M = argInit_UnboundedxUnbounded_real_T();
      auto T = argInit_UnboundedxUnbounded_real_T();
      double B[9];
      double A[9];
      auto iirstate = argInit_UnboundedxUnbounded_real_T();

      // Call the entry-point 'asr_calibrate_simple'.
      asr_calibrate_simple(X, 100, M,T,B,A,iirstate);
      indata=argInit_UnboundedxUnbounded_real_T();
      outdata=argInit_UnboundedxUnbounded_real_T();
      argInit_asr_state_t(&instate);
      argInit_asr_state_t(&outstate);
  }
    
    mha_wave_t * process(mha_wave_t * signal)
    {
        //TODO get real data from somewhere, preferably LSL, maybe via lsl2ac plugin
        //asr_process_simple(indata, 100, &instate, outdata, &outstate);
        return signal;
    }
    
private:
    asr_state_t instate;
    asr_state_t outstate;
    coder::array<double, 2U> indata;
    coder::array<double, 2U> outdata;

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
