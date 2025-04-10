#include "../spi/spi.hpp"
#include "../uart/uart.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"

using namespace spi;

#define BUFFER_SIZE 3

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

    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}

///
/// Test a transaction on the hardware spi bus
///
void test_spi_hw_transaction(){
    setup_pins();

    HwCsSpiBus spiBus(2);

    uart::UartController uart1(1);

    SpiBusConfig conf;
    conf.mFreq = 3000000U;
    conf.mPhase = 0;
    conf.mPolarity = 1;
    conf.mWordSize = 8;
    conf.mIoSwap = true;
    conf.mMidi = 0;

    // Setup tx buffer to read device ID from DAC
    txBuff[0] = 0x81;
    txBuff[1] = 0x00;
    txBuff[2] = 0x00;

    spiBus.configure(conf);

    SpiBusConfig gotConf = spiBus.getConfiguration();

    assert(gotConf.mFreq <= conf.mFreq);
    assert(gotConf.mPhase == conf.mPhase);
    assert(gotConf.mPolarity == conf.mPolarity);
    assert(gotConf.mWordSize == conf.mWordSize);
    assert(gotConf.mIoSwap == conf.mIoSwap);
    assert(gotConf.mMidi == conf.mMidi);

    spiBus.prepare(txBuff, rxBuff, BUFFER_SIZE, 0, sizeof(*txBuff));

    while(1){
        // setup rx buffer
        for(size_t i = 0; i < BUFFER_SIZE; i++){
            rxBuff[i] = 1;
        }

        spiBus.transact();
        
        spiBus.waitForCompletion();

        uint16_t devId = (uint16_t)rxBuff[1] << 8 | rxBuff[2];
        print_buffer(&uart1, (void*)&devId, sizeof(devId), 1);


        for(int i = 0; i < 0x1FFFFF; i++); //delay...s

    }

}

///
/// Test that DMA exceptions are caught
/// @note To check for success check that exception messages are printed to UART1
///
void test_spi_hw_exceptions()
{
    bool exceptionCaught = false;
    setup_pins();
    uart::UartController uart1(1);

    // Test getting an invalid controller
    EXPECT_EXCEPTION(HwCsSpiBus badSpiBus(0));
    
    HwCsSpiBus spiBus(2);

    SpiBusConfig conf;
    conf.mFreq = 30000U;
    conf.mPhase = 0;
    conf.mPolarity = 1;
    conf.mWordSize = 8;
    conf.mIoSwap = true;
    conf.mMidi = 0;

    // Setup tx buffer to read device ID from DAC
    txBuff[0] = 0x81;
    txBuff[1] = 0x00;
    txBuff[2] = 0x00;

    spiBus.configure(conf);
    spiBus.prepare(txBuff, rxBuff, BUFFER_SIZE, 0, sizeof(*txBuff));


    spiBus.transact();
    // Test transacting while active
    EXPECT_EXCEPTION(spiBus.transact());
    spiBus.waitForCompletion();

    spiBus.transact();
    // Test configuring while active
    EXPECT_EXCEPTION(spiBus.configure(conf));
    spiBus.waitForCompletion();

    spiBus.transact();
    // Test preparing while active
    EXPECT_EXCEPTION(spiBus.prepare(txBuff, rxBuff, BUFFER_SIZE, 0, sizeof(*txBuff)));
    spiBus.waitForCompletion();
}