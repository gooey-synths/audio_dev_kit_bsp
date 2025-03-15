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

// ADC pins

const GPIOPin adc1_7_pin = {
    .port = 0,
    .pin = 0
};

const GPIOConf adc1_7_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_8_pin = {
    .port = 2,
    .pin = 5
};

const GPIOConf adc1_8_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_9_pin = {
    .port = 1,
    .pin = 0
};

const GPIOConf adc1_9_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_10_pin = {
    .port = 2,
    .pin = 0
};

const GPIOConf adc1_10_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_11_pin = {
    .port = 2,
    .pin = 1
};

const GPIOConf adc1_11_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_14_pin = {
    .port = 0,
    .pin = 2
};

const GPIOConf adc1_14_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_15_pin = {
    .port = 0,
    .pin = 3
};

const GPIOConf adc1_14_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin adc1_18_pin = {
    .port = 0,
    .pin = 4
};

const GPIOConf adc1_18_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};


const GPIOPin adc1_19_pin = {
    .port = 0,
    .pin = 5
};

const GPIOConf adc1_19_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};