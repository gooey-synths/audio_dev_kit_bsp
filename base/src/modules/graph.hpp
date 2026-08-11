#pragma once

#include "module_basics.hpp"

#include <vector>

namespace graph_infrastructure {

///
/// Structure for defining a connection between modules.
/// Data flows from module output -> module input.
///
struct Connection {
    size_t inModIdx;   ///< Module to input data into.
    size_t inPortIdx;  ///< Port to input data into.
    size_t outModIdx;  ///< Module to get data from.
    size_t outPortIdx; ///< Port to get data from.

    ///
    /// Equality operator.
    /// @param rhs Connection to compare.
    /// @return True if the Connection is the same.
    ///
    bool operator==(const Connection& rhs) const {
        return inModIdx == rhs.inModIdx &&
            inPortIdx == rhs.inPortIdx &&
            outModIdx == rhs.outModIdx &&
            outPortIdx == rhs.outPortIdx;
    }
};

///
/// Structure for defining a graph.
///
struct Graph {
    std::vector<module_basics::ModuleInterface*> mods; ///< Modules in the graph
    std::vector<Connection> cons;                      ///< Connections between modules
};

} // namespace graph_infrastructure
