#include "physical_board.hpp"
#include "system/board_defs.h"

namespace board {

const char* const ProtoBoardV1::scInvalidSpeedMsg = "Invalid IO speed";
const char* const ProtoBoardV1::scInvalidIOMsg = "Invalid IO index";
const char* const ProtoBoardV1::scInvalidTimerMsg = "Invalid Timer index";

///
/// Constructor.
///
ProtoBoardV1::ProtoBoardV1() :
    mAdc(1),
    mHwSpiBus(2),
    mDac(mHwSpiBus, 0),
    mTimerHw {
        timer::BASIC_TIMER_6,
        timer::BASIC_TIMER_7
    },
    mTimers {
        mTimerHw[0],
        mTimerHw[1]
    },
    mFastDigitalInputs {
        gpio::GPIOController::getInstance()->getPin(&dio_0_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_1_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_2_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_3_pin),
    },
    mFastDigitalOutputs {
        gpio::GPIOController::getInstance()->getPin(&dio_4_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_5_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_6_pin),
        gpio::GPIOController::getInstance()->getPin(&dio_7_pin),
    },
    mFastAnalogInputs {
        PhysicalAnalogInput(mAdc, 0),
        PhysicalAnalogInput(mAdc, 1),
        PhysicalAnalogInput(mAdc, 2),
        PhysicalAnalogInput(mAdc, 3),
        PhysicalAnalogInput(mAdc, 4),
        PhysicalAnalogInput(mAdc, 5),
        PhysicalAnalogInput(mAdc, 6),
        PhysicalAnalogInput(mAdc, 7),
    },
    mFastAnalogOutputs {
        PhysicalAnalogOutput(mDac, 0),
        PhysicalAnalogOutput(mDac, 1),
        PhysicalAnalogOutput(mDac, 2),
        PhysicalAnalogOutput(mDac, 3),
        PhysicalAnalogOutput(mDac, 4),
        PhysicalAnalogOutput(mDac, 5),
        PhysicalAnalogOutput(mDac, 6),
        PhysicalAnalogOutput(mDac, 7),
    }
    {
        // Setup Digital IOs
        gpio::GPIOController* gpioController = gpio::GPIOController::getInstance();
        gpioController->setConfig(&dio_0_pin, &dio_input_conf);
        gpioController->setConfig(&dio_1_pin, &dio_input_conf);
        gpioController->setConfig(&dio_2_pin, &dio_input_conf);
        gpioController->setConfig(&dio_3_pin, &dio_input_conf);
        gpioController->setConfig(&dio_4_pin, &dio_output_conf);
        gpioController->setConfig(&dio_5_pin, &dio_output_conf);
        gpioController->setConfig(&dio_6_pin, &dio_output_conf);
        gpioController->setConfig(&dio_7_pin, &dio_output_conf);

        // Setup SPI pins
        gpioController->setConfig(&spi2_clk_pin,  &spi2_clk_conf);
        gpioController->setConfig(&spi2_cipo_pin, &spi2_cipo_conf);
        gpioController->setConfig(&spi2_copi_pin, &spi2_copi_conf);
        gpioController->setConfig(&spi2_cs0_pin,  &spi2_cs0_conf);

        // Setup DAC
        mDac.setup();
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
        mAdc.setSequence(const_cast<uint8_t*>(scProtoBoardV1AdcSeq), seq_size);
        mAdc.beginConversion(true);
    }

}