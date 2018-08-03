// Author: David Alexander

#include <ConsensusCore/Version.hpp>

#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <string>
#include <vector>

namespace ConsensusCore {
int Version::Major() { return API_MAJOR; }

int Version::Minor() { return API_MINOR; }

int Version::Patch() { return API_PATCH; }

std::vector<int> Version::VersionTuple()
{
    int version[3] = {API_MAJOR, API_MINOR, API_PATCH};
    return std::vector<int>(version, version + 3);
}

std::string Version::VersionString()
{
    using boost::format;
    using boost::str;

    std::string version = str(format("%d.%d.%d") % API_MAJOR % API_MINOR % API_PATCH);
#ifndef NDEBUG
    version += "-DEBUG";
#endif  // !NDEBUG
    return version;
}
}
