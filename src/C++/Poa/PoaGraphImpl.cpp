#include "PoaGraphImpl.hpp"
#include "PoaGraph.hpp"

#include "Align/AlignConfig.hpp"
#include "Utils.hpp"

namespace boost
{
    using ConsensusCore::detail::VertexInfoMap;
    using boost::format;

    class my_label_writer
    {
    public:
        my_label_writer(VertexInfoMap map, bool color, bool verbose)
            : map_(map),
              color_(color),
              verbose_(verbose)
        {}

        template <class VertexOrEdge>
        void operator()(std::ostream& out, const VertexOrEdge& v) const
        {
            std::string nodeColoringAttribute =
                (color_ && map_[v]->IsInConsensus?
                 " style=\"filled\", fillcolor=\"lightblue\" ," : "");
            if (!verbose_)
            {
                out << format("[shape=Mrecord,%s label=\"{ %c | %d }\"]")
                    % nodeColoringAttribute
                    % map_[v]->Base
                    % map_[v]->Reads;
            }
            else
            {
                out <<  format("[shape=Mrecord,%s label=\"{ "
                               "{ %d | %c } |"
                               "{ %d | %d } |"
                               "{ %0.2f | %0.2f } }\"]")
                    % nodeColoringAttribute
                    % v % map_[v]->Base
                    % map_[v]->Reads % map_[v]->SpanningReads
                    % map_[v]->Score % map_[v]->ReachingScore;
            }
        }
    private:
        VertexInfoMap map_;
        bool color_;
        bool verbose_;
    };
}

namespace ConsensusCore {
namespace detail {
    //
    // PoaGraphImpl
    //

    PoaGraphImpl::PoaGraphImpl()
    {
        vertexInfoMap_ = get(vertex_info, g_);
        enterVertex_ = add_vertex(g_);
        vertexInfoMap_[enterVertex_] = new PoaNode('^', 0);
        exitVertex_ = add_vertex(g_);
        vertexInfoMap_[exitVertex_] = new PoaNode('$', 0);
    }

    PoaGraphImpl::~PoaGraphImpl()
    {
        foreach (Vertex v, vertices(g_))
        {
            delete vertexInfoMap_[v];
        }
    }

    void PoaGraphImpl::repCheck()
    {
        // assert the representation invariant for the object
        foreach (Vertex v, vertices(g_))
        {
            if (v == enterVertex_)
            {
                assert(in_degree(v, g_) == 0);
                assert(out_degree(v, g_) > 0 || NumSequences() == 0);
            }
            else if (v == exitVertex_)
            {
                assert(in_degree(v, g_) > 0 || NumSequences() == 0);
                assert(out_degree(v, g_) == 0);
            }
            else
            {
                assert(in_degree(v, g_) > 0);
                assert(out_degree(v, g_) > 0);
            }
            assert(vertexInfoMap_[v] != NULL);
        }
    }


    static inline vector<const AlignmentColumn*>
    getPredecessorColumns(BoostGraph& g,
                          Vertex v,
                          const AlignmentColumnMap& alignmentColumnForVertex)
    {
        vector<const AlignmentColumn*> predecessorColumns;
        const AlignmentColumn* predCol;
        foreach (Edge e, in_edges(v, g))
        {
            Vertex u = source(e, g);
            predCol = alignmentColumnForVertex.at(u);
            assert(predCol != NULL);
            predecessorColumns.push_back(predCol);
        }
        return predecessorColumns;
    }


    tuple<string, float, vector<ScoredMutation>* >
    PoaGraphImpl::FindConsensus(const AlignConfig& config)
    {
       std::stringstream ss;
       std::vector<Vertex> bestPath = maxPath(config.Mode);
       foreach (Vertex v, bestPath)
       {
           PoaNode* consensusNode = vertexInfoMap_[v];
           consensusNode->IsInConsensus = true;
           ss << consensusNode->Base;
       }

       // if requested, identify likely sequence variants
       std::vector<ScoredMutation>* variants = findPossibleVariants(bestPath);
       return boost::make_tuple(ss.str(), 0.0f, variants);
    }

    const AlignmentColumn*
    PoaGraphImpl::makeAlignmentColumnForExit(Vertex v,
                                               const AlignmentColumnMap& alignmentColumnForVertex,
                                               const std::string& sequence,
                                               const AlignConfig& config)
    {
        assert(out_degree(v, g_) == 0);

        // this is kind of unnecessary as we are only actually using one entry in this column
        int I = sequence.length();
        AlignmentColumn* curCol = new AlignmentColumn(v, I + 1);

        float bestScore = -FLT_MAX;
        Vertex prevVertex = null_vertex;

        // Under local or semiglobal alignment the vertex $ can be
        // "reached" in the dynamic programming from any other vertex
        // in one step via the End move--not just its predecessors in
        // the graph.  In local alignment, it may have been from any
        // row, not necessarily I.
        if (config.Mode == SEMIGLOBAL || config.Mode == LOCAL)
        {
            foreach (Vertex u, vertices(g_))
            {
                if (u != exitVertex_)
                {
                    const AlignmentColumn* predCol = alignmentColumnForVertex.at(u);
                    int prevRow = (config.Mode == LOCAL ? ArgMaxVector(predCol->Score) : I);

                    if (predCol->Score[prevRow] > bestScore)
                    {
                        bestScore = predCol->Score[prevRow];
                        prevVertex = predCol->CurrentVertex;
                    }
                }
            }
        }
        else
        {
            // regular predecessors
            vector<const AlignmentColumn*> predecessorColumns  =
                    getPredecessorColumns(g_, v, alignmentColumnForVertex);
            foreach (const AlignmentColumn * predCol, predecessorColumns)
            {
                if (predCol->Score[I] > bestScore)
                {
                    bestScore = predCol->Score[I];
                    prevVertex = predCol->CurrentVertex;
                }
            }
        }
        assert(prevVertex != null_vertex);
        curCol->Score[I] = bestScore;
        curCol->PreviousVertex[I] = prevVertex;
        curCol->ReachingMove[I] = EndMove;
        return curCol;
    }

    const AlignmentColumn*
    PoaGraphImpl::makeAlignmentColumn(Vertex v,
                                        const AlignmentColumnMap& alignmentColumnForVertex,
                                        const std::string& sequence,
                                        const AlignConfig& config)
    {
        AlignmentColumn* curCol = new AlignmentColumn(v, sequence.length() + 1);
        const PoaNode* vertexInfo = vertexInfoMap_[v];
        vector<const AlignmentColumn*> predecessorColumns =
                getPredecessorColumns(g_, v, alignmentColumnForVertex);

        //
        // handle row 0 separately:
        //
        if (predecessorColumns.size() == 0)
        {
            // if this vertex doesn't have any in-edges (^), then it has
            // no reaching move
            assert(v == enterVertex_);
            curCol->Score[0] = 0;
            curCol->ReachingMove[0] = InvalidMove;
            curCol->PreviousVertex[0] = null_vertex;
        }
        else if (config.Mode == SEMIGLOBAL  || config.Mode == LOCAL)
        {
            // under semiglobal or local alignment, we use the Start move
            curCol->Score[0] = 0;
            curCol->ReachingMove[0] = StartMove;
            curCol->PreviousVertex[0] = enterVertex_;
        }
        else
        {
            // otherwise it's a deletion
            float candidateScore;
            float bestScore = -FLT_MAX;
            Vertex prevVertex = null_vertex;
            MoveType reachingMove = InvalidMove;

            foreach (const AlignmentColumn * prevCol, predecessorColumns)
            {
                candidateScore = prevCol->Score[0] + config.Params.Delete;
                if (candidateScore > bestScore)
                {
                    bestScore = candidateScore;
                    prevVertex = prevCol->CurrentVertex;
                    reachingMove = DeleteMove;
                }
            }
            assert(reachingMove != InvalidMove);
            curCol->Score[0] = bestScore;
            curCol->ReachingMove[0] = reachingMove;
            curCol->PreviousVertex[0] = prevVertex;
        }

        //
        // tackle remainder of read.
        //
        // i represents position in array
        // readPos=i-1 represents position in read
        for (unsigned int i = 1, readPos = 0;  i <= sequence.length(); i++, readPos++)
        {
            float candidateScore, bestScore;
            Vertex prevVertex;
            MoveType reachingMove;

            if (config.Mode == LOCAL)
            {
                bestScore = 0;
                prevVertex = enterVertex_;
                reachingMove = StartMove;
            }
            else
            {
                bestScore = -FLT_MAX;
                prevVertex = null_vertex;
                reachingMove = InvalidMove;
            }

            foreach (const AlignmentColumn* prevCol, predecessorColumns)
            {
                // Incorporate (Match or Mismatch)
                bool isMatch = sequence[readPos] == vertexInfo->Base;
                candidateScore = prevCol->Score[i - 1] + (isMatch ?
                                                             config.Params.Match :
                                                             config.Params.Mismatch);
                if (candidateScore > bestScore)
                {
                    bestScore = candidateScore;
                    prevVertex = prevCol->CurrentVertex;
                    reachingMove = (isMatch ? MatchMove : MismatchMove);
                }
                // Delete
                candidateScore = prevCol->Score[i] + config.Params.Delete;
                if (candidateScore > bestScore)
                {
                    bestScore = candidateScore;
                    prevVertex = prevCol->CurrentVertex;
                    reachingMove = DeleteMove;
                }
            }
            // Extra
            candidateScore = curCol->Score[i - 1] + config.Params.Insert;
            if (candidateScore > bestScore)
            {
                bestScore = candidateScore;
                prevVertex = v;
                reachingMove = ExtraMove;
            }
            assert(reachingMove != InvalidMove);
            curCol->Score[i] = bestScore;
            curCol->ReachingMove[i] = reachingMove;
            curCol->PreviousVertex[i] = prevVertex;
        }

        return curCol;
    }

    void PoaGraphImpl::AddSequence(const std::string& sequence, const AlignConfig& config)
    {
        DEBUG_ONLY(repCheck());
        assert(sequence.length() > 0);

        int seqNo = sequences_.size();

        // Yes, it's true!  We need to retain a COPY of the SequenceFeatures
        // so that the shared_ptr's within will always have positive usage count
        // so long as this object exists.
        sequences_.push_back(sequence);

        if (seqNo == 0)
        {
            threadFirstRead(sequence);
        }
        else
        {
            // calculate alignment columns of sequence vs. graph
            AlignmentColumnMap alignmentColumnForVertex;
            vector<Vertex> sortedVertices(num_vertices(g_));
            topological_sort(g_, sortedVertices.rbegin());
            const AlignmentColumn* curCol;
            foreach (Vertex v, sortedVertices)
            {
                if (v != exitVertex_)
                {
                    curCol = makeAlignmentColumn(v, alignmentColumnForVertex,
                                                 sequence, config);
                }
                else
                {
                    curCol = makeAlignmentColumnForExit(v, alignmentColumnForVertex,
                                                        sequence, config);
                }
                alignmentColumnForVertex[v] = curCol;
            }

            tracebackAndThread(sequence, alignmentColumnForVertex, config.Mode);

            // Clean up the mess we created.  Might be nicer to use scoped ptrs.
            foreach (AlignmentColumnMap::value_type& kv, alignmentColumnForVertex)
            {
                delete kv.second;
            }
        }

        DEBUG_ONLY(repCheck());
    }


    int PoaGraphImpl::NumSequences() const
    {
       return sequences_.size();
    }

    string PoaGraphImpl::ToGraphViz(int flags) const
    {
       std::stringstream ss;
       write_graphviz(ss, g_, my_label_writer(vertexInfoMap_,
                                              flags & PoaGraph::COLOR_NODES,
                                              flags & PoaGraph::VERBOSE_NODES));
       return ss.str();
    }

   void
   PoaGraphImpl::WriteGraphVizFile(string filename, int flags) const
   {
       std::ofstream outfile(filename.c_str());
       outfile << ToGraphViz(flags);
       outfile.close();
   }

}} // ConsensusCore::detail
