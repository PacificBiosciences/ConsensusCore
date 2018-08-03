// Author: David Alexander

#pragma once

#include <ConsensusCore/Matrix/DenseMatrix.hpp>
#include <ConsensusCore/Quiver/QvEvaluator.hpp>
#include <ConsensusCore/Quiver/detail/Combiner.hpp>
#include <ConsensusCore/Quiver/detail/RecursorBase.hpp>

namespace ConsensusCore {

template <typename M, typename E, typename C>
class SimpleRecursor : public detail::RecursorBase<M, E, C>
{
public:
    void FillAlpha(const E& e, const M& guide, M& alpha) const;
    void FillBeta(const E& e, const M& guide, M& beta) const;

    float LinkAlphaBeta(const E& e, const M& alpha, int alphaColumn, const M& beta, int betaColumn,
                        int absoluteColumn) const;

    void ExtendAlpha(const E& e, const M& alpha, int beginColumn, M& ext,
                     int numExtColumns = 2) const;

    void ExtendBeta(const E& e, const M& beta, int endColumn, M& ext, int numExtColumns = 2,
                    int lengthDiff = 0) const;

public:
    //
    // Constructors
    //
    SimpleRecursor(int movesAvailable, const BandingOptions& banding);
};

typedef SimpleRecursor<DenseMatrix, QvEvaluator, detail::ViterbiCombiner> SimpleQvRecursor;

typedef SimpleRecursor<DenseMatrix, QvEvaluator, detail::SumProductCombiner>
    SimpleQvSumProductRecursor;

typedef SimpleRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner> SparseSimpleQvRecursor;

typedef SimpleRecursor<SparseMatrix, QvEvaluator, detail::SumProductCombiner>
    SparseSimpleQvSumProductRecursor;
}
