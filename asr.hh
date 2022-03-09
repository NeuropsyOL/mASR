#include "asr_calibrate_simple.h"
#include "asr_process_simple.h"
#include "asr_terminate.h"
#include "rapidcsv/rapidcsv.h"
#include <random>
#include <optional>
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
  result->cov = argInit_1xUnbounded_real_T(0,0);
  result->carry = argInit_1xUnbounded_real_T(0,0);
  result->iir = argInit_1xUnbounded_real_T(8,20);
  result->last_R =  argInit_1xUnbounded_real_T(0,0);
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

coder::array<double,2> to_array(const rapidcsv::Document& doc){
  auto X=argInit_UnboundedxUnbounded_real_T(doc.GetRowCount(),doc.GetColumnCount(),0);
  for(unsigned icol=0U;icol<doc.GetColumnCount();++icol)
    for(unsigned irow=0U;irow<doc.GetRowCount();++irow)
      X.at(irow,icol)=doc.GetCell<double>(icol,irow);
  return X;
}
