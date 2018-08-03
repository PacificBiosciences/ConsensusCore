// Author: David Alexander, and complement array from Mark Chaisson

#include <ConsensusCore/Utils.hpp>

#include <cassert>
#include <cstdlib>

union FloatInt
{
    int i;
    float f;
};

int FloatAsInt(float A)
{
    union FloatInt u;
    u.f = A;
    return u.i;
}

// Floating point comparison method from Bruce Dawson's site
// (http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm)
bool AlmostEqual(float A, float B, int maxUlps)
{
    // Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    int aInt = FloatAsInt(A);
    // Make aInt lexicographically ordered as a twos-complement int
    if (aInt < 0) aInt = 0x80000000 - aInt;
    // Make bInt lexicographically ordered as a twos-complement int
    int bInt = FloatAsInt(B);
    if (bInt < 0) bInt = 0x80000000 - bInt;
    int intDiff = abs(aInt - bInt);
    if (intDiff <= maxUlps) return true;
    return false;
}
