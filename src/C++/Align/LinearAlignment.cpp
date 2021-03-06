// Author: David Alexander

//
// This is the basic Hirschberg algo, not the affine variation by Myers and
// Miller.
// The tricky part of this code is that we use the a single array to store
// portions
// of two rows in the alignment matrices.
//
// Straightforward notes here: http://globin.cse.psu.edu/courses/fall2001/DP.pdf
// ;
// taken from "Recent Developments in Linear-Space Alignment Methods: A Survey".
// I follow them pretty closely except for the semiglobal alignment mode
// implemented
// here.
//
// For the affine (Gotoh) variation, not yet implemented, see Myers & Miller
// 1988
//

#include <ConsensusCore/Align/LinearAlignment.hpp>
#include <ConsensusCore/Align/PairwiseAlignment.hpp>
#include <ConsensusCore/Utils.hpp>

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using ConsensusCore::PairwiseAlignment;
using ConsensusCore::AlignParams;
using ConsensusCore::AlignConfig;
using ConsensusCore::Align;
using ConsensusCore::GLOBAL;
using ConsensusCore::SEMIGLOBAL;

//#define DEBUG_LINEAR_ALIGNMENT

#ifdef DEBUG_LINEAR_ALIGNMENT
#include <iostream>
using std::cout;
using std::endl;
#endif  // DEBUG_LINEAR_ALIGNMENT

namespace {

using ConsensusCore::NotYetImplementedException;

namespace ublas = boost::numeric::ublas;

int INSERT_SCORE = -2;
int DELETE_SCORE = -2;
int MISMATCH_SCORE = -1;
int MATCH_SCORE = +2;

const AlignParams params(MATCH_SCORE, MISMATCH_SCORE, INSERT_SCORE, DELETE_SCORE);
const AlignConfig config(params, GLOBAL);

//
// Get transcript of NW alignment taking
//   target[j1..j2] into query[i1..i2] (one-based indexing)
// used for trivial base cases.
//
std::string NWTranscript(const std::string& target, int j1, int j2, const std::string& query,
                         int i1, int i2, int* score)
{
    assert((i1 <= i2) && (j1 <= j2));
    // implement this inline later
    std::string T = target.substr(j1 - 1, j2 - j1 + 1);
    std::string Q = query.substr(i1 - 1, i2 - i1 + 1);
    PairwiseAlignment* aln = Align(T, Q, score, config);
    std::string X = aln->Transcript();
    delete aln;
    return X;
}

#ifndef NDEBUG
bool CheckTranscript(const std::string& transcript, const std::string& unalnTarget,
                     const std::string& unalnQuery)
{
    PairwiseAlignment* pa = PairwiseAlignment::FromTranscript(transcript, unalnTarget, unalnQuery);
    if (pa == NULL) {
        return false;
    } else {
        delete pa;
        return true;
    }
}
#endif  // NDEBUG

//
// Hirschberg recursion:
// Find optimal transcript taking target[j1..j2] into query[i1..i2] (one-based
// indices)
// Operates by divide-and-conquer, finding midpoint (m, j*) and recursing on
// halves, then joining.
// Notes:
//
//    | Alignment  | L                | L_1               | L_2 |
//    | Path       | (0,0) ~> (I,J)   | (0,0) ~> (m, j*)  | (m, j*) ~> (I, J) |
//    | T, Q       | T[1..J], Q[1..I] | T[1..j*], Q[1..m] | T[j*+1..J],
//    Q[m+1..I] |
//    | Transcript | X                | X_1               | X_2 |
//
// target on horizontal, query on vertical
// i refers to query; j refers to target
// this gives better balanced recursion in the (common) semiglobal case
//
std::string OptimalTranscript(const std::string& target, int j1, int j2, const std::string& query,
                              int i1, int i2, ublas::vector<int>& buf1, ublas::vector<int>& buf2,
                              int* score = NULL)
{
    DEBUG_ONLY(std::string subtarget = target.substr(j1 - 1, j2 - j1 + 1);
               std::string subquery = query.substr(i1 - 1, i2 - i1 + 1);)

#ifdef DEBUG_LINEAR_ALIGNMENT
    cout << "Called OptimalTranscript("
         << "T[" << j1 << ".." << j2 << "], "
         << "Q[" << i1 << ".." << i2 << "]) "
         << " " << target << " " << query << " "
         << "(" << subtarget << ", " << subquery << ")" << endl;
#endif

    std::string x, x1, x2;
    int segmentScore;
    const AlignParams& configParams = config.Params;

    //
    // Base case
    //
    if ((j2 - j1 <= 1) || (i2 - i1 <= 1)) {
        x = NWTranscript(target, j1, j2, query, i1, i2, &segmentScore);
    }

    //
    // Recursive case
    //
    else {
        assert(buf1.size() == target.size() + 1);
        assert(buf2.size() == target.size() + 1);

        ublas::vector<int>& Sm = buf1;  // S-
        ublas::vector<int>& Sp = buf2;  // S+

        int mid = (i1 + i2) / 2;

        //
        // Score forward, i1 upto mid
        // ( T[j1..j2] vs Q[i1..m] )
        //
        Sm(j1 - 1) = 0;
        for (int j = j1; j <= j2; j++) {
            Sm(j) = Sm(j - 1) + configParams.Delete;
        }
        for (int i = i1; i <= mid; i++) {
            int s, c;
            s = Sm(j1 - 1);
            c = Sm(j1 - 1) + configParams.Insert;
            Sm(j1 - 1) = c;
            for (int j = j1; j <= j2; j++) {
                char t = target[j - 1];
                char q = query[i - 1];
                c = Max3(Sm(j) + configParams.Insert,
                         s + (t == q ? configParams.Match : configParams.Mismatch),
                         c + configParams.Delete);
                s = Sm(j);
                Sm(j) = c;
            }
        }

        //
        // Score backwards, i2 downto mid
        // ( T[j1..j2] vs Q[m+1..i2] )
        //
        Sp(j2) = 0;
        for (int j = j2 - 1; j >= j1 - 1; j--) {
            Sp(j) = Sp(j + 1) + configParams.Delete;
        }
        for (int i = i2 - 1; i >= mid; i--) {
            int s, c;
            s = Sp(j2);
            c = Sp(j2) + configParams.Delete;
            Sp(j2) = c;
            for (int j = j2 - 1; j >= j1 - 1; j--) {
                char t = target[j];  // j + 1 - 1
                char q = query[i];   // i + 1 - 1
                c = Max3(Sp(j) + configParams.Insert,
                         s + (t == q ? configParams.Match : configParams.Mismatch),
                         c + configParams.Delete);
                s = Sp(j);
                Sp(j) = c;
            }
        }

        //
        // Find where optimal path crosses the mid row
        //
        ublas::vector<int> sum = Sm + Sp;
        int j = std::max_element(sum.begin() + j1, sum.begin() + j2 + 1) - sum.begin();
        segmentScore = sum[j];

        int segment1Score, segment2Score;
        x1 = OptimalTranscript(target, j1, j, query, i1, mid, buf1, buf2, &segment1Score);
        x2 = OptimalTranscript(target, j + 1, j2, query, mid + 1, i2, buf1, buf2, &segment2Score);
        assert(segmentScore == segment1Score + segment2Score);
        x = x1 + x2;
    }

    // Check 1: transcript has to take target[j1..j2] into query[i1..i2]
    assert(CheckTranscript(x, subtarget, subquery));

    // Check 2: same score as basic N/W?
    DEBUG_ONLY(int peerScore;
               PairwiseAlignment* peerAlignment = Align(subtarget, subquery, &peerScore, config);
               assert(peerScore == segmentScore); delete peerAlignment;)

    if (score != NULL) {
        *score = segmentScore;
    }
    return x;
}
}

PairwiseAlignment* ConsensusCore::AlignLinear(const std::string& target, const std::string& query,
                                              int* score, AlignConfig)
{
    int J = target.length();
    ublas::vector<int> buf1(J + 1), buf2(J + 1);
    std::string x =
        OptimalTranscript(target, 1, target.length(), query, 1, query.length(), buf1, buf2, score);
    return PairwiseAlignment::FromTranscript(x, target, query);
}

PairwiseAlignment* ConsensusCore::AlignLinear(const std::string& target, const std::string& query,
                                              AlignConfig config_)
{
    return AlignLinear(target, query, NULL, config_);
}
