#pragma once

#include "ice40_loader.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace fpga {

///
/// SPI config for interacting with the SPI bus.
///
static const spi::SpiBusConfig scFpgaSpiBusCfg = {
    .mPolarity = 1,
    .mPhase = 1,
    .mIoSwap = false,
    .mFreq = 2000000,
    .mWordSize = 8,
    .mMidi = 0
};

///
/// Prepare FPGA for loading, this will reset it.
/// @param resetPin GPIO to use for reset.
/// @param csPin GPIO to use for chip select.
///
void ice40_init(gpio::Pin& resetPin, gpio::Pin& csPin) {
    // Start FPGA in slave mode
    resetPin = false;
    csPin  = false;
    vTaskDelay(10);
    resetPin = true;
    vTaskDelay(10);
}

///
/// Write some number of bytes to the FPGA.
/// @param spi Reference to the spi bus that the FPGA is connected to.
/// @param bytes Array of bytes to write.
/// @param len Number of bytes to write.
///
void ice40_write_bytes(spi::ISpiBus& spi, char* bytes, size_t len) {
    spi.configure(scFpgaSpiBusCfg);
    spi.prepare(bytes, NULL, len, spi::ISpiBus::NO_CS_SELECTED, sizeof(*bytes));
    spi.transact();

    // This is not great tbh
    while(!spi.isComplete()) {
        vTaskDelay(10);
    }
}

///
/// Check if the FPGA is done programming.
/// @param cDonePin Pin to use for the CDone connection.
/// @return true if the FPGA is done loading the bitstream.
///
bool ice40_is_done(gpio::Pin& cDonePin) {
    return cDonePin();
}

///
/// Finish the bitstream loading.
/// @param spi SPI bus to use
/// @param csPin Pin to use as the chip select line to the FPGA
///
void ice40_finish(spi::ISpiBus& spi, gpio::Pin& csPin) {
    char dummyBytes[8] = {0};
    ice40_write_bytes(spi, dummyBytes, sizeof(dummyBytes));
    csPin = true;
}

} // namespace fpga
