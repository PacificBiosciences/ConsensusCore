// Author: David Alexander

#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/tuple/tuple.hpp>
#include <cassert>

#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/Matrix/DenseMatrix.hpp>
#include <ConsensusCore/Utils.hpp>

using boost::numeric::ublas::matrix_column;

namespace ConsensusCore {
//
// Nullability
//
inline const DenseMatrix& DenseMatrix::Null()
{
    static DenseMatrix* nullObj = new DenseMatrix(0, 0);
    return *nullObj;
}

inline bool DenseMatrix::IsNull() const { return (Rows() == 0 && Columns() == 0); }

//
// Size information
//
inline int DenseMatrix::Rows() const { return size1(); }

inline int DenseMatrix::Columns() const { return size2(); }

//
// Entry range queries per column
//
inline void DenseMatrix::StartEditingColumn(int j, int, int)
{
    assert(columnBeingEdited_ == -1);
    columnBeingEdited_ = j;
    ClearColumn(j);
}

inline void DenseMatrix::FinishEditingColumn(int j, int usedRowsBegin, int usedRowsEnd)
{
    assert(columnBeingEdited_ == j);
    usedRanges_[j] = Interval(usedRowsBegin, usedRowsEnd);
    DEBUG_ONLY(CheckInvariants(columnBeingEdited_));
    columnBeingEdited_ = -1;
}

inline Interval DenseMatrix::UsedRowRange(int j) const
{
    assert(0 <= j && j < static_cast<int>(usedRanges_.size()));
    return usedRanges_[j];
}

inline bool DenseMatrix::IsColumnEmpty(int j) const
{
    assert(0 <= j && j < static_cast<int>(usedRanges_.size()));
    return (usedRanges_[j].Begin >= usedRanges_[j].End);
}

//
// Accessors
//
inline void DenseMatrix::Set(int i, int j, float v)
{
    assert(columnBeingEdited_ == j);
    boost_dense_matrix::operator()(i, j) = v;
}

inline bool DenseMatrix::IsAllocated(int
#ifndef NDEBUG
                                         i
#endif
                                     ,
                                     int
#ifndef NDEBUG
                                         j
#endif
                                     ) const
{
    assert(0 <= i && i < Rows() && 0 <= j && j < Columns());
    return true;
}

inline float DenseMatrix::Get(int i, int j) const { return (*this)(i, j); }

inline const float& DenseMatrix::operator()(int i, int j) const
{
    return boost_dense_matrix::operator()(i, j);
}

inline void DenseMatrix::ClearColumn(int j)
{
    DEBUG_ONLY(CheckInvariants(j);)
    // (Rely on the fact that the underlying memory is stored
    // contiguously)
    int begin, end;
    boost::tie(begin, end) = usedRanges_[j];
    std::fill_n(reinterpret_cast<float*>(&boost_dense_matrix::operator()(begin, j)),  // NOLINT
                end - begin, value_type());
    usedRanges_[j] = Interval(0, 0);
    DEBUG_ONLY(CheckInvariants(j);)
}

//
// SSE
//
inline __m128 DenseMatrix::Get4(int i, int j) const
{
    assert(0 <= i && i <= Rows() - 4);
    return _mm_loadu_ps(&boost_dense_matrix::operator()(i, j).value);
}

inline void DenseMatrix::Set4(int i, int j, __m128 v4)
{
    assert(columnBeingEdited_ == j);
    assert(0 <= i && i <= Rows() - 4);
    _mm_storeu_ps(&boost_dense_matrix::operator()(i, j).value, v4);
}
}
