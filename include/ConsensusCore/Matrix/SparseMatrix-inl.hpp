// Author: David Alexander

#pragma once

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <cassert>

#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/LFloat.hpp>
#include <ConsensusCore/Matrix/SparseMatrix.hpp>

using std::min;
using std::max;

namespace ConsensusCore {
//
// Nullability
//
inline const SparseMatrix& SparseMatrix::Null()
{
    static SparseMatrix* nullObj = new SparseMatrix(0, 0);
    return *nullObj;
}

inline bool SparseMatrix::IsNull() const { return (Rows() == 0 && Columns() == 0); }

//
// Size information
//
inline int SparseMatrix::Rows() const { return nRows_; }

inline int SparseMatrix::Columns() const { return nCols_; }

//
// Entry range queries per column
//
inline void SparseMatrix::StartEditingColumn(int j, int hintBegin, int hintEnd)
{
    assert(columnBeingEdited_ == -1);
    columnBeingEdited_ = j;
    if (columns_[j] != NULL) {
        columns_[j]->ResetForRange(hintBegin, hintEnd);
    } else {
        columns_[j] = new SparseVector(Rows(), hintBegin, hintEnd);
    }
}

inline void SparseMatrix::FinishEditingColumn(int j, int usedRowsBegin, int usedRowsEnd)
{
    assert(columnBeingEdited_ == j);
    usedRanges_[j] = Interval(usedRowsBegin, usedRowsEnd);
    DEBUG_ONLY(CheckInvariants(columnBeingEdited_));
    columnBeingEdited_ = -1;
}

inline Interval SparseMatrix::UsedRowRange(int j) const
{
    assert(0 <= j && j < static_cast<int>(usedRanges_.size()));
    return usedRanges_[j];
}

inline bool SparseMatrix::IsColumnEmpty(int j) const
{
    assert(0 <= j && j < static_cast<int>(usedRanges_.size()));
    return (usedRanges_[j].Begin >= usedRanges_[j].End);
}

//
// Accessors
//
inline const float& SparseMatrix::operator()(int i, int j) const
{
    static const float emptyCell = Zero<lfloat>();
    if (columns_[j] == NULL) {
        return emptyCell;
    } else {
        return (*columns_[j])(i);
    }
}

inline bool SparseMatrix::IsAllocated(int i, int j) const
{
    return columns_[j] != NULL && columns_[j]->IsAllocated(i);
}

inline float SparseMatrix::Get(int i, int j) const { return (*this)(i, j); }

inline void SparseMatrix::Set(int i, int j, float v)
{
    assert(columnBeingEdited_ == j);
    columns_[j]->Set(i, v);
}

inline void SparseMatrix::ClearColumn(int j)
{
    usedRanges_[j] = Interval(0, 0);
    columns_[j]->Clear();
    DEBUG_ONLY(CheckInvariants(j);)
}

//
// SSE
//
inline __m128 SparseMatrix::Get4(int i, int j) const
{
    if (columns_[j] == NULL) {
        return Zero4<lfloat>();
    } else {
        return columns_[j]->Get4(i);
    }
}

inline void SparseMatrix::Set4(int i, int j, __m128 v4)
{
    assert(columnBeingEdited_ == j);
    columns_[j]->Set4(i, v4);
}
}
