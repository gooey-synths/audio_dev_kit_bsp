#include "graph_processor.hpp"
#include "graph_runner.hpp"
#include "graph_loader.hpp"
#include "module_loader.hpp"

namespace graph_infrastructure {

static constexpr size_t scRxBufSize = 4096;
static char sRxBuffer[scRxBufSize];
const char scNewline[] = "\r\n";

///
/// Read input of a USB interface until and EOF character is found
/// @param buf Buffer to place received characters into
/// @param bufLen Length of the buffer
/// @param itf Interface to read from
/// @return True if EOF character was found befor buffer overflow
///
static bool readUntilEOF(char* buf, size_t bufLen, board::CommunicationInterface& itf) {

    size_t bufIdx = 0;
    char c;

    while(1) {
        size_t numRead = itf.ReadN(&c, 1);

        if(numRead) {
            // Check for windows or linux EOF character.
            if(c == 0x1A || c == 0x04) {
                itf.WriteN((char*)&scNewline, sizeof(scNewline));
                itf.Flush();
                return true;
            } else if(bufIdx >= bufLen) {
                return false;
            } else {
                itf.WriteN(&c, 1);
                itf.Flush();
                buf[bufIdx] = c;
                bufIdx++;
            }
        }

    }
}

///
/// Run the graph processor loop.
///
void run_graph_processor(board::BoardInterface &board) {
    ModuleLoader modLoader(board);
    GraphLoader graphLoader(modLoader);
    GraphRunner graphRunner(board);

    board::CommunicationInterface& comm = *board.GetComm(0);
    while(1) {
        memset(sRxBuffer, 0, scRxBufSize);
        bool foundEof = readUntilEOF(sRxBuffer, scRxBufSize, comm);
        if(!foundEof) {
            continue;
        }

        try {
            Graph* newGraph = graphLoader.load(sRxBuffer, scRxBufSize);
            if(newGraph != nullptr) {
                graphRunner.stop();
                graphRunner.setGraph(newGraph);
                graphRunner.start();
            }
        } catch(const char* errorMsg) {
            comm.WriteN(const_cast<char*>(errorMsg), strlen(errorMsg));
            comm.WriteN((char*)&scNewline, sizeof(scNewline));
        }
    }
}

} // namespace graph_infrastructure
