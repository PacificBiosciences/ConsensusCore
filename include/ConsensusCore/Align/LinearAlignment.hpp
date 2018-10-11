// Author: David Alexander

//
// Linear-space basic alignment following Hirschberg
//

#pragma once

#include <string>

#include <ConsensusCore/Align/PairwiseAlignment.hpp>

namespace ConsensusCore {

class PairwiseAlignment;

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query,
                               AlignConfig config = AlignConfig::Default());

PairwiseAlignment* AlignLinear(const std::string& target, const std::string& query, int* score,
                               AlignConfig config = AlignConfig::Default());
}
