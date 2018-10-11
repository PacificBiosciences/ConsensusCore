// Author: David Alexander

#pragma once

#include <xmmintrin.h>
#include <utility>
#include <vector>

#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

namespace ConsensusCore {

class SparseVector
{
public:  // Constructor, destructor
    SparseVector(int logicalLength, int beginRow, int endRow);
    SparseVector(const SparseVector& other);
    ~SparseVector();

    // Ensures there is enough allocated storage to
    // hold entries for at least [beginRow, endRow) (plus padding);
    // clears existing entries.
    void ResetForRange(int beginRow, int endRow);

public:
    const float& operator()(int i) const;
    bool IsAllocated(int i) const;
    float Get(int i) const;
    void Set(int i, float v);
    __m128 Get4(int i) const;
    void Set4(int i, __m128 v);
    void Clear();

public:
    int AllocatedEntries() const;
    void CheckInvariants() const;

private:
    // Expand the range of rows for which we have backing storage,
    // while preserving contents.  The arguments will become the
    // new allocated bounds, so caller should add padding if desired
    // before calling.
    void ExpandAllocated(int newAllocatedBegin, int newAllocatedEnd);

private:
    std::vector<float>* storage_;

    // the "logical" length of the vector, of which only
    // a subset of entries are actually allocated
    int logicalLength_;

    // row numbers in the abstraction we are presenting
    int allocatedBeginRow_;
    int allocatedEndRow_;

    // analytics
    int nReallocs_;
};
}

#include <ConsensusCore/Matrix/SparseVector-inl.hpp>
