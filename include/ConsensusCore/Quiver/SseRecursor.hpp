// Author: David Alexander

#pragma once

#include <ConsensusCore/Edna/EdnaEvaluator.hpp>
#include <ConsensusCore/Matrix/DenseMatrix.hpp>
#include <ConsensusCore/Matrix/SparseMatrix.hpp>
#include <ConsensusCore/Quiver/QvEvaluator.hpp>
#include <ConsensusCore/Quiver/SimpleRecursor.hpp>
#include <ConsensusCore/Quiver/detail/Combiner.hpp>
#include <ConsensusCore/Quiver/detail/RecursorBase.hpp>

namespace ConsensusCore {

template <typename M, typename E, typename C>
class SseRecursor : public detail::RecursorBase<M, E, C>
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
    SseRecursor(int movesAvailable, const BandingOptions& banding);

private:
    // Used during bringup
    SimpleRecursor<M, E, C> simpleRecursor_;
};

typedef SseRecursor<DenseMatrix, QvEvaluator, detail::ViterbiCombiner> SseQvRecursor;

typedef SseRecursor<DenseMatrix, QvEvaluator, detail::SumProductCombiner> SseQvSumProductRecursor;

typedef SseRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner> SparseSseQvRecursor;

typedef SseRecursor<SparseMatrix, QvEvaluator, detail::SumProductCombiner>
    SparseSseQvSumProductRecursor;

typedef SseRecursor<SparseMatrix, EdnaEvaluator, detail::SumProductCombiner> SparseSseEdnaRecursor;
}
