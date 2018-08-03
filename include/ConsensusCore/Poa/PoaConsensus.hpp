// Author: David Alexander

#pragma once

#include <boost/utility.hpp>
#include <climits>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Align/AlignConfig.hpp>
#include <ConsensusCore/Poa/PoaGraph.hpp>

namespace ConsensusCore {
using boost::noncopyable;

class PoaGraph;
class PoaGraphPath;
class ScoredMutation;

AlignConfig DefaultPoaConfig(AlignMode mode = GLOBAL);

/// \brief A multi-sequence consensus obtained from a partial-order alignment
struct PoaConsensus : private noncopyable
{
    const std::string Sequence;
    PoaGraph Graph;
    std::vector<PoaGraph::Vertex> Path;

    PoaConsensus(const std::string& css, const PoaGraph& g,
                 const std::vector<PoaGraph::Vertex>& ConsensusPath);

    // NB: this constructor exists to provide a means to avoid an unnecessary copy
    // of the
    // boost graph.  If we had move semantics (C++11) we would be able to get by
    // without
    // this.
    PoaConsensus(const std::string& css, const detail::PoaGraphImpl& g,
                 const std::vector<PoaGraph::Vertex>& ConsensusPath);

    ~PoaConsensus();

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads);

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads,
                                             const AlignConfig& config, int minCoverage = -INT_MAX);

    static const PoaConsensus* FindConsensus(const std::vector<std::string>& reads, AlignMode mode,
                                             int minCoverage = -INT_MAX);

public:
    // Additional accessors, which do things on the graph/graphImpl
    // LikelyVariants

public:
    std::string ToGraphViz(int flags = 0) const;

    void WriteGraphVizFile(std::string filename, int flags = 0) const;
};
}
