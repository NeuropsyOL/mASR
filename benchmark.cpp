#include <benchmark/benchmark.h>
#include "asr_calibrate_simple.h"
#include "asr_process_simple.h"
#include "asr_terminate.h"
#include <random>
#include <optional>
// Function Declarations
static coder::array<double, 2U> argInit_1xUnbounded_real_T(unsigned,std::optional<double>);
static coder::array<double, 2U> argInit_UnboundedxUnbounded_real_T(unsigned,unsigned, std::optional<double>);
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
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      if(val)
        result[idx1]=*val;
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

static void BM_asr_calibrate_simple(benchmark::State& state) {

  auto X = argInit_UnboundedxUnbounded_real_T(20,state.range(0));
  auto M = argInit_UnboundedxUnbounded_real_T(20,20,0);
  auto T = argInit_UnboundedxUnbounded_real_T(20,20,0);
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T(8,20);

  for (auto _ : state) {
    // This code gets timed
    asr_calibrate_simple(X, 100, M,T,B,A,iirstate);
  }
}

static void BM_asr_process_simple(benchmark::State& state) {
  auto X = argInit_UnboundedxUnbounded_real_T(20,50000);
  asr_state_t instate;
  argInit_asr_state_t(&instate);
  double A[9];
  double B[9];
  asr_calibrate_simple(X, 100, instate.M,instate.T,B,A,instate.iir);
  for(int i=0;i<9;i++){
    instate.A[i]=A[i];
    instate.B[i]=B[i];
  }
  asr_state_t outstate;
  argInit_asr_state_t(&outstate);
  coder::array<double, 2U> indata;
  coder::array<double, 2U> outdata;
  indata=argInit_UnboundedxUnbounded_real_T(20,state.range(0));
  outdata=argInit_UnboundedxUnbounded_real_T(20,state.range(0),0);

  for (auto _ : state) {
    // Need to reset size every time, for some reason the generated
    // code appends to indata on every call
    indata.set_size(20,state.range(0));
    outdata.set_size(20,state.range(0));
    asr_process_simple(indata, 100, &instate, outdata, &outstate);
  }
}
//Register the function as a benchmark, use milliseconds as time unit
BENCHMARK(BM_asr_calibrate_simple)->Unit(benchmark::kMillisecond)->Range(10000,50000);
BENCHMARK(BM_asr_process_simple)->Unit(benchmark::kMicrosecond)->DenseRange(1e2,1000,100);
// Run the benchmark
BENCHMARK_MAIN();
