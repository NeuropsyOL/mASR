#include "asr.hh"
#include "rapidcsv.h"
#include "print_array.hh"
#include <gtest/gtest.h>
#include <limits>

std::array<std::string,20> infiles{"sme_1_1.xdf_filt.csv", "sme_1_2.xdf_filt.csv", "sme_1_3.xdf_filt.csv", "sme_1_4.xdf_filt.csv", "sme_1_5.xdf_filt.csv","sme_1_6.xdf_filt.csv",
  "sme_2_1.xdf_filt.csv", "sme_2_2.xdf_filt.csv", "sme_2_3.xdf_filt.csv", "sme_2_4.xdf_filt.csv", "sme_2_5.xdf_filt.csv", "sme_2_6.xdf_filt.csv",
  "sme_3_1.xdf_filt.csv", "sme_3_2.xdf_filt.csv", "sme_3_3.xdf_filt.csv", "sme_3_4.xdf_filt.csv", "sme_3_5.xdf_filt.csv", "sme_4_1.xdf_filt.csv",
  "sme_4_2.xdf_filt.csv", "sme_4_3.xdf_filt.csv"};

::testing::AssertionResult AssertArraysAreEqual(const char* a_expr, const char* b_expr, const coder::array<double,2>& A, const coder::array<double,2> B){
  if(A.size(0)!=B.size(0) or A.size(1)!=B.size(1)){
    return testing::AssertionFailure()<<a_expr<<"~="<<b_expr<<": SIZE DIFFERS "<<" (expected ["<<A.size(0)<<"x"<<A.size(1)<<"]; actual ["<<B.size(0)<<"x"<<B.size(1)<<"])";
  };
  for(int irow=0;irow<A.size(0);++irow)
    for(int icol=0;icol<A.size(0);++icol)
      if(fabs(A.at(irow,icol)-B.at(irow,icol))>1e-3)
        return testing::AssertionFailure() << a_expr << "~=" << b_expr
                                           << ": VALUES DIFFER TOO MUCH, first difference at index ("<<irow<<","<<icol<<"): "<<A.at(irow,icol)<<"~="<<B.at(irow,icol);
  return testing::AssertionSuccess();
  }

coder::array<double,2> to_array(double* buf, size_t numel){
  auto X=argInit_UnboundedxUnbounded_real_T(numel,1,0);
  for(unsigned irow=0U;irow<numel;++irow)
    X[irow]=buf[irow];
  return X;
}


TEST(calib,compare_to_matlab){
  auto X=to_array(rapidcsv::Document("calib_csv/X1.csv",rapidcsv::LabelParams(-1,-1)));
  auto M = argInit_UnboundedxUnbounded_real_T(20,20,0);
  auto T = argInit_UnboundedxUnbounded_real_T(20,20,0);
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T(8,20);
  asr_calibrate_simple(X, 250, M,T,B,A,iirstate);

  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,M,to_array(rapidcsv::Document("calib_csv/M.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,T,to_array(rapidcsv::Document("calib_csv/T.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,iirstate,to_array(rapidcsv::Document("calib_csv/iirstate.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,to_array(A,9).reshape(1,9),to_array(rapidcsv::Document("calib_csv/A.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,to_array(B,9).reshape(1,9),to_array(rapidcsv::Document("calib_csv/B.csv",rapidcsv::LabelParams(-1,-1))));
}

TEST(process,compare_to_matlab){
  auto X=to_array(rapidcsv::Document("calib_csv/X1.csv",rapidcsv::LabelParams(-1,-1)));
  auto X2=to_array(rapidcsv::Document("calib_csv/X2.csv",rapidcsv::LabelParams(-1,-1)));
  auto Y=to_array(rapidcsv::Document("calib_csv/Y.csv",rapidcsv::LabelParams(-1,-1)));
  auto M = argInit_UnboundedxUnbounded_real_T(24,24,0);
  auto T = argInit_UnboundedxUnbounded_real_T(24,24,0);
  double B[9];
  double A[9];
  auto iirstate = argInit_UnboundedxUnbounded_real_T(8,24);
  asr_state_t instate;
  argInit_asr_state_t(&instate);
  asr_calibrate_simple(X, 250, instate.M,instate.T,B,A,instate.iir);
 
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,instate.M,to_array(rapidcsv::Document("calib_csv/M.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,instate.T,to_array(rapidcsv::Document("calib_csv/T.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,instate.iir,to_array(rapidcsv::Document("calib_csv/iirstate.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,to_array(A,9).reshape(1,9),to_array(rapidcsv::Document("calib_csv/A.csv",rapidcsv::LabelParams(-1,-1))));
  EXPECT_PRED_FORMAT2(AssertArraysAreEqual,to_array(B,9).reshape(1,9),to_array(rapidcsv::Document("calib_csv/B.csv",rapidcsv::LabelParams(-1,-1))));

 

  for(int i=0;i<9;i++){
    instate.A[i]=A[i];
    instate.B[i]=B[i];
  }
  asr_state_t outstate;
  argInit_asr_state_t(&outstate);
  coder::array<double, 2U> indata;
  coder::array<double, 2U> outdata;
  indata=argInit_UnboundedxUnbounded_real_T(X.size(0),X.size(1));
  outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),X.size(1));
  //indata=argInit_UnboundedxUnbounded_real_T(X.size(0),X.size(1));
  //outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),X.size(1));

  auto tmp_state=instate;

  for(size_t i=1;i<infiles.size();++i){
    std::cout<<"\n\n------------------------------";
    std::cout<<"calib_csv/"+std::string(infiles[i].substr(0,infiles[i].size()-4))+".out.csv\n";
    indata=to_array(rapidcsv::Document("calib_csv/"+infiles[i],rapidcsv::LabelParams(-1,-1)));
    auto refdata=to_array(rapidcsv::Document("calib_csv/"+std::string(infiles[i].substr(0,infiles[i].size()-4))+".out.csv",rapidcsv::LabelParams(-1,-1)));
    asr_process_simple(indata, 250, &tmp_state, outdata, &outstate);
    tmp_state=instate;
    ASSERT_PRED_FORMAT2(AssertArraysAreEqual,outdata,refdata);
    outdata=argInit_UnboundedxUnbounded_real_T(X.size(0),X.size(1));
  }
}
