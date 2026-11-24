#include <board/physical_board.hpp>
#include <modules/graph_processor.hpp>
#include <tests/tests.hpp>


int main() {
    test_DAC60508();
    static board::ProtoBoardV1 b;
    graph_infrastructure::run_graph_processor(b);

    return 0;
}
