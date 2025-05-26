#ifndef OSM_PARSER_H
#define OSM_PARSER_H

#include "graph.h"

#include <string>
#include <stdexcept>

namespace osm_parser
{
    std::unique_ptr<Graph> parse(const std::string&);

    class ParserError: public std::runtime_error
    {
    public:
        explicit ParserError(const std::string& what)
            : std::runtime_error(what) {}
    };
}

#endif // OSM_PARSER_H
