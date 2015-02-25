// Copyright (c) 2011-2015, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: David Alexander

#include "PoaGraph.hpp"
#include "PoaGraphImpl.hpp"
#include "Utils.hpp"

namespace ConsensusCore {
namespace detail {

    static boost::unordered_set<Vertex>
    childVertices(Vertex v,
                  const BoostGraph& g)
    {
        boost::unordered_set<Vertex> result;
        foreach (Edge e, out_edges(v, g))
        {
            result.insert(target(e, g));
        }
        return result;
    }

    static boost::unordered_set<Vertex>
    parentVertices(Vertex v,
                   const BoostGraph& g)
    {
        boost::unordered_set<Vertex> result;
        foreach (Edge e, in_edges(v, g))
        {
            result.insert(source(e, g));
        }
        return result;
    }

    void PoaGraphImpl::tagSpan(Vertex start, Vertex end)
    {
        // cout << "Tagging span " << start << " to " << end << endl;
        std::list<Vertex> sortedVertices(num_vertices(g_));
        topological_sort(g_, sortedVertices.rbegin());
        bool spanning = false;
        foreach (Vertex v, sortedVertices)
        {
            if (v == start)
            {
                spanning = true;
            }
            if (v == end)
            {
                break;
            }
            if (spanning)
            {
                vertexInfoMap_[v]->SpanningReads++;
            }
        }
    }

    std::vector<Vertex>
    PoaGraphImpl::maxPath(AlignMode mode) const
    {
        int totalReads = NumSequences();

        std::list<Vertex> path;
        std::list<Vertex> sortedVertices(num_vertices(g_));
        topological_sort(g_, sortedVertices.rbegin());
        unordered_map<Vertex, Vertex> bestPrevVertex;

        // ignore ^ and $
        // TODO(dalexander): find a cleaner way to do this
        vertexInfoMap_[sortedVertices.front()]->ReachingScore = 0;
        sortedVertices.pop_back();
        sortedVertices.pop_front();

        Vertex bestVertex = null_vertex;
        float bestReachingScore = -FLT_MAX;
        foreach (Vertex v, sortedVertices)
        {
            const PoaNode* vertexInfo = vertexInfoMap_[v];
            int containingReads = vertexInfo->Reads;
            int spanningReads = vertexInfo->SpanningReads;
            float score = (mode != GLOBAL) ?
                          (2 * containingReads - 1 * spanningReads - 0.0001f) :
                          (2 * containingReads - 1 * totalReads - 0.0001f);
            vertexInfoMap_[v]->Score = score;
            vertexInfoMap_[v]->ReachingScore = score;
            bestPrevVertex[v] = null_vertex;
            foreach (Edge e, in_edges(v, g_))
            {
                Vertex sourceVertex = source(e, g_);
                float rsc = score + vertexInfoMap_[sourceVertex]->ReachingScore;
                if (rsc > vertexInfoMap_[v]->ReachingScore)
                {
                    vertexInfoMap_[v]->ReachingScore = rsc;
                    bestPrevVertex[v] = sourceVertex;
                }
                if (rsc > bestReachingScore)
                {
                    bestVertex = v;
                    bestReachingScore = rsc;
                }
            }
        }
        assert(bestVertex != null_vertex);

        // trace back from best-scoring vertex
        Vertex v = bestVertex;
        while (v != null_vertex)
        {
            path.push_front(v);
            v = bestPrevVertex[v];
        }
        return std::vector<Vertex>(path.begin(), path.end());
    }

    void PoaGraphImpl::threadFirstRead(std::string sequence)
    {
        // first sequence in the alignment
        Vertex u = null_vertex, v;
        Vertex startSpanVertex = null_vertex, endSpanVertex;
        int readPos = 0;

        foreach (char base, sequence)
        {
            v = add_vertex(g_);
            vertexInfoMap_[v] = new PoaNode(base);
            if (readPos == 0)
            {
                add_edge(enterVertex_, v, g_);
                startSpanVertex = v;
            }
            else
            {
                add_edge(u, v, g_);
            }
            u = v;
            readPos++;
        }
        assert(startSpanVertex != null_vertex);
        assert(u != null_vertex);
        endSpanVertex = u;
        add_edge(u, exitVertex_, g_);  // terminus -> $
        tagSpan(startSpanVertex, endSpanVertex);
    }

    void PoaGraphImpl::tracebackAndThread
      (std::string sequence,
       const AlignmentColumnMap& alignmentColumnForVertex,
       AlignMode alignMode)
    {
        const int I = sequence.length();

        // perform traceback from (I,$), threading the new sequence into the graph as
        // we go.
        int i = I;
        const AlignmentColumn* curCol;
        Vertex v = null_vertex, forkVertex = null_vertex;
        Vertex u = exitVertex_;
        Vertex startSpanVertex;
        Vertex endSpanVertex = alignmentColumnForVertex.at(exitVertex_)->PreviousVertex[I];

        while ( !(u == enterVertex_ && i == 0) )
        {
            // u -> v
            // u: current vertex
            // v: vertex last visited in traceback (could be == u)
            // forkVertex: the vertex that will be the target of a new edge
            int readPos = i - 1; // (INVARIANT)
            curCol = alignmentColumnForVertex.at(u);
            assert(curCol != NULL);
            PoaNode* curNodeInfo = vertexInfoMap_[u];
            Vertex prevVertex = curCol->PreviousVertex[i];
            MoveType reachingMove = curCol->ReachingMove[i];

            if (reachingMove == StartMove)
            {
                assert (v != null_vertex);

                if (forkVertex == null_vertex)
                {
                    forkVertex = v;
                }
                // In local model thread read bases, adjusting i (should stop at 0)
                while (i > 0)
                {
                    assert(alignMode == LOCAL);
                    Vertex newForkVertex = add_vertex(g_);
                    vertexInfoMap_[newForkVertex] = new PoaNode(sequence[readPos]);
                    add_edge(newForkVertex, forkVertex, g_);
                    forkVertex = newForkVertex;
                    i--;
                    readPos = i - 1;
                }
            }
            else if (reachingMove == EndMove)
            {
                assert(forkVertex == null_vertex && u == exitVertex_ && v == null_vertex);

                forkVertex = exitVertex_;

                if (alignMode == LOCAL) {
                    // Find the row # we are coming from, walk
                    // back to there, threading read bases onto
                    // graph via forkVertex, adjusting i.
                    const AlignmentColumn* prevCol = alignmentColumnForVertex.at(prevVertex);
                    int prevRow = ArgMaxVector(prevCol->Score);

                    while (i > static_cast<int>(prevRow))
                    {
                        Vertex newForkVertex = add_vertex(g_);
                        vertexInfoMap_[newForkVertex] = new PoaNode(sequence[readPos]);
                        add_edge(newForkVertex, forkVertex, g_);
                        forkVertex = newForkVertex;
                        i--;
                        readPos = i - 1;
                    }
                }
            }
            else if (reachingMove == MatchMove)
            {
                // if there is an extant forkVertex, join it
                if (forkVertex != null_vertex)
                {
                    add_edge(u, forkVertex, g_);
                    forkVertex = null_vertex;
                }
                // add to existing node
                curNodeInfo->Reads++;
                i--;
            }
            else if (reachingMove == DeleteMove)
            {
                if (forkVertex == null_vertex)
                {
                    forkVertex = v;
                }
            }
            else if (reachingMove == ExtraMove ||
                     reachingMove == MismatchMove)
            {
                // begin a new arc with this read base
                Vertex newForkVertex = add_vertex(g_);
                vertexInfoMap_[newForkVertex] = new PoaNode(sequence[readPos]);
                if (forkVertex == null_vertex)
                {
                    forkVertex = v;
                }
                add_edge(newForkVertex, forkVertex, g_);
                forkVertex = newForkVertex;
                i--;
            }
            else
            {
                ShouldNotReachHere();
            }

            v = u;
            u = prevVertex;
        }
        startSpanVertex = v;
        if (startSpanVertex != exitVertex_)
        {
            tagSpan(startSpanVertex, endSpanVertex);
        }

        // if there is an extant forkVertex, join it to enterVertex
        // is this still desirable?
        if (forkVertex != null_vertex)
        {
            add_edge(enterVertex_, forkVertex, g_);
            forkVertex = null_vertex;
        }
    }

    vector<ScoredMutation>*
    PoaGraphImpl::findPossibleVariants(const std::vector<Vertex>& bestPath) const
    {
        // Return value will be deallocated by PoaConsensus destructor.
        vector<ScoredMutation>* variants = new vector<ScoredMutation>();

        for (int i = 2; i < (int)bestPath.size() - 2; i++) // NOLINT
        {
            Vertex v = bestPath[i];
            boost::unordered_set<Vertex> children = childVertices(v, g_);

            // Look for a direct edge from the current node to the node
            // two spaces down---suggesting a deletion with respect to
            // the consensus sequence.
            if (children.find(bestPath[i + 2]) != children.end())
            {
                float score = -vertexInfoMap_[bestPath[i + 1]]->Score;
                variants->push_back(Mutation(DELETION, i + 1, '-').WithScore(score));
            }

            // Look for a child node that connects immediately back to i + 1.
            // This indicates we should try inserting the base at i + 1.

            // Parents of (i + 1)
            boost::unordered_set<Vertex> lookBack = parentVertices(bestPath[i + 1], g_);

            // (We could do this in STL using std::set sorted on score, which would then
            // provide an intersection mechanism (in <algorithm>) but that actually ends
            // up being more code.  Sad.)
            float bestInsertScore = -FLT_MAX;
            Vertex bestInsertVertex = null_vertex;

            foreach (Vertex v, children)
            {
                boost::unordered_set<Vertex>::iterator found = lookBack.find(v);
                if (found != lookBack.end())
                {
                    float score = vertexInfoMap_[*found]->Score;
                    if (score > bestInsertScore)
                    {
                        bestInsertScore = score;
                        bestInsertVertex = *found;
                    }
                }
            }

            if (bestInsertVertex != null_vertex)
            {
                char base = vertexInfoMap_[bestInsertVertex]->Base;
                variants->push_back(
                        Mutation(INSERTION, i + 1, base).WithScore(bestInsertScore));
            }

            // Look for a child node not in the consensus that connects immediately
            // to i + 2.  This indicates we should try mismatching the base i + 1.

            // Parents of (i + 2)
            lookBack = parentVertices(bestPath[i + 2], g_);

            float bestMismatchScore = -FLT_MAX;
            Vertex bestMismatchVertex = null_vertex;

            foreach (Vertex v, children)
            {
                if (v == bestPath[i + 1]) continue;

                boost::unordered_set<Vertex>::iterator found = lookBack.find(v);
                if (found != lookBack.end())
                {
                    float score = vertexInfoMap_[*found]->Score;
                    if (score > bestMismatchScore)
                    {
                        bestMismatchScore = score;
                        bestMismatchVertex = *found;
                    }
                }
            }

            if (bestMismatchVertex != null_vertex)
            {
                // TODO(dalexander): As implemented (compatibility), this returns
                // the score of the mismatch node. I think it should return the score
                // difference, no?
                char base = vertexInfoMap_[bestMismatchVertex]->Base;
                variants->push_back(
                        Mutation(SUBSTITUTION, i + 1, base).WithScore(bestMismatchScore));
            }
        }
        return variants;
    }

}}  // ConsensusCore::detail
