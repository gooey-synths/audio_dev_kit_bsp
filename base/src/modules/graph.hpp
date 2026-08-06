#pragma once

#include "module_basics.hpp"

#include <vector>

namespace graph_infrastructure {

///
/// Structure for defining a connection between modules.
///
struct Connection {
    size_t inModIdx;   ///< Module index of connection input
    size_t inPortIdx;  ///< Port index of connection input
    size_t outModIdx;  ///< Module index of connection output
    size_t outPortIdx; ///< Port index of connection ouptut
};

///
/// Structure for defining a graph.
///
struct Graph {
    std::vector<module_basics::ModuleInterface*> mods; ///< Modules in the graph
    std::vector<Connection> cons;                      ///< Connections between modules
};

} // namespace graph_infrastructure
