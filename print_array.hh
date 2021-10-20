#ifndef PRINT_ARRAY_HH
#define PRINT_ARRAY_HH
#include "codegen/coder_array.h"
#include <iostream>
#include <iomanip>

template<typename T>
inline void print_array(const coder::array<T,2>& M)
{
  for(int irow=0;irow<M.size(0);++irow){
    for(int icol=0;icol<M.size(1);++icol){
      std::cout<<std::setw(8)<<std::fixed<<std::setprecision(4)<<M.at(irow,icol)<<" ";
    }
    std::cout<<"\n";
  }
}

template<>
inline void print_array<creal_T>(const coder::array<creal_T,2>& M)
{
  for(int irow=0;irow<M.size(0);++irow){
    for(int icol=0;icol<M.size(1);++icol){
      std::cout<<std::setw(8)<<std::fixed<<std::setprecision(4)<<M.at(irow,icol).re<<" ";
    }
    std::cout<<"\n";
  }
}



#endif // PRINT_ARRAY_HH
