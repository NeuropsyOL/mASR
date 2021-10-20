#include <benchmark/benchmark.h>
#include "asr.hh"
#include <iostream>
#include <random>
#include <optional>

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
    indata=argInit_UnboundedxUnbounded_real_T(20,state.range(0));
    outdata=argInit_UnboundedxUnbounded_real_T(20,state.range(0));
    //indata.set_size(20,state.range(0));
    //outdata.set_size(20,state.range(0));
    asr_process_simple(indata, 100, &instate, outdata, &outstate);
    //std::cerr<<indata.size(0)<<"x"<<indata.size(1)<<" ";
    //std::cerr<<outdata.size(0)<<"x"<<outdata.size(1)<<"\n";
  }
}

static void BM_asr_process_simple_norealloc(benchmark::State& state) {
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
static void BM_asr_process_simple_norealloc_reserve_lookahead(benchmark::State& state) {
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
  // Reserve additional space for lookahead
  indata.set_size(20,state.range(0)+15);
  for (auto _ : state) {
    // Need to reset size every time, for some reason the generated
    // code appends to indata on every call
    indata.set_size(20,state.range(0));
    outdata.set_size(20,state.range(0));
    asr_process_simple(indata, 100, &instate, outdata, &outstate);
  }
}

//Register the function as a benchmark, use milliseconds as time unit
BENCHMARK(BM_asr_calibrate_simple)->Unit(benchmark::kMillisecond)->Range(1e4,1e4);
BENCHMARK(BM_asr_process_simple)->Unit(benchmark::kMicrosecond)->DenseRange(5e1,5e2,1e2);
BENCHMARK(BM_asr_process_simple_norealloc)->Unit(benchmark::kMicrosecond)->DenseRange(5e1,5e2,1e2);
BENCHMARK(BM_asr_process_simple_norealloc_reserve_lookahead)->Unit(benchmark::kMicrosecond)->DenseRange(5e1,5e2,1e2);
// Run the benchmark
BENCHMARK_MAIN();
