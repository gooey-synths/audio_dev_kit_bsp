#include "../gpio/gpio_defs.h"

// LED pins

const GPIOPin led_pin = {
    .port = 1, // Port B
    .pin = 1  // Pin 1
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
    .pin = 14
};

const GPIOConf uart_1_tx_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 4
};

const GPIOPin uart_1_rx_pin = {
    .port = 1,
    .pin = 15
};

const GPIOConf uart_1_rx_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 4
};
