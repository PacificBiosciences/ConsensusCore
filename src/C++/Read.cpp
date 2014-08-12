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

#include <string>
#include <sstream>
#include <boost/format.hpp>

#include "Checksum.hpp"
#include "Features.hpp"
#include "Types.hpp"
#include "Read.hpp"

namespace ConsensusCore {

    Read::Read(QvSequenceFeatures features,
               std::string name,
               std::string chemistry)
        : Features(features),
          Name(name),
          Chemistry(chemistry)
    {}

    Read::Read(const Read& other)
        : Features(other.Features),
          Name(other.Name),
          Chemistry(other.Chemistry)
    {}

    int Read::Length() const
    {
        return Features.Length();
    }

    std::string Read::ToString() const
    {
        return (boost::format("%s (%s) Length=%d Data=%s")
                % Name % Chemistry % Length() % Checksum::Of(Features)).str();

    }

    Read Read::Null()
    {
        return Read(QvSequenceFeatures(""), "", "");
    }

    MappedRead::MappedRead(const Read& read,
                           StrandEnum strand,
                           int templateStart,
                           int templateEnd,
                           bool pinStart,
                           bool pinEnd)
        : Read(read),
          Strand(strand),
          TemplateStart(templateStart),
          TemplateEnd(templateEnd),
          PinStart(pinStart),
          PinEnd(pinEnd)
    {}

    MappedRead::MappedRead(const MappedRead& other)
        : Read(other),
          Strand(other.Strand),
          TemplateStart(other.TemplateStart),
          TemplateEnd(other.TemplateEnd),
          PinStart(other.PinStart),
          PinEnd(other.PinEnd)
    {}

    std::string MappedRead::ToString() const
    {
        std::stringstream ss;
        ss << (PinStart ? "[" : "(");
        ss << TemplateStart << "," << TemplateEnd;
        ss << (PinEnd   ? "]" : ")");
        return Read::ToString() + " @ " + ss.str();
    }

}
