// Author: Patrick Marks and David Alexander

#pragma once

#include <boost/noncopyable.hpp>
#include <string>

// TODO(dalexander): how can we remove this include??
//  We should move all template instantiations out to another
//  header, I presume.
#include <ConsensusCore/Mutation.hpp>
#include <ConsensusCore/Quiver/SimpleRecursor.hpp>
#include <ConsensusCore/Quiver/SseRecursor.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {
template <typename R>
class MutationScorer
{
public:
    typedef typename R::MatrixType MatrixType;
    typedef typename R::EvaluatorType EvaluatorType;
    typedef R RecursorType;

public:
    MutationScorer(const EvaluatorType& evaluator, const R& recursor);

    MutationScorer(const MutationScorer& other);
    virtual ~MutationScorer();

public:
    std::string Template() const;
    void Template(std::string tpl);

    float Score() const;
    float ScoreMutation(const Mutation& m) const;

public:
    // Accessors that are handy for debugging.
    const MatrixType* Alpha() const;
    const MatrixType* Beta() const;
    const PairwiseAlignment* Alignment() const;
    const EvaluatorType* Evaluator() const;
    int NumFlipFlops() const { return numFlipFlops_; }

private:
    EvaluatorType* evaluator_;
    R* recursor_;
    MatrixType* alpha_;
    MatrixType* beta_;
    MatrixType* extendBuffer_;
    int numFlipFlops_;
};

typedef MutationScorer<SimpleQvRecursor> SimpleQvMutationScorer;
typedef MutationScorer<SseQvRecursor> SseQvMutationScorer;
typedef MutationScorer<SparseSimpleQvRecursor> SparseSimpleQvMutationScorer;
typedef MutationScorer<SparseSseQvRecursor> SparseSseQvMutationScorer;
typedef MutationScorer<SparseSimpleQvSumProductRecursor> SparseSimpleQvSumProductMutationScorer;
typedef MutationScorer<SparseSseQvSumProductRecursor> SparseSseQvSumProductMutationScorer;
typedef MutationScorer<SparseSseEdnaRecursor> SparseSseEdnaMutationScorer;
}
