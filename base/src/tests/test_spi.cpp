#include "../spi/spi.hpp"
#include "../system/board_defs.h"

using namespace spi;

#define BUFFER_SIZE 10

uint8_t txBuff[BUFFER_SIZE];
uint8_t rxBuff[BUFFER_SIZE];


///
/// Setup SPI pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();

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
}

///
/// Test a transaction on the hardware spi bus
///
void  test_spi_hw_transaction(){
    setup_pins();

    HwSpiBus spi_bus(2);

    SpiBusConfig conf;
    conf.mFreq = 3000000U;
    conf.mPhase = 1;
    conf.mPolarity = 1;
    conf.mWordSize = 8;

    // setup tx buffer
    for(size_t i = 0; i < BUFFER_SIZE; i++){
        txBuff[i] = i;
    }

    spi_bus.configure(conf);

    spi_bus.prepare(txBuff, rxBuff, BUFFER_SIZE, 0);

    while(1){

        // setup rx buffer
        for(size_t i = 0; i < BUFFER_SIZE; i++){
            rxBuff[i] = 0;
        }

        spi_bus.transact();
        
        spi_bus.waitForCompletion();

    }

}
