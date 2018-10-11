// Author: Patrick Marks

#include <pmmintrin.h>
#include <xmmintrin.h>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

#include <ConsensusCore/Edna/EdnaCounts.hpp>
#include <ConsensusCore/Edna/EdnaEvaluator.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Interval.hpp>
#include <ConsensusCore/LFloat.hpp>
#include <ConsensusCore/Matrix/SparseMatrix.hpp>
#include <ConsensusCore/Quiver/MutationScorer.hpp>
#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Quiver/SimpleRecursor.hpp>
#include <ConsensusCore/Quiver/detail/Combiner.hpp>
#include <ConsensusCore/Quiver/detail/RecursorBase.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>

using std::min;
using std::max;

#define NEG_INF -FLT_MAX

namespace ConsensusCore {
INLINE_CALLEES void EdnaCounts::DoCount(Feature<int> channelRead, EdnaEvaluator& eval,
                                        MutationScorer<SparseSseEdnaRecursor>& scorer, int j1,
                                        int j2, float* results)
{
    const SparseMatrix* alpha = scorer.Alpha();
    const SparseMatrix* beta = scorer.Beta();

    int usedBegin, usedEnd;
    boost::tie(usedBegin, usedEnd) = RangeUnion(alpha->UsedRowRange(j1), beta->UsedRowRange(j2));

    for (int k = 0; k < 5; k++)
        results[k] = NEG_INF;

    for (int i = usedBegin; i < usedEnd; i++) {
        results[0] = detail::logAdd(
            results[0], alpha->Get(i, j1) + eval.ScoreMove(j1, j2, 0) + beta->Get(i, j2));
    }

    int nRows = alpha->Rows();
    int usedCap = usedEnd < nRows - 1 ? usedEnd : nRows - 1;

    for (int i = usedBegin; i < usedCap; i++) {
        int readBase = channelRead[i];
        results[readBase] =
            detail::logAdd(results[readBase], alpha->Get(i, j1) + eval.ScoreMove(j1, j2, readBase) +
                                                  beta->Get(i + 1, j2));
    }
}
}
