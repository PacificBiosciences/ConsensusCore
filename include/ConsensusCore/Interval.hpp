// Author: David Alexander

#pragma once

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <climits>
#include <utility>
#include <vector>

#include <ConsensusCore/Utils.hpp>

namespace ConsensusCore {

struct Interval
{
    int Begin;
    int End;

    Interval() : Begin(0), End(0) {}
    Interval(const int& b, const int& e) : Begin(b), End(e) {}

    bool operator==(const Interval& other) const
    {
        return (Begin == other.Begin && End == other.End);
    }

#ifndef SWIG
    operator boost::tuple<int, int>() { return boost::make_tuple(Begin, End); }

    operator boost::tuple<int&, int&>()
    {
        return boost::make_tuple(boost::ref(Begin), boost::ref(End));
    }
#endif
};

inline Interval RangeUnion(const Interval& range1, const Interval& range2)
{
    return Interval(std::min(range1.Begin, range2.Begin), std::max(range1.End, range2.End));
}

inline Interval RangeUnion(const Interval& range1, const Interval& range2, const Interval& range3)
{
    return RangeUnion(range1, RangeUnion(range2, range3));
}

inline Interval RangeUnion(const Interval& range1, const Interval& range2, const Interval& range3,
                           const Interval& range4)
{
    return RangeUnion(RangeUnion(range1, range2), RangeUnion(range3, range4));
}

inline Interval RangeUnion(const std::vector<Interval> ranges)
{
    Interval result = Interval(INT_MAX / 2, -INT_MAX / 2);
    foreach (const Interval& r, ranges) {
        result = RangeUnion(result, r);
    }
    return result;
}
}
