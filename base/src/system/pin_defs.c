#include "../gpio/gpio_defs.h"

// LED pins

const GPIOPin led_pin = {
    .port = 2, // Port C
    .pin = 7  // Pin 7
};

const GPIOConf led_pin_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = OUTPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL
};

// Clock test pins

const GPIOPin mco2_pin = {
    .port = 2, // Port C
    .pin = 9  // Pin 9
};

const GPIOConf mco2_pin_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 0
};


// UART pins

const GPIOPin uart_1_tx_pin = {
    .port = 1,
    .pin = 6
};

const GPIOConf uart_1_tx_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 7
};

const GPIOPin uart_1_rx_pin = {
    .port = 1,
    .pin = 7
};

const GPIOConf uart_1_rx_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 7
};

// SPI pins

GPIOPin spi2_clk_pin = {
    .port = 1,
    .pin = 13
};

GPIOConf spi2_clk_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

GPIOPin spi2_cipo_pin = {
    .port = 1,
    .pin = 15
};

GPIOConf spi2_cipo_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

GPIOPin spi2_copi_pin = {
    .port = 1,
    .pin = 14
};

GPIOConf spi2_copi_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

GPIOPin spi2_cs0_pin = {
    .port = 1,
    .pin = 12
};

const GPIOConf spi2_cs0_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

GPIOPin spi3_clk_pin = {
    .port = 2,
    .pin = 10
};


GPIOConf spi3_clk_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

GPIOPin spi3_cipo_pin = {
    .port = 2,
    .pin = 11
};

GPIOConf spi3_cipo_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

GPIOPin spi3_copi_pin = {
    .port = 2,
    .pin = 12
};

GPIOConf spi3_copi_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

GPIOPin spi3_cs0_pin = {
    .port = 2,
    .pin = 6
};

const GPIOConf spi3_cs0_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};


GPIOPin spi3_cs1_pin = {
    .port = 2,
    .pin = 7
};

const GPIOConf spi3_cs1_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

GPIOPin spi3_cs2_pin = {
    .port = 2,
    .pin = 8
};

const GPIOConf spi3_cs2_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};
