// Author: David Alexander

#include <ConsensusCore/Poa/PoaConsensus.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Align/AlignConfig.hpp>
#include <ConsensusCore/Utils.hpp>

using boost::tie;

namespace ConsensusCore {
AlignConfig DefaultPoaConfig(AlignMode mode)
{
    AlignParams params(3, -5, -4, -4);
    AlignConfig config(params, mode);
    return config;
}

PoaConsensus::PoaConsensus(const std::string& css, const PoaGraph& g,
                           const std::vector<size_t>& cssPath)
    : Sequence(css), Graph(g), Path(cssPath)
{
}

PoaConsensus::PoaConsensus(const std::string& css, const detail::PoaGraphImpl& gi,
                           const std::vector<size_t>& cssPath)
    : Sequence(css), Graph(gi), Path(cssPath)
{
}

PoaConsensus::~PoaConsensus() {}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads)
{
    return FindConsensus(reads, DefaultPoaConfig(GLOBAL), -INT_MAX);
}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads,
                                                const AlignConfig& config, int minCoverage)
{
    PoaGraph pg;
    foreach (const std::string& read, reads) {
        if (read.length() == 0) {
            throw InvalidInputError("Input sequences must have nonzero length.");
        }
        pg.AddRead(read, config);
    }
    return pg.FindConsensus(config, minCoverage);
}

const PoaConsensus* PoaConsensus::FindConsensus(const std::vector<std::string>& reads,
                                                AlignMode mode, int minCoverage)
{
    return FindConsensus(reads, DefaultPoaConfig(mode), minCoverage);
}

std::string PoaConsensus::ToGraphViz(int flags) const { return Graph.ToGraphViz(flags, this); }

void PoaConsensus::WriteGraphVizFile(std::string filename, int flags) const
{
    Graph.WriteGraphVizFile(filename, flags, this);
}
}
