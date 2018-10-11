// Author: David Alexander

#pragma once

#include <utility>
#include <vector>

#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
// These APIs are a little more awkward than I'd have liked---see
// "winLen" instead of winEnd.  Had to contort a bit to get SWIG
// bindings working well.

void CoverageInWindow(int tStartDim, int* tStart, int tEndDim, int* tEnd, int winStart, int winLen,
                      int* coverage);

std::vector<Interval> CoveredIntervals(int minCoverage, int tStartDim, int* tStart, int tEndDim,
                                       int* tEnd, int winStart, int winLen);
}
