#include "../gpio/gpio.hpp"
#include "../uart/uart.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"

using namespace uart;

#define TEST_UART_BAUD 9600

// Change this to change the frame of the test
const UartFrameConfig TEST_FRAME = {
    .mWordLength = EIGHT_BITS,
    .mParity = false,
    .mParityOdd = false,
    .mStopBits = ONE_STOP_BIT
};

///
/// Setup the UART pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}

///
/// Prints "Hello World" to UART1
/// @note To check for success check uart1 pins at @ref TEST_UART_BAUD baud
///
void test_uart_hello_world(){
    static char message[] = "Hello World!\r\n";
    setup_pins();
    UartController uart_1(1);

    while(1){
        uart_1.setBaudRate(TEST_UART_BAUD);
        uart_1.write(message, sizeof(message));
        for(int i = 0; i < 0x1FFFFF; i++); //delay...
    }

}

///
/// Test echo capabilities
/// @note To check for success open UART1 and ensure characters are echoed.
/// @note Baud rate should be @ref TEST_UART_BAUD baud
///
void test_uart_echo(){
    setup_pins();
    UartController uart_1(1);
    uart_1.setBaudRate(TEST_UART_BAUD);
    uart_1.setFraming(TEST_FRAME);
    while(1){
        char in = uart_1.readChar();
        uart_1.write(&in, sizeof(in));
    }

}

///
/// Test that UART exceptions are caught
/// @note To check for success check that exception messages are printed to UART1
///
void test_uart_exceptions(){
    bool exceptionCaught = false;
    setup_pins();
    uart::UartController uart1(1);


    // Test invalid controller
    EXPECT_EXCEPTION(UartController badUart(0));
}