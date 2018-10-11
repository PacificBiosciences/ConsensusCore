// Author: David Alexander

#pragma once

#include <stdint.h>
#include <string>
#include <utility>

//
// Forward declarations
//
namespace ConsensusCore {
struct AlignConfig;
struct BandingOptions;
class DenseMatrix;
struct QuiverConfig;
class PairwiseAlignment;
class AnnotatedPairwiseAlignment;
struct QvModelParams;
struct EdnaModelParams;
struct ChannelSequenceFeatures;
struct QvSequenceFeatures;
struct SequenceFeatures;
class SparseMatrix;
class Mutation;
class ScoredMutation;
struct Interval;
struct Read;
class PoaGraph;
struct PoaConsensus;
}

namespace ConsensusCore {
namespace detail {
class ViterbiCombiner;
class SumProductCombiner;
class SdpRangeFinder;
class PoaGraphImpl;
}
}

//
// Exception types
//

namespace ConsensusCore {
/// \brief Abstract base class for "error"-type exceptions.  Do
///        not catch these.
class ErrorBase
{
public:
    virtual std::string Message() const throw() = 0;
    virtual ~ErrorBase() {}
};

/// \brief Abstract base class for exceptions, which user code
///        may safely catch.
class ExceptionBase
{
public:
    virtual std::string Message() const throw() = 0;
    virtual ~ExceptionBase() {}
};

/// \brief An exception signaling an error in ConsensusCore's internal logic.
class InternalError : public ErrorBase
{
public:
    InternalError() : msg_("Internal error encountered!") {}

    explicit InternalError(const std::string& msg) : msg_(msg) {}

    std::string Message() const throw() { return msg_; }

private:
    std::string msg_;
};

class InvalidInputError : public ErrorBase
{
public:
    InvalidInputError() : msg_("Invalid input!") {}

    explicit InvalidInputError(const std::string& msg) : msg_(msg) {}

    std::string Message() const throw() { return msg_; }

private:
    std::string msg_;
};

class UnsupportedFeatureError : public ErrorBase
{
public:
    UnsupportedFeatureError() : msg_("Unsupported feature!") {}

    explicit UnsupportedFeatureError(const std::string& msg) : msg_(msg) {}

    std::string Message() const throw() { return msg_; }

private:
    std::string msg_;
};

class NotYetImplementedException : public ErrorBase
{
public:
    std::string Message() const throw() { return "Feature not yet implemented"; }
};

/// \brief An exception indicating the Alpha and Beta matrices could
/// not be matched up by the Recursor
class AlphaBetaMismatchException : public ExceptionBase
{
    std::string Message() const throw() { return "Alpha and beta could not be mated."; }
};
}
