// Author: David Alexander

/// \file  Combiner.hpp
/// \brief Logic for how scores are combined when two alignment paths merge.

#include <algorithm>

#include <ConsensusCore/Quiver/detail/SseMath.hpp>
#include <ConsensusCore/Utils.hpp>

#pragma once

namespace ConsensusCore {
namespace detail {

/// \brief A tag dispatch class calculating path-join score in the
/// Viterbi recursion
class ViterbiCombiner
{
public:
    static float Combine(float x, float y) { return std::max(x, y); }

    static __m128 Combine4(__m128 x4, __m128 y4) { return _mm_max_ps(x4, y4); }
};

/// \brief A tag dispatch class calculating path-join score in the
/// Sum-Product recursion
class SumProductCombiner
{
public:
    static float Combine(float x, float y) { return logAdd(x, y); }

    static __m128 Combine4(__m128 x4, __m128 y4) { return logAdd4(x4, y4); }
};
}
}
