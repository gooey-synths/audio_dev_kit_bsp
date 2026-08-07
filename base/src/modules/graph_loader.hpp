#pragma once

#include "graph.hpp"
#include "module_loader_interface.hpp"

namespace graph_infrastructure {

///
/// Class for loading graph from a text input.
///
class GraphLoader {
public:
    GraphLoader(ModuleLoaderInterface& moduleLoader);

    virtual ~GraphLoader();

    Graph* load(char* input, size_t len);
private:
    static void validateGraph(Graph* graph);
    static void clearGraph(Graph* graph);

    // Graph double buffer so that if new graph fails to load, current graph does not get deleted.
    Graph mGraphA; ///< First graph
    Graph mGraphB; ///< Second graph
    Graph* mRunnableGraph = &mGraphA; ///< Fully loaded graph ready for running
    Graph* mLoadingGraph = &mGraphB; ///< Cleared graph, ready for loading

    ModuleLoaderInterface& mModuleLoader; ///< Module loader.
};

} // namespace graph_infrastructure
