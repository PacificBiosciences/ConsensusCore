// Author: David Alexander

#include <ConsensusCore/Features.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include <ConsensusCore/Feature.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

ConsensusCore::SequenceFeatures::SequenceFeatures(const std::string& seq)
    : sequence_(seq.c_str(), seq.length())
{
}

namespace {
void CheckTagFeature(ConsensusCore::Feature<float> feature)
{
    foreach (const float& tag, feature) {
        if (!(tag == 'A' || tag == 'C' || tag == 'G' || tag == 'T' || tag == 'N' || tag == 0)) {
            throw ConsensusCore::InternalError("Invalid DelTag field---must be ASCII encoded.");
        }
    }
}
}

namespace ConsensusCore {
QvSequenceFeatures::QvSequenceFeatures(const std::string& seq)
    : SequenceFeatures(seq)
    , SequenceAsFloat(Length())
    , InsQv(Length())
    , SubsQv(Length())
    , DelQv(Length())
    , DelTag(Length())
    , MergeQv(Length())
{
    for (int i = 0; i < Length(); i++) {
        SequenceAsFloat[i] = static_cast<float>(seq[i]);
    }
}

QvSequenceFeatures::QvSequenceFeatures(const std::string& seq, const float* insQv,
                                       const float* subsQv, const float* delQv, const float* delTag,
                                       const float* mergeQv)
    : SequenceFeatures(seq)
    , SequenceAsFloat(Length())
    , InsQv(insQv, Length())
    , SubsQv(subsQv, Length())
    , DelQv(delQv, Length())
    , DelTag(delTag, Length())
    , MergeQv(mergeQv, Length())
{
    for (int i = 0; i < Length(); i++) {
        SequenceAsFloat[i] = static_cast<float>(seq[i]);
    }
    CheckTagFeature(DelTag);
}

QvSequenceFeatures::QvSequenceFeatures(const std::string& seq, const unsigned char* insQv,
                                       const unsigned char* subsQv, const unsigned char* delQv,
                                       const unsigned char* delTag, const unsigned char* mergeQv)
    : SequenceFeatures(seq)
    , SequenceAsFloat(Length())
    , InsQv(insQv, Length())
    , SubsQv(subsQv, Length())
    , DelQv(delQv, Length())
    , DelTag(delTag, Length())
    , MergeQv(mergeQv, Length())
{
    for (int i = 0; i < Length(); i++) {
        SequenceAsFloat[i] = static_cast<float>(seq[i]);
    }
    CheckTagFeature(DelTag);
}

QvSequenceFeatures::QvSequenceFeatures(const std::string& seq, const Feature<float> insQv,
                                       const Feature<float> subsQv, const Feature<float> delQv,
                                       const Feature<float> delTag, const Feature<float> mergeQv)
    : SequenceFeatures(seq)
    , SequenceAsFloat(Length())
    , InsQv(insQv)
    , SubsQv(subsQv)
    , DelQv(delQv)
    , DelTag(delTag)
    , MergeQv(mergeQv)
{
    for (int i = 0; i < Length(); i++) {
        SequenceAsFloat[i] = static_cast<float>(seq[i]);
    }
    CheckTagFeature(DelTag);
}

ChannelSequenceFeatures::ChannelSequenceFeatures(const std::string& seq)
    : SequenceFeatures(seq), Channel(Length())
{
}

ChannelSequenceFeatures::ChannelSequenceFeatures(const std::string& seq,
                                                 const std::vector<int>& channel)
    : SequenceFeatures(seq), Channel(&(channel[0]), Length())
{
}
}
