// Author: David Alexander

#include <ConsensusCore/Align/AlignConfig.hpp>

namespace ConsensusCore {

AlignParams::AlignParams(int match, int mismatch, int insert, int delete_)
    : Match(match), Mismatch(mismatch), Insert(insert), Delete(delete_)
{
}

AlignParams AlignParams::Default() { return AlignParams(0, -1, -1, -1); }

AlignConfig::AlignConfig(AlignParams params, AlignMode mode) : Params(params), Mode(mode) {}

AlignConfig AlignConfig::Default() { return AlignConfig(AlignParams::Default(), GLOBAL); }
}
