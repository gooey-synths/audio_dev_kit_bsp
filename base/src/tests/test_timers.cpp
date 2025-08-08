#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"
#include "../timer/basic_timer.hpp"

using namespace timer;

///
/// Setup UART pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
    gpio_controller->setConfig(&led_pin, &led_pin_conf);
}

constexpr uint32_t BLINK_FREQ = 2;

///
/// Timer callback to toggle LED.
///
static void blinkIsr() {
    static gpio::Pin led = gpio::GPIOController::getInstance()->getPin(&led_pin);

    led = !led();
}

void test_basic_timer_exceptions() {

}

///
/// Test a basic timer by blinking and LED.
///
void test_basic_timer_blinky() {
    setup_pins();

    BasicTimer timer6(6);

    timer6.setFreq(BLINK_FREQ*2);

    timer6.SetInterrupt(blinkIsr);

    timer6.start(false);

    while(1);
}