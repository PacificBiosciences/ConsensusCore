// Author: David Alexander

#include <string>

#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {

class Checksum
{
public:
    static std::string Of(const QvSequenceFeatures& f);
};
}
