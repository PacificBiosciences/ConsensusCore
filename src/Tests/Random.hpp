// Author: David Alexander

#pragma once

#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Quiver/QvEvaluator.hpp>
#include <ConsensusCore/Read.hpp>
#include <ConsensusCore/Utils.hpp>

#include "ParameterSettings.hpp"

using ConsensusCore::Mutation;
using ConsensusCore::MutationType;
using ConsensusCore::QvEvaluator;
using ConsensusCore::QvModelParams;
using ConsensusCore::QvSequenceFeatures;
using ConsensusCore::Read;

template <typename RNG>
std::string RandomSequence(RNG& rng, int length)
{
    const char* bases = "ACGT";
    boost::random::uniform_int_distribution<> indexDist(0, 3);
    std::stringstream ss;
    for (int i = 0; i < length; ++i) {
        ss << bases[indexDist(rng)];
    }
    return ss.str();
}

template <typename RNG>
float* RandomQvArray(RNG& rng, int length)
{
    float* array = new float[length];
    boost::random::uniform_int_distribution<> indexDist(0, 127);
    for (int i = 0; i < length; ++i) {
        array[i] = static_cast<float>(indexDist(rng));
    }
    return array;
}

template <typename RNG>
float* RandomTagArray(RNG& rng, int length)
{
    std::string seq = RandomSequence(rng, length);
    float* array = new float[length];
    for (int i = 0; i < length; ++i) {
        array[i] = static_cast<float>(seq[i]);
    }
    return array;
}

template <typename RNG>
int RandomPoissonDraw(RNG& rng, int mean)
{
    boost::random::poisson_distribution<> dist(mean);
    return dist(rng);
}

template <typename RNG>
bool RandomBernoulliDraw(RNG& rng, float p)
{
    boost::random::bernoulli_distribution<> dist(static_cast<double>(p));
    return dist(rng);
}

template <typename RNG>
QvEvaluator RandomQvEvaluator(RNG& rng, int length)
{
    std::string tpl = RandomSequence(rng, length);

    int readLength = RandomPoissonDraw(rng, length);
    std::string seq = RandomSequence(rng, readLength);

    float* insQv = RandomQvArray(rng, readLength);
    float* subsQv = RandomQvArray(rng, readLength);
    float* delQv = RandomQvArray(rng, readLength);
    float* delTag = RandomTagArray(rng, readLength);
    float* mergeQv = RandomQvArray(rng, readLength);

    QvSequenceFeatures f(seq, insQv, subsQv, delQv, delTag, mergeQv);
    Read read(f, "anonymous", "unknown");

    delete[] insQv;
    delete[] subsQv;
    delete[] delQv;
    delete[] delTag;
    delete[] mergeQv;

    bool pinStart = RandomBernoulliDraw(rng, 0.5);
    bool pinEnd = RandomBernoulliDraw(rng, 0.5);
    return QvEvaluator(read, tpl, TestingParams(), pinStart, pinEnd);
}

template <typename RNG>
std::vector<int> RandomSampleWithoutReplacement(RNG& rng, int n, int k)
{
    // Random sample of k elements from [0..n) without replacement
    std::vector<int> draws;
    boost::random::uniform_int_distribution<> indexDist(0, n - 1);
    while (static_cast<int>(draws.size()) < k) {
        int draw = indexDist(rng);
        if (std::find(draws.begin(), draws.end(), draw) == draws.end()) {
            draws.push_back(draw);
        }
    }
    return draws;
}

typedef boost::mt19937 Rng;
