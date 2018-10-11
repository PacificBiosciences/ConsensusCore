// Author: David Alexander

#include <ConsensusCore/Checksum.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Read.hpp>
#include <ConsensusCore/Types.hpp>

#include <boost/format.hpp>
#include <sstream>
#include <string>

namespace ConsensusCore {

Read::Read(QvSequenceFeatures features, std::string name, std::string chemistry)
    : Features(features), Name(name), Chemistry(chemistry)
{
}

Read::Read(const Read& other)
    : Features(other.Features), Name(other.Name), Chemistry(other.Chemistry)
{
}

int Read::Length() const { return Features.Length(); }

std::string Read::ToString() const
{
    return (boost::format("%s (%s) Length=%d Data=%s") % Name % Chemistry % Length() %
            Checksum::Of(Features))
        .str();
}

Read Read::Null() { return Read(QvSequenceFeatures(""), "", ""); }

MappedRead::MappedRead(const Read& read, StrandEnum strand, int templateStart, int templateEnd,
                       bool pinStart, bool pinEnd)
    : Read(read)
    , Strand(strand)
    , TemplateStart(templateStart)
    , TemplateEnd(templateEnd)
    , PinStart(pinStart)
    , PinEnd(pinEnd)
{
}

MappedRead::MappedRead(const MappedRead& other)
    : Read(other)
    , Strand(other.Strand)
    , TemplateStart(other.TemplateStart)
    , TemplateEnd(other.TemplateEnd)
    , PinStart(other.PinStart)
    , PinEnd(other.PinEnd)
{
}

std::string MappedRead::ToString() const
{
    std::stringstream ss;
    ss << (PinStart ? "[" : "(");
    ss << TemplateStart << "," << TemplateEnd;
    ss << (PinEnd ? "]" : ")");
    return Read::ToString() + " @ " + ss.str();
}
}
