#include <board/physical_board.hpp>
#include <modules/graph_processor.hpp>

int main() {
    static board::ProtoBoardV1 board;
    graph_infrastructure::run_graph_processor(board);
    return 0;
}
