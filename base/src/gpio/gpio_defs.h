#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#include <stdint.h>

enum eGPIOPinMode{
    INPUT_MODE = 0,
    OUTPUT_MODE,
    ALTERNATE,
    ANALOG_MODE
};

enum eGPIOPinSpeed{
    LOW_SPEED = 0,
    MEDIUM_SPEED,
    HIGH_SPEED,
    VERY_HIGH_SPEED
};

enum eGPIOPinType{
    PUSH_PULL_TYPE = 0,
    OPEN_DRAIN_TYPE
};

enum eGPIOPinPull{
    NO_PULL = 0,
    PULL_UP,
    PULL_DOWN
    // RESERVED
};

typedef struct GPIOConf_t{
    enum eGPIOPinType type;
    enum eGPIOPinMode mode;
    enum eGPIOPinSpeed speed;
    enum eGPIOPinPull pull;
    uint8_t alternate_function;
}GPIOConf;

typedef struct GPIOPin_t{
    uint8_t port;
    uint8_t pin;
}GPIOPin;



#endif // GPIO_DEFS_H