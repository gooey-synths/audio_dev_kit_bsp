#include "blinky.hpp"
#include <system/board_defs.h>
#include <system/tasks.h>
#include <FreeRTOS.h>
#include <task.h>

#include <cassert>

namespace blinky {

///
/// Constructor.
///
Blinky::Blinky():
    mBlinkPin(gpio::GPIOController::getInstance()->getPin(&led_pin))
{
    gpio::GPIOController::getInstance()->setConfig(&led_pin, &led_pin_conf);

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    Blinky::blinkTask,         /* Function that implements the task. */
                    "Blinky",                      /* Text name for the task. */
                    BLINKY_TASK_STACK_SIZE,  /* Stack size in words, not bytes. */
                    NULL,                    /* Parameter passed into the task. */
                    BLINKY_TASK_PRIO,          /* Priority at which the task is created. */
                    &xHandle );             /* Used to pass out the created task's handle. */

    assert(xReturned == pdPASS);

}

///
/// Set the blink period in milliseconds.
/// @param ms Desired blink period in milliseconds.
///
void Blinky::setPeriod(uint32_t ms) {
    taskENTER_CRITICAL();

    mBlinkPeriod = ms;

    taskEXIT_CRITICAL();
}

///
/// LED blinking task.
/// @param pvParameters parameters passed into the task.
///
void Blinky::blinkTask(void* pvParameters) {
    (void) pvParameters;

    while(1) {
        TheBlinky.blinkTaskBody();
    }
}

///
/// Body of blinking task.
///
void Blinky::blinkTaskBody() {
    mBlinkPin = !mBlinkPin();
    vTaskDelay(mBlinkPeriod/2);
}

Blinky TheBlinky;

} // namespace blinky

