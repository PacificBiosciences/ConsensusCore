// Author: David Alexander

#include <ConsensusCore/Quiver/ReadScorer.hpp>

#include <ConsensusCore/Align/PairwiseAlignment.hpp>
#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Quiver/QvEvaluator.hpp>
#include <ConsensusCore/Quiver/SseRecursor.hpp>

#include <iostream>
#include <string>

using std::string;
using std::cout;
using std::endl;

namespace ConsensusCore {
ReadScorer::ReadScorer(QuiverConfig& config) : _quiverConfig(config) {}

float ReadScorer::Score(const string& tpl, const Read& read) const
{
    int I, J;
    SparseSseQvRecursor r(_quiverConfig.MovesAvailable, _quiverConfig.Banding);
    QvEvaluator e(read, tpl, _quiverConfig.QvParams);

    I = read.Length();
    J = tpl.length();
    SparseMatrix alpha(I + 1, J + 1), beta(I + 1, J + 1);
    r.FillAlphaBeta(e, alpha, beta);

    return beta(0, 0);
}

const PairwiseAlignment* ReadScorer::Align(const string& tpl, const Read& read) const
{
    int I, J;
    SparseSseQvRecursor r(_quiverConfig.MovesAvailable, _quiverConfig.Banding);
    QvEvaluator e(read, tpl, _quiverConfig.QvParams);

    I = read.Length();
    J = tpl.length();
    SparseMatrix alpha(I + 1, J + 1), beta(I + 1, J + 1);
    r.FillAlphaBeta(e, alpha, beta);
    return r.Alignment(e, alpha);
}

const SparseMatrix* ReadScorer::Alpha(const string& tpl, const Read& read) const
{
    int I, J;
    SparseSseQvRecursor r(_quiverConfig.MovesAvailable, _quiverConfig.Banding);
    QvEvaluator e(read, tpl, _quiverConfig.QvParams);

    I = read.Length();
    J = tpl.length();
    SparseMatrix* alpha = new SparseMatrix(I + 1, J + 1);
    SparseMatrix* beta = new SparseMatrix(I + 1, J + 1);
    r.FillAlphaBeta(e, *alpha, *beta);
    return alpha;
}

const SparseMatrix* ReadScorer::Beta(const string& tpl, const Read& read) const
{
    int I, J;
    SparseSseQvRecursor r(_quiverConfig.MovesAvailable, _quiverConfig.Banding);
    QvEvaluator e(read, tpl, _quiverConfig.QvParams);

    I = read.Length();
    J = tpl.length();
    SparseMatrix* alpha = new SparseMatrix(I + 1, J + 1);
    SparseMatrix* beta = new SparseMatrix(I + 1, J + 1);
    r.FillAlphaBeta(e, *alpha, *beta);
    return beta;
}
}
