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