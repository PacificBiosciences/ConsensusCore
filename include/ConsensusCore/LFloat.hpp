// Author: David Alexander

#pragma once
#include <xmmintrin.h>
#include <cfloat>
#include <ostream>

namespace ConsensusCore {

/// \brief A class representing a floating point number on the logarithmic scale
struct lfloat
{
    float value;

    lfloat() { value = -FLT_MAX; }

    lfloat(float f)  // NOLINT(runtime/explicit)
    {
        value = f;
    }

    lfloat& operator=(float f)
    {
        value = f;
        return *this;
    }

    operator const float&() const { return value; }
    operator float&() { return value; }

    friend std::ostream& operator<<(std::ostream& out, const lfloat& f)
    {
        out << f.value;
        return out;
    }
};

template <typename T>
float Zero()
{
    return T();
}
template <typename T>
__m128 Zero4()
{
    return _mm_set_ps1(T());
}
}
