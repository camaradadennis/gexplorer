#ifndef GRAPH_H
#define GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>

#include <utility>      // for pair
#include <vector>


class Graph final
{
public:
    struct VertexProperties
    {
        std::size_t id;
        double x_coord;
        double y_coord;
    };

    struct EdgeProperties
    {
        std::string name;
        std::size_t weight;
        bool oneway;
    };

    typedef boost::adjacency_list<
        boost::listS, boost::vecS, boost::directedS,
        VertexProperties, EdgeProperties
    > AdjList;

    using vertex_t = boost::graph_traits<AdjList>::vertex_descriptor;
    using edge_t = boost::graph_traits<AdjList>::edge_descriptor;
    using vertex_iterator = boost::graph_traits<AdjList>::vertex_iterator;
    using edge_iterator = boost::graph_traits<AdjList>::edge_iterator;
    using Point = std::pair<double, double>;

    Graph() = default;
    ~Graph() = default;

    Graph(AdjList&&);

    std::size_t num_vertices() const;
    std::size_t get_vertex_id(const Graph::vertex_t&) const;

    double plot_path(const vertex_t&, const vertex_t&, std::vector<vertex_t>&);

    vertex_t get_edge_source(const edge_t&);
    vertex_t get_edge_target(const edge_t&);

    Point get_vertex_coords(const vertex_t&);

    vertex_iterator vertex_begin();
    vertex_iterator vertex_end();

    edge_iterator edge_begin();
    edge_iterator edge_end();

private:
    AdjList m_adj_list;
};

#endif // GRAPH_H
