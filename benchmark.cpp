#include <benchmark/benchmark.h>
#include "asr_calibrate_simple.h"
#include "asr_process_simple.h"
#include "asr_terminate.h"
// Function Declarations
static coder::array<double, 2U> argInit_1xUnbounded_real_T();
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T(unsigned,unsigned);
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
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T(unsigned N=20, unsigned M=20000)
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

static void BM_asr_calibrate_simple(benchmark::State& state) {

  auto X = argInit_UnboundedxUnbounded_real_T(20,state.range(0));
  auto M = argInit_UnboundedxUnbounded_real_T();
  auto T = argInit_UnboundedxUnbounded_real_T();
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T();

  for (auto _ : state) {
    // This code gets timed
    asr_calibrate_simple(X, 100, M,T,B,A,iirstate);
  }
}

static void BM_asr_process_simple(benchmark::State& state) {
  auto X = argInit_UnboundedxUnbounded_real_T();
  auto M = argInit_UnboundedxUnbounded_real_T();
  auto T = argInit_UnboundedxUnbounded_real_T();
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T();
  asr_calibrate_simple(X, 100, M,T,B,A,iirstate);
  // Perform setup here
  asr_state_t instate;
  asr_state_t outstate;
  coder::array<double, 2U> indata;
  coder::array<double, 2U> outdata;
  indata=argInit_UnboundedxUnbounded_real_T(20,state.range(0));
  outdata=argInit_UnboundedxUnbounded_real_T(20,state.range(0));
  argInit_asr_state_t(&instate);
  instate.M=M;
  instate.T=T;
  int idx=0;
  for(auto &elm:instate.B)
	elm=B[idx++];
idx=0;
for(auto & elm:instate.A)
elm=A[idx++];
  instate.iir=iirstate;
  argInit_asr_state_t(&outstate);
  for (auto _ : state) {
    // This code gets timed
    asr_process_simple(indata, 100, &instate, outdata, &outstate);
  }
}
//Register the function as a benchmark, use milliseconds as time unit
BENCHMARK(BM_asr_calibrate_simple)->Unit(benchmark::kMillisecond)->RangeMultiplier(10)->Range(1e3,1e6);
BENCHMARK(BM_asr_process_simple)->Unit(benchmark::kMillisecond)->RangeMultiplier(10)->Range(1e1,1e6);
// Run the benchmark
BENCHMARK_MAIN();
