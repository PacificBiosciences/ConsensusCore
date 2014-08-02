%{
/* Includes the header in the wrapper code */
#include <Types.hpp>
#include <Matrix/AbstractMatrix.hpp>
#include <Matrix/DenseMatrix.hpp>
#include <Matrix/SparseMatrix.hpp>
using namespace ConsensusCore;
%}

%include <Types.hpp>

#ifdef SWIGPYTHON
        // apply this typemap to ToHostMatrix
        %apply (float** ARGOUTVIEW_ARRAY2, int* DIM1, int* DIM2)
             { (float** mat, int* rows, int* cols) };
#endif // SWIGPYTHON

%newobject *::UsedRowRange;


%include <Matrix/AbstractMatrix.hpp>
%include <Matrix/DenseMatrix.hpp>
%include <Matrix/SparseMatrix.hpp>
