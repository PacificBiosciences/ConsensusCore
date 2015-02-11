%{
/* Includes the header in the wrapper code */
#include <Align/PairwiseAlignment.hpp>
#include <Align/AffineAlignment.hpp>
using namespace ConsensusCore;
%}

%include "Types.i"

%newobject Align;
%newobject AlignAffine;
%newobject AlignAffineIupac;

%include <Align/PairwiseAlignment.hpp>
%include <Align/AffineAlignment.hpp>
