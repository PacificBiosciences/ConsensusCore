// Author: David Alexander

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <limits>
#include <vector>

#include <ConsensusCore/Matrix/SparseMatrix.hpp>

namespace ConsensusCore {
// Performance insensitive routines are not inlined

SparseMatrix::SparseMatrix(int rows, int cols)
    : columns_(cols)
    , nCols_(cols)
    , nRows_(rows)
    , columnBeingEdited_(-1)
    , usedRanges_(cols, Interval(0, 0))
{
    for (int j = 0; j < nCols_; j++) {
        columns_[j] = NULL;
    }
}

SparseMatrix::SparseMatrix(const SparseMatrix& other)
    : columns_(other.nCols_)
    , nCols_(other.nCols_)
    , nRows_(other.nRows_)
    , columnBeingEdited_(other.columnBeingEdited_)
    , usedRanges_(other.usedRanges_)
{
    for (int j = 0; j < nCols_; j++) {
        if (other.columns_[j] != NULL) {
            columns_[j] = new SparseVector(*other.columns_[j]);
        } else {
            columns_[j] = NULL;
        }
    }
}

SparseMatrix::~SparseMatrix()
{
    for (int j = 0; j < nCols_; j++) {
        if (columns_[j] != NULL) delete columns_[j];
    }
}

int SparseMatrix::UsedEntries() const
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

int SparseMatrix::AllocatedEntries() const
{
    int sum = 0;
    for (int j = 0; j < nCols_; j++) {
        sum += (columns_[j] != NULL ? columns_[j]->AllocatedEntries() : 0);
    }
    return sum;
}

void SparseMatrix::ToHostMatrix(float** mat, int* rows, int* cols) const
{
    const float nan = std::numeric_limits<float>::quiet_NaN();
    *mat = new float[Rows() * Columns()];
    *rows = Rows();
    *cols = Columns();
    for (int i = 0; i < Rows(); i++) {
        for (int j = 0; j < Columns(); j++) {
            (*mat)[i * Columns() + j] = IsAllocated(i, j) ? Get(i, j) : nan;
        }
    }
}

void SparseMatrix::CheckInvariants(int) const
{
    for (int j = 0; j < nCols_; j++) {
        if (columns_[j] != NULL) columns_[j]->CheckInvariants();
    }
}
}
