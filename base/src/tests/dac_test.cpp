#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"
#include "../timer/basic_timer.hpp"
#include "../spi/spi.hpp"
#include "../spi/dacx050y.hpp"
#include <cassert>


using namespace timer;
using namespace spi;

constexpr uint32_t BLINK_FREQ = 2;


constexpr uint32_t DAC_FREQ = 262;
constexpr uint32_t DAC_NUM_PTS = 64;

///
/// Setup UART and LED pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&led_pin, &led_pin_conf);


    gpio_controller->setConfig(&spi2_clk_pin,  &spi2_clk_conf);
    gpio_controller->setConfig(&spi2_cipo_pin, &spi2_cipo_conf);
    gpio_controller->setConfig(&spi2_copi_pin, &spi2_copi_conf);
    gpio_controller->setConfig(&spi2_cs0_pin,  &spi2_cs0_conf);

    gpio_controller->setConfig(&spi3_clk_pin,  &spi3_clk_conf);
    gpio_controller->setConfig(&spi3_cipo_pin, &spi3_cipo_conf);
    gpio_controller->setConfig(&spi3_copi_pin, &spi3_copi_conf);
    gpio_controller->setConfig(&spi3_cs0_pin,  &spi3_cs0_conf);
    gpio_controller->setConfig(&spi3_cs1_pin,  &spi3_cs1_conf);
    gpio_controller->setConfig(&spi3_cs2_pin,  &spi3_cs2_conf);

    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}

///
/// Test a basic timer by blinking an LED and printing to UART.
///
void test_dac_board() {
    setup_pins();


    static gpio::Pin led = gpio::GPIOController::getInstance()->getPin(&led_pin);
    static uart::UartController uart1(1);
    static const char* msg = "Hello timer!\r\n";



    spi::HwCsSpiBus spiBus(2);

    static spi::DAC60508 dac(spiBus, 0);

    dac.setup();

    // Check that register mode works
    dac.setMode(eDACx050yMode::DACx050y_REG_MODE);
    assert(dac.getMode() == eDACx050yMode::DACx050y_REG_MODE);

    uint16_t devId = dac.ReadReg(eDACx050yRegAddr::DACx050y_ID);
    assert(devId == 10390);

    // Set gain register for gain of 2 and div of 2.
    dac.WriteReg(eDACx050yRegAddr::DACx050y_GAIN, 0x1FF);

    uint16_t gainReg = dac.ReadReg(eDACx050yRegAddr::DACx050y_GAIN);
    assert(gainReg == 0x1FF);

    // Check that stream mode works
    dac.setMode(eDACx050yMode::DACx050y_STREAM_MODE);
    assert(dac.getMode() == eDACx050yMode::DACx050y_STREAM_MODE);

    InterruptFunctionPtr blinkLambda = [] () {
        led = !led();
    };

    InterruptFunctionPtr dacLambda = [] () {
        static uint16_t counter = 0;


        for(size_t i=0; i<8; i++) {
            dac.setStreamVal(i, counter * (4096/DAC_NUM_PTS)<<4);
        }

        dac.updateStream();

        counter++;
        if(counter == DAC_NUM_PTS*4){
            counter = 0;
        }

    };


    BasicTimer timer6(BASIC_TIMER_6);
    BasicTimer timer7(BASIC_TIMER_7);

    timer6.setFreq(BLINK_FREQ*2);
    timer7.setFreq(DAC_FREQ*DAC_NUM_PTS);

    timer6.setInterrupt(blinkLambda);
    timer7.setInterrupt(dacLambda);

    timer6.start(false);
    timer7.start(false);

    while(1);
}

