// Author: David Alexander

#pragma once

#include <cmath>
#include <string>
#include <vector>

#include <ConsensusCore/Align/AlignConfig.hpp>

// Utility functions common to implementations of aligners

inline int Max3(int a, int b, int c) { return std::max((a), std::max((b), (c))); }

inline int ArgMax3(int a, int b, int c)
{
    if (a >= b && a >= c)
        return 0;
    else if (b >= c)
        return 1;
    else
        return 2;
}

namespace ConsensusCore {
/// \brief A pairwise alignment
class PairwiseAlignment
{
private:
    std::string target_;
    std::string query_;
    std::string transcript_;

public:
    // target string, including gaps; usually the "reference"
    std::string Target() const;

    // query string, including gaps; usually the "read"
    std::string Query() const;

    // transcript as defined by Gusfield pg 215.
    std::string Transcript() const;

public:
    float Accuracy() const;
    int Matches() const;
    int Errors() const;
    int Mismatches() const;
    int Insertions() const;
    int Deletions() const;
    int Length() const;

public:
    PairwiseAlignment(const std::string& target, const std::string& query);

    static PairwiseAlignment* FromTranscript(const std::string& transcript,
                                             const std::string& unalnTarget,
                                             const std::string& unalnQuery);
};

PairwiseAlignment* Align(const std::string& target, const std::string& query, int* score,
                         AlignConfig config = AlignConfig::Default());

PairwiseAlignment* Align(const std::string& target, const std::string& query,
                         AlignConfig config = AlignConfig::Default());

// These calls return an array, same len as target, containing indices into the
// query string.
std::vector<int> TargetToQueryPositions(const std::string& transcript);
std::vector<int> TargetToQueryPositions(const PairwiseAlignment& aln);
}
