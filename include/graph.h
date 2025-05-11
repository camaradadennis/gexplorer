#ifndef GRAPH_H
#define GRAPH_H

#include <memory>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>


class Graph final
{
public:
    struct Point
    {
        double x;
        double y;

        Point& operator=(const boost::square_topology<>::point_type p)
        {
            this->x = p[0];
            this->y = p[1];
            return *this;
        }
    };

    struct VertexProperties
    {
        std::size_t id;
        std::string name;
        Point coords;
    };

    struct EdgeProperties
    {
        std::size_t weight;
    };

    typedef boost::adjacency_list<
        boost::listS, boost::vecS, boost::undirectedS,
        VertexProperties, EdgeProperties
    > AdjList;

    using vertex_t = boost::graph_traits<AdjList>::vertex_descriptor;
    using edge_t = boost::graph_traits<AdjList>::edge_descriptor;
    using vertex_iterator = boost::graph_traits<AdjList>::vertex_iterator;
    using edge_iterator = boost::graph_traits<AdjList>::edge_iterator;

    static std::unique_ptr<Graph> from(const char*);

    Graph();

    void compute_vertex_coords(int, int);
    std::size_t num_vertices();

    bool plot_path(vertex_t, vertex_t, std::vector<vertex_t>&);

    bool is_selected(edge_t);
    bool is_selected(vertex_t);
    void set_selected(edge_t);
    void set_selected(vertex_t);
    void add_selected(edge_t);
    void add_selected(vertex_t);

    vertex_t get_edge_source(edge_t);
    vertex_t get_edge_target(edge_t);

    Point& get_vertex_coords(vertex_t);

    vertex_iterator vertex_begin();
    vertex_iterator vertex_end();

    edge_iterator edge_begin();
    edge_iterator edge_end();

private:
    AdjList m_adj_list;
    std::vector<vertex_t> m_selected_v;
    std::vector<edge_t> m_selected_e;
};

#endif // GRAPH_H
