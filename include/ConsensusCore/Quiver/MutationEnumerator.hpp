// Author: David Alexander, Lance Hepler

#pragma once

#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

#include <string>
#include <vector>

namespace ConsensusCore {
namespace detail {
struct AbstractMutationEnumerator
{
    explicit AbstractMutationEnumerator(const std::string& tpl);
    virtual ~AbstractMutationEnumerator();

    virtual std::vector<Mutation> Mutations() const = 0;
    virtual std::vector<Mutation> Mutations(int beginPos, int endPos) const = 0;

protected:
    const std::string tpl_;
};
}  // detail

struct AllSingleBaseMutationEnumerator : detail::AbstractMutationEnumerator
{
    explicit AllSingleBaseMutationEnumerator(const std::string& tpl);

    std::vector<Mutation> Mutations() const;
    std::vector<Mutation> Mutations(int beginPos, int endPos) const;
};

struct UniqueSingleBaseMutationEnumerator : detail::AbstractMutationEnumerator
{
    explicit UniqueSingleBaseMutationEnumerator(const std::string& tpl);

    std::vector<Mutation> Mutations() const;
    std::vector<Mutation> Mutations(int beginPos, int endPos) const;
};

struct DinucleotideRepeatMutationEnumerator : detail::AbstractMutationEnumerator
{
    DinucleotideRepeatMutationEnumerator(const std::string& tpl, int minDinucRepeatElements = 3);

    std::vector<Mutation> Mutations() const;
    std::vector<Mutation> Mutations(int beginPos, int endPos) const;

private:
    int minDinucRepeatElements_;
};

template <typename T>
std::vector<Mutation> UniqueNearbyMutations(const T& mutationEnumerator,
                                            const std::vector<Mutation>& centers,
                                            int neighborhoodSize);
}

#include <ConsensusCore/Quiver/MutationEnumerator-inl.hpp>
