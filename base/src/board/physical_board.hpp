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
#include "usb/usb_serial.hpp"

namespace board {

///
/// Class for physical digital output
///
class PhysicalDigitalOutput : public DigitalOutput {
public:
    ///
    /// Constructor
    /// @param out Pin to use for digital output
    ///
    PhysicalDigitalOutput(gpio::Pin out) :
    mOut(out) { ; /* Do nothing */}

    virtual void SetValue(bool val) { mOut = val; }

private:
    gpio::Pin mOut;  ///< GPIO to use for output.
};

///
/// Class for physical digital input
///
class PhysicalDigitalInput : public DigitalInput {
public:
    ///
    /// Constructor
    /// @param out Pin to use for digital input
    ///
    PhysicalDigitalInput(gpio::Pin in) :
    mIn(in) { ; /* Do nothing */}

    virtual bool GetValue() { return mIn(); }

private:
    gpio::Pin mIn;  ///< GPIO to use for input.
};

///
/// Class for physical analog output
///
class PhysicalAnalogOutput : public AnalogOutput {
public:

    ///
    /// Constructor.
    /// @param dac DAC to use for analog output
    /// @param idx Index of the DAC channel to use
    ///
    PhysicalAnalogOutput(spi::IDACx050y& dac, size_t idx) :
    mDac(dac), mIdx(idx) { ; /* Do nothing */}

    virtual void SetValue(BoardData val) { mDac.setStreamVal(mIdx, val); }

private:
    spi::IDACx050y& mDac; ///< Reference to DAC.
    size_t mIdx; ///< DAC index.
};

///
/// Class for physical analog input
///
class PhysicalAnalogInput : public AnalogInput {
public:
    ///
    /// Constructor.
    /// @param dac ADC to use for analog input
    /// @param idx Index of the ADC channel to use
    ///
    PhysicalAnalogInput(adc::IOnChipADC& adc, size_t idx) :
    mAdc(adc), mIdx(idx) { ; /* Do nothing */}

    virtual BoardData GetValue() { return mAdc.getConversion(mIdx); }

private:
    adc::IOnChipADC& mAdc; ///< Reference to ADC.
    size_t mIdx; ///< ADC index.
};

///
/// Class for physical timer
///
class PhysicalTimer : public Timer {
public:
    ///
    /// Constructor
    /// @param timer Timer instance to use.
    /// 
    PhysicalTimer(timer::BasicTimer& timer) :
    mTimer(timer) { ; /* Do nothing */}

    virtual void SetFrequency(float frequency) { mTimer.setFreq(static_cast<uint32_t>(frequency)); }

    virtual void SetCallback(CallbackFunc callback) { mTimer.setInterrupt(callback); }

    virtual void Start() { mTimer.start(false); }

    virtual void Stop() { mTimer.stop(); }
private:
    timer::BasicTimer& mTimer; ///< Reference to basic timer.
};

///
/// Class for defining a board for the initial prototype board
///
class ProtoBoardV1 : public BoardInterface {
public:

    ProtoBoardV1();

    /// Sequence of ADC channels that maps to physical layout.
    static constexpr uint8_t scProtoBoardV1AdcSeq[] = {10,11,16,14,15,3,8,9};

    static const char* const scInvalidSpeedMsg; ///< Invalid IO speed error message
    static const char* const scInvalidIOMsg; ///< Invalid IO index error message
    static const char* const scInvalidTimerMsg; ///< Invalid timer index error message

    /// Board configuration of initial prototype board
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
        .numTimers = 1,
    };

    ///
    /// Get the board configuration.
    /// @return The board configuration.
    ///
    virtual BoardConfig GetBoardConfig() { return scProtoBoardV1Cfg; }

    virtual AnalogInput& GetAnalogInput(IOSpeed speed, size_t idx) {
        switch (speed) {
        case IOSpeed::SLOW:
            // No slow IO supported at this moment.
            throw scInvalidIOMsg;

        case IOSpeed::FAST:
            if(idx >= scProtoBoardV1Cfg.fastIO.numAnalogInputs) {
                throw scInvalidIOMsg;
            }
            return mFastAnalogInputs[idx];

        default:
            throw scInvalidSpeedMsg;
        }
    }

    virtual AnalogOutput& GetAnalogOutput(IOSpeed speed, size_t idx) {
        switch (speed) {
        case IOSpeed::SLOW:
            // No slow IO supported at this moment.
            throw scInvalidIOMsg;
 
        case IOSpeed::FAST:
            if(idx >= scProtoBoardV1Cfg.fastIO.numAnalogOutputs) {
                throw scInvalidIOMsg;
            }
            return mFastAnalogOutputs[idx];

        default:
            throw scInvalidSpeedMsg;
        }
    }

    virtual DigitalInput& GetDigitalInput(IOSpeed speed, size_t idx) {
        switch (speed) {
        case IOSpeed::SLOW:
            // No slow IO supported at this moment.
            throw scInvalidIOMsg;
 
        case IOSpeed::FAST:
            if(idx >= scProtoBoardV1Cfg.fastIO.numDigitalInputs) {
                throw scInvalidIOMsg;
            }
            return mFastDigitalInputs[idx];

        default:
            throw scInvalidSpeedMsg;
        }
    }

    virtual DigitalOutput& GetDigitalOutput(IOSpeed speed, size_t idx) {
        switch (speed) {
        case IOSpeed::SLOW:
            // No slow IO supported at this moment.
            throw scInvalidIOMsg;
 
        case IOSpeed::FAST:
            if(idx >= scProtoBoardV1Cfg.fastIO.numDigitalOutputs) {
                throw scInvalidIOMsg;
            }
            return mFastDigitalOutputs[idx];

        default:
            throw scInvalidSpeedMsg;
        }
    }

    virtual Timer& GetTimer(size_t idx) {
        if(idx >= scProtoBoardV1Cfg.numTimers) {
            throw scInvalidTimerMsg;
        }
        return mTimers[idx];
    }

    virtual CommunicationInterface* GetComm(size_t idx) {
        return &usb::USBSerial::getInstance().getInterface(idx);
    }

    virtual void UpdateSlowIO() { ; /* Do nothing for now */ }

    virtual void UpdateFastIO() {
        mDac.updateStream();
    }

private:
    adc::OnChipADC mAdc;       ///< ADC
    spi::HwCsSpiBus mHwSpiBus; ///< Hw SPI bus
    spi::DAC60508 mDac;        ///< DAC
    timer::BasicTimer mTimerHw[scProtoBoardV1Cfg.numTimers]; /// Timer HW

    PhysicalTimer mTimers[scProtoBoardV1Cfg.numTimers]; ///< Timers

    PhysicalDigitalInput mFastDigitalInputs[scProtoBoardV1Cfg.fastIO.numDigitalInputs];   ///< Fast digitial inputs
    PhysicalDigitalOutput mFastDigitalOutputs[scProtoBoardV1Cfg.fastIO.numDigitalOutputs]; ///< Fast digitial outputs

    PhysicalAnalogInput mFastAnalogInputs[scProtoBoardV1Cfg.fastIO.numAnalogInputs]; ///< Fast analog inputs
    PhysicalAnalogOutput mFastAnalogOutputs[scProtoBoardV1Cfg.fastIO.numAnalogOutputs]; ///< Fast analog outputs
};

} // namspace board

#endif // #ifndef PHYSICAL_BOARD_HPP
