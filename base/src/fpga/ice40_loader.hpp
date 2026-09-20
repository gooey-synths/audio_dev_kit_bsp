#pragma once

#include <gpio/gpio.hpp>
#include <spi/spibus_interface.hpp>

namespace fpga {

void ice40_init(gpio::Pin& resetPin, gpio::Pin& csPin);

void ice40_write_bytes(spi::ISpiBus& spi, char* bytes, size_t len);

bool ice40_is_done(gpio::Pin& cDonePin);

void ice40_finish(spi::ISpiBus& spi, gpio::Pin& csPin);

} // namespace fpga
