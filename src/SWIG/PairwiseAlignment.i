%{
/* Includes the header in the wrapper code */
#include <Align/PairwiseAlignment.hpp>
#include <Align/AffineAlignment.hpp>
#include <Align/LinearAlignment.hpp>
using namespace ConsensusCore;
%}

%include "Types.i"

%newobject Align;
%newobject AlignAffine;
%newobject AlignAffineIupac;
%newobject AlignLinear;

%include <Align/PairwiseAlignment.hpp>
%include <Align/AffineAlignment.hpp>
%include <Align/LinearAlignment.hpp>
