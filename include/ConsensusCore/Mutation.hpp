// Author: Patrick Marks, David Alexander

#pragma once

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

namespace ConsensusCore {
enum MutationType
{
    INSERTION = 0,
    DELETION = 1,
    SUBSTITUTION = 2
};

/// \brief Single mutation to a template sequence.
class Mutation
{
private:
    MutationType type_;
    int start_;
    int end_;
    std::string newBases_;

    bool CheckInvariants() const;

public:
    Mutation(MutationType type, int start, int end, std::string newBases);
    Mutation(MutationType type, int position, char base);
    Mutation(const Mutation& other);

    // Note: this defines a default mutation.  This is really only needed to fix
    // SWIG compilation.
    Mutation();

    MutationType Type() const;

    bool IsSubstitution() const;
    bool IsInsertion() const;
    bool IsDeletion() const;

    /// \brief Template positions of the mutation.
    /// Convention: the bases of the template changed by the mutation are
    /// [ Start, End ).
    /// For a substitution, tpl[Start..End) are mutated;
    /// for a deletion, tpl[Start..End) are removed;
    /// for an insertion, Start=End=template position after the
    /// new bases are to be inserted.
    int Start() const;
    int End() const;

    std::string NewBases() const;
    int LengthDiff() const;
    std::string ToString() const;

public:
    bool operator==(const Mutation& other) const;
    bool operator<(const Mutation& other) const;

public:
    ScoredMutation WithScore(float score) const;
};

std::ostream& operator<<(std::ostream& out, const Mutation& m);

std::string ApplyMutation(const Mutation& mut, const std::string& tpl);
std::string ApplyMutations(const std::vector<Mutation>& muts, const std::string& tpl);

std::string MutationsToTranscript(const std::vector<Mutation>& muts, const std::string& tpl);

std::vector<int> TargetToQueryPositions(const std::vector<Mutation>& mutations,
                                        const std::string& tpl);

class ScoredMutation : public Mutation
{
private:
    float score_;

public:
    ScoredMutation();
    ScoredMutation(const Mutation& m, float score);
    float Score() const;
    std::string ToString() const;
};

std::ostream& operator<<(std::ostream& out, const ScoredMutation& m);
}

#include <ConsensusCore/Mutation-inl.hpp>
