// These are defined as extern so that they do not include each other

// GPIO tests
extern void test_gpio_blinky();
extern void test_gpio_mco2();
extern void test_gpio_exceptions();

// DMA tests
extern void test_dma_single_buffer();
extern void test_dma_exceptions();

// UART test
extern void test_uart_hello_world();
extern void test_uart_echo();
extern void test_uart_exceptions();

// SPI tests
extern void test_spi_hw_transaction();
extern void test_DAC60508();
extern void test_spi_hw_exceptions();
