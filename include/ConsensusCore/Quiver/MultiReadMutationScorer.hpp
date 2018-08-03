// Author: David Alexander

#pragma once

#include <ConsensusCore/Matrix/AbstractMatrix.hpp>
#include <ConsensusCore/Quiver/MutationScorer.hpp>
#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Quiver/SseRecursor.hpp>
#include <ConsensusCore/Read.hpp>
#include <ConsensusCore/Types.hpp>

#include <boost/noncopyable.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace ConsensusCore {

class AbstractMultiReadMutationScorer
{
protected:
    AbstractMultiReadMutationScorer() {}
    virtual ~AbstractMultiReadMutationScorer() {}

public:
    virtual int TemplateLength() const = 0;
    virtual int NumReads() const = 0;
    virtual const MappedRead* Read(int readIndex) const = 0;

    virtual std::string Template(StrandEnum strand = FORWARD_STRAND) const = 0;
    virtual std::string Template(StrandEnum strand, int templateStart, int templateEnd) const = 0;

    virtual void ApplyMutations(const std::vector<Mutation>& mutations) = 0;

    // Reads provided must be clipped to the reference/scaffold window implied by
    // the
    // template, however they need not span the window entirely---nonspanning
    // reads
    // must be provided with (0-based) template start/end coordinates.
    virtual bool AddRead(const MappedRead& mappedRead, float threshold) = 0;
    virtual bool AddRead(const MappedRead& mappedRead) = 0;

    virtual float Score(const Mutation& m) const = 0;
    virtual float FastScore(const Mutation& m) const = 0;

    // Return a vector (of length NumReads) of the difference in
    // the score of each read caused by the template mutation.  In
    // the case where the mutation cannot be scored for a read
    // (i.e., it is too close to the end of the template, or the
    // read does not span the mutation site) that entry in the
    // vector is 0
    virtual std::vector<float> Scores(const Mutation& m, float unscoredValue) const = 0;
    virtual std::vector<float> Scores(const Mutation& m) const = 0;

    virtual bool IsFavorable(const Mutation& m) const = 0;
    virtual bool FastIsFavorable(const Mutation& m) const = 0;

    // Rough estimate of memory consumption of scoring machinery
    virtual std::vector<int> AllocatedMatrixEntries() const = 0;
    virtual std::vector<int> UsedMatrixEntries() const = 0;
    virtual const AbstractMatrix* AlphaMatrix(int i) const = 0;
    virtual const AbstractMatrix* BetaMatrix(int i) const = 0;
    virtual std::vector<int> NumFlipFlops() const = 0;

#if !defined(SWIG) || defined(SWIGCSHARP)
    // Alternate entry points for C# code, not requiring zillions of object
    // allocations.
    virtual float Score(MutationType mutationType, int position, char base) const = 0;
    virtual std::vector<float> Scores(MutationType mutationType, int position, char base,
                                      float unscoredValue) const = 0;
    virtual std::vector<float> Scores(MutationType mutationType, int position, char base) const = 0;
#endif

    // Return the actual sum of scores for the current template.
    // TODO(dalexander): need to refactor to make the semantics of
    // the various "Score" functions clearer.
    virtual float BaselineScore() const = 0;
    virtual std::vector<float> BaselineScores() const = 0;

    virtual std::string ToString() const = 0;
};

bool ReadScoresMutation(const MappedRead& mr, const Mutation& mut);
Mutation OrientedMutation(const MappedRead& mr, const Mutation& mut);

namespace detail {
template <typename ScorerType>
struct ReadState
{
    MappedRead* Read;
    ScorerType* Scorer;
    bool IsActive;

    ReadState(MappedRead* read, ScorerType* scorer, bool isActive);

    ReadState(const ReadState& other);
    ~ReadState();
    void CheckInvariants() const;
    std::string ToString() const;
};
}

template <typename R>
class MultiReadMutationScorer : public AbstractMultiReadMutationScorer
{
public:
    typedef R RecursorType;
    typedef typename R::EvaluatorType EvaluatorType;
    typedef typename ConsensusCore::MutationScorer<R> ScorerType;
    typedef typename detail::ReadState<ScorerType> ReadStateType;

public:
    MultiReadMutationScorer(const QuiverConfigTable& paramsByChemistry, std::string tpl);
    MultiReadMutationScorer(const MultiReadMutationScorer<R>& scorer);
    virtual ~MultiReadMutationScorer();

    int TemplateLength() const;
    int NumReads() const;
    const MappedRead* Read(int readIndex) const;

    std::string Template(StrandEnum strand = FORWARD_STRAND) const;
    std::string Template(StrandEnum strand, int templateStart, int templateEnd) const;
    void ApplyMutations(const std::vector<Mutation>& mutations);

    // Reads provided must be clipped to the reference/scaffold window implied by
    // the
    // template, however they need not span the window entirely---nonspanning
    // reads
    // must be provided with (0-based) template start/end coordinates.
    bool AddRead(const MappedRead& mappedRead, float threshold);
    bool AddRead(const MappedRead& mappedRead);

    float Score(const Mutation& m) const;
    float FastScore(const Mutation& m) const;

    // Return a vector (of length NumReads) of the difference in
    // the score of each read caused by the template mutation.  In
    // the case where the mutation cannot be scored for a read
    // (i.e., it is too close to the end of the template, or the
    // read does not span the mutation site) that entry in the
    // vector is -FLT_MAX, which is to be interpreted as NA.
    std::vector<float> Scores(const Mutation& m, float unscoredValue) const;
    std::vector<float> Scores(const Mutation& m) const { return Scores(m, 0.0f); }

    bool IsFavorable(const Mutation& m) const;
    bool FastIsFavorable(const Mutation& m) const;

    // Rough estimate of memory consumption of scoring machinery
    std::vector<int> AllocatedMatrixEntries() const;
    std::vector<int> UsedMatrixEntries() const;
    const AbstractMatrix* AlphaMatrix(int i) const;
    const AbstractMatrix* BetaMatrix(int i) const;
    std::vector<int> NumFlipFlops() const;

#if !defined(SWIG) || defined(SWIGCSHARP)
    // Alternate entry points for C# code, not requiring zillions of object
    // allocations.
    float Score(MutationType mutationType, int position, char base) const;
    std::vector<float> Scores(MutationType mutationType, int position, char base,
                              float unscoredValue) const;
    std::vector<float> Scores(MutationType mutationType, int position, char base) const
    {
        return Scores(mutationType, position, base, 0.0f);
    }
#endif

public:
    // Return the actual sum of scores for the current template.
    // TODO(dalexander): need to refactor to make the semantics of
    // the various "Score" functions clearer.
    float BaselineScore() const;
    std::vector<float> BaselineScores() const;

public:
    std::string ToString() const;

private:
    void CheckInvariants() const;

private:
    QuiverConfigTable quiverConfigByChemistry_;
    float fastScoreThreshold_;
    std::string fwdTemplate_;
    std::string revTemplate_;
    std::vector<ReadStateType> reads_;
};

typedef MultiReadMutationScorer<SparseSseQvRecursor> SparseSseQvMultiReadMutationScorer;
typedef MultiReadMutationScorer<SparseSseQvSumProductRecursor>
    SparseSseQvSumProductMultiReadMutationScorer;
}
