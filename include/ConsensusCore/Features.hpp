// Author: David Alexander

#pragma once

#include <boost/range.hpp>
#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#include <ConsensusCore/Feature.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
/// \brief An object containing observed features from a sequencing run.
struct SequenceFeatures
{
public:
    explicit SequenceFeatures(const std::string& seq);
    int Length() const { return sequence_.Length(); }
    Feature<char> Sequence() const { return sequence_; }

    /// Access to the sequence bases
    const char& operator[](int i) const { return sequence_[i]; }
    char ElementAt(int i) const { return (*this)[i]; }

private:
    Feature<char> sequence_;
};

/// \brief A features object that contains PulseToBase QV metrics
struct QvSequenceFeatures : public SequenceFeatures
{
    Feature<float> SequenceAsFloat;
    Feature<float> InsQv;
    Feature<float> SubsQv;
    Feature<float> DelQv;
    Feature<float> DelTag;
    Feature<float> MergeQv;

    explicit QvSequenceFeatures(const std::string& seq);

    QvSequenceFeatures(const std::string& seq, const float* insQv, const float* subsQv,
                       const float* delQv, const float* delTag, const float* mergeQv);

    QvSequenceFeatures(const std::string& seq, const Feature<float> insQv,
                       const Feature<float> subsQv, const Feature<float> delQv,
                       const Feature<float> delTag, const Feature<float> mergeQv);

    QvSequenceFeatures(const std::string& seq, const unsigned char* insQv,
                       const unsigned char* subsQv, const unsigned char* delQv,
                       const unsigned char* delTag, const unsigned char* mergeQv);
};

/// \brief A features object that contains sequence in channel space.
struct ChannelSequenceFeatures : SequenceFeatures
{
    Feature<int> Channel;

    explicit ChannelSequenceFeatures(const std::string& seq);

    ChannelSequenceFeatures(const std::string& seq, const std::vector<int>& channel);
};
}
