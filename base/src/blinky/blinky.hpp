#pragma once

#include <gpio/gpio.hpp>

namespace blinky {

///
/// Class for controlling the blinky
///
class Blinky {
public:
    Blinky();
    void setPeriod(uint32_t ms);

private:
    // Disallow copy constructor.
    Blinky(const Blinky&) = delete;

    // Disallow assignment operator.
    Blinky& operator=(const Blinky) = delete;

    void blinkTaskBody();
    static void blinkTask(void* pvParameters);

    gpio::Pin mBlinkPin;           ///< Led pin to toggle
    uint32_t mBlinkPeriod = 1000;  ///< Blink period
};

extern Blinky TheBlinky;

} // namespace blinky

