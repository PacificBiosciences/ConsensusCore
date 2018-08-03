// Author: David Alexander

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ConsensusCore/Coverage.hpp>
#include <ConsensusCore/Interval.hpp>

using namespace ConsensusCore;  // NOLINT
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

#define t(a, b) (Interval((a), (b)))

TEST(CoverageTests, CoverageInWindowTest)
{
    int coverage[10];
    int tStart[] = {1, 2, 3, 8, 10, 15};
    int tEnd[] = {3, 4, 5, 10, 10, 200};

    CoverageInWindow(6, tStart, 6, tEnd, 0, 10, coverage);
    int expectedCoverage1[] = {0, 1, 2, 2, 1, 0, 0, 0, 1, 1};
    ASSERT_THAT(coverage, ElementsAreArray(expectedCoverage1, 10));

    CoverageInWindow(6, tStart, 6, tEnd, 10, 10, coverage);
    int expectedCoverage2[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
    ASSERT_THAT(coverage, ElementsAreArray(expectedCoverage2, 10));
}

TEST(CoverageTests, CoveredIntervalsTest)
{
    // CHECK:
    // CoveredIntervals(0, [1,2,3,8,900,2000], [3,4,5,10,1010,20000], 0, 10000) ->
    // ((0, 10000),)
    // CoveredIntervals(1, [1,2,3,8,900,2000], [3,4,5,10,1010,20000], 0, 10000) ->
    // ((1, 5), (8, 10), (900, 1010), (2000, 10000))  // NOLINT
    // CoveredIntervals(2, [1,2,3,8,900,2000], [3,4,5,10,1010,20000], 0, 10000) ->
    // ((2, 4),)
    // CoveredIntervals(3, [1,2,3,8,900,2000], [3,4,5,10,1010,20000], 0, 10000) ->
    // ()

    int tStart[] = {1, 2, 3, 8, 900, 2000};
    int tEnd[] = {3, 4, 5, 10, 1010, 20000};
    ASSERT_THAT(CoveredIntervals(0, 6, tStart, 6, tEnd, 0, 10000), ElementsAre(t(0, 10000)));
    ASSERT_THAT(CoveredIntervals(1, 6, tStart, 6, tEnd, 0, 10000),
                ElementsAre(t(1, 5), t(8, 10), t(900, 1010), t(2000, 10000)));  // NOLINT
    ASSERT_THAT(CoveredIntervals(2, 6, tStart, 6, tEnd, 0, 10000), ElementsAre(t(2, 4)));
    ASSERT_THAT(CoveredIntervals(3, 6, tStart, 6, tEnd, 0, 10000), ElementsAre());

    ASSERT_THAT(CoveredIntervals(0, 6, tStart, 6, tEnd, 100, 9900), ElementsAre(t(100, 10000)));
    ASSERT_THAT(CoveredIntervals(1, 6, tStart, 6, tEnd, 100, 9900),
                ElementsAre(t(900, 1010), t(2000, 10000)));  // NOLINT
}

TEST(CoverageTests, CoveredIntervalsTest2)
{
    // Regression test
    int tStart[] = {48853};
    int tEnd[] = {50687};
    ASSERT_THAT(CoveredIntervals(1, 1, tStart, 1, tEnd, 50000, 500), ElementsAre(t(50000, 50500)));
}
