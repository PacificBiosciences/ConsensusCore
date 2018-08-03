// Author: Lance Hepler

#pragma once

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <utility>

#include <ConsensusCore/Interval.hpp>

namespace ConsensusCore {
namespace detail {

template <typename M>
inline Interval RowRange(int j, const M& matrix, float scoreDiff)
{
    int beginRow, endRow;
    boost::tie(beginRow, endRow) = matrix.UsedRowRange(j);
    int maxRow = beginRow;
    float maxScore = matrix(maxRow, j);
    int i;

    for (i = beginRow + 1; i < endRow; i++) {
        float score = matrix(i, j);

        if (score > maxScore) {
            maxRow = i;
            maxScore = score;
        }
    }

    float thresholdScore = maxScore - scoreDiff;

    for (i = beginRow; i < maxRow && matrix(i, j) < thresholdScore; i++)
        ;
    beginRow = i;

    for (i = endRow - 1; i >= maxRow && matrix(i, j) < thresholdScore; i--)
        ;
    endRow = i + 1;

    return Interval(beginRow, endRow);
}

template <typename M, typename E, typename C>
inline bool RecursorBase<M, E, C>::RangeGuide(int j, const M& guide, const M& matrix, int* beginRow,
                                              int* endRow) const
{
    bool useGuide = !(guide.IsNull() || guide.IsColumnEmpty(j));
    bool useMatrix = !(matrix.IsNull() || matrix.IsColumnEmpty(j));

    if (!useGuide && !useMatrix) {
        return false;
    }

    float scoreDiff = bandingOptions_.ScoreDiff;
    Interval interval(*beginRow, *endRow);

    if (useGuide) {
        interval = RangeUnion(RowRange(j, guide, scoreDiff), interval);
    }

    if (useMatrix) {
        interval = RangeUnion(RowRange(j, matrix, scoreDiff), interval);
    }

    boost::tie(*beginRow, *endRow) = interval;

    return true;
}
}
}
