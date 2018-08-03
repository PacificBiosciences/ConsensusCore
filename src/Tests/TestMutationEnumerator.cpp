// Author: David Alexander

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <boost/assign.hpp>
#include <boost/assign/std/set.hpp>
#include <boost/range/as_array.hpp>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Quiver/MutationEnumerator.hpp>
#include <ConsensusCore/Utils.hpp>

using std::string;
using std::vector;
using std::cout;
using std::endl;

using namespace boost::assign;  // NOLINT
using namespace ConsensusCore;  // NOLINT

using ::testing::UnorderedElementsAreArray;

TEST(MutationEnumerationTest, TestAllMutations)
{
    std::string tpl = "GAATC";
    std::vector<Mutation> result = AllSingleBaseMutationEnumerator(tpl).Mutations();
    // 4 insertions, 3 substitutions, and 1 deletion per base
    EXPECT_EQ(8 * tpl.length(), result.size());
}

TEST(MutationEnumerationTest, TestUniqueMutations)
{
    std::string tpl = "GAATC";
    std::vector<Mutation> result = UniqueSingleBaseMutationEnumerator(tpl).Mutations();
    // 3 insertions, 3 substitions, and 1 deletion per base,
    // except the first (which has an extra insertion),
    // and the homopolymeric A (which is less a deletion)
    EXPECT_EQ(7 * tpl.length() + 1 - 1, result.size());
}

TEST(MutationEnumerationTest, TestUniqueNearbyMutations)
{
    std::string tpl = "GAATC";

    std::vector<Mutation> centers;
    centers.push_back(Mutation(SUBSTITUTION, 1, 'T'));

    UniqueSingleBaseMutationEnumerator enumerator(tpl);
    std::vector<Mutation> result = UniqueNearbyMutations(enumerator, centers, 1);
    // 8 mutations for the G,
    // but only 7 for the A because we don't want a repeat insertion
    EXPECT_EQ(8 + 7, result.size());

    result = UniqueNearbyMutations(enumerator, centers, 2);
    // 8 for the first, 7 for the second, 6 for the third (no homopolymeric
    // deletion)
    EXPECT_EQ(8 + 7 + 6, result.size());

    centers.push_back(Mutation(SUBSTITUTION, 3, 'G'));
    result = UniqueNearbyMutations(enumerator, centers, 2);
    std::vector<Mutation> expected = UniqueSingleBaseMutationEnumerator(tpl).Mutations();
    EXPECT_THAT(result, UnorderedElementsAreArray(expected));
}

TEST(MutationEnumerationTest, TestDinucleotideMutations)
{
    std::string tpl = "ACACACGCGCGTGTG";
    std::vector<Mutation> result = DinucleotideRepeatMutationEnumerator(tpl, 3).Mutations();
    // 4 extra mutations because of ACACAC, CGCGCG, but not GTGTG
    EXPECT_EQ(4, result.size());

    std::vector<Mutation> expected;
    expected.push_back(Mutation(INSERTION, 0, 0, std::string("AC")));
    expected.push_back(Mutation(DELETION, 0, 2, std::string("")));
    expected.push_back(Mutation(INSERTION, 5, 5, std::string("CG")));
    expected.push_back(Mutation(DELETION, 5, 7, std::string("")));
    EXPECT_THAT(result, UnorderedElementsAreArray(expected));
}
