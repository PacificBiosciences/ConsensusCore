// Author: David Alexander

#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>

#pragma once

#define API_MAJOR 1
#define API_MINOR 0
#define API_PATCH 2

namespace ConsensusCore {
class Version
{
public:
    static int Major();
    static int Minor();
    static int Patch();

    // Sadly SWIG doesn't support boost::tuple
    static std::vector<int> VersionTuple();

    static std::string VersionString();
};
}
