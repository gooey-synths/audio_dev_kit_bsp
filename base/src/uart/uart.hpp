#ifndef UART_HPP
#define UART_HPP


#include <stdint.h>
#include "../system/stm32h750xx.h"

class UartController{

public:
    UartController(USART_TypeDef* usart, uint8_t usart_hw_num);

    void setBaudRate(uint16_t baud_rate);
    void read(char* buf, uint8_t buf_len);
    void write(char* buf, uint8_t buf_len);
    char readChar();

private:
    USART_TypeDef* mUsart; //< Pointer to USART registers
    uint16_t mBaudRate; //< Selected baud rate
    uint8_t mUsartNum; //< Number of USART
};


#endif