#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"

using namespace gpio;

///
/// Setup UART pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
}


///
/// Test blinking the light on the board
/// @note To check for success look for a blinking led
///
void test_gpio_blinky(){
    GPIOController* gpio_controller = GPIOController::getInstance();
    gpio_controller->setConfig(&led_pin, &led_pin_conf);
    Pin led = gpio_controller->getPin(&led_pin);

    while(1){
        led = true;
        for(int i = 0; i < 0x1FFFFF; i++); //delay...
        led = false;
        for(int i = 0; i < 0x1FFFFF; i++); //delay...
    }
}

///
/// Test to see what the CPU frequency is
/// @note This also checks that the GPIO speed and alternate function can be set
/// @note To check for success measure MCO2 pin, CPU frequency is 10x the MCO2 pin
///
void test_gpio_mco2(){
    GPIOController* gpio_controller = GPIOController::getInstance();
    gpio_controller->setConfig(&mco2_pin, &mco2_pin_conf);

    while(1){
        ; // Do nothing
    }
}

///
/// Test that GPIO exceptions are caught
/// @note To check for success check that exception messages are printed to UART1
///
void test_gpio_exceptions(){
    bool exceptionCaught = false;

    const GPIOPin good_pin = {
        .port = 2, // Port C
        .pin = 9  // Pin 9
    };

    GPIOConf good_conf = {
        .type = PUSH_PULL_TYPE,
        .mode = ALTERNATE,
        .speed = VERY_HIGH_SPEED,
        .pull = NO_PULL,
        .alternate_function = 0
    };

    GPIOPin my_pin = good_pin;
    GPIOConf my_pin_conf = good_conf;

    setup_pins();
    uart::UartController uart1(1);
    GPIOController* gpio_controller = GPIOController::getInstance();

    // test invalid pin
    EXPECT_EXCEPTION(gpio_controller->getPin(NULL));

    // test invalid port
    my_pin.port = 255;
    EXPECT_EXCEPTION(gpio_controller->setConfig(&my_pin, &my_pin_conf));
    my_pin = good_pin;
    my_pin_conf = good_conf;

    // test invalid pin
    my_pin.pin = 255;
    EXPECT_EXCEPTION(gpio_controller->setConfig(&my_pin, &my_pin_conf));
    my_pin = good_pin;
    my_pin_conf = good_conf;

    // test invalid alt
    my_pin_conf.alternate_function = 255;
    EXPECT_EXCEPTION(gpio_controller->setConfig(&my_pin, &my_pin_conf));
    my_pin = good_pin;
    my_pin_conf = good_conf;

}