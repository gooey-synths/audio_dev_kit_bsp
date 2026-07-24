#pragma once

#include <board/board_interface.hpp>
#include "graph.hpp"

namespace graph_infrastructure {

///
/// Class for running a graph.
///
class GraphRunner {
public:
    /// Timer index for fast processing
    static constexpr size_t scFastTimerIdx = 0;

    GraphRunner(board::BoardInterface& board);

    virtual ~GraphRunner();

    void setGraph(Graph* graph);

    void start();

    void stop();

    void invalidateGraph();

    // Delete copy and assignment.
    GraphRunner(GraphRunner const&) = delete;
    void operator=(GraphRunner const&)  = delete;

private:
    void fastCallBack() noexcept;

    board::BoardInterface& mBoard; ///< Reference to board
    bool mIsRunning {false};       ///< True if the graph is valid
    Graph* mGraph {NULL};          ///< Pointer to graph

    ///
    /// Static method for member method to function pointer binding.
    ///
    static void fastCallBackCallBack() {
        sInstance->fastCallBack();
    }

    static GraphRunner* sInstance; ///< Pointer to GraphRunner instance
};

} // namespace graph_infrastructure
