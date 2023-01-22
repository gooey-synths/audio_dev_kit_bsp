#ifndef GPIO_HPP
#define GPIO_HPP

#include "../system/stm32h750xx.h"
#include "gpio_defs.h"

namespace gpio{

#define NUM_GPIO_PORTS 11

class GPIOPort{
public:
    GPIOPort(GPIO_TypeDef* mPort);

    void setPinData(uint8_t pin, bool data);
    bool getPinData(uint8_t pin);
    void setPinMode(uint8_t pin, eGPIOPinMode mode);
    void setPinSpeed(uint8_t pin, eGPIOPinSpeed speed);
    void setPinType(uint8_t pin, eGPIOPinType speed);
    void setPinPull(uint8_t pin, eGPIOPinPull pull);
    void setPinAlt(uint8_t pin, uint8_t alt);

    void setParallelData(uint16_t data);
    uint16_t getParallelData();

private:

    GPIO_TypeDef* mPort; ///< GPIO Port i.e. GPIOA
    uint8_t mPortNum;    ///< GPIO port num i.e A is 0
};

class Pin{
public:

    ///
    /// Constructor.
    ///
    Pin(GPIOPort* port, uint8_t pin):
    mPort(port),
    mPin(pin)
    {;} // Do nothing in constructor.

    void operator = (bool value){
        mPort->setPinData(mPin, value);
    }

    bool operator () (){
        return mPort->getPinData(mPin);
    }

private:
    GPIOPort* mPort; //< Ponter to port
    uint8_t mPin;    //< Pin number

};

class GPIOController{

public:

    ///
    /// Get a pointer to the GPIOController instance.
    /// @return Pointer to the GPIOController instance
    ///
    static GPIOController* getInstance(){
        static int a = 0;
        static GPIOController instance;        
        return &instance;
    }

    ///
    /// Get a pin object.
    /// @param pin Pointer to GPIOPin struct
    /// @return A pin object
    ///
    Pin getPin(GPIOPin* pin){
        if(!pin || pin->port > NUM_GPIO_PORTS || pin->pin > 15){
            return Pin(&mPorts[0], 0); // Should throw exception here.
        }
        return Pin(&mPorts[pin->port], pin->pin);
    }
    
    ///
    /// Set the configuration of a pin.
    /// @param pin Pointer to GPIOPin struct.
    /// @param conf Pointer to GPIOConf struct. 
    ///
    void setConfig(GPIOPin *pin, GPIOConf* conf){
        if(!pin || !conf){
            return; // Null pointers not allowed.
        }
        if(pin->port > NUM_GPIO_PORTS){
            return; // Port does not exist.
        }
        mPorts[pin->port].setPinMode(pin->pin, conf->mode);
        mPorts[pin->port].setPinType(pin->pin, conf->type);
        mPorts[pin->port].setPinPull(pin->pin, conf->pull);
        mPorts[pin->port].setPinSpeed(pin->pin, conf->speed);
        mPorts[pin->port].setPinAlt(pin->pin, conf->alternate_function);
    }
    

private:

    ///
    /// Constructor.
    ///
    GPIOController():
        mPorts{ 
            GPIOPort(GPIOA),
            GPIOPort(GPIOB),
            GPIOPort(GPIOC),
            GPIOPort(GPIOD),
            GPIOPort(GPIOE),
            GPIOPort(GPIOF),
            GPIOPort(GPIOG),
            GPIOPort(GPIOH),
            GPIOPort(GPIOI),
            GPIOPort(GPIOJ),
            GPIOPort(GPIOK),
        }{;}// Do nothing in constructor

    GPIOPort mPorts[NUM_GPIO_PORTS]; ///< Collection of all the ports


};

} // namespace gpio
#endif