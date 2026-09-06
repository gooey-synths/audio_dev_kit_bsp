#include <tests/tests.hpp>

#include <FreeRTOS.h>
#include <task.h>

int main() {
    while(1) {
        vTaskDelay(1000);
    }
}
