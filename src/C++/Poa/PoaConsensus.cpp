// Copyright (c) 2011-2013, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

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

namespace {

    using namespace ConsensusCore;

    // TODO(dalexander): these weird numbers are historical.  I think we
    // could just switch to 1, -1, -1, -1 and it would be fine.
    AlignConfig DefaultPoaConfig(AlignMode mode = GLOBAL)
    {
        AlignParams params(3, -5, -4, -4);
        AlignConfig config(params, mode);
        return config;
    }

}

namespace ConsensusCore
{
    PoaConsensus::PoaConsensus(const AlignConfig& config)
        : config_(config),
          variants_(NULL)
    {
        poaGraph_ = new PoaGraph();
    }

    PoaConsensus::~PoaConsensus()
    {
        delete poaGraph_;
        if (variants_ != NULL)
        {
            delete variants_;
        }
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads, const AlignConfig& config)
    {
        // do we need to filter zero-length reads here?
        PoaConsensus* pc = new PoaConsensus(config);
        foreach (const std::string& read, reads)
        {
            if (read.length() == 0)
            {
                throw InvalidInputError("Input sequences must have nonzero length.");
            }
            pc->poaGraph_->AddSequence(read, config);
        }
        boost::tie(pc->consensusSequence_, pc->score_, pc->variants_) =
            pc->poaGraph_->FindConsensus(config);
        return pc;
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads, AlignMode mode)
    {
        return FindConsensus(reads, DefaultPoaConfig(mode));
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads)
    {
        return PoaConsensus::FindConsensus(reads, DefaultPoaConfig());
    }

    const PoaGraph*
    PoaConsensus::Graph() const
    {
        return poaGraph_;
    }

    float
    PoaConsensus::Score() const
    {
        return score_;
    }

    std::string
    PoaConsensus::Sequence() const
    {
        return consensusSequence_;
    }

    std::string
    PoaConsensus::ToString() const
    {
        return "Poa Consensus: " + Sequence();
    }

    const std::vector<ScoredMutation>*
    PoaConsensus::Mutations() const
    {
        return variants_;
    }
}
