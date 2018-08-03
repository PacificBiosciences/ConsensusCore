// Author: David Alexander

#pragma once

#include <xmmintrin.h>
#include <utility>
#include <vector>

#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/Matrix/AbstractMatrix.hpp>
#include <ConsensusCore/Matrix/SparseVector.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

namespace ConsensusCore {

class SparseMatrix : public AbstractMatrix
{
public:  // Constructor, destructor
    SparseMatrix(int rows, int cols);
    SparseMatrix(const SparseMatrix& other);
    ~SparseMatrix();

public:  // Nullability
    static const SparseMatrix& Null();
    bool IsNull() const;

public:  // Size information
    int Rows() const;
    int Columns() const;

public:  // Information about entries filled by column
    void StartEditingColumn(int j, int hintBegin, int hintEnd);
    void FinishEditingColumn(int j, int usedBegin, int usedEnd);
    Interval UsedRowRange(int j) const;
    bool IsColumnEmpty(int j) const;
    int UsedEntries() const;
    int AllocatedEntries() const;  // an entry may be allocated but not used

public:  // Accessors
    const float& operator()(int i, int j) const;
    bool IsAllocated(int i, int j) const;
    float Get(int i, int j) const;
    void Set(int i, int j, float v);
    void ClearColumn(int j);

public:  // SSE accessors, which access 4 successive entries in a column
    __m128 Get4(int i, int j) const;
    void Set4(int i, int j, __m128 v);

public:
    // Method SWIG clients can use to get a native matrix (e.g. Numpy)
    // mat must be filled as a ROW major matrix
    void ToHostMatrix(float** mat, int* rows, int* cols) const;

private:
    void CheckInvariants(int column) const;

private:
    std::vector<SparseVector*> columns_;
    int nCols_;
    int nRows_;
    int columnBeingEdited_;
    std::vector<Interval> usedRanges_;
};
}

#include <ConsensusCore/Matrix/SparseMatrix-inl.hpp>
