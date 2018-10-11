// Author: David Alexander, Lance Hepler

#pragma once

#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Quiver/MultiReadMutationScorer.hpp>

#include <vector>

namespace ConsensusCore {
struct RefineOptions
{
    int MaximumIterations;
    int MutationSeparation;
    int MutationNeighborhood;
};

static const RefineOptions DefaultRefineOptions = {
    40,  // MaximumIterations
    10,  // MutationSeparation
    20   // MutationNeighborhood
};

bool RefineConsensus(AbstractMultiReadMutationScorer& mms,
                     const RefineOptions& = DefaultRefineOptions);

void RefineDinucleotideRepeats(AbstractMultiReadMutationScorer& mms,
                               int minDinucleotideRepeatElements = 3);

std::vector<int> ConsensusQVs(AbstractMultiReadMutationScorer& mms);

//
// Lower priority:
//
// Matrix<float> MutationScoresMatrix(mms);
// Matrix<float> MutationScoresMatrix(mms, mutationsToScore);
}
