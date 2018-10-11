// Author: David Alexander

#include <ConsensusCore/Matrix/DenseMatrix.hpp>

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <cassert>
#include <vector>

#include <ConsensusCore/LFloat.hpp>

using boost::numeric::ublas::matrix;
using boost::numeric::ublas::row_major;

namespace ConsensusCore {

// Performance insensitive routines are not inlined

DenseMatrix::DenseMatrix(int rows, int cols)
    : boost_dense_matrix(rows, cols), usedRanges_(cols, Interval(0, 0)), columnBeingEdited_(-1)
{
    for (int j = 0; j < cols; j++) {
        CheckInvariants(j);
    }
}

DenseMatrix::~DenseMatrix() {}

int DenseMatrix::UsedEntries() const
{
    // use column ranges
    int filledEntries = 0;
    for (int col = 0; col < Columns(); ++col) {
        int start, end;
        boost::tie(start, end) = UsedRowRange(col);
        filledEntries += (end - start);
    }
    return filledEntries;
}

int DenseMatrix::AllocatedEntries() const { return Rows() * Columns(); }

void DenseMatrix::ToHostMatrix(float** mat, int* rows, int* cols) const
{
    // TODO(dalexander): make sure SWIG client deallocates this memory -- use
    // %newobject flag
    matrix<lfloat, row_major> rowMajorPeer(*this);
    *mat = new float[Rows() * Columns()];
    std::copy(rowMajorPeer.data().begin(), rowMajorPeer.data().end(), *mat);
    *rows = Rows();
    *cols = Columns();
}

void DenseMatrix::CheckInvariants(int column) const
{
    // make sure no used entries are outside of the bands
    int start, end;
    boost::tie(start, end) = UsedRowRange(column);
    assert(0 <= start && start <= end && end <= Rows());
    for (int i = 0; i < Rows(); i++) {
        if (!(start <= i && i < end)) {
            assert((*this)(i, column) == value_type());
        }
    }
}
}
