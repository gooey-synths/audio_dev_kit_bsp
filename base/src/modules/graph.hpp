#pragma once

#include "module_basics.hpp"

#include <vector>

namespace graph_infrastructure {

///
/// Structure for defining a connection between modules.
/// Data flows from module output -> module input.
///
struct Connection {
    size_t inModIdx;   ///< Module to input into
    size_t inPortIdx;  ///< Port to to input into
    size_t outModIdx;  ///< Module to get the output from
    size_t outPortIdx; ///< Port to get the output from
};

///
/// Structure for defining a graph.
///
struct Graph {
    std::vector<module_basics::ModuleInterface*> mods; ///< Modules in the graph
    std::vector<Connection> cons;                      ///< Connections between modules
};

} // namespace graph_infrastructure
