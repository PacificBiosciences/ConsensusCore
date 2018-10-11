// Author: David Alexander

#include <ConsensusCore/Statistics/Binomial.hpp>

#include <boost/math/distributions/binomial.hpp>
#include <cmath>

using boost::math::binomial_distribution;
using boost::math::complement;
using boost::math::cdf;

namespace ConsensusCore {
double BinomialSurvival(int q, int size, double prob, bool asPhred)
{
    binomial_distribution<double> dist(size, prob);
    double tail = q < 0 ? 1 : cdf(complement(dist, q));
    return asPhred ? -10. * log10(tail) : tail;
}
}
