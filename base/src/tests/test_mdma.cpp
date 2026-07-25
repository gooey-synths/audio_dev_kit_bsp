#include "../gpio/gpio.hpp"
#include "../uart/uart.hpp"
#include "../mdma/mdma.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"
#include "stdio.h"
#include "string.h"

#define TEST_BUFFER_STIZE 32

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
void test_mdma_single_buffer(){
    setup_pins();
    UartController uart_1(USART1, 1);

    uint16_t srcBuffer[TEST_BUFFER_STIZE];
    uint16_t dstBuffer[TEST_BUFFER_STIZE];
    
    // Fill source buffer
    for(int iElem = 0; iElem < TEST_BUFFER_STIZE; iElem++){
        srcBuffer[iElem] = iElem;
    }

    // Get the controler and a channel.
    mdma::MDMAController* mdma_controller = mdma::MDMAController::getInstance();
    mdma::MDMAChannel* mdma_channel = mdma_controller->getChannel(0);
    
    // Create a list node to define the transfer
    mdma::ListNode node;
    node.setSource((void*) &srcBuffer, sizeof(srcBuffer[0]), sizeof(srcBuffer[0]), true);
    node.setDestination((void*) &dstBuffer, sizeof(dstBuffer[0]), sizeof(dstBuffer[0]), true);
    node.setNumberData(sizeof(srcBuffer), sizeof(srcBuffer[0]));
    node.setTrigger(0, true, mdma::BUF_TRANS);

    while(1){

        // Reset channel.
        mdma_channel->disable();
        mdma_channel->configureTransfer(&node);
        mdma_channel->enable();

        // Clear destination buffer
        for(int iElem = 0; iElem < TEST_BUFFER_STIZE; iElem++){
            dstBuffer[iElem] = 0;
        }
        // Print source buffer before transfer
        uart_1.write((char*)"srcBuffer: \r\n", sizeof("srcBuffer: \r\n"));
        print_buffer(&uart_1, srcBuffer, sizeof(*srcBuffer), sizeof(srcBuffer));

        // Transfer using single triggers
        for(int i = 0; i < TEST_BUFFER_STIZE; i++){
            mdma_channel->trigger();
            volatile uint32_t error = mdma_channel->getError();
            for(int i = 0; i < 0x1F; i++); // wait fo transfer to happen, not sure if this is needed.
        }
        
        // Write destination buffer after transfer
        uart_1.write((char*)"dstBuffer: \r\n", sizeof("dstBuffer: \r\n"));
        print_buffer(&uart_1, dstBuffer, sizeof(*dstBuffer), sizeof(dstBuffer));
        
        for(int i = 0; i < 0x7FFFFF; i++); //delay...
    }

}