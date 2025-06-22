#include "graph.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <limits>           // for numeric_limits<>::max()


std::unique_ptr<Graph> Graph::create()
{
    return std::make_unique<Graph>();
}

Graph::VertexT Graph::add_vertex(const Graph::VertexProperties& vertex)
{
    return boost::add_vertex(vertex, m_adj_list);
}


void Graph::remove_vertex(const Graph::VertexT& vertex)
{
    boost::clear_vertex(vertex, m_adj_list);
    boost::remove_vertex(vertex, m_adj_list);
}


std::optional<Graph::EdgeT> Graph::add_edge(const Graph::VertexT& src,
                                            const Graph::VertexT& tgt,
                                            const Graph::EdgeProperties& edge)
{
    auto [descriptor, success] = boost::add_edge(src, tgt, edge, m_adj_list);

    if (success)
        return descriptor;
    else
        return std::nullopt;
}


std::size_t Graph::num_vertices() const
{
    return boost::num_vertices(m_adj_list);
}


const Graph::VertexProperties&
Graph::get_vertex_properties(const Graph::VertexT& vertex) const
{
    return m_adj_list[vertex];
}


const Graph::EdgeProperties&
Graph::get_edge_properties(const Graph::EdgeT& edge) const
{
    return m_adj_list[edge];
}


const Graph::VertexCoords&
Graph::get_vertex_coords(const Graph::VertexT& vertex) const
{
    return m_adj_list[vertex].coord;
}


std::size_t Graph::get_vertex_id(const Graph::VertexT& vertex) const
{
    return m_adj_list[vertex].id;
}


std::vector<std::size_t> Graph::get_vertex_id_list() const
{
    std::vector<std::size_t> vec;
    vec.reserve(boost::num_vertices(m_adj_list));

    for (auto [vi, vend] = boost::vertices(m_adj_list); vi != vend; ++vi)
        vec.push_back(m_adj_list[*vi].id);

    return vec;
}


double Graph::get_edge_weight(const Graph::EdgeT& edge) const
{
    return m_adj_list[edge].weight;
}


bool Graph::is_edge_oneway(const Graph::EdgeT& edge) const
{
    return m_adj_list[edge].oneway;
}


double Graph::plot_path(const Graph::VertexT& src,
                        const Graph::VertexT& tgt,
                        std::vector<Graph::VertexT>& path) const
{
    std::vector<VertexT> predecessors(num_vertices());

    for (std::size_t i = 0; i < predecessors.size(); ++i)
        predecessors[i] = static_cast<VertexT>(i);

    std::vector<double> distances(num_vertices());

    boost::dijkstra_shortest_paths(m_adj_list, src,
        predecessor_map(boost::make_iterator_property_map(
            predecessors.begin(), get(boost::vertex_index, m_adj_list)))
        .distance_map(boost::make_iterator_property_map(
            distances.begin(), get(boost::vertex_index, m_adj_list)))
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


Graph::VertexT Graph::get_edge_src(const Graph::EdgeT& edge) const
{
    return boost::source(edge, m_adj_list);
}


Graph::VertexT Graph::get_edge_tgt(const Graph::EdgeT& edge) const
{
    return boost::target(edge, m_adj_list);
}


std::pair<Graph::VertexIter, Graph::VertexIter> Graph::iter_vertices() const
{
    return boost::vertices(m_adj_list);
}


std::pair<Graph::EdgeIter, Graph::EdgeIter> Graph::iter_edges() const
{
    return boost::edges(m_adj_list);
}


std::optional<Graph::VertexT>
Graph::find_vertex_with_coords(double x, double y, double margin) const
{
    for (auto [vi, vend] = boost::vertices(m_adj_list); vi != vend; ++vi)
    {
        const auto& point = m_adj_list[*vi].coord;

        if (x <= point.x + margin &&
            x >= point.x - margin &&
            y <= point.y + margin &&
            y >= point.y - margin)
        {
            return *vi;
        }
    }

    return {};
}


std::pair<Graph::VertexIter, Graph::VertexIter>
Graph::find_vertex_id(std::size_t id) const
{
    auto [vi, vend] = boost::vertices(m_adj_list);

    while (vi != vend && m_adj_list[*vi].id != id)
        ++vi;

    return { vi, vend };
}


std::pair<Graph::EdgeIter, Graph::EdgeIter>
Graph::find_edge_name(const std::string& name) const
{
    auto [vi, vend] = boost::edges(m_adj_list);

    while (vi != vend && m_adj_list[*vi].name != name)
        ++vi;

    return { vi, vend };
}
