// Author: David Alexander

#include <string>

#include <ConsensusCore/Quiver/QuiverConfig.hpp>

#include "ParameterSettings.hpp"

using namespace ConsensusCore;  // NOLINT

QvModelParams TestingParams(const std::string& chem)
{
    return QvModelParams(chem,    // Chemistry
                         "test",  // Model
                         0.f,     // Match,
                         -10.f,   // Mismatch,
                         -0.1f,   // MismatchS,
                         -5.f,    // Branch,
                         -0.1f,   // BranchS,
                         -6.f,    // DeletionN,
                         -7.f,    // DeletionWithTag,
                         -0.1f,   // DeletionWithTagS,
                         -8.f,    // Nce,
                         -0.1f,   // NceS,
                         -2.f,    // Merge,
                         0.f);    // MergeS
}

QuiverConfig TestingConfig(const std::string& chem)
{
    return QuiverConfig(TestingParams(chem), ALL_MOVES, BandingOptions(4, 200), -12.5);
}
