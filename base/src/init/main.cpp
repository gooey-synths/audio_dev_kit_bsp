#include <stdint.h>
#include "../tests/tests.hpp"

uint16_t b = 42;
char message[] = "Hello World!\r\n";

int main(){

    // Tests can only be run one at a time.
    test_gpio_blinky();
    test_gpio_mco2();

}