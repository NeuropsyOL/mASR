#include <benchmark/benchmark.h>
#include "rapidcsv.h"
#include "asr.hh"
#include <iostream>
#include <array>
#include <random>
#include <optional>
#include <string>
using namespace std::string_literals;
std::array infiles{"sme_1_1.xdf_filt.csv"s, "sme_1_5.xdf_filt.csv"s};

void set_array(const coder::array<double,2U>& in, coder::array<double,2U>& out,int from, int to){
  out.set_size(in.size(0),to-from+1);
  for(int i=0;i<in.size(0);++i)
    for(int k=from;k<to+1;++k)
      out.at(i,k)=in.at(i,k);
}


static void BM_asr_calibrate_simple(benchmark::State& state) {

  auto X=to_array(rapidcsv::Document("calib_csv/X1.csv",rapidcsv::LabelParams(-1,-1)));
  auto M = argInit_UnboundedxUnbounded_real_T(24,24,0);
  auto T = argInit_UnboundedxUnbounded_real_T(24,24,0);
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T(8,24);

  for (auto _ : state) {
    // This code gets timed
    asr_calibrate_simple(X, 100, M,T,B,A,iirstate);
  }
}

static void BM_asr_process_simple_real(benchmark::State& state) {
  auto X=to_array(rapidcsv::Document("calib_csv/"+infiles[0],rapidcsv::LabelParams(-1,-1)));
  auto M = argInit_UnboundedxUnbounded_real_T(24,24,0);
  auto T = argInit_UnboundedxUnbounded_real_T(24,24,0);
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T(8,24);
  asr_state_t instate;
  argInit_asr_state_t(&instate);
  asr_calibrate_simple(X, 100, instate.M,instate.T,B,A,instate.iir);
  for(int i=0;i<9;i++){
    instate.A[i]=A[i];
    instate.B[i]=B[i];
  }
  asr_state_t outstate;
  argInit_asr_state_t(&outstate);
  coder::array<double, 2U> indata;
  coder::array<double, 2U> outdata;
  outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),50);
  auto tmp_state=instate;
  indata=to_array(rapidcsv::Document("calib_csv/"+infiles[0],rapidcsv::LabelParams(-1,-1)));
  auto use_data=argInit_UnboundedxUnbounded_real_T(indata.size(0),50);
  for(auto _ :state){
    set_array(indata,use_data,0,state.range(0)-1);
    asr_process_simple(use_data, 100, &tmp_state, outdata, &outstate);
    outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),50);
  }
}

static void BM_asr_process_simple_random(benchmark::State& state) {
  auto X = argInit_UnboundedxUnbounded_real_T(24,10000);
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
  indata=argInit_UnboundedxUnbounded_real_T(24,state.range(0));
  outdata=argInit_UnboundedxUnbounded_real_T(24,state.range(0),0);

  for (auto _ : state) {
    // Need to reset size every time, for some reason the generated
    // code appends to indata on every call
    indata=argInit_UnboundedxUnbounded_real_T(24,state.range(0));
    outdata=argInit_UnboundedxUnbounded_real_T(24,state.range(0),0);
    asr_process_simple(indata, 100, &instate, outdata, &outstate);
  }
}

//Register the function as a benchmark, use milliseconds as time unit
BENCHMARK(BM_asr_calibrate_simple)->Unit(benchmark::kMillisecond)->Range(1e4,1e4);
BENCHMARK(BM_asr_process_simple_random)->Unit(benchmark::kMicrosecond)->DenseRange(5e1,5e2,1e2);
BENCHMARK(BM_asr_process_simple_real)->Unit(benchmark::kMicrosecond)->DenseRange(5e1,5e2,1e2);
// Run the benchmark
BENCHMARK_MAIN();
