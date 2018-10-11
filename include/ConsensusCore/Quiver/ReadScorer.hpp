// Author: David Alexander

#pragma once

#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Types.hpp>

#include <string>

namespace ConsensusCore {
// A ReadScorer provides a simple but inefficient means of scoring
// a single read against a template.  This is only to be used for
// testing, not in production code.

class ReadScorer
{
public:
    explicit ReadScorer(QuiverConfig& config);

    float Score(const std::string& tpl, const Read& read) const;

    const PairwiseAlignment* Align(const std::string& tpl, const Read& read) const;

    const SparseMatrix* Alpha(const std::string& tpl, const Read& read) const;

    const SparseMatrix* Beta(const std::string& tpl, const Read& read) const;

private:
    QuiverConfig _quiverConfig;
};
}
