// Author: David Alexander

#include <algorithm>
#include <cassert>
#include <vector>

#include <ConsensusCore/LFloat.hpp>
#include <ConsensusCore/Matrix/SparseVector.hpp>

#define PADDING 8
#define LZERO (-FLT_MAX)
#define SHRINK_THRESHOLD 0.8

namespace ConsensusCore {
using std::vector;
using std::max;
using std::min;

inline SparseVector::SparseVector(int logicalLength, int beginRow, int endRow)
{
    assert(beginRow >= 0 && beginRow <= endRow && endRow <= logicalLength);
    logicalLength_ = logicalLength;
    allocatedBeginRow_ = max(beginRow - PADDING, 0);
    allocatedEndRow_ = min(endRow + PADDING, logicalLength_);
    storage_ = new vector<float>(allocatedEndRow_ - allocatedBeginRow_, LZERO);
    nReallocs_ = 0;
    DEBUG_ONLY(CheckInvariants());
}

inline SparseVector::SparseVector(const SparseVector& other)
    : logicalLength_(other.logicalLength_)
    , allocatedBeginRow_(other.allocatedBeginRow_)
    , allocatedEndRow_(other.allocatedEndRow_)
    , nReallocs_(0)
{
    storage_ = new vector<float>(*other.storage_);
    nReallocs_ = 0;
    DEBUG_ONLY(CheckInvariants());
}

inline SparseVector::~SparseVector() { delete storage_; }

inline void SparseVector::ResetForRange(int beginRow, int endRow)
{
    // Allows reuse.  Destructive.
    DEBUG_ONLY(CheckInvariants());
    assert(beginRow >= 0 && beginRow <= endRow && endRow <= logicalLength_);
    int newAllocatedBegin = max(beginRow - PADDING, 0);
    int newAllocatedEnd = min(endRow + PADDING, logicalLength_);
    if ((newAllocatedEnd - newAllocatedBegin) > (allocatedEndRow_ - allocatedBeginRow_)) {
        storage_->resize(newAllocatedEnd - newAllocatedBegin);
        nReallocs_++;
        Clear();
    } else if ((newAllocatedEnd - newAllocatedBegin) <
               static_cast<int>(SHRINK_THRESHOLD * (allocatedEndRow_ - allocatedBeginRow_))) {
        // use swap trick to free allocated but unused memory,
        // see: http://stackoverflow.com/questions/253157/how-to-downsize-stdvector
        std::vector<float>(newAllocatedEnd - newAllocatedBegin, LZERO).swap(*storage_);
        nReallocs_++;
    } else {
        Clear();
    }
    allocatedBeginRow_ = newAllocatedBegin;
    allocatedEndRow_ = newAllocatedEnd;
    DEBUG_ONLY(CheckInvariants());
}

inline void SparseVector::ExpandAllocated(int newAllocatedBegin, int newAllocatedEnd)
{
    // Expands allocated storage while preserving the contents.
    DEBUG_ONLY(CheckInvariants());
    assert(newAllocatedBegin >= 0 && newAllocatedBegin <= newAllocatedEnd &&
           newAllocatedEnd <= logicalLength_);
    assert(newAllocatedBegin <= allocatedBeginRow_ && newAllocatedEnd >= allocatedEndRow_);
    // Resize the underlying storage.
    storage_->resize(newAllocatedEnd - newAllocatedBegin);
    // Use memmove to robustly relocate the old data (handles overlapping ranges).
    //   Data is at:
    //      storage[0 ... (end - begin) )
    //   Must be moved to:
    //      storage[(begin - newBegin) ... (end - newBegin)]
    memmove(&(*storage_)[allocatedBeginRow_ - newAllocatedBegin], &(*storage_)[0],
            (allocatedEndRow_ - allocatedBeginRow_) * sizeof(float));  // NOLINT
    // "Zero"-fill the allocated but unused space.
    std::fill(storage_->begin(), storage_->begin() + (allocatedBeginRow_ - newAllocatedBegin),
              LZERO);
    std::fill(storage_->begin() + (allocatedEndRow_ - newAllocatedBegin), storage_->end(), LZERO);
    // Update pointers.
    allocatedBeginRow_ = newAllocatedBegin;
    allocatedEndRow_ = newAllocatedEnd;
    nReallocs_++;
    DEBUG_ONLY(CheckInvariants());
}

inline bool SparseVector::IsAllocated(int i) const
{
    assert(i >= 0 && i < logicalLength_);
    return i >= allocatedBeginRow_ && i < allocatedEndRow_;
}

inline const float& SparseVector::operator()(int i) const
{
    if (IsAllocated(i)) {
        return (*storage_)[i - allocatedBeginRow_];
    } else {
        static const float emptyCell_ = LZERO;
        return emptyCell_;
    }
}

inline float SparseVector::Get(int i) const { return (*this)(i); }

inline void SparseVector::Set(int i, float v)
{
    DEBUG_ONLY(CheckInvariants());
    assert(i >= 0 && i < logicalLength_);
    if (!IsAllocated(i)) {
        int newBeginRow = max(min(i - PADDING, allocatedBeginRow_), 0);
        int newEndRow = min(max(i + PADDING, allocatedEndRow_), logicalLength_);
        ExpandAllocated(newBeginRow, newEndRow);
    }
    (*storage_)[i - allocatedBeginRow_] = v;
    DEBUG_ONLY(CheckInvariants());
}

inline __m128 SparseVector::Get4(int i) const
{
    assert(i >= 0 && i < logicalLength_ - 3);
    if (i >= allocatedBeginRow_ && i < allocatedEndRow_ - 3) {
        return _mm_loadu_ps(&(*storage_)[i - allocatedBeginRow_]);
    } else {
        return _mm_set_ps(Get(i + 3), Get(i + 2), Get(i + 1), Get(i + 0));
    }
}

inline void SparseVector::Set4(int i, __m128 v4)
{
    assert(i >= 0 && i < logicalLength_ - 3);
    if (i >= allocatedBeginRow_ && i < allocatedEndRow_ - 3) {
        _mm_storeu_ps(&(*storage_)[i - allocatedBeginRow_], v4);
    } else {
        float vbuf[4];
        _mm_storeu_ps(vbuf, v4);
        Set(i + 0, vbuf[0]);
        Set(i + 1, vbuf[1]);
        Set(i + 2, vbuf[2]);
        Set(i + 3, vbuf[3]);
    }
}

inline void SparseVector::Clear() { std::fill(storage_->begin(), storage_->end(), LZERO); }

inline int SparseVector::AllocatedEntries() const
{
    // We want the real memory usage.  std::vector is holding some memory back
    // from us.
    return storage_->capacity();
}

inline void SparseVector::CheckInvariants() const
{
    assert(logicalLength_ >= 0);
    assert(0 <= allocatedBeginRow_ && allocatedBeginRow_ < logicalLength_);
    assert(0 <= allocatedEndRow_ && allocatedEndRow_ <= logicalLength_);
    assert(allocatedBeginRow_ <= allocatedEndRow_);
    assert((allocatedEndRow_ - allocatedBeginRow_) <= static_cast<int>(storage_->size()));
}
}
