
#pragma once
// This header is internal, not part of the API!  ConsensusCore doesn't have a
// good directory-level separation of internal-vs-API, but we can at least prevent
// SWIG export
#ifdef SWIG
#error "PoaGraphImpl.hpp is not an API-facing header!"
#endif  // SWIG

#include "PoaGraph.hpp"
#include "Align/AlignConfig.hpp"

#include <boost/config.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>
#include <cfloat>
#include <vector>

using std::string;
using std::vector;

///
///  Boost graph library typedefs, properties, and graphviz output.
///
using namespace boost; // NOLINT

namespace boost
{
    enum vertex_info_t { vertex_info = 424 };  // a unique #
    BOOST_INSTALL_PROPERTY(vertex, info);
}


namespace ConsensusCore {
namespace detail {

    class SdpRangeFinder;

    enum MoveType
    {
        InvalidMove,  // Invalid move reaching ^ (start)
        StartMove,    // Start move: ^ -> vertex in row 0 of local alignment
        EndMove,      // End move: vertex -> $ in row 0 of local alignment, or
                      //  in global alignment, terminal vertex -> $
        MatchMove,
        MismatchMove,
        DeleteMove,
        ExtraMove
    };

    struct PoaNode
    {
        char Base;
        int Reads;
        int SpanningReads;
        float Score;
        float ReachingScore;
        bool IsInConsensus;

        void Init(char base, int reads)
        {
            this->Base = base;
            this->Reads = reads;
            this->SpanningReads = 0;
            this->Score = 0;
            this->ReachingScore = 0;
            this->IsInConsensus = false;
        }

        explicit PoaNode(char base)
        {
            Init(base, 1);
        }

        PoaNode(char base, int reads)
        {
            Init(base, reads);
        }
    };

    typedef adjacency_list < setS, vecS, bidirectionalS, property<vertex_info_t, PoaNode*> > BoostGraph;
    typedef graph_traits<BoostGraph>::edge_descriptor Edge;
    typedef graph_traits<BoostGraph>::vertex_descriptor Vertex;
    typedef property_map<BoostGraph, vertex_info_t>::type VertexInfoMap;
    static const Vertex null_vertex = graph_traits<BoostGraph>::null_vertex();

    struct AlignmentColumn : noncopyable
    {
        Vertex CurrentVertex;
        vector<float> Score;
        vector<MoveType> ReachingMove;
        vector<Vertex> PreviousVertex;

        AlignmentColumn(Vertex vertex, int len)
            : CurrentVertex(vertex),
              Score(len, -FLT_MAX),
              ReachingMove(len, InvalidMove),
              PreviousVertex(len, null_vertex)
        {}

        ~AlignmentColumn()
        {}
    };

    typedef unordered_map<Vertex, const AlignmentColumn*> AlignmentColumnMap;


    class PoaGraphImpl
    {
        friend class SdpRangeFinder;

        BoostGraph g_;
        VertexInfoMap vertexInfoMap_;
        Vertex enterVertex_;
        Vertex exitVertex_;
        vector<std::string> sequences_;

        void repCheck();

        //
        // utility routines
        //
        const AlignmentColumn*
        makeAlignmentColumn(Vertex v,
                            const AlignmentColumnMap& alignmentColumnForVertex,
                            const std::string& sequence,
                            const AlignConfig& config);

        const AlignmentColumn*
        makeAlignmentColumnForExit(Vertex v,
                                   const AlignmentColumnMap& alignmentColumnForVertex,
                                   const std::string& sequence,
                                   const AlignConfig& config);

    public:
        //
        // Graph traversal functions, defined in PoaGraphTraversals
        //
        void tagSpan(Vertex start, Vertex end);

        std::vector<Vertex> consensusPath(AlignMode mode) const;

        void threadFirstRead(std::string sequence);

        void tracebackAndThread
          (std::string sequence,
           const AlignmentColumnMap& alignmentColumnForVertex,
           AlignMode mode);

        vector<ScoredMutation>*
        findPossibleVariants(const std::vector<Vertex>& bestPath) const;

    public:
        PoaGraphImpl();
        ~PoaGraphImpl();

        void AddSequence(const std::string& sequence,
                         const AlignConfig& config,
                         SdpRangeFinder* rangeFinder=NULL);

        // TODO(dalexander): make this const
        tuple<string, float, vector<ScoredMutation>*>
        FindConsensus(const AlignConfig& config, bool findVariants=true);

        int NumSequences() const;
        string ToGraphViz(int flags) const;
        void WriteGraphVizFile(string filename, int flags) const;
    };


    // Note this is left-biased, which may not be what we want!
    template<class T>
    inline size_t ArgMaxVector(const std::vector<T>& v)
    {
        return std::distance(v.begin(), std::max_element(v.begin(), v.end()));
    }

    // free functions, we should put these all in traversals
    std::string sequenceAlongPath(const BoostGraph& g,
                                  const VertexInfoMap& vertexInfoMap,
                                  std::vector<Vertex> path);


}} // ConsensusCore::detail
