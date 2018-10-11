// Author: David Alexander

#include <gtest/gtest.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Quiver/QvEvaluator.hpp>
#include <ConsensusCore/Utils.hpp>

#include "ParameterSettings.hpp"
#include "Random.hpp"
#include "SseTestingUtils.hpp"

using namespace ConsensusCore;  // NOLINT
using std::cout;
using std::endl;

class QvEvaluatorTest : public testing::Test
{
public:
    void SetUp()
    {
        int numEvaluators = 200;
        int tplLen = 20;
        int seed = 42;

        Rng rng(seed);
        for (int n = 0; n < numEvaluators; n++) {
            fuzzEvaluators_.push_back(RandomQvEvaluator(rng, tplLen));
        }
    }

    virtual ~QvEvaluatorTest() {}

protected:
    std::vector<QvEvaluator> fuzzEvaluators_;
};

TEST_F(QvEvaluatorTest, IncVsInc4)
{
    foreach (const QvEvaluator& e, this->fuzzEvaluators_) {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        for (int j = 0; j <= J - 1; j++)
            for (int i = 0; i <= I - 4; i++)
                COMPARE4(e.Inc4, e.Inc, i, j);
    }
}

TEST_F(QvEvaluatorTest, DelVsDel4)
{
    foreach (const QvEvaluator& e, this->fuzzEvaluators_) {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        for (int j = 0; j <= J - 1; j++)
            for (int i = 0; i <= I - 3; i++)
                COMPARE4(e.Del4, e.Del, i, j);
    }
}

TEST_F(QvEvaluatorTest, ExtraVsExtra4)
{
    foreach (const QvEvaluator& e, this->fuzzEvaluators_) {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        for (int j = 0; j <= J; j++)
            for (int i = 0; i <= I - 4; i++)
                COMPARE4(e.Extra4, e.Extra, i, j);
    }
}

TEST_F(QvEvaluatorTest, MergeVsMerge4)
{
    foreach (const QvEvaluator& e, this->fuzzEvaluators_) {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        for (int j = 0; j <= J - 2; j++)
            for (int i = 0; i <= I - 4; i++)
                COMPARE4(e.Merge4, e.Merge, i, j);
    }
}

TEST_F(QvEvaluatorTest, BadTagTest)
{
    Rng rng(42);
    int n = 20;

    int readLength = RandomPoissonDraw(rng, n);
    std::string seq = RandomSequence(rng, readLength);

    float* insQv = RandomQvArray(rng, readLength);
    float* subsQv = RandomQvArray(rng, readLength);
    float* delQv = RandomQvArray(rng, readLength);
    float* delTag = RandomTagArray(rng, readLength);
    float* mergeQv = RandomQvArray(rng, readLength);
    delTag[5] = 3;

    ASSERT_THROW(new QvSequenceFeatures(seq, insQv, subsQv, delQv, delTag, mergeQv), InternalError);

    delete[] insQv;
    delete[] subsQv;
    delete[] delQv;
    delete[] delTag;
    delete[] mergeQv;
}
