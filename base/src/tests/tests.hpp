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

// ADC tests
extern void test_adc_single_conversion();
extern void test_adc_continuous_conversion();
extern void test_adc_stop();
extern void test_adc_exceptions();

// SPI tests
extern void test_spi_hw_transaction();
extern void test_spi_hw_exceptions();
extern void test_DAC60508();

// Timer tests
extern void test_basic_timer_exceptions();
extern void test_basic_timer_continuous();
extern void test_basic_timer_oneshot();
extern void test_basic_timer_accuracy();

void test_dac_board();