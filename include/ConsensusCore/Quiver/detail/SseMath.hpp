// Author: David Alexander

#pragma once

#include <xmmintrin.h>
#include <limits>

#include <ConsensusCore/Quiver/detail/sse_mathfun.h>

// todo: turn these into inline functions
#define ADD4(a, b) _mm_add_ps((a), (b))

#define AFFINE4(offset, slope, dataptr) \
    (_mm_add_ps(_mm_set_ps1(offset), _mm_mul_ps(_mm_set_ps1(slope), _mm_loadu_ps((dataptr)))))

#define MUX4(mask, a, b) (_mm_or_ps(_mm_and_ps((mask), (a)), _mm_andnot_ps((mask), (b))))

#define MAX4(a, b) _mm_max_ps((a), (b))

namespace ConsensusCore {
namespace detail {
//
// Log-space arithmetic
//
static const __m128 ones = _mm_set_ps1(1.0f);

inline __m128 logAdd4(__m128 aa, __m128 bb)
{
    __m128 max = _mm_max_ps(aa, bb);
    __m128 min = _mm_min_ps(aa, bb);
    __m128 diff = _mm_sub_ps(min, max);
    return _mm_add_ps(max, log_ps(_mm_add_ps(ones, exp_ps(diff))));

    // Precision of this is currently too low -- causes problems w/ Edna
    // need a more accurate impl.
    // return logAddApprox_ps(aa, bb);
}

inline float logAdd(float a, float b)
{
    __m128 aa = _mm_set_ps1(a);
    __m128 bb = _mm_set_ps1(b);
    __m128 acc = logAdd4(aa, bb);

    // Use an aligned buf to extract
    ALIGN16_BEG float buf[4] ALIGN16_END;
    _mm_store_ps(&buf[0], acc);
    return buf[0];
}
}
}
