// These are defined as extern so that they do not include each other

// GPIO tests
extern void test_gpio_blinky();
extern void test_gpio_mco2();

// DMA tests
extern void test_dma_single_buffer();

// UART test
extern void test_uart_hello_world();
extern void test_uart_echo();

// SPI tests
extern void test_spi_hw_transaction();