#include "uart.hpp"
#include "../system/board_defs.h"

///
/// Constructor
/// @param usart Pointer to usart registers
/// @param usart_num Usart numbered
///
UartController::UartController(USART_TypeDef* usart, uint8_t usart_num){


    mUsart = usart;
    mUsartNum = usart_num;
    mBaudRate = (uint16_t) 9600; // Default baud rate
    

    switch(mUsartNum){
        case 1:
            RCC->APB2RSTR |= 1 << 4; // Reset.
            RCC->APB2RSTR &= !(1 << 4); // unReset.

            RCC->APB2ENR |= 1 << 4; // Start.
            break;

        case 6:
            RCC->APB2RSTR |= 1 << 5; // Reset.
            RCC->APB2RSTR &= !(1 << 5); // unReset.

            RCC->APB2ENR |= 1 << 5; // Start.
            break;

        default:
            ; // Should throw an error or something
    }


    mUsart->CR1 =  // M bits control word length
        1 << 29 |  // Enable fifo.
        0 << 28 |  // set M1
        0 << 12 |  // set MO
        1 << 2;    // Receiver enable
        //1 << 3;    // Transmitter enable.


    mUsart->BRR = APB2_TARGET / mBaudRate; // Configure baud rate.

    // Dafault of 1 stop bit,use CR2 to change.

    mUsart->CR1 |= (1<<0); // Enable UART.
}


///
/// Set the baud rate
/// @param baud_rate_set Baud rate to set to.
/// @todo baud rate is doubled? Should fix that...
///
void UartController::setBaudRate(uint16_t baud_rate_set){

    mUsart->CR1 &= ~(1<<0); // Disable UART

    mBaudRate = baud_rate_set;

    mUsart->BRR = APB2_TARGET / mBaudRate;

    mUsart->CR1 |= (1<<0); // Enable UART

}

///
/// Blocking write a buffer.
/// @param buf Character buffer
/// @param buf_len Character buffer length in bytes
/// @todo implement non-blocking write with DMAs
///
void UartController::write(char* buf, uint8_t buf_len){
    // enable transmitter and send idle
    mUsart->CR1 |= 1 <<3 ;

    // Write the data to transmitter
    for(uint8_t iByte = 0; iByte < buf_len; iByte++){
        while(!mUsart->ISR & (1 << 7)); // If the fifo is full, wait.
        mUsart->TDR = buf[iByte];
    } 

    while(!(mUsart->ISR & (1 << 6))){
        ; // wait for transmission to complete
    }


    mUsart->CR1 &= ~(1 <<3) ;
}


///
/// Wait for and read a character from the UART
/// @return Character read from the terminal.
///
char UartController::readChar(){
    while(!(mUsart->ISR &  (1 << 5))){
        ; // Wait for RX FIFO to not be empty
    }
    return mUsart->RDR;
}