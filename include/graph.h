#ifndef GRAPH_H
#define GRAPH_H

#include <boost/graph/adjacency_list.hpp>

#include <optional>
#include <string>
#include <utility>      // for pair
#include <vector>


template<typename Iter, typename T>
class GraphIterator;


class Graph
{
public:
    struct VertexCoords { double x; double y; };

    struct VertexProperties
    {
        std::size_t id;
        VertexCoords coord;
    };

    struct EdgeProperties
    {
        std::string name;
        double weight;
        bool oneway;
    };

    using AdjList = boost::adjacency_list<
        boost::listS, boost::vecS, boost::directedS,
        VertexProperties, EdgeProperties>;

    using VertexT = boost::graph_traits<AdjList>::vertex_descriptor;
    using EdgeT = boost::graph_traits<AdjList>::edge_descriptor;
    using VertexIter = boost::graph_traits<AdjList>::vertex_iterator;
    using EdgeIter = boost::graph_traits<AdjList>::edge_iterator;


    VertexT add_vertex(const VertexProperties&);

    std::optional<EdgeT> add_edge(const VertexT&, const VertexT&,
                                  const EdgeProperties&);

    std::size_t num_vertices() const;

    const VertexProperties& get_vertex_properties(const VertexT&) const;
    const EdgeProperties& get_edge_properties(const EdgeT&) const;

    const VertexCoords& get_vertex_coords(const VertexT&) const;
    std::size_t get_vertex_id(const VertexT&) const;
    std::vector<std::size_t> get_vertex_id_list() const;

    double plot_path(const VertexT&, const VertexT&, std::vector<VertexT>&) const;

    VertexT get_edge_src(const EdgeT&) const;
    VertexT get_edge_tgt(const EdgeT&) const;

    std::pair<VertexIter, VertexIter> iter_vertices() const;
    std::pair<EdgeIter, EdgeIter> iter_edges() const;

    std::pair<VertexIter, VertexIter> find_vertex_id(std::size_t) const;
    std::pair<EdgeIter, EdgeIter> find_edge_name(const std::string&) const;

private:
    AdjList m_adj_list;
};


#endif // GRAPH_H
