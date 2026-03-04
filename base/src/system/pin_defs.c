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

const GPIOPin spi2_clk_pin = {
    .port = 1,
    .pin = 13
};

const GPIOConf spi2_clk_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

const GPIOPin spi2_cipo_pin = {
    .port = 1,
    .pin = 15
};

const GPIOConf spi2_cipo_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

const GPIOPin spi2_copi_pin = {
    .port = 1,
    .pin = 14
};

const GPIOConf spi2_copi_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 5
};

const GPIOPin spi2_cs0_pin = {
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

const GPIOPin spi3_clk_pin = {
    .port = 2,
    .pin = 10
};

const GPIOConf spi3_clk_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

const GPIOPin spi3_cipo_pin = {
    .port = 2,
    .pin = 11
};

const GPIOConf spi3_cipo_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

const GPIOPin spi3_copi_pin = {
    .port = 2,
    .pin = 12
};

const GPIOConf spi3_copi_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 6
};

const GPIOPin spi3_cs0_pin = {
    .port = 2,
    .pin = 6
};

const GPIOConf spi3_cs0_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};


const GPIOPin spi3_cs1_pin = {
    .port = 2,
    .pin = 7
};

const GPIOConf spi3_cs1_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

const GPIOPin spi3_cs2_pin = {
    .port = 2,
    .pin = 8
};

const GPIOConf spi3_cs2_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
};

// ADC pins

const GPIOPin adc1_16_pin = {
    .port = 0,
    .pin = 0
};

const GPIOConf adc1_16_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ANALOG_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL
};

const GPIOPin adc1_3_pin = {
    .port = 0,
    .pin = 6
};

const GPIOConf adc1_3_conf = {
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

const GPIOConf adc1_15_conf = {
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

// USB pins

GPIOPin usb_vbus_id_pin = {
    .port = 0,
    .pin = 10
};

GPIOConf usb_vbus_id_conf = {
    .type = OPEN_DRAIN_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = PULL_UP,
    .alternate_function = 10
};

GPIOPin usb_vbus_dect_pin = {
    .port = 0,
    .pin = 9
};

GPIOConf usb_vbus_dect_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = INPUT_MODE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 0
};

GPIOPin usb_d_plus_pin = {
    .port = 0,
    .pin = 12
};

GPIOConf usb_d_plus_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 10
};

GPIOPin usb_d_minus_pin = {
    .port = 0,
    .pin = 11
};

GPIOConf usb_d_minus_conf = {
    .type = PUSH_PULL_TYPE,
    .mode = ALTERNATE,
    .speed = VERY_HIGH_SPEED,
    .pull = NO_PULL,
    .alternate_function = 10
};