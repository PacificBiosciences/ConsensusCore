// Author: David Alexander

#pragma once

#include <vector>

namespace ConsensusCore {
/// \brief A parameter vector for analysis using the QV model
struct EdnaModelParams
{
    std::vector<float> pStay_;
    std::vector<float> pMerge_;
    std::vector<float> moveDists_;
    std::vector<float> stayDists_;

    // By providing a single constructor with default arguments,
    // we allow Python and other SWIG client languages to use
    // named parameters. However if the user forgets to set one of
    // the parameters, trouble!

    EdnaModelParams(std::vector<float> pStay, std::vector<float> pMerge,
                    std::vector<float> moveDists, std::vector<float> stayDists)
        : pStay_(pStay), pMerge_(pMerge), moveDists_(moveDists), stayDists_(stayDists)
    {
    }
};
}
