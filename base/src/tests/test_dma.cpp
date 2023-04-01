#include "../gpio/gpio.hpp"
#include "../uart/uart.hpp"
#include "../dma/dma.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#define TEST_BUFFER_STIZE 42
#define TEST_PRINT_WIDTH 16

#define TEST_CONTROLLER 1
#define TEST_CHANNEL 5

uint16_t srcBuffer[TEST_BUFFER_STIZE];
uint16_t dstBuffer[TEST_BUFFER_STIZE];

///
/// Setup UART pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}


///
/// Test single trigger buffer transfer capabilites 
/// @note To check for success check both buffers printed on UART1 are equal
///
void test_dma_single_buffer(){
    setup_pins();
    UartController uart_1(USART1, 1);

    
    // Fill source buffer
    for(int iElem = 0; iElem < TEST_BUFFER_STIZE; iElem++){
        srcBuffer[iElem] = iElem+1;
        dstBuffer[iElem] = 0;
    }

    // Get the controler and a channel.
    dma::DmaController* dma_controller = dma::DmaController::getInstance(TEST_CONTROLLER);
    dma::DmaController::DmaChannel* channel = dma_controller->claimChannel(TEST_CHANNEL);

    while(1){
        
        // Reset channel.
        channel->disable();
        //Define the transfer
        channel->setTransferType(dma::MEM2MEM, 1);
        channel->setDest(dstBuffer, sizeof(*dstBuffer), 1);
        channel->setSource(srcBuffer, sizeof(*srcBuffer), 1);
        channel->setNumTransfers(TEST_BUFFER_STIZE, 0);

        // Clear destination buffer
        for(int iElem = 0; iElem < TEST_BUFFER_STIZE; iElem++){
            dstBuffer[iElem] = 0;
        }
        // Print source buffer before transfer
        uart_1.write((char*)"srcBuffer: \r\n", sizeof("srcBuffer: \r\n"));
        print_buffer(&uart_1, srcBuffer, sizeof(*srcBuffer), sizeof(srcBuffer));

        // Transfer using software trigger
        channel->begin();
        for(int i = 0; i < 0x1FFFFF; i++); // wait fo transfer to happen, not sure if this is needed.
        
        // Write destination buffer after transfer
        uart_1.write((char*)"dstBuffer: \r\n", sizeof("dstBuffer: \r\n"));
        print_buffer(&uart_1, dstBuffer, sizeof(*dstBuffer), sizeof(dstBuffer));
        
        for(int i = 0; i < 0x7FFFFF; i++); //delay...
    }

}