// Author: Patrick Marks, David Alexander

#pragma once

#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Utils.hpp>

#include <string>

namespace ConsensusCore {

inline Mutation::Mutation() : type_(SUBSTITUTION), start_(0), end_(1), newBases_("A") {}

inline Mutation::Mutation(MutationType type, int start, int end, std::string newBases)
    : type_(type), start_(start), end_(end), newBases_(newBases)
{
    if (!CheckInvariants()) throw InvalidInputError();
}

inline Mutation::Mutation(MutationType type, int position, char base)
    : type_(type), start_(position)
{
    if (type == INSERTION) {
        end_ = position;
    } else {
        end_ = position + 1;
    }
    newBases_ = (type == DELETION ? "" : std::string(1, base));
    if (!CheckInvariants()) throw InvalidInputError();
}

inline Mutation::Mutation(const Mutation& other)
    : type_(other.type_), start_(other.start_), end_(other.end_), newBases_(other.newBases_)
{
}

inline bool Mutation::CheckInvariants() const
{
    if (!((type_ == INSERTION && (start_ == end_) && newBases_.length() > 0) ||
          (type_ == DELETION && (start_ < end_) && newBases_.length() == 0) ||
          (type_ == SUBSTITUTION && (start_ < end_) &&
           (static_cast<int>(newBases_.length()) == end_ - start_))))  // NOLINT
    {
        return false;
    }
    if (!(type_ == INSERTION || type_ == DELETION || type_ == SUBSTITUTION)) {
        return false;
    }
    return true;
}

inline bool Mutation::IsSubstitution() const { return (type_ == SUBSTITUTION); }

inline bool Mutation::IsInsertion() const { return (type_ == INSERTION); }

inline bool Mutation::IsDeletion() const { return (type_ == DELETION); }

inline int Mutation::Start() const { return start_; }

inline int Mutation::End() const { return end_; }

inline std::string Mutation::NewBases() const { return newBases_; }

inline MutationType Mutation::Type() const { return type_; }

inline int Mutation::LengthDiff() const
{
    if (IsInsertion())
        return newBases_.length();
    else if (IsDeletion())
        return start_ - end_;
    else
        return 0;
}

inline bool Mutation::operator==(const Mutation& other) const
{
    return (Start() == other.Start() && End() == other.End() && Type() == other.Type() &&
            NewBases() == other.NewBases());
}

inline bool Mutation::operator<(const Mutation& other) const
{
    if (Start() != other.Start()) {
        return Start() < other.Start();
    }
    if (End() != other.End()) {
        return End() < other.End();
    }
    if (Type() != other.Type()) {
        return Type() < other.Type();
    }
    return NewBases() < other.NewBases();
}
}
