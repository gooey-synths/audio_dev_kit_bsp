#pragma once

#include <board/board_interface.hpp>
#include <spi/spi.hpp>
#include <gpio/gpio.hpp>
#include "FreeRTOS.h"
#include "task.h"

namespace fpga {

///
/// Class for loading a bitstream from a communication interface.
///
class CommIce40Loader {
public:
    static const char scHeader[3];
    CommIce40Loader(board::CommunicationInterface& commItf,
            gpio::Pin& csPin, gpio::Pin& cDonePin, gpio::Pin& resetPin, spi::ISpiBus& spi);

    virtual ~CommIce40Loader();

private:
    static CommIce40Loader* sInstance;

    static void loaderTask(void* v);
    void loaderTaskBody();

    void waitForHeader();

    board::CommunicationInterface& mCommItf; ///< Communication interface.
    spi::ISpiBus& mSpiBus; ///< SPI bus that the FPGA is connected to.

    gpio::Pin& mResetPin;     ///< Reset pin of the FPGA.
    gpio::Pin& mCDonePin;     ///< CDone pin of the FPGA.
    gpio::Pin& mCsPin;        ///< CS pin of the FPGA.
    TaskHandle_t mTaskHandle; ///< Handle of the FPGA loader task.
};

} // namespace fpga
