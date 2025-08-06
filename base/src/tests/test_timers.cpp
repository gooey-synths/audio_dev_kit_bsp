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

constexpr uint32_t BLINK_TIMER_FREQ = 4800;
constexpr uint32_t BLINK_FREQ = 2;

static void blinkIsr() {
    static bool on_or_off = true;
    static gpio::Pin& led = gpio::GPIOController::getInstance()->getPin(&led_pin);
    static uint32_t counter = 0;

    if(counter == BLINK_TIMER_FREQ/(BLINK_FREQ*2)) {
        led = on_or_off;
        on_or_off = !on_or_off;
        counter = 0;
    } else {
        counter++;
    }
}

void test_basic_timer_exceptions() {

}

void test_basic_timer() {
    BasicTimer timer6(6, 100000);

    timer6.setFreq(BLINK_TIMER_FREQ);

    timer6.start();
}