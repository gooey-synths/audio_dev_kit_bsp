#include "../adc/adc.hpp"
#include "../gpio/gpio.hpp"
#include "../uart/uart.hpp"
#include "../system/board_defs.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

using namespace adc;

// See schematic for organization of ADC channels
uint8_t seq[] = {10,11,14,15,7,8,9,16}; // Define conversion sequence here, remember to initialize the ADC pins

///
/// Setup UART and ADC pins
///
static void setup_pins(){
    gpio::GPIOController* gpioController = gpio::GPIOController::getInstance();
    
    gpioController->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpioController->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);

    gpioController->setConfig(&adc1_7_pin, &adc1_7_conf);
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

void test_adc_single_conversion(){

    OnChipADC myADC(1);

    char char_buff[12];
    int num_chars = 0;
    uart::UartController uart1(1);
    uint8_t seq_size = sizeof(seq)/sizeof(*seq);
    myADC.setSequence(seq, seq_size);

    setup_pins();

    for(;;){
        myADC.beginSingleConversion();

        for(int i = 0; i < 0x4FFFFF; i++); // wait fo transfer to happen

        uart1.write("============\r\n", sizeof("============\r\n"));

        // print out conversions
        for(uint8_t iConv = 0; iConv < seq_size; iConv++){
            uart1.write("ch ", sizeof("ch "));
            num_chars = sprintf(char_buff, "%d: ", seq[iConv]);
            uart1.write(char_buff, num_chars);
            num_chars = sprintf(char_buff, "%d\r\n", myADC.getConversion(iConv)>>2);
            uart1.write(char_buff, num_chars);

        }

    }

}

void test_adc_continuous_conversion(){
   
    OnChipADC myADC(1);

    char char_buff[12];
    int num_chars = 0;
    uart::UartController uart1(1);
    uint8_t seq_size = sizeof(seq)/sizeof(*seq);
    myADC.setSequence(seq, seq_size);

    setup_pins();

    myADC.beginContinuousConversion();

    for(;;){
        //myADC.beginSingleConversion();

        for(int i = 0; i < 0x1FFFFF; i++); // wait fo transfer to happen

        uart1.write("============\r\n", sizeof("============\r\n"));

        // print out conversions
        for(uint8_t iConv = 0; iConv < seq_size; iConv++){
            uart1.write("ch ", sizeof("ch "));
            num_chars = sprintf(char_buff, "%d: ", seq[iConv]);
            uart1.write(char_buff, num_chars);
            num_chars = sprintf(char_buff, "%d\r\n", myADC.getConversion(iConv)>>2);
            uart1.write(char_buff, num_chars);

        }

    }

}