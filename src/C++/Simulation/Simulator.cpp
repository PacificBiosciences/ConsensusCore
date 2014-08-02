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


#include <Simulation/Simulator.hpp>
#include <Simulation/Random.hpp>
#include <Utils.hpp>

#include <string>
#include <vector>
#include <numeric>

#include <boost/random/discrete_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

using std::vector;
using std::string;

namespace ConsensusCore
{
    // Nominal C2 parameters
    // ... reckoned from EDNA plot from job ???
    // at SNR level 10
    SequencingParameters SequencingParameters::C2() {
        SequencingParameters c2 = {
            //     T     G     A     C
            { { 0.04, 0.02, 0.02, 0.02 },     // T > Ins _
              { 0.01, 0.06, 0.01, 0.02 },     // G > Ins _
              { 0.01, 0.02, 0.08, 0.02 },     // A > Ins _
              { 0.01, 0.06, 0.02, 0.08 },     // C > Ins _
            },
              { 0.03, 0.03, 0.03, 0.03 },     // _ > Dark
              { 0.05, 0.05, 0.07, 0.11 },     // _ > Merge
              { 0.005, 0.005, 0.005, 0.005 }  // _ > Miscall
        };
        return c2;
    }

    static inline std::vector<double> ErrorProbs(const SequencingParameters& p,
                                                 int channel, bool canMerge)

    {
        vector<double> errorProbs;
        for (int e = 0; e < 4; e++)                       // Insert
        {
            errorProbs.push_back(p.Ins[channel][e]);
        }

        errorProbs.push_back(p.Dark[channel]);       // Dark
        errorProbs.push_back(p.Miscall[channel]);    // Miscall
        if (canMerge) {
            errorProbs.push_back(p.Merge[channel]);  // Merge
        }

        double sum = std::accumulate(errorProbs.begin(), errorProbs.end(), 0);
        errorProbs.push_back(1.0-sum);
        return errorProbs;
    }



    string SimulateRead(const SequencingParameters& p,
                        const std::string& tpl,
                        RandomNumberGenerator& rng)
    {
        std::string read;
        read.reserve(tpl.length() * 2);

        int pos = 0;
        while (pos < (int)tpl.length())
        {
            char base = tpl[pos];
            char prevBase = pos > 0 ? tpl[pos-1] : 'N';
            int channel = Channel(base);

            //
            // Tabulate the different possible move probabilities, then choose one
            //
            bool canMerge =  base == prevBase;
            vector<double> errorProbs = ErrorProbs(p, channel, canMerge);
            int choice = rng.RandomChoice(errorProbs);

            if (choice == (int) errorProbs.size() - 1) {  // Match
                read.push_back(base);
                pos++;

            } else if (choice < 4) {                     // Insert
                vector<double> insertProbs = vector<double>(errorProbs.begin(),
                                                            errorProbs.begin() + 4);
                int eChannel = rng.RandomChoice(insertProbs);
                char eBase = "TGAC"[eChannel];
                read.push_back(eBase);

            } else if (choice == 4) {                   // Dark
                pos++;

            } else if (choice == 5) {                   // Miscall
                read.push_back(rng.RandomBase());
                pos++;

            } else {                                   // Merge
                assert (canMerge);
                pos++;
            }
        }

        return read;
    }
}
