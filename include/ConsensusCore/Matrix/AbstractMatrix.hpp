// Author: Lance Hepler

#pragma once

namespace ConsensusCore {

class AbstractMatrix
{
protected:
    AbstractMatrix() {}
    virtual ~AbstractMatrix() {}

public:  // Size information
    virtual int Rows() const = 0;
    virtual int Columns() const = 0;

public:  // Information about entries filled by column
    virtual int UsedEntries() const = 0;
    virtual int AllocatedEntries() const = 0;

public:  // Accessors
    virtual bool IsAllocated(int i, int j) const = 0;
    virtual float Get(int i, int j) const = 0;

public:
    // Method SWIG clients can use to get a native matrix (e.g. Numpy)
    // mat must be filled as a ROW major matrix
    virtual void ToHostMatrix(float** mat, int* rows, int* cols) const = 0;
};
}
