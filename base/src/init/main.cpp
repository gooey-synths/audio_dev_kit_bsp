#include "../tests/tests.hpp"
#include <cstdlib>
#include <cstdint>

#include <FreeRTOS.h>
#include <task.h>


int main() {
    while(1) {
        vTaskDelay(1000);
    }
    test_ArduinoJSON();
    return 0;
}
