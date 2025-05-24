#include "../adc/adc.hpp"
#include "../gpio/gpio.hpp"
#include "../uart/uart.hpp"
#include "../system/board_defs.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

using namespace adc;

// See schematic for organization of ADC channels
uint8_t seq[] = {10,11,16,14,15,3,8,9}; // Define conversion sequence here, remember to initialize the ADC pins

///
/// Setup UART and ADC pins
///
static void setup_pins(){
    gpio::GPIOController* gpioController = gpio::GPIOController::getInstance();
    
    gpioController->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpioController->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);

    gpioController->setConfig(&adc1_3_pin, &adc1_3_conf);
    gpioController->setConfig(&adc1_8_pin, &adc1_8_conf);
    gpioController->setConfig(&adc1_9_pin, &adc1_9_conf);
    gpioController->setConfig(&adc1_10_pin, &adc1_10_conf);
    gpioController->setConfig(&adc1_11_pin, &adc1_11_conf);
    gpioController->setConfig(&adc1_14_pin, &adc1_14_conf);
    gpioController->setConfig(&adc1_15_pin, &adc1_15_conf);
    gpioController->setConfig(&adc1_16_pin, &adc1_16_conf);
    gpioController->setConfig(&adc1_18_pin, &adc1_18_conf);
    gpioController->setConfig(&adc1_19_pin, &adc1_19_conf);
}

///
/// ADC test. See output on UART 1 for ADC conversions.
/// @param continuous True if ADC should be tested in continuous mode
///
static void adc_test(bool continuous) {

    setup_pins();

    OnChipADC myADC(1);
    uint8_t seq_size = sizeof(seq)/sizeof(*seq);
    myADC.setSequence(seq, seq_size);

    char char_buff[12];
    int num_chars = 0;
    uart::UartController uart1(1);

    if(continuous) {
        myADC.beginConversion(true);
    }

    for(;;){
        
        if(!continuous) {
            myADC.beginConversion(false);
        }

        for(int i = 0; i < 0x4FFFFF; i++); // wait for transfer to happen

        uart1.write((char*)"============\r\n", sizeof("============\r\n"));

        // print out conversions
        for(uint8_t iConv = 0; iConv < seq_size; iConv++){
            uart1.write((char*)"ch ", sizeof("ch "));
            num_chars = sprintf(char_buff, "%d: ", seq[iConv]);
            uart1.write(char_buff, num_chars);
            num_chars = sprintf(char_buff, "%d\r\n", myADC.getConversion(iConv)>>4);
            uart1.write(char_buff, num_chars);

        }

    }
}

///
/// Test that adc single conversion is always producing new samples.
///
void test_adc_single_conversion() {

    adc_test(false);

}

///
/// Test that adc continous conversion is always producing new samples.
///
void test_adc_continuous_conversion() {

    adc_test(true);

}


///
/// Test that calling adc stop actually stops converting and can be restarted.
/// See output on UART 1 for ADC conversions, check they do not change betweend the "->".
///
void test_adc_stop() {

    setup_pins();

    OnChipADC myADC(1);
    uint8_t seq_size = sizeof(seq)/sizeof(*seq);
    myADC.setSequence(seq, seq_size);

    char char_buff[12];
    int num_chars = 0;
    uart::UartController uart1(1);

    for(;;) {
        myADC.beginConversion(true);
        for(int i = 0; i < 0x4FFFFFF; i++); // wait for transfer to happen

        myADC.stop();

        uart1.write((char*)"============\r\n", sizeof("============\r\n"));
        
        for(uint8_t iConv = 0; iConv < seq_size; iConv++){

            uint16_t prevConversion = myADC.getConversion(iConv)>>4;
            for(int i = 0; i < 0xFFFFF; i++); // Wait for a bit
            uint16_t newConversion = myADC.getConversion(iConv)>>4;

            num_chars = sprintf(char_buff, "%d: ", seq[iConv]);
            uart1.write(char_buff, num_chars);
            num_chars = sprintf(char_buff, "%d ->", prevConversion);
            uart1.write(char_buff, num_chars);
            num_chars = sprintf(char_buff, " %d\r\n", newConversion);
            uart1.write(char_buff, num_chars);

        }
        
    }
}