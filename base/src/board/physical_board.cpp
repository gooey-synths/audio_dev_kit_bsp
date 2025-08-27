#include "physical_board.hpp"
#include "system/board_defs.h"

namespace board {

const char* const ProtoBoardV1::scInvalidSpeedMsg = "Invalid IO speed";

///
/// Constructor.
///
ProtoBoardV1::ProtoBoardV1() :
    mAdc(1),
    mHwSpiBus(2),
    mDac(mHwSpiBus, 0),
    mTimers {
        timer::BasicTimer(timer::BASIC_TIMER_6),
        timer::BasicTimer(timer::BASIC_TIMER_7)
    },
    mFastDigitalInputs {
        dio_0_pin,
        dio_1_pin,
        dio_2_pin,
        dio_3_pin,
    },
    mFastDigitalOutputs {
        dio_4_pin,
        dio_5_pin,
        dio_6_pin,
        dio_7_pin,
    } {
        // Setup Digital IOs
        gpio::GPIOController* gpioController = gpio::GPIOController::getInstance();
        for(GPIOPin& pin : mFastDigitalInputs) {
            gpioController->setConfig(&pin, &dio_input_conf);
        }
        for(GPIOPin& pin : mFastDigitalOutputs) {
            gpioController->setConfig(&pin, &dio_output_conf);
        }

        // Setup DAC
        mDac.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);
        // Set gain to 2 and divide ref by 2
        mDac.WriteReg(spi::eDACx050yRegAddr::DACx050y_GAIN, 0x1FF);

        mDac.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);

        // Setup ADC
        gpioController->setConfig(&adc1_3_pin, &adc1_conf);
        gpioController->setConfig(&adc1_8_pin, &adc1_conf);
        gpioController->setConfig(&adc1_9_pin, &adc1_conf);
        gpioController->setConfig(&adc1_10_pin, &adc1_conf);
        gpioController->setConfig(&adc1_11_pin, &adc1_conf);
        gpioController->setConfig(&adc1_14_pin, &adc1_conf);
        gpioController->setConfig(&adc1_15_pin, &adc1_conf);
        gpioController->setConfig(&adc1_16_pin, &adc1_conf);
        gpioController->setConfig(&adc1_18_pin, &adc1_conf);
        gpioController->setConfig(&adc1_19_pin, &adc1_conf);
    
        uint8_t seq_size = sizeof(scProtoBoardV1AdcSeq)/sizeof(*scProtoBoardV1AdcSeq);
        mAdc.setSequence(scProtoBoardV1AdcSeq, seq_size);
        mAdc.beginConversion(true);
    }

}