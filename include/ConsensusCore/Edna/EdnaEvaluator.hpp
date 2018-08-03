// Author: Patrick Marks

#pragma once

#include <pmmintrin.h>
#include <xmmintrin.h>

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Edna/EdnaConfig.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/LFloat.hpp>
#include <ConsensusCore/Read.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

#ifndef SWIG
using std::min;
using std::max;
#endif  // SWIG

#define NEG_INF -FLT_MAX

namespace ConsensusCore {
/// \brief An Evaluator for the Edna Edna model
class EdnaEvaluator
{
public:
    typedef ChannelSequenceFeatures FeaturesType;
    typedef EdnaModelParams ParamsType;

public:
    EdnaEvaluator(const ChannelSequenceFeatures& features, const std::string& tpl,
                  const std::vector<int> channelTpl, const EdnaModelParams& params)
        : features_(features)
        , params_(params)
        , tpl_(tpl)
        , channelTpl_(&(channelTpl[0]), tpl_.length())
        , pinStart_(true)
        , pinEnd_(true)
    {
    }

    ~EdnaEvaluator() {}

    std::string ReadName() const { return "(anonymous)"; }

    std::string Basecalls() const { return features_.Sequence(); }

    std::string Template() const { return tpl_; }

    void Template(std::string tpl) { tpl_ = tpl; }

    int ReadLength() const { return features_.Length(); }

    int TemplateLength() const { return tpl_.length(); }

    bool PinEnd() const { return pinEnd_; }

    bool PinStart() const { return pinStart_; }

    bool IsMatch(int i, int j) const
    {
        assert(0 <= i && i < ReadLength());
        assert(0 <= j && j < TemplateLength());
        return (features_.Channel[i] == channelTpl_[j]);
    }

    bool mergeable(int j) const
    {
        if (j < TemplateLength() - 1 && channelTpl_[j] == channelTpl_[j + 1]) return true;

        return false;
    }

    int templateBase(int j) const
    {
        if (j >= TemplateLength()) return 1;

        return channelTpl_[j];
    }

    float pStay(int j) const { return params_.pStay_[templateBase(j) - 1]; }

    float pMerge(int j) const
    {
        if (mergeable(j)) return params_.pMerge_[templateBase(j) - 1];

        return 0.0;
    }

    float moveDist(int obs, int j) const
    {
        int tplBase = templateBase(j) - 1;
        return params_.moveDists_[tplBase * 5 + obs];
    }

    float stayDist(int obs, int j) const
    {
        int tplBase = templateBase(j) - 1;
        return params_.stayDists_[tplBase * 5 + obs];
    }

    float Inc(int i, int j) const
    {
        assert(0 <= j && j < TemplateLength() && 0 <= i && i < ReadLength());

        float ps = pStay(j);
        float pm = (1.0f - ps) * pMerge(j);
        float trans = 1.0f - ps - pm;

        float em = moveDist(features_.Channel[i], j);
        return log(trans * em);
    }

    float Del(int i, int j) const
    {
        assert(0 <= j && j < TemplateLength() && 0 <= i && i <= ReadLength());
        if ((!PinStart() && i == 0) || (!PinEnd() && i == ReadLength())) {
            return 0.0f;
        } else {
            float ps = pStay(j);
            float pm = (1.0f - ps) * pMerge(j);
            float trans = 1.0f - ps - pm;

            float em = moveDist(0, j);
            return log(trans * em);
        }
    }

    float Extra(int i, int j) const
    {
        assert(0 <= j && j <= TemplateLength() && 0 <= i && i < ReadLength());

        float trans = pStay(j);
        float em = stayDist(features_.Channel[i], j);
        return log(trans * em);
    }

    float Merge(int i, int j) const
    {
        assert(0 <= j && j < TemplateLength() - 1 && 0 <= i && i < ReadLength());
        if (!(features_.Channel[i] == channelTpl_[j] &&
              features_.Channel[i] == channelTpl_[j + 1])) {
            return -FLT_MAX;
        } else {
            float ps = pStay(j);
            float pm = (1.0f - ps) * pMerge(j);
            return log(pm);
        }
    }

    float ScoreMove(int j1, int j2, int obs)
    {
        if (j1 == j2) {
            float trans = pStay(j1);
            float em = stayDist(obs, j1);
            return log(trans * em);
        } else if (j1 + 1 == j2) {
            float ps = pStay(j1);
            float pm = (1.0f - ps) * pMerge(j1);
            float trans = 1.0f - ps - pm;

            float em = moveDist(obs, j1);
            return log(trans * em);
        } else if (j1 + 2 == j2) {
            float ps = pStay(j1);
            float pm = (1.0f - ps) * pMerge(j1);

            if (obs == templateBase(j1))
                return log(pm);

            else
                return NEG_INF;
        }

        return NEG_INF;
    }

    float Burst(int, int, int) const
    {
        NotYetImplemented();
        return NEG_INF;
    }

    //
    // SSE
    //

    __m128 Inc4(int i, int j) const
    {
        __m128 res = _mm_setr_ps(Inc(i + 0, j), Inc(i + 1, j), Inc(i + 2, j), Inc(i + 3, j));
        return res;
    }

    __m128 Del4(int i, int j) const
    {
        __m128 res = _mm_setr_ps(Del(i + 0, j), Del(i + 1, j), Del(i + 2, j), Del(i + 3, j));
        return res;
    }

    __m128 Extra4(int i, int j) const
    {
        __m128 res =
            _mm_setr_ps(Extra(i + 0, j), Extra(i + 1, j), Extra(i + 2, j), Extra(i + 3, j));
        return res;
    }

    __m128 Merge4(int i, int j) const
    {
        __m128 res =
            _mm_setr_ps(Merge(i + 0, j), Merge(i + 1, j), Merge(i + 2, j), Merge(i + 3, j));
        return res;
    }

    __m128 Burst4(int, int, int) const
    {
        NotYetImplemented();
        return Zero4<lfloat>();
    }

protected:
    ChannelSequenceFeatures features_;
    EdnaModelParams params_;
    std::string tpl_;
    Feature<int> channelTpl_;
    bool pinStart_;
    bool pinEnd_;
};
}
