#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"
#include "test_helper.hpp"
#include "../timer/basic_timer.hpp"

using namespace timer;

constexpr uint32_t BLINK_FREQ = 2;
constexpr uint32_t PRINT_FREQ = 1;

///
/// Setup UART and LED pins
///
static void setup_pins(){
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();
    
    gpio_controller->setConfig(&uart_1_tx_pin, &uart_1_tx_conf);
    gpio_controller->setConfig(&uart_1_rx_pin, &uart_1_rx_conf);
    gpio_controller->setConfig(&led_pin, &led_pin_conf);
}

///
/// Test basic timer exceptions
///
void test_basic_timer_exceptions() {
    setup_pins();

    bool exceptionCaught = false;
    uart::UartController uart1(1);

    // Test bad timer index.
    EXPECT_EXCEPTION(
        BasicTimer badTimerIdx(static_cast<eBasicTimerNumber>(0xFF));
    );

    const eBasicTimerNumber timerIdx = BASIC_TIMER_6;

    BasicTimer timer6(timerIdx);

    // Test double claim timer.
    EXPECT_EXCEPTION(
        BasicTimer alreadyClaimedTimer(timerIdx);
    );
}

///
/// Test a basic timer by blinking an LED and printing to UART.
///
void test_basic_timer_continuous() {
    static gpio::Pin led = gpio::GPIOController::getInstance()->getPin(&led_pin);
    static uart::UartController uart1(1);
    static const char* msg = "Hello timer!\r\n";

    InterruptFunctionPtr blinkLambda = [] () {
        led = !led();
    };

    InterruptFunctionPtr printLambda = [] () {
        uart1.write(const_cast<char*>(msg), strlen(msg));
    };

    setup_pins();

    BasicTimer timer6(BASIC_TIMER_6);
    BasicTimer timer7(BASIC_TIMER_7);

    timer6.setFreq(BLINK_FREQ*2);
    timer7.setFreq(PRINT_FREQ);

    timer6.setInterrupt(blinkLambda);
    timer7.setInterrupt(printLambda);

    timer6.start(false);
    timer7.start(false);

    while(1);
}

///
/// Test a basic timer one shot operation by setting the led to on once.
///
void test_basic_timer_oneshot() {
    static gpio::Pin led = gpio::GPIOController::getInstance()->getPin(&led_pin);

    led = false;

    InterruptFunctionPtr blinkLambda = [] () {
        led = !led();
    };

    setup_pins();

    BasicTimer timer7(BASIC_TIMER_7);

    timer7.setFreq(BLINK_FREQ*2);

    timer7.setInterrupt(blinkLambda);

    timer7.start(true);

    while(1);
}

///
/// Test the accuracy of a basic timer.
/// @note To check for success compare frequency on PE4 with frequency printed to UART.
///
void test_basic_timer_accuracy() {
    static uart::UartController uart1(1);

    static GPIOPin freq_out_pin = {
        .port = 4, // Port E
        .pin = 4  // Pin 4
    };

    static GPIOConf freq_out_pin_conf = {
        .type = PUSH_PULL_TYPE,
        .mode = OUTPUT_MODE,
        .speed = VERY_HIGH_SPEED,
        .pull = NO_PULL
    };

    gpio::GPIOController::getInstance()->setConfig(&freq_out_pin, &freq_out_pin_conf);
    static gpio::Pin freq_out = gpio::GPIOController::getInstance()->getPin(&freq_out_pin);

    InterruptFunctionPtr freqLambda = [] () {
        freq_out = !freq_out();
    };

    static constexpr uint32_t TARGET_FREQ = APB1_TARGET / 1000;

    setup_pins();

    BasicTimer timer7(BASIC_TIMER_7);

    timer7.setFreq(TARGET_FREQ*2);

    timer7.setInterrupt(freqLambda);

    timer7.start(true);

    char buf[30];
    size_t len;

    len = snprintf(buf, sizeof(buf), "Target freq: %d Hz\r\n", TARGET_FREQ);
    uart1.write(buf, len);
    len = snprintf(buf, sizeof(buf), "Calculated freq: %d Hz\r\n", timer7.getFreq()/2);
    uart1.write(buf, len);
    while(1);
}
