#ifndef PHYSICAL_BOARD_HPP
#define PHYSICAL_BOARD_HPP

#include "board_interface.hpp"
#include "gpio/gpio.hpp"
#include "spi/dacx050y.hpp"
#include "spi/spi.hpp"
#include "adc/adc_interface.hpp"
#include "timer/basic_timer.hpp"
#include "adc/adc.hpp"

namespace board {

class PhysicalDigitalOutput : public DigitalOutput {
public:
    PhysicalDigitalOutput(gpio::Pin& out);

private:
    gpio::Pin& mOut;  ///< GPIO to use for output.
};

class PhysicalDigitalInput : public DigitalInput {
public:
    PhysicalDigitalInput(gpio::Pin& in);

private:
    gpio::Pin& mIn;  ///< GPIO to use for input.
};

class PhysicalAnaloglOutput : public AnalogOutput {
public:
    PhysicalAnaloglOutput(spi::IDACx050y& dac, size_t idx);

private:
    spi::IDACx050y& mDac; ///< Reference to DAC.
    size_t mIdx; ///< DAC index.
};

class PhysicalAnalogInput : public AnalogInput {
public:
    PhysicalAnalogInput(adc::IOnChipADC& adc, size_t idx);

private:
    adc::IOnChipADC& mAdc; ///< Reference to ADC.
    size_t mIdx; ///< ADC index.
};


class PhysicalTimer : public Timer {
public:
    PhysicalTimer(timer::BasicTimer& timer);

private:
    timer::BasicTimer& timer; ///< Reference to basic timer.
};

class ProtoBoardV1 : public BoardInterface {
public:

    static constexpr BoardConfig scProtoBoardV1Cfg {
        .slowIO = {
            .numAnalogInputs = 0,
            .numAnalogOutputs = 0,
            .numDigitalInputs = 0,
            .numDigitalOutputs = 0,
        },
        .fastIO = {
            .numAnalogInputs = 8,
            .numAnalogOutputs = 8,
            .numDigitalInputs = 4,
            .numDigitalOutputs = 4,
        },
        .numTimers = 2,
    };


    ProtoBoardV1();

private:
    adc::OnChipADC mAdc; ///< ADC

    spi::HwCsSpiBus mHwSpiBus; ///< Hw SPI bus
    spi::DAC60508 mDac; ///< DAC

    timer::BasicTimer mTimers[scProtoBoardV1Cfg.numTimers]; ///< Timers

    gpio::Pin mFastDigitalInputs[scProtoBoardV1Cfg.fastIO.numDigitalInputs]; ///< Fast digitial inputs
    gpio::Pin mFastDigitalOutputs[scProtoBoardV1Cfg.fastIO.numDigitalOutputs]; ///< Fast digitial outputs
    gpio::Pin mFastDigitalInputs[scProtoBoardV1Cfg.slowIO.numDigitalInputs]; ///< Slow digitial inputs
    gpio::Pin mFastDigitalOutputs[scProtoBoardV1Cfg.slowIO.numDigitalOutputs]; ///< Slow digitial outputs
};

}
#endif