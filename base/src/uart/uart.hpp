#ifndef UART_HPP
#define UART_HPP

#include <stdint.h>
#include "../system/stm32h750xx.h"

namespace uart{

enum eWordLength{
    EIGHT_BITS = 0,
    NINE_BITS,
    SEVEN_BITS
};

enum eStopBits{
    ONE_STOP_BIT = 0,
    HALF_STOP_BIT,
    TWO_STOP_BITS,
    ONE_AND_HALF_STOP_BITS
};

struct UartFrameConfig{
    eWordLength mWordLength;
    bool mParity;
    bool mParityOdd;
    eStopBits mStopBits;
};

class UartController{

static const uint16_t DEFULT_BAUD;
static const UartFrameConfig DEFAULT_FRAME;

public:
    UartController(uint8_t usart_hw_num);

    void setBaudRate(uint16_t baud_rate);

    ///
    /// Get the baud rate.
    /// @return The baud rate
    ///
    uint16_t getBaudRate(){ return mBaudRate; }

    void setFraming(UartFrameConfig config);

    ///
    /// Get the current frame configuration of the UART
    /// @return The current frame configuration of the UART
    ///
    UartFrameConfig getFraming(){ return mFramingConfig; }

    void write(char* buf, uint8_t buf_len);
    char readChar();

private:
    USART_TypeDef* mUsart; ///< Pointer to USART registers
    uint16_t mBaudRate; ///< Selected baud rate
    uint8_t mUsartNum; ///< Number of USART
    UartFrameConfig mFramingConfig; ///< Framing configuration
    uint32_t mBusFreq; ///< Frequency of the APB connected to the USART
};


} // namespace uart
#endif // UART_HPP