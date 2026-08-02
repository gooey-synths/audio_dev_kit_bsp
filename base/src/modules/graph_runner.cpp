#include "graph_runner.hpp"

namespace graph_infrastructure {

GraphRunner* GraphRunner::sInstance = NULL;

static const char* scGraphAlreadyRunningMsg = "Graph already running";

///
/// Constructor.
/// @param board Reference to board
///
GraphRunner::GraphRunner(board::BoardInterface& board) :
    mBoard(board)
{
    if(sInstance) {
        throw "GraphRunner already exists";
    }

    if(board.GetBoardConfig().numTimers < 1) {
        throw "Invalid board";
    }

    board.GetTimer(scFastTimerIdx).SetCallback(fastCallBackCallBack);

    sInstance = this;
}

///
/// Destructor.
///
GraphRunner::~GraphRunner() {
    stop();
    sInstance = NULL;
    mBoard.GetTimer(scFastTimerIdx).SetCallback(NULL);
}

///
/// Set the graph to run.
/// @param graph Graph to run.
/// @note graph should be validated before calling this function.
///
void GraphRunner::setGraph(Graph* graph) {
    if(mIsRunning) {
        throw scGraphAlreadyRunningMsg;
    }
    mGraph = graph;
}

///
/// Begin running the graph.
///
void GraphRunner::start() {
    if(!mGraph) {
        throw "Invalid graph";
    }
    if(mIsRunning) {
        throw scGraphAlreadyRunningMsg;
    }
    mBoard.GetTimer(scFastTimerIdx).Start();
    mIsRunning = true;
}

///
/// Stop running the graph.
///
void GraphRunner::stop() {
    mBoard.GetTimer(scFastTimerIdx).Stop();
    mIsRunning = false;
}

///
/// Callback for fast processing.
///
inline void GraphRunner::fastCallBack() noexcept {
    // Loop through all of the modules and run them.
    for(module_basics::ModuleInterface*& mod : mGraph->mods) {
        mod->run();
    }

    // Loop through all connections and move data.
    for(Connection& con : mGraph->cons) {
        uint16_t val = mGraph->mods[con.inModIdx]->getOutput(con.inPortIdx);
        mGraph->mods[con.outModIdx]->setInput(con.outPortIdx, val);
    }

    // Update our boards fast IO.
    mBoard.UpdateFastIO();
}

} // namespace graph_infrastructure
