#ifndef PHYSICAL_BOARD_HPP
#define PHYSICAL_BOARD_HPP

#include "board_interface.hpp"
#include "gpio/gpio.hpp"
#include "gpio/gpio_defs.h"
#include "spi/dacx050y.hpp"
#include "spi/spi.hpp"
#include "adc/adc_interface.hpp"
#include "timer/basic_timer.hpp"
#include "adc/adc.hpp"

namespace board {

class PhysicalDigitalOutput : public DigitalOutput {
public:
    PhysicalDigitalOutput(gpio::Pin& out);
    ///
    /// Set the value of the output.
    /// @param val The desired value of the output.
    ///
    virtual void SetValue(bool val) { mOut = val; }

private:
    gpio::Pin& mOut;  ///< GPIO to use for output.
};

class PhysicalDigitalInput : public DigitalInput {
public:
    PhysicalDigitalInput(gpio::Pin& in);

    ///
    /// Get the value of the input.
    /// @return The value of the input.
    ///
    virtual bool GetValue() const { return mIn(); }

private:
    gpio::Pin& mIn;  ///< GPIO to use for input.
};

class PhysicalAnaloglOutput : public AnalogOutput {
public:
    PhysicalAnaloglOutput(spi::IDACx050y& dac, size_t idx);
    ///
    /// Set the value of the output.
    /// @param val The desired value of the output.
    ///
    virtual void SetValue(BoardData val) { mDac.setStreamVal(mIdx, val); }

private:
    spi::IDACx050y& mDac; ///< Reference to DAC.
    size_t mIdx; ///< DAC index.
};

class PhysicalAnalogInput : public AnalogInput {
public:
    PhysicalAnalogInput(adc::IOnChipADC& adc, size_t idx);
    ///
    /// Get the value of the input.
    /// @return The value of the input.
    ///
    virtual BoardData GetValue() const { return mAdc.getConversion(mIdx); }

private:
    adc::IOnChipADC& mAdc; ///< Reference to ADC.
    size_t mIdx; ///< ADC index.
};


class PhysicalTimer : public Timer {
public:
    PhysicalTimer(timer::BasicTimer& timer);

    ///
    /// Set the frequency of the timer.
    /// @param frequency Frequency of the timer.
    ///
    virtual void SetFrequency(float frequency) { mTimer.setFreq(static_cast<uint32_t>(frequency)); }

    ///
    /// Set the callback function to execute.
    /// @param callback Callback function to execute. Null for no callback.
    ///
    virtual void SetCallback(CallbackFunc callback) { mTimer.setInterrupt(callback); }

    ///
    /// Start
    ///
    virtual void Start() { mTimer.start(false); }

    ///
    /// Stop
    ///
    virtual void Stop() { mTimer.stop(); }
private:
    timer::BasicTimer& mTimer; ///< Reference to basic timer.
};

class ProtoBoardV1 : public BoardInterface {
public:

    ProtoBoardV1();

    static constexpr uint8_t scProtoBoardV1AdcSeq[] = {10,11,16,14,15,3,8,9};

    static constexpr BoardConfig scProtoBoardV1Cfg = {
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

    ///
    /// Get the board configuration.
    /// @return The board configuration.
    ///
    virtual BoardConfig GetBoardConfig() { return scProtoBoardV1Cfg; }

    ///
    /// Get a reference to an analog input on the board.
    /// @param speed Speed of the analog input to get a reference to.
    /// @param idx Index of the analog input to get.
    /// @return A reference to an analog input on the board.
    ///
    virtual AnalogInput& GetAnalogInput(IOSpeed speed, size_t idx) const {
        
    }
    
    ///
    /// Get a reference to an analog output on the board.
    /// @param speed Speed of the analog output to get a reference to.
    /// @param idx Index of the analog output to get.
    /// @return A reference to an analog output on the board.
    ///
    virtual AnalogOutput& GetAnalogOutput(IOSpeed speed, size_t idx) const {
        
    }

    ///
    /// Get a reference to an digital input on the board.
    /// @param speed Speed of the digital input to get a reference to.
    /// @param idx Index of the digital input to get.
    /// @return A reference to an digital input on the board.
    ///
    virtual DigitalInput& GetDigitalInput(IOSpeed speed, size_t idx) const {
        
    }
    
    ///
    /// Get a reference to an digital output on the board.
    /// @param speed Speed of the digital output to get a reference to.
    /// @param idx Index of the digital output to get.
    /// @return A reference to an digital output on the board.
    ///
    virtual DigitalOutput& GetDigitalOutput(IOSpeed speed, size_t idx) const {

    }

    ///
    /// Get a timer from the board.
    /// @param idx Index of the timer to get.
    /// @return A reference to a timer on the board.
    ///
    virtual Timer& GetTimer(size_t idx) const {
        
    }

    ///
    /// Update all of the slow IO devices on the board.
    ///
    virtual void UpdateSlowIO() { ; /* Do nothing for now*/ }

    ///
    /// Update all of the fast IO devices on the board
    ///
    virtual void UpdateFastIO() {
        mDac.updateStream();
    }

private:
    adc::OnChipADC mAdc;       ///< ADC
    spi::HwCsSpiBus mHwSpiBus; ///< Hw SPI bus
    spi::DAC60508 mDac;        ///< DAC

    timer::BasicTimer mTimers[scProtoBoardV1Cfg.numTimers]; ///< Timers

    GPIOPin& mFastDigitalInputs[scProtoBoardV1Cfg.fastIO.numDigitalInputs];   ///< Fast digitial inputs
    GPIOPin& mFastDigitalOutputs[scProtoBoardV1Cfg.fastIO.numDigitalOutputs]; ///< Fast digitial outputs
};

}
#endif