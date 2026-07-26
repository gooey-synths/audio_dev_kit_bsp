#include <gpio/gpio.hpp>
#include <uart/uart.hpp>
#include <mdma/mdma.hpp>
#include <system/board_defs.h>
#include "test_helper.hpp"

#define TEST_BUFFER_SIZE 32

///
/// Setup UART pins
///
static void setup_pins() {
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();

    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}

///
/// Test single trigger buffer transfer capabilites 
/// @note To check for success check both buffers printed on UART1 are equal
///
void test_mdma_single_buffer() {
    setup_pins();
    uart::UartController uart1(1);

    uint16_t srcBuffer[TEST_BUFFER_SIZE];
    uint16_t dstBuffer[TEST_BUFFER_SIZE];

    // Fill source buffer
    for(int iElem = 0; iElem < TEST_BUFFER_SIZE; iElem++) {
        srcBuffer[iElem] = iElem;
    }

    // Get the controler and a channel.
    mdma::MDMAController* mdma_controller = mdma::MDMAController::getInstance();
    mdma::MDMAChannel* mdma_channel = mdma_controller->getChannel(0);

    // Create a list node to define the transfer
    mdma::ListNode node;
    node.setSource((void*) &srcBuffer, sizeof(srcBuffer[0]), sizeof(srcBuffer[0]), false);
    node.setDestination((void*) &dstBuffer, sizeof(dstBuffer[0]), sizeof(dstBuffer[0]), false);
    node.setNumberData(sizeof(srcBuffer), sizeof(srcBuffer[0]));
    node.setTrigger(0, true, mdma::BUF_TRANS);

    while(1) {
        // Reset channel.
        mdma_channel->disable();
        mdma_channel->configureTransfer(&node);
        mdma_channel->enable();

        // Clear destination buffer
        for(int iElem = 0; iElem < TEST_BUFFER_SIZE; iElem++) {
            dstBuffer[iElem] = 0;
        }
        // Print source buffer before transfer
        uart1.write((char*)"srcBuffer: \r\n", sizeof("srcBuffer: \r\n"));
        print_buffer(&uart1, srcBuffer, sizeof(*srcBuffer), sizeof(srcBuffer));

        // Transfer using single triggers
        for(int i = 0; i < TEST_BUFFER_SIZE; i++) {
            mdma_channel->trigger();
            for(int i = 0; i < 0x1FFF; i++); // wait for transfer to happen.
        }

        // Write destination buffer after transfer
        uart1.write((char*)"dstBuffer: \r\n", sizeof("dstBuffer: \r\n"));
        print_buffer(&uart1, dstBuffer, sizeof(*dstBuffer), sizeof(dstBuffer));

        for(int i = 0; i < 0x7FFFFFF; i++); // delay...
    }
}
