#include <board/physical_board.hpp>
#include <modules/graph_processor.hpp>
#include <tests/tests.hpp>

int main() {

    test_qspi_W25Q128JV();

    static board::ProtoBoardV1 board;
    graph_infrastructure::run_graph_processor(board);

    return 0;
}
