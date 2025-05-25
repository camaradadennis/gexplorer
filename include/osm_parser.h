#ifndef OSM_PARSER_H
#define OSM_PARSER_H

#include "graph.h"

#include <string>

std::unique_ptr<Graph> parse_osm(const std::string&);

#endif // OSM_PARSER_H
