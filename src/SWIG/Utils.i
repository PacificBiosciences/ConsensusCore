%{
/* Includes the header in the wrapper code */
#include "Utils.hpp"
#include "Coverage.hpp"
using namespace ConsensusCore;
%}

%include "Types.hpp"

#ifdef SWIGPYTHON
    %apply (int DIM1, int* IN_ARRAY1)
         { (int tStartDim, int* tStart),
           (int tEndDim,   int* tEnd)  };
    %apply (int DIM1, int* ARGOUT_ARRAY1)
         { (int winLen, int* coverage) };
#endif // SWIGPYTHON

%include "Utils.hpp"
%include "Coverage.hpp"
