#include "gpio.hpp"

using namespace gpio;

///
/// Constructor.
/// @param port pointer to GPIO port struct
/// @todo Error handling.
///
GPIOPort::GPIOPort(GPIO_TypeDef* port):
mPort(port)
{

    mPortNum = ((unsigned long)port - GPIOA_BASE) / 0x400;

    RCC->AHB4ENR |= 1 << mPortNum; // Start clock to GPIO port;
}

///
/// Set pin data/value.
/// @param pin pin number (0-15)
/// @param data value of the pin (true is high)
///
void GPIOPort::setPinData(uint8_t pin, bool data){
    if(pin > 15){
        return; // invalid pin, do nothing.
    }
    if(data){
        mPort->ODR |= 1 << pin;
    }
    else{
        mPort->ODR &= ~(1 << pin);
    }
}

///
/// Get pin data/value.
/// @param pin pin number (0-15)
/// @return true for pin high, false for pin low.
///
bool GPIOPort::getPinData(uint8_t pin){
    if(pin > 15){
        return false; // invalid pin, return false.
    }

    return !!(mPort->IDR & 1<< pin);
}

///
/// Set the mode of a pin.
/// @param pin pin number (0-15)
/// @param mode mode to set the pin to.
///
void GPIOPort::setPinMode(uint8_t pin, eGPIOPinMode mode){
    if(pin > 15){
        return; // invalid pin, do nothing.
    }

    mPort->MODER &= ~(3 << (pin*2)); // clear mode bits.
    mPort->MODER |= (uint8_t)mode << (pin*2);
}

///
/// Set the output speed of a pin.
/// @param pin pin number (0-15)
/// @param speed speed to set the pin to.
///
void GPIOPort::setPinSpeed(uint8_t pin, eGPIOPinSpeed speed){
    if(pin > 15){
        return; // invalid pin, do nothing.
    }

    mPort->OSPEEDR &= ~(3 << (pin*2)); // clear speed bits.
    mPort->OSPEEDR |= (uint8_t)speed << (pin*2);
}

///
/// Set the output type of a pin.
/// @param pin pin number (0-15)
/// @param speed speed to set the pin to.
///
void GPIOPort::setPinType(uint8_t pin, eGPIOPinType type){
    if(pin > 15){
        return; // invalid pin, do nothing.
    }

    mPort->OTYPER &= ~(1 << pin); // clear type bits.
    mPort->OTYPER |= (uint8_t)type << pin;
}

///
/// Set the pull of a pin.
/// @param pin pin number (0-15)
/// @param pull pull to set the pin to.
///
void GPIOPort::setPinPull(uint8_t pin, eGPIOPinPull pull){
    if(pin > 15){
        return; // invalid pin, do nothing.
    }

    mPort->PUPDR &= ~(3 << (pin*2)); // clear pull bits.
    mPort->PUPDR |= (uint8_t)pull << (pin*2);
}

void GPIOPort::setPinAlt(uint8_t pin, uint8_t alt){
    if(pin > 15 || alt > 15){
        return; // invalid pin or allernate function, do nothing.
    }

    if(pin < 8){
        mPort->AFRL &= ~(0xF << (pin*4)); // clear alt bits.
        mPort->AFRL |=  alt << (pin*4);  // set alt bits.

    }
    else{
        pin -= 8;
        mPort->AFRH &= ~(0xF << (pin*4)); // clear alt bits.
        mPort->AFRH |=  alt << (pin*4);  // set alt bits.
    }

}


