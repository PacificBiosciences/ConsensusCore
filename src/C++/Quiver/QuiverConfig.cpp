// Author: David Alexander

#include <ConsensusCore/Quiver/QuiverConfig.hpp>

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace ConsensusCore {
QuiverConfig::QuiverConfig(const QvModelParams& qvParams, int movesAvailable,
                           const BandingOptions& bandingOptions, float fastScoreThreshold,
                           float addThreshold)
    : QvParams(qvParams)
    , MovesAvailable(movesAvailable)
    , Banding(bandingOptions)
    , FastScoreThreshold(fastScoreThreshold)
    , AddThreshold(addThreshold)
{
}

QuiverConfig::QuiverConfig(const QuiverConfig& qvConfig)
    : QvParams(qvConfig.QvParams)
    , MovesAvailable(qvConfig.MovesAvailable)
    , Banding(qvConfig.Banding)
    , FastScoreThreshold(qvConfig.FastScoreThreshold)
    , AddThreshold(qvConfig.AddThreshold)
{
}

QuiverConfigTable::QuiverConfigTable() {}

bool QuiverConfigTable::InsertAs_(const std::string& name, const QuiverConfig& config)
{
    const_iterator it;

    for (it = table.begin(); it != table.end(); it++)
        if (name.compare(it->first) == 0) return false;

    table.push_front(std::make_pair(name, config));

    return true;
}

#define FALLBACK "*"

bool QuiverConfigTable::InsertAs(const std::string& name, const QuiverConfig& config)
{
    if (name.compare(FALLBACK) == 0)
        throw InvalidInputError("Cannot Insert(...) a QuiverConfig with chemistry '" FALLBACK "'");
    return InsertAs_(name, config);
}

bool QuiverConfigTable::Insert(const QuiverConfig& config)
{
    const std::string& name = config.QvParams.ChemistryName;
    return InsertAs(name, config);
}

bool QuiverConfigTable::InsertDefault(const QuiverConfig& config)
{
    return InsertAs_(FALLBACK, config);
}

int QuiverConfigTable::Size() const { return table.size(); }

const QuiverConfig& QuiverConfigTable::At(const std::string& name) const
{
    const_iterator it;

    // If we find a direct match for the chemistry, use it
    for (it = table.begin(); it != table.end(); it++)
        if (it->first.compare(name) == 0) return it->second;

    // Fallback
    for (it = table.begin(); it != table.end(); it++)
        if (it->first.compare(FALLBACK) == 0) return it->second;

    throw InvalidInputError("Chemistry not found in QuiverConfigTable");
}

std::vector<std::string> QuiverConfigTable::Keys() const
{
    std::vector<std::string> keys;
    for (const_iterator it = table.begin(); it != table.end(); it++) {
        keys.push_back(it->first);
    }
    return keys;
}

QuiverConfigTable::const_iterator QuiverConfigTable::begin() const { return table.begin(); }

QuiverConfigTable::const_iterator QuiverConfigTable::end() const { return table.end(); }
}
