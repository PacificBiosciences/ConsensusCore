// Author: David Alexander

#include <boost/crc.hpp>
#include <boost/format.hpp>

#include <string>

#include <ConsensusCore/Checksum.hpp>
#include <ConsensusCore/Features.hpp>

namespace ConsensusCore {

std::string Checksum::Of(const QvSequenceFeatures& x)
{
    boost::crc_32_type summer;

    int len = x.Length();
    summer.process_bytes(x.Sequence().get(), len * sizeof(char));        // NOLINT
    summer.process_bytes(x.SequenceAsFloat.get(), len * sizeof(float));  // NOLINT
    summer.process_bytes(x.InsQv.get(), len * sizeof(float));            // NOLINT
    summer.process_bytes(x.SubsQv.get(), len * sizeof(float));           // NOLINT
    summer.process_bytes(x.DelQv.get(), len * sizeof(float));            // NOLINT
    summer.process_bytes(x.DelTag.get(), len * sizeof(float));           // NOLINT
    summer.process_bytes(x.MergeQv.get(), len * sizeof(float));          // NOLINT

    int checksum = summer.checksum();

    return (boost::format("0x%x") % checksum).str();
}
}
