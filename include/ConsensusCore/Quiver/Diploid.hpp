// Author: David Alexander

#pragma once

#include <utility>
#include <vector>

namespace ConsensusCore {

class Mutation;

struct DiploidSite
{
    int Allele0;
    int Allele1;
    float LogBayesFactor;
    std::vector<int> AlleleForRead;
    DiploidSite(int, int, float, std::vector<int>);
};

// NB: The prototype
//  (float* siteScores, int dim1, int dim2)
// gets transformed to just
//  (siteScores)
// for Python by SWIG.
DiploidSite* IsSiteHeterozygous(const float* siteScores, int dim1, int dim2, float logPriorRatio);
}
