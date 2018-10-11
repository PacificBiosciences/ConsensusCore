// Author: David Alexander

#include <gtest/gtest.h>

#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <ConsensusCore/Matrix/SparseVector.hpp>

using namespace ConsensusCore;  // NOLINT

TEST(SparseVectorTest, BasicTest)
{
    SparseVector sv(100, 10, 20);
    EXPECT_LE(10, sv.AllocatedEntries());

    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(sv(i), -FLT_MAX);
    }

    for (int i = 10; i < 20; i++) {
        sv.Set(i, i);
    }
    for (int i = 0; i < 100; i++) {
        if (i >= 10 && i < 20)
            EXPECT_EQ(i, sv(i));
        else
            EXPECT_EQ(-FLT_MAX, sv(i));  // NOLINT
    }

    sv.Set(50, 50);
    EXPECT_LE(40, sv.AllocatedEntries());
    for (int i = 0; i < 100; i++) {
        if (i >= 10 && i < 20)
            EXPECT_EQ(i, sv(i));
        else if (i == 50)
            EXPECT_EQ(i, sv(i));
        else
            EXPECT_EQ(-FLT_MAX, sv(i));  // NOLINT
    }
}

TEST(SparseVectorTest, BasicTest2)
{
    SparseVector sv(100, 50, 60);

    sv.Set(5, 5);
    for (int i = 0; i < 100; i++) {
        if (i == 5)
            EXPECT_EQ(i, sv(i));
        else
            EXPECT_EQ(-FLT_MAX, sv(i));  // NOLINT
    }
}

TEST(SparseVector, CopyTest)
{
    SparseVector sv(10, 3, 7);
    sv.Set(4, 5);

    SparseVector svCopy(sv);
    ASSERT_EQ(5, svCopy(4));

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(sv(i), svCopy(i));
    }
}
