#include "uart.hpp"
#include "../system/board_defs.h"
//#include "assert.h"

using namespace uart;

const uint16_t UartController::DEFULT_BAUD = 9600;
const UartFrameConfig UartController::DEFAULT_FRAME = {
    .mWordLength = EIGHT_BITS,
    .mParity = false,
    .mParityOdd = false,
    .mStopBits = ONE_STOP_BIT
};

static const char* const scInvalidUartController = "Invalid UART controller";

///
/// Constructor
/// @param usart Pointer to usart registers
/// @param usart_num Usart numbered
///
UartController::UartController(uint8_t usart_num):
mUsartNum(usart_num),
mBaudRate(UartController::DEFULT_BAUD),
mFramingConfig(UartController::DEFAULT_FRAME)
{
    switch(mUsartNum){
        case 1:
            mUsart = USART1;
            RCC->APB2RSTR |= 1 << 4; // Reset.
            mBusFreq = APB2_TARGET;
            RCC->APB2RSTR &= !(1 << 4); // unReset.
            RCC->APB2ENR |= 1 << 4; // Start.
            break;

        case 2:
            mUsart = USART2;
            RCC->APB1LRSTR |= 1 << 17; // Reset.
            mBusFreq = APB1_TARGET;
            RCC->APB2RSTR &= !(1 << 17); // unReset.

            RCC->APB1LENR |= 1 << 17; // Start.
            break;

        case 3:
            mUsart = USART3;
            RCC->APB1LRSTR |= 1 << 18; // Reset.
            mBusFreq = APB1_TARGET;
            RCC->APB1LRSTR &= !(1 << 18); // unReset.

            RCC->APB1LENR |= 1 << 18; // Start.
            break;
    
        case 4:
            mUsart = UART4;
            RCC->APB1LRSTR |= 1 << 19; // Reset.
            mBusFreq = APB1_TARGET;
            RCC->APB1LRSTR &= !(1 << 19); // unReset.

            RCC->APB1LENR |= 1 << 19; // Start.
            break;
            
        case 5:
            mUsart = UART5;
            RCC->APB1LRSTR |= 1 << 20; // Reset.'
            mBusFreq = APB1_TARGET;
            RCC->APB1LRSTR &= !(1 << 20); // unReset.

            RCC->APB1LENR |= 1 << 20; // Start.
            break;

        case 6:
            mUsart = USART6;
            RCC->APB2RSTR |= 1 << 5; // Reset.
            mBusFreq = APB2_TARGET;
            RCC->APB2RSTR &= !(1 << 5); // unReset.

            RCC->APB2ENR |= 1 << 5; // Start.
            break;

        case 7:
            mUsart = UART7;
            RCC->APB1LRSTR |= 1 << 30; // Reset.
            mBusFreq = APB1_TARGET;
            RCC->APB1LRSTR &= !(1 << 30); // unReset.

            RCC->APB1LENR |= 1 << 30; // Start.
            break;
            
        case 8:
            mUsart = UART8;
            RCC->APB1LRSTR |= 1 << 31; // Reset.
            RCC->APB1LRSTR &= !(1 << 31); // unReset.

            RCC->APB1LENR |= 1 << 31; // Start.
            break;

        default:
            throw scInvalidUartController;

    }

    mUsart->CR1 =  // M bits control word length
        1 << 29 |  // Enable fifo.
        0 << 28 |  // set M1
        0 << 12 |  // set MO
        1 << 2;    // Receiver enable

    setBaudRate(mBaudRate); // Configure baud rate
    setFraming(mFramingConfig); // Configure framing

}


///
/// Set the baud rate
/// @param baud_rate_set Baud rate to set to.
///
void UartController::setBaudRate(uint16_t baud_rate_set){

    mUsart->CR1 &= ~(1<<0); // Disable UART

    mUsart->BRR = mBusFreq / mBaudRate;

    mUsart->CR1 |= (1<<0); // Enable UART

    mBaudRate = baud_rate_set;
}

///
/// Set the framing configuration
/// @param config Framing configuration to set to.
///
void UartController::setFraming(UartFrameConfig config){

    mUsart->CR1 &= ~(1<<0); // Disable UART

    uint8_t m0_word_length = config.mWordLength & 1 << 0;
    uint8_t m1_word_length = config.mWordLength & 1 << 1;

    // Clear framing configuration bits.
    mUsart->CR1 &= ~(
        1 << 28 | // Word length M1
        1 << 12 | // Word length M0
        1 << 10 | // Parity enable 
        1 << 9    // Parity selection
    );
    // Clear stop bits.
    mUsart->CR2 &= ~(3<<12);

    // Set framing configuration bits.
    mUsart->CR1 |= (
        1 << m1_word_length | // Word length M1
        1 << m0_word_length | // Word length M0
        1 << (uint8_t) config.mParity | // Parity enable 
        1 << (uint8_t) config.mParityOdd // Parity selection
    );
    // Set stop bits.
    mUsart->CR2 |= ~((3 & config.mStopBits) << 12);

    mUsart->CR1 |= (1<<0); // Enable UART
    mFramingConfig = config;
}


///
/// Blocking write a buffer over uart
/// @param buf Character buffer
/// @param buf_len Character buffer length in bytes
///
void UartController::write(char* buf, uint8_t buf_len){
    // enable transmitter and send idle
    mUsart->CR1 |= 1 <<3 ;

    // Write the data to transmitter
    for(uint8_t iByte = 0; iByte < buf_len; iByte++){
        while(!(mUsart->ISR & (1 << 7))); // If the fifo is full, wait.
        mUsart->TDR = buf[iByte];
    } 

    while(!(mUsart->ISR & (1 << 6))){
        ; // wait for transmission to complete
    }


    mUsart->CR1 &= ~(1 <<3) ;
}


///
/// Wait for and read a character over uart
/// @return Character read from the terminal.
///
char UartController::readChar(){
    while(!(mUsart->ISR &  (1 << 5))){
        ; // Wait for RX FIFO to not be empty
    }
    return mUsart->RDR;
}