#include "graph.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <limits>           // for numeric_limits<>::max()
#include <utility>          // for move()
#include <vector>

using namespace boost;


Graph::Graph(Graph::AdjList&& adj_list)
    : m_adj_list{ std::move(adj_list) }
{
}


std::size_t Graph::num_vertices() const
{
    return boost::num_vertices(m_adj_list);
}


double Graph::plot_path(const Graph::vertex_t& src,
                        const Graph::vertex_t& tgt,
                        std::vector<Graph::vertex_t>& path)
{
    std::vector<vertex_t> predecessors(num_vertices());

    for (std::size_t i = 0; i < predecessors.size(); ++i)
        predecessors[i] = static_cast<vertex_t>(i);

    std::vector<double> distances(num_vertices());

    dijkstra_shortest_paths(m_adj_list, src,
        predecessor_map(make_iterator_property_map(
            predecessors.begin(), get(vertex_index, m_adj_list)))
        .distance_map(make_iterator_property_map(
            distances.begin(), get(vertex_index, m_adj_list)))
        .weight_map(get(&EdgeProperties::weight, m_adj_list)));

    if (distances[tgt] == std::numeric_limits<double>::max())
        return std::numeric_limits<double>::max();

    auto current_vertex = tgt;
    while (current_vertex != src)
    {
        path.push_back(current_vertex);
        current_vertex = predecessors[current_vertex];
    }

    path.push_back(src);

    return distances[tgt];
}


Graph::vertex_t Graph::get_edge_source(const Graph::edge_t& edge)
{
    return source(edge, m_adj_list);
}


Graph::vertex_t Graph::get_edge_target(const Graph::edge_t& edge)
{
    return target(edge, m_adj_list);
}


Graph::Point Graph::get_vertex_coords(const Graph::vertex_t& vertex)
{
    return {m_adj_list[vertex].x_coord, m_adj_list[vertex].y_coord};
}

std::size_t Graph::get_vertex_id(const Graph::vertex_t& vertex) const
{
    return m_adj_list[vertex].id;
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
