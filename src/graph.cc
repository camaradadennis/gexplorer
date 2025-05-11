#include "graph.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/random_layout.hpp>

#include <vector>

using namespace boost;


std::unique_ptr<Graph> Graph::from(const char* contents)
{
    // A small, static graph is better for testing
    // Deal with file parsing latter

    return std::make_unique<Graph>();
}


Graph::Graph()
    : m_adj_list{}
{
    VertexProperties vertices[] = {
        {0, "A", Point(0, 0)},
        {1, "B", Point(0, 0)},
        {2, "C", Point(0, 0)},
        {3, "D", Point(0, 0)},
        {4, "E", Point(0, 0)}
    };

    for (std::size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); ++i)
        add_vertex(vertices[i], m_adj_list);

    add_edge(0, 2, EdgeProperties(1), m_adj_list);
    add_edge(2, 1, EdgeProperties(7), m_adj_list);
    add_edge(2, 3, EdgeProperties(3), m_adj_list);
    add_edge(3, 4, EdgeProperties(1), m_adj_list);
    add_edge(4, 0, EdgeProperties(1), m_adj_list);
    add_edge(4, 1, EdgeProperties(1), m_adj_list);
    add_edge(1, 4, EdgeProperties(2), m_adj_list);
    add_edge(1, 3, EdgeProperties(1), m_adj_list);
}


void Graph::compute_vertex_coords(int width, int height)
{
    square_topology<> topo(1.0);
    auto pmap = get(&VertexProperties::coords, m_adj_list);
    random_graph_layout(m_adj_list, pmap, topo);

    typename graph_traits<AdjList>::vertex_iterator vi, vend;

    for (boost::tie(vi, vend) = vertices(m_adj_list); vi != vend; ++vi)
    {
        m_adj_list[*vi].coords.x *= width;
        m_adj_list[*vi].coords.y *= height;
    }
}


std::size_t Graph::num_vertices()
{
    return boost::num_vertices(m_adj_list);
}


bool Graph::plot_path(Graph::vertex_t src,
                      Graph::vertex_t tgt,
                      std::vector<Graph::vertex_t>& path)
{
    std::vector<vertex_t> predecessors(num_vertices());
    std::vector<int> distances(num_vertices());

    dijkstra_shortest_paths(m_adj_list, src,
        predecessor_map(make_iterator_property_map(
            predecessors.begin(), get(vertex_index, m_adj_list)))
        .distance_map(make_iterator_property_map(
            distances.begin(), get(vertex_index, m_adj_list)))
        .weight_map(get(&EdgeProperties::weight, m_adj_list)));

    auto current_vertex = tgt;
    do
    {
        path.push_back(current_vertex);
        current_vertex = predecessors[current_vertex];
    }
    while (current_vertex != src);

    path.push_back(src);

    return true;
}


Graph::vertex_t Graph::get_edge_source(Graph::edge_t edge)
{
    return source(edge, m_adj_list);
}


Graph::vertex_t Graph::get_edge_target(Graph::edge_t edge)
{
    return target(edge, m_adj_list);
}


Graph::Point& Graph::get_vertex_coords(Graph::vertex_t vertex)
{
    return m_adj_list[vertex].coords;
}


Graph::vertex_iterator Graph::vertex_begin()
{
    vertex_iterator start, end;
    boost::tie(start, end) = vertices(m_adj_list);
    return start;
}


Graph::vertex_iterator Graph::vertex_end()
{
    vertex_iterator start, end;
    boost::tie(start, end) = vertices(m_adj_list);
    return end;
}


Graph::edge_iterator Graph::edge_begin()
{
    edge_iterator start, end;
    boost::tie(start, end) = edges(m_adj_list);
    return start;
}


Graph::edge_iterator Graph::edge_end()
{
    edge_iterator start, end;
    boost::tie(start, end) = edges(m_adj_list);
    return end;
}
