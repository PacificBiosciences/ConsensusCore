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

#pragma once

#include <Sequence.hpp>
#include <Utils.hpp>

#include <string>

using std::string;


namespace ConsensusCore {

    class RandomNumberGenerator;

    // SequencingParams represents the Markov error probabilities
    // We use the channel codes to agree with order shown in EDNA:
    //     T = 0;
    //     G = 1;
    //     A = 2;
    //     C = 3
    struct SequencingParameters
    {
        double Ins[4][4];   // Insertion error [tplBase][emittedBase]
        double Dark[4];     // Spontaneous deletion [tplBase]
        double Merge[4];    // Pulse merge [tplBase], (conditional on HP context)
        double Miscall[4];  // Miscall

        static SequencingParameters C2();
    };


    inline int Channel(char base)
    {
        switch (base) {
            case 'T': return 0;
            case 'G': return 1;
            case 'A': return 2;
            case 'C': return 3;
            default: ShouldNotReachHere();
        }
    }

    string SimulateRead(const SequencingParameters& p,
                        const std::string& tpl,
                        RandomNumberGenerator& rng);
}
