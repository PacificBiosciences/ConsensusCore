// Author: David Alexander

#pragma once

#include <list>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

namespace ConsensusCore {
enum Move
{
    INVALID_MOVE = 0x0,
    INCORPORATE = 0x1,
    EXTRA = 0x2,
    DELETE = 0x4,
    MERGE = 0x8,
    BASIC_MOVES = (INCORPORATE | EXTRA | DELETE),
    ALL_MOVES = (BASIC_MOVES | MERGE)
};

/// \brief The banding optimizations to be used by a recursor
struct BandingOptions
{
    float ScoreDiff;

    BandingOptions(int, float scoreDiff) : ScoreDiff(scoreDiff) {}

    BandingOptions(int, float scoreDiff, float, float) : ScoreDiff(scoreDiff) {}
};

/// \brief A parameter vector for analysis using the QV model
struct QvModelParams
{
    std::string ChemistryName;
    std::string ModelName;
    float Match;
    float Mismatch;
    float MismatchS;
    float Branch;
    float BranchS;
    float DeletionN;
    float DeletionWithTag;
    float DeletionWithTagS;
    float Nce;
    float NceS;
    float Merge[4];
    float MergeS[4];

    //
    // Constructor for single merge rate and merge rate slope
    //
    QvModelParams(const std::string& ChemistryName_, const std::string& ModelName_, float Match_,
                  float Mismatch_, float MismatchS_, float Branch_, float BranchS_,
                  float DeletionN_, float DeletionWithTag_, float DeletionWithTagS_, float Nce_,
                  float NceS_, float Merge_, float MergeS_)
        : ChemistryName(ChemistryName_)
        , ModelName(ModelName_)
        , Match(Match_)
        , Mismatch(Mismatch_)
        , MismatchS(MismatchS_)
        , Branch(Branch_)
        , BranchS(BranchS_)
        , DeletionN(DeletionN_)
        , DeletionWithTag(DeletionWithTag_)
        , DeletionWithTagS(DeletionWithTagS_)
        , Nce(Nce_)
        , NceS(NceS_)
    {
        for (int base = 0; base < 4; base++) {
            this->Merge[base] = Merge_;
            this->MergeS[base] = MergeS_;
        }
    }

    //
    // Constructor for per-channel merge rate and merge rate slope
    //
    QvModelParams(const std::string& ChemistryName_, const std::string& ModelName_, float Match_,
                  float Mismatch_, float MismatchS_, float Branch_, float BranchS_,
                  float DeletionN_, float DeletionWithTag_, float DeletionWithTagS_, float Nce_,
                  float NceS_, float Merge_A_, float Merge_C_, float Merge_G_, float Merge_T_,
                  float MergeS_A_, float MergeS_C_, float MergeS_G_, float MergeS_T_)
        : ChemistryName(ChemistryName_)
        , ModelName(ModelName_)
        , Match(Match_)
        , Mismatch(Mismatch_)
        , MismatchS(MismatchS_)
        , Branch(Branch_)
        , BranchS(BranchS_)
        , DeletionN(DeletionN_)
        , DeletionWithTag(DeletionWithTag_)
        , DeletionWithTagS(DeletionWithTagS_)
        , Nce(Nce_)
        , NceS(NceS_)
    {
        this->Merge[0] = Merge_A_;
        this->Merge[1] = Merge_C_;
        this->Merge[2] = Merge_G_;
        this->Merge[3] = Merge_T_;
        this->MergeS[0] = MergeS_A_;
        this->MergeS[1] = MergeS_C_;
        this->MergeS[2] = MergeS_G_;
        this->MergeS[3] = MergeS_T_;
    }

    // Access to the array-stored params

    float Merge_A() const { return this->Merge[0]; }
    float Merge_C() const { return this->Merge[1]; }
    float Merge_G() const { return this->Merge[2]; }
    float Merge_T() const { return this->Merge[3]; }

    float MergeS_A() const { return this->MergeS[0]; }
    float MergeS_C() const { return this->MergeS[1]; }
    float MergeS_G() const { return this->MergeS[2]; }
    float MergeS_T() const { return this->MergeS[3]; }
};

struct QuiverConfig
{
    QvModelParams QvParams;
    int MovesAvailable;
    BandingOptions Banding;
    float FastScoreThreshold;
    float AddThreshold;

    QuiverConfig(const QvModelParams& qvParams, int movesAvailable,
                 const BandingOptions& bandingOptions, float fastScoreThreshold,
                 float addThreshold = 1.0f);

    QuiverConfig(const QuiverConfig& qvConfig);
};

class QuiverConfigTable
{
private:
    typedef std::pair<const std::string, const QuiverConfig> QuiverConfigTableEntry;

private:
    std::list<QuiverConfigTableEntry> table;
    bool InsertAs_(const std::string& name, const QuiverConfig& config);

public:
    typedef std::list<QuiverConfigTableEntry>::const_iterator const_iterator;

    QuiverConfigTable();

    // Insert as the default config (when a read's chemistry is not found.
    bool InsertDefault(const QuiverConfig& config);

    // Insert, using the chemistry found in the config.
    bool Insert(const QuiverConfig& config);

    // Insert, aliasing as a different chemistry name.  This is
    // important, for example, when a read presents itself as
    // "XL-C2" but we have no trained models for "XL-C2", so we
    // want to have At("XL-C2") fetch the config for a similar
    // chemistry.
    bool InsertAs(const std::string& name, const QuiverConfig& config);

    int Size() const;

    const QuiverConfig& At(const std::string& name) const;

    std::vector<std::string> Keys() const;

#ifndef SWIG
    const_iterator begin() const;
    const_iterator end() const;
#endif
};
}
