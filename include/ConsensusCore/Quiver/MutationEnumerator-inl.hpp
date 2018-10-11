// Author: David Alexander, Lance Hepler

#include <algorithm>
#include <set>
#include <vector>

namespace ConsensusCore {
///
/// Enumerate all mutations within a neighborhood of another set of
/// mutations of interest.  Note that the neighborhoods are presently
/// lopsided due to the end-exclusive definition for how we do ranges.
/// (In other words a neighborhood of size 2 includes two before but one
//   after).
template <typename T>
std::vector<Mutation> UniqueNearbyMutations(const T& mutationEnumerator,
                                            const std::vector<Mutation>& centers,
                                            int neighborhoodSize)
{
    std::set<Mutation> muts;
    foreach (const Mutation& center, centers) {
        int c = center.Start();
        int l = c - neighborhoodSize;
        // FIXME: r should probably be +1 to be symmetric
        int r = c + neighborhoodSize;
        std::vector<Mutation> mutsInRange = mutationEnumerator.Mutations(l, r);
        muts.insert(mutsInRange.begin(), mutsInRange.end());
    }
    std::vector<Mutation> result;
    std::copy(muts.begin(), muts.end(), back_inserter(result));
    return result;
}
}
