// Author: David Alexander

#include <ConsensusCore/Quiver/Diploid.hpp>

#include <ConsensusCore/Mutation.hpp>

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>

using std::vector;
using std::accumulate;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

typedef boost::numeric::ublas::vector<float> fvec;
typedef boost::numeric::ublas::matrix<float> fmat;
typedef boost::numeric::ublas::matrix_column<const fmat> fmat_column;

#if defined(_MSC_VER) && _MSC_VER < 1800

// compute log(1+x) without losing precision for small values of x
float log1p(float x)
{
    if (x <= -1.0) {
        std::stringstream os;
        os << "Invalid input argument (" << x << "); must be greater than -1.0";
        throw std::invalid_argument(os.str());
    }

    if (fabs(x) > 1e-4) {
        // x is large enough that the obvious evaluation is OK
        return log(1.0 + x);
    }

    // Use Taylor approx. log(1 + x) = x - x^2/2 with error roughly x^3/3
    // Since |x| < 10^-4, |x|^3 < 10^-12, relative error less than 10^-8

    return (-0.5 * x + 1.0) * x;
}

#endif

namespace ConsensusCore {

// This needs to be configurable.
DEBUG_ONLY(const int MUTATIONS_PER_SITE = 9;)  // NOLINT
const int LENGTH_DIFFS[] = {0, 0, 0, 0, 1, 1, 1, 1, -1};

DiploidSite::DiploidSite(int allele0, int allele1, float logBayesFactor,
                         std::vector<int> alleleForRead)
    : Allele0(allele0)
    , Allele1(allele1)
    , LogBayesFactor(logBayesFactor)
    , AlleleForRead(alleleForRead)
{
}

static inline float logaddexp(float x, float y)
{
    float diff = x - y;
    if (diff > 0) {
        return x + std::log1p(std::exp(-diff));
    } else {
        return y + std::log1p(std::exp(diff));
    }
}

//
// Computes Pr(R | hom)
//
static float HomozygousLogLikelihood(const fmat& siteScores)
{
    int G = siteScores.size2();
    fvec gScores(G);
    for (int g = 0; g < G; g++) {
        gScores(g) = sum(column(siteScores, g));
    }
    return accumulate(gScores.begin(), gScores.end(), -FLT_MAX, logaddexp);
}

//
// Computes: Pr(R | het)
//
static float HeterozygousLogLikelihood(const fmat& siteScores, int* allele0, int* allele1)
{
    assert(siteScores.size2() == MUTATIONS_PER_SITE);

    int I = siteScores.size1();
    int G = siteScores.size2();
    float log2 = log(2);

    vector<float> varScores;
    float runningMax = -FLT_MAX;
    int runningAllele0 = -1;
    int runningAllele1 = -1;
    for (int g0 = 0; g0 < G; g0++) {
        for (int g1 = g0 + 1; g1 < G; g1++) {
            if (LENGTH_DIFFS[g0] == LENGTH_DIFFS[g1]) {
                float total = -I * log2;
                for (int i = 0; i < I; i++) {
                    total += logaddexp(siteScores(i, g0), siteScores(i, g1));
                }
                varScores.push_back(total);
                if (total > runningMax) {
                    runningMax = total;
                    runningAllele0 = g0;
                    runningAllele1 = g1;
                }
            }
        }
    }
    if (allele0 != NULL && allele1 != NULL) {
        *allele0 = runningAllele0;
        *allele1 = runningAllele1;
    }
    return accumulate(varScores.begin(), varScores.end(), -FLT_MAX, logaddexp);
}

static inline fmat ToMatrix(const float* siteScores, int dim1, int dim2)
{
    // Kind of kludgy.  I blame ublas--a pretty crummy matrix library.
    fmat M(dim1, dim2);
    std::copy(siteScores, siteScores + dim1 * dim2, M.data().begin());
    return M;
}

#if 0
    static void PrintMatrix(const fmat& siteScores)
    {
        for (int i = 0; i < (int)siteScores.size1(); i++)
        {
            for (int j = 0; j < (int)siteScores.size2(); j++)
            {
                cout << siteScores(i, j) << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
#endif  // 0

vector<int> AssignReadsToAlleles(const fmat& siteScores, int allele0, int allele1)
{
    int I = siteScores.size1();
    vector<int> assignment(I, -1);
    for (int i = 0; i < I; i++) {
        assignment[i] = (siteScores(i, allele0) > siteScores(i, allele1) ? 0 : 1);
    }
    return assignment;
}

// Is the site detected as a heterozygote?
//  - If not, return NULL.
//  - If so, return a pointer to a new DiploidSite object
// logPriorRatio >= 0 is log {Pr(hom)/Pr(het)}
DiploidSite* IsSiteHeterozygous(const float* siteScores, int dim1, int dim2, float logPriorRatio)
{
    // First column of siteScores must correspond to no-op mutation.
    int allele0, allele1;

    fmat M = ToMatrix(siteScores, dim1, dim2);
    float homScore = HomozygousLogLikelihood(M);
    float hetScore = HeterozygousLogLikelihood(M, &allele0, &allele1);
    float logBF = hetScore - homScore;

    if (logBF - logPriorRatio > 0) {
        return new DiploidSite(allele0, allele1, logBF, AssignReadsToAlleles(M, allele0, allele1));
    } else {
        return NULL;
    }
}
}
