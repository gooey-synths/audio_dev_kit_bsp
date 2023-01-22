#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"

using namespace gpio;

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