#include "../tests/tests.hpp"
#include <malloc.h>
#include <cstdint>

int main() {
    test_gpio_exceptions();
    test_dma_exceptions();
    test_spi_hw_exceptions();
    test_uart_exceptions();
}
