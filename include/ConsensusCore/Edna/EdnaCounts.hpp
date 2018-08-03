// Author: Patrick Marks

#pragma once

#include <ConsensusCore/Edna/EdnaEvaluator.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/Quiver/MutationScorer.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
class EdnaCounts
{
public:
    EdnaCounts() {}

    ~EdnaCounts() {}

    void DoCount(Feature<int> channelRead, EdnaEvaluator& eval,
                 MutationScorer<SparseSseEdnaRecursor>& scorer, int j1, int j2, float* results);
};
}
