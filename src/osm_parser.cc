#include "osm_parser.h"

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <algorithm>        // for reverse()
#include <cmath>            // for cos(), sqrt() and pow()
#include <map>
#include <memory>           // for unique_ptr
#include <string>
#include <utility>          // for move()
#include <vector>


using Vertex = Graph::VertexProperties;
using Edge = Graph::EdgeProperties;
using AdjList = Graph::AdjList;

typedef std::map<std::size_t, Vertex> NodeMap;

namespace pt = boost::property_tree;


/* Parameters used in map projection.
 *
 * Since OSM marks points in a geographic coordinate system, we need a method
 * to project those points in a planar surface while maintaining their
 * relative distances.
 * The choosen method is to apply an equirectangular projection, since
 * this application is expected to deal mostly with small area sizes
 * on closer to 0º latitudes.
 */
namespace
{
    constexpr double METERS_PER_DEGREE_LAT = 111320.0;
    double METERS_PER_DEGREE_LON;
    double center_lat, center_lon;

    double project_lon(double lon)
    {
        return (lon - center_lon) * METERS_PER_DEGREE_LON;
    }

    double project_lat(double lat)
    {
        return (lat - center_lat) * METERS_PER_DEGREE_LAT;
    }

    double vertex_distance(const Vertex& a, const Vertex& b)
    {
        return std::sqrt(
            std::pow(a.x_coord - b.x_coord, 2)
            + std::pow(a.y_coord - b.y_coord, 2)
        );
    }

    void set_projection_params(double minlat, double maxlat,
                                      double minlon, double maxlon)
    {
        center_lat = (minlat + maxlat) / 2.0;
        center_lon = (minlon + maxlon) / 2.0;

        METERS_PER_DEGREE_LON =
            std::cos(center_lat * M_PI / 180.0) * METERS_PER_DEGREE_LAT;
    }
}


static inline bool is_visible(const pt::ptree& el)
{
    return el.get<std::string>("<xmlattr>.visible") == "true";
}


static std::unique_ptr<Graph> _parse_internal (const std::string& filename)
{
    pt::ptree tree;
    pt::read_xml(filename, tree);
    pt::ptree root = tree.get_child("osm");

    set_projection_params(
        root.get<double>("bounds.<xmlattr>.minlat"),
        root.get<double>("bounds.<xmlattr>.maxlat"),
        root.get<double>("bounds.<xmlattr>.minlon"),
        root.get<double>("bounds.<xmlattr>.maxlon")
    );

    pt::ptree::assoc_iterator element, element_end;
    NodeMap node_map;

    for (boost::tie(element, element_end) = root.equal_range("node");
         element != element_end;
         ++element)
    {
        const pt::ptree& node = element->second;

        Vertex vertex;

        vertex.id = node.get<std::size_t>("<xmlattr>.id");
        vertex.x_coord = project_lon(node.get<double>("<xmlattr>.lon"));
        vertex.y_coord = project_lat(node.get<double>("<xmlattr>.lat"));

        node_map.insert({vertex.id, vertex});
    }

    std::map<std::size_t, std::size_t> nodeid_to_vd;
    AdjList adjacency_list;

    for (boost::tie(element, element_end) = root.equal_range("way");
         element != element_end;
         ++element)
    {
        const pt::ptree& way = element->second;

        // We will only care about features that are visible
        // Historical OSM data should be disregarded.
        if (!is_visible(way))
            continue;

        Edge edge;
        edge.name = "";
        edge.oneway = false;
        std::vector<std::size_t> waypoints;

        bool is_way = false;

        for (const pt::ptree::value_type& el: way)
        {
            if (el.first == "nd")
            {
                waypoints.push_back(el.second.get<std::size_t>("<xmlattr>.ref"));
            }
            else if (el.first == "tag")
            {
                std::string key{ el.second.get<std::string>("<xmlattr>.k") };

                std::string value{ el.second.get<std::string>("<xmlattr>.v") };

                if (key == "name")
                    edge.name = value;
                else if (key == "oneway")
                {
                    if (value == "yes")
                        edge.oneway = true;
                    else if (value == "-1")
                    {
                        edge.oneway = true;

                        // This reverse only works here because OSM XML
                        // assures us the element order won't change.
                        // That is: <nd> elements always comes before <tag> ones.
                        // This means that at this point, the waypoints
                        // vector is already complete.
                        std::reverse(waypoints.begin(), waypoints.end());
                    }
                }
                else if (key == "highway")
                {
                    is_way = true;
                }
            }
        }

        if (!is_way)
            continue;

        for (std::size_t i = 1; i < waypoints.size(); ++i)
        {
            std::size_t src_nodeid = waypoints[i - 1];
            std::size_t tgt_nodeid = waypoints[i];

            // If src or tgt nodes don't exist, jump to next pair
            if (!node_map.contains(src_nodeid) || !node_map.contains(tgt_nodeid))
                continue;

            Vertex& src = node_map[src_nodeid];
            Vertex& tgt = node_map[tgt_nodeid];
            std::size_t src_vd, tgt_vd;

            if (!nodeid_to_vd.contains(src_nodeid))
            {
                // Vertex is not yet added to adjacency_list
                src_vd = boost::add_vertex(src, adjacency_list);
                nodeid_to_vd[src_nodeid] = src_vd;
            }
            else
                src_vd = nodeid_to_vd[src_nodeid];

            if (!nodeid_to_vd.contains(tgt_nodeid))
            {
                tgt_vd = boost::add_vertex(tgt, adjacency_list);
                nodeid_to_vd[tgt_nodeid] = tgt_vd;
            }
            else
                tgt_vd = nodeid_to_vd[tgt_nodeid];

            edge.weight = vertex_distance(src, tgt);

            boost::add_edge(src_vd, tgt_vd, edge, adjacency_list);

            // Graph is directed. If we have a two-way path between vertices,
            // then we must add another inverted edge.
            if (!edge.oneway)
                boost::add_edge(tgt_vd, src_vd, edge, adjacency_list);
        }
    }

    return std::make_unique<Graph>(std::move(adjacency_list));
}


std::unique_ptr<Graph> osm_parser::parse(const std::string& filename)
{
    std::unique_ptr<Graph> g;

    try
    {
        g = _parse_internal(filename);
    }
    catch (const pt::ptree_error& err)
    {
        throw ParserError(
            std::string("could not parse xml file: ").append(err.what())
        );
    }

    return g;
}
