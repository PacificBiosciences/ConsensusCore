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

#include <algorithm>
#include <cfloat>
#include <map>
#include <string>
#include <vector>

#include "Quiver/MutationScorer.hpp"
#include "Quiver/MultiReadMutationScorer.hpp"
#include "Mutation.hpp"
#include "Sequence.hpp"
#include "Utils.hpp"

#define MIN_FAVORABLE_SCOREDIFF 0.04  // Chosen such that 0.49 = 1 / (1 + exp(minScoreDiff))

namespace ConsensusCore
{
    //
    // Could the mutation change the contents of the portion of the
    // template that is mapped to the read?
    //
    bool ReadScoresMutation(const MappedRead& read, const Mutation& mut)
    {
        int ts = read.TemplateStart;
        int te = read.TemplateEnd;
        int ms = mut.Start();
        int me = mut.End();
        if (mut.IsInsertion()) {
            return (ts < ms && me <= te);   // Insertion starts within?
        } else {
            return (ts < me && ms < te);    // Intervals intersect?
        }
    }

    //
    // Logic for turning a mutation to the global template space to
    // one in the coordinates understood by each individual mutation
    // scorer.  This involves translation, complementation, and also
    // possible clipping, if the mutation is not wholly within the
    // mapped read.
    //
    Mutation OrientedMutation(const MappedRead& mr,
                              const Mutation& mut)
    {
        using std::min;
        using std::max;

        // Clip mutation to bounds of mapped read, so that overhanging
        // multibase changes are handled correctly
        Mutation cmut(INSERTION, 0, 0, "N");
        if (mut.End() - mut.Start() > 1)
        {
            int cs, ce;
            cs = max(mut.Start(), mr.TemplateStart);
            ce = min(mut.End(), mr.TemplateEnd);
            if (mut.IsSubstitution())
            {
                std::string cNewBases = mut.NewBases().substr(cs-mut.Start(), ce-cs);
                cmut = Mutation(mut.Type(), cs, ce, cNewBases);
            }
            else
            {
                cmut = Mutation(mut.Type(), cs, ce, mut.NewBases());
            }
        }
        else
        {
            cmut = mut;
        }

        // Now orient
        if (mr.Strand == FORWARD_STRAND)
        {
            return Mutation(cmut.Type(),
                            cmut.Start() - mr.TemplateStart,
                            cmut.End() - mr.TemplateStart,
                            cmut.NewBases());
        }
        else
        {
            // This is tricky business
            int end   = mr.TemplateEnd - cmut.Start();
            int start = mr.TemplateEnd - cmut.End();
            return Mutation(cmut.Type(), start, end, ReverseComplement(cmut.NewBases()));
        }
    }



    template<typename R>
    MultiReadMutationScorer<R>::MultiReadMutationScorer
    (const QuiverConfigTable& quiverConfigByChemistry,
     std::string tpl)
        : quiverConfigByChemistry_(quiverConfigByChemistry),
          fwdTemplate_(tpl),
          revTemplate_(ReverseComplement(tpl)),
          readsAndScorers_()
    {
        DEBUG_ONLY(CheckInvariants());
        fastScoreThreshold_ = 0;
        QuiverConfigTable::const_iterator it;
        for (it = quiverConfigByChemistry_.begin(); it != quiverConfigByChemistry_.end(); it++)
        {
            fastScoreThreshold_ = std::min(fastScoreThreshold_, it->second.FastScoreThreshold);
        }
    }

    template<typename R>
    MultiReadMutationScorer<R>::MultiReadMutationScorer(const MultiReadMutationScorer<R>& other)
        : quiverConfigByChemistry_(other.quiverConfigByChemistry_),
          fastScoreThreshold_(other.fastScoreThreshold_),
          fwdTemplate_(other.fwdTemplate_),
          revTemplate_(other.revTemplate_),
          readsAndScorers_()
    {
        // Make a deep copy of the readsAndScorers
        foreach(const item_t& kv, other.readsAndScorers_)
        {
            MappedRead* mr = new MappedRead(*kv.first);
            MutationScorer<R>* scorer = new MutationScorer<R>(*kv.second);
            readsAndScorers_.push_back(std::make_pair(mr, scorer));
        }

        DEBUG_ONLY(CheckInvariants());
    }


    template<typename R>
    MultiReadMutationScorer<R>::~MultiReadMutationScorer()
    {
        foreach (const item_t& kv, readsAndScorers_)
        {
            delete kv.first;
            delete kv.second;
        }
    }

    template<typename R>
    int
    MultiReadMutationScorer<R>::TemplateLength() const
    {
        return fwdTemplate_.length();
    }

    template<typename R>
    int
    MultiReadMutationScorer<R>::NumReads() const
    {
        return readsAndScorers_.size();
    }

    template<typename R>
    const MappedRead*
    MultiReadMutationScorer<R>::Read(int readIdx) const
    {
        return readsAndScorers_[readIdx].first;
    }

    template<typename R>
    std::string
    MultiReadMutationScorer<R>::Template(StrandEnum strand) const
    {
        return (strand == FORWARD_STRAND ? fwdTemplate_ : revTemplate_);
    }

    template<typename R>
    std::string
    MultiReadMutationScorer<R>::Template(StrandEnum strand,
                                         int templateStart,
                                         int templateEnd) const
    {
        int len = templateEnd - templateStart;
        if (strand == FORWARD_STRAND)
        {
            return fwdTemplate_.substr(templateStart, len);
        }
        else
        {
            return revTemplate_.substr(TemplateLength() - templateEnd, len);
        }
    }

    template<typename R>
    void
    MultiReadMutationScorer<R>::ApplyMutations(const std::vector<Mutation>& mutations)
    {
        DEBUG_ONLY(CheckInvariants());
        std::vector<int> mtp = TargetToQueryPositions(mutations, fwdTemplate_);
        fwdTemplate_ = ConsensusCore::ApplyMutations(mutations, fwdTemplate_);
        revTemplate_ = ReverseComplement(fwdTemplate_);

        foreach (const item_t& kv, readsAndScorers_)
        {
            int newTemplateStart = mtp[kv.first->TemplateStart];
            int newTemplateEnd   = mtp[kv.first->TemplateEnd];
            kv.second->Template(Template(kv.first->Strand,
                                         newTemplateStart,
                                         newTemplateEnd));
            kv.first->TemplateStart = newTemplateStart;
            kv.first->TemplateEnd   = newTemplateEnd;
        }
        DEBUG_ONLY(CheckInvariants());
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::AddRead(const MappedRead& mr, float threshold)
    {
        DEBUG_ONLY(CheckInvariants());
        const QuiverConfig* config = &quiverConfigByChemistry_.at(mr.Chemistry);
        EvaluatorType ev(mr.Features,
                         Template(mr.Strand, mr.TemplateStart, mr.TemplateEnd),
                         config->QvParams);
        RecursorType recursor(config->MovesAvailable, config->Banding);

        ScorerType* scorer = new MutationScorer<R>(ev, recursor);

        if (threshold < 1.0f)
        {
            int I = ev.ReadLength();
            int J = ev.TemplateLength();
            int maxSize = static_cast<int>(0.5 + threshold * (I + 1) * (J + 1));

            if (scorer->Alpha()->AllocatedEntries() >= maxSize ||
                scorer->Beta()->AllocatedEntries() >= maxSize)
            {
                delete scorer;
                return false;
            }
        }

        readsAndScorers_.push_back(std::make_pair(new MappedRead(mr), scorer));
        DEBUG_ONLY(CheckInvariants());
        return true;
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::AddRead(const MappedRead& mr)
    {
        DEBUG_ONLY(CheckInvariants());
        const QuiverConfig* config = &quiverConfigByChemistry_.at(mr.Chemistry);
        return AddRead(mr, config->AddThreshold);
    }

    template<typename R>
    float MultiReadMutationScorer<R>::Score(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, readsAndScorers_)
        {
            if (ReadScoresMutation(*kv.first, m))
            {
                Mutation orientedMut = OrientedMutation(*kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
            }
        }
        return sum;
    }

    template<typename R>
    float MultiReadMutationScorer<R>::Score(MutationType mutationType,
                                            int position, char base) const
    {
        Mutation m(mutationType, position, base);
        return Score(m);
    }

    template<typename R>
    float MultiReadMutationScorer<R>::FastScore(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, readsAndScorers_)
        {
            if (ReadScoresMutation(*kv.first, m))
            {
                Mutation orientedMut = OrientedMutation(*kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
                if (sum < fastScoreThreshold_)
                {
                    return sum;
                }
            }
        }
        return sum;
    }

    template<typename R>
    std::vector<float>
    MultiReadMutationScorer<R>::Scores(const Mutation& m, float unscoredValue) const
    {
        std::vector<float> scoreByRead;
        foreach (const item_t& kv, readsAndScorers_)
        {
            if (ReadScoresMutation(*kv.first, m))
            {
                Mutation orientedMut = OrientedMutation(*kv.first, m);
                scoreByRead.push_back(kv.second->ScoreMutation(orientedMut) -
                                      kv.second->Score());
            }
            else
            {
                scoreByRead.push_back(unscoredValue);
            }
        }
        return scoreByRead;
    }

    template<typename R>
    std::vector<float> MultiReadMutationScorer<R>::Scores(MutationType mutationType,
                                                          int position, char base,
                                                          float unscoredValue) const
    {
        Mutation m(mutationType, position, base);
        return Scores(m, unscoredValue);
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::IsFavorable(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, readsAndScorers_)
        {
            if (ReadScoresMutation(*kv.first, m))
            {
                Mutation orientedMut = OrientedMutation(*kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
            }
        }
        return (sum > MIN_FAVORABLE_SCOREDIFF);
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::FastIsFavorable(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, readsAndScorers_)
        {
            if (ReadScoresMutation(*kv.first, m))
            {
                Mutation orientedMut = OrientedMutation(*kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
                if (sum < fastScoreThreshold_)
                {
                    return false;
                }
            }
        }
        return (sum > MIN_FAVORABLE_SCOREDIFF);
    }


    template<typename R>
    std::vector<int> MultiReadMutationScorer<R>::AllocatedMatrixEntries() const
    {
        std::vector<int> allocatedCounts;

        foreach(const item_t& kv, readsAndScorers_)
        {
            int n = 0;
            n += kv.second->Alpha()->AllocatedEntries();
            n += kv.second->Beta()->AllocatedEntries();

            allocatedCounts.push_back(n);
        }

        return allocatedCounts;
    }


    template<typename R>
    std::vector<int> MultiReadMutationScorer<R>::UsedMatrixEntries() const
    {
        std::vector<int> usedCounts;

        foreach(const item_t& kv, readsAndScorers_)
        {
            int n = 0;
            n += kv.second->Alpha()->UsedEntries();
            n += kv.second->Beta()->UsedEntries();

            usedCounts.push_back(n);
        }

        return usedCounts;
    }


    template<typename R>
    const AbstractMatrix* MultiReadMutationScorer<R>::AlphaMatrix(int i) const
    {
        ScorerType* scorer = readsAndScorers_.at(i).second;
        return scorer->Alpha();
    }


    template<typename R>
    const AbstractMatrix* MultiReadMutationScorer<R>::BetaMatrix(int i) const
    {
        ScorerType* scorer = readsAndScorers_.at(i).second;
        return scorer->Beta();
    }


    template<typename R>
    std::vector<int> MultiReadMutationScorer<R>::NumFlipFlops() const
    {
        std::vector<int> nFlipFlops;

        foreach(const item_t& kv, readsAndScorers_)
        {
            nFlipFlops.push_back(kv.second->NumFlipFlops());
        }

        return nFlipFlops;
    }


    template<typename R>
    float MultiReadMutationScorer<R>::BaselineScore() const
    {
        float sum = 0;
        foreach (const item_t& kv, readsAndScorers_)
        {
            sum += kv.second->Score();
        }
        return sum;
    }


    template<typename R>
    std::vector<float> MultiReadMutationScorer<R>::BaselineScores() const
    {
        std::vector<float> scoreByRead;
        foreach (const item_t& kv, readsAndScorers_)
        {
            scoreByRead.push_back(kv.second->Score());
        }
        return scoreByRead;
    }


    template<typename R>
    void MultiReadMutationScorer<R>::CheckInvariants() const
    {
#ifndef NDEBUG
        assert(revTemplate_ == ReverseComplement(fwdTemplate_));
        foreach (const item_t& kv, readsAndScorers_)
        {
            assert((int)kv.second->Template().length() ==
                   kv.first->TemplateEnd - kv.first->TemplateStart);
            assert(kv.second->Template() == Template(kv.first->Strand,
                                                     kv.first->TemplateStart,
                                                     kv.first->TemplateEnd));
        }
#endif  // !NDEBUG
    }


    template class MultiReadMutationScorer<SparseSseQvRecursor>;
    template class MultiReadMutationScorer<SparseSseQvSumProductRecursor>;
}
