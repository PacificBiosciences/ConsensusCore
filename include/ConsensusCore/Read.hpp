// Author: David Alexander

#pragma once

#include <string>

#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
struct Read
{
    QvSequenceFeatures Features;
    std::string Name;
    std::string Chemistry;

    Read(QvSequenceFeatures features, std::string name, std::string chemistry);

    Read(const Read& other);

    int Length() const;
    std::string ToString() const;

    static Read Null();
};

enum StrandEnum
{
    FORWARD_STRAND = 0,
    REVERSE_STRAND = 1
};

struct MappedRead : public Read
{
    StrandEnum Strand;
    int TemplateStart;
    int TemplateEnd;
    bool PinStart;
    bool PinEnd;

    MappedRead(const Read& read, StrandEnum strand, int templateStart, int templateEnd,
               bool pinStart = true, bool pinEnd = true);

    MappedRead(const MappedRead& other);

    std::string ToString() const;
};
}
