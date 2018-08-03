// Author: David Alexander

#pragma once

namespace ConsensusCore {
// Returns P[X > q], where X ~ Binom(size, prob);
// i.e. the same value as
//    pbinom(q, size, prob, lower.tail=F)
// in R.   If `asPhred` is true, the probability is converted
// to the "phred" quality scale via Q=-10*log10(p)
double BinomialSurvival(int q, int size, double prob, bool asPhred = false);
}
