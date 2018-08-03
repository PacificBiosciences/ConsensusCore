// Author: David Alexander

#pragma once

#include <algorithm>
#include <string>
#include <utility>

#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Types.hpp>

namespace ConsensusCore {

namespace detail {

/// \brief A base class for recursors, providing some functionality
///        based on polymorphic virtual private methods.
template <typename M, typename E, typename C>
class RecursorBase
{
public:  // Types
    typedef M MatrixType;
    typedef E EvaluatorType;
    typedef C CombinerType;

public:
    //
    // API methods
    //

    /// \brief Calculate the recursion score by "linking" partial alpha and/or
    ///        beta matrices.
    virtual float LinkAlphaBeta(const E& e, const M& alpha, int alphaColumn, const M& beta,
                                int betaColumn, int absoluteColumn) const = 0;

    /// \brief Fill the alpha and beta matrices.
    /// This routine will fill the alpha and beta matrices, ensuring
    /// that the score computed from the alpha and beta recursions are
    /// identical, refilling back-and-forth if necessary.
    virtual int FillAlphaBeta(const E& e, M& alpha, M& beta) const;

    /// \brief Reband alpha and beta matrices.
    /// This routine will reband alpha and beta to the convex hull
    /// of the maximum path through each and the inputs for column j.
    virtual bool RangeGuide(int j, const M& guide, const M& matrix, int* beginRow,
                            int* endRow) const;

    /// \brief Raw FillAlpha, provided primarily for testing purposes.
    ///        Client code should use FillAlphaBeta.
    virtual void FillAlpha(const E& e, const M& guide, M& alpha) const = 0;

    /// \brief Raw FillBeta, provided primarily for testing purposes.
    ///        Client code should use FillAlphaBeta.
    virtual void FillBeta(const E& e, const M& guide, M& beta) const = 0;

    /// \brief Compute two columns of the alpha matrix starting at columnBegin,
    ///        storing the output in ext.
    virtual void ExtendAlpha(const E& e, const M& alphaIn, int columnBegin, M& ext,
                             int numExtColumns = 2) const = 0;

    /// \brief Read out the alignment from the computed alpha matrix.
    const PairwiseAlignment* Alignment(const E& e, const M& alpha) const;

    RecursorBase(int movesAvailable, const BandingOptions& banding);
    virtual ~RecursorBase();

protected:
    int movesAvailable_;
    BandingOptions bandingOptions_;
};
}
}

#include <ConsensusCore/Quiver/detail/RecursorBase-inl.hpp>
