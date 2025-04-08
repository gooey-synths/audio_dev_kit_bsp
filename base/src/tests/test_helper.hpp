#ifndef TEST_HELPER_HPP
#define TEST_HELPER_HPP

#include "../uart/uart.hpp"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#define TEST_PRINT_WIDTH 16
#define MAX_STR_LEN 64

static constexpr char* scExceptionNotThrown = "Exception not thrown!";

///
/// Print a buffer to a Uart
/// @param uart Pointer to uart controller
/// @param bufffer Buffer to print
/// @param elem_size Number of bytes per element
/// @param buffer_len Total number of bytes in the buffer
/// @note data elements must be less than 10 characters
/// @note This print function is VERY VERY unsafe
///
static void print_buffer(uart::UartController* uart, void* buffer, uint8_t elem_size, uint32_t buffer_len){
    uint8_t* buf_ptr = (uint8_t*) buffer;
    uint8_t elem_ctr = 0;
    char print_buff[13]; // 10 characters plus comma, space, and null termintor
    for(;buf_ptr < (uint8_t*) buffer + buffer_len; buf_ptr+=elem_size){
        if(elem_ctr >= TEST_PRINT_WIDTH){
            uart->write((char*)"\r\n", sizeof("\r\n"));
            elem_ctr = 0;
        }
        switch(elem_size){
            case 1: // uint8_t
                sprintf(print_buff, "%d, ", *(uint8_t*)buf_ptr);
                break;
            case 2: // uin16_t
                sprintf(print_buff, "%d, ", *(uint16_t*)buf_ptr);
                break;
            case 4: // uint32_t
                sprintf(print_buff, "%d, ", *(uint32_t*)buf_ptr);
                break;
            case 8: // uint64_t
                sprintf(print_buff, "%d, ", *(uint64_t*)buf_ptr);
                break;
        }
        uart->write(print_buff, strlen(print_buff));
        elem_ctr += strlen(print_buff);

    }
    uart->write((char*)"\r\n\r\n", sizeof("\r\n\r\n"));
}

///
/// Print a string to a UART.
/// @param uart UART to print the string to.
/// @param s String to print.
/// @note String length is clipped to @ref MAX_STR_LEN characters.
/// 
static void print_str(uart::UartController* uart, char* s) {
    size_t len = strlen(s);

    // Clip length
    if(len > MAX_STR_LEN) {
        len = MAX_STR_LEN;
    }

    uart->write(s, len);
}

///
/// Helper macro to call statements that may have exceptions and print if the exception happened.
/// @note uart::UartController uart1 and bool exceptionCaught must be defined before using this macro.
///
#define EXPECT_EXCEPTION(statement)              \
do{                                              \
    exceptionCaught = false;                     \
    try {                                        \
        statement;                               \
    } catch (char* e) {                          \
        print_str(&uart1, e);                    \
        exceptionCaught = true;                  \
    }                                            \
    if(!exceptionCaught) {                       \
        print_str(&uart1, scExceptionNotThrown); \
    }                                            \
} while(0)                                       \

#endif // TEST_HELPER_HPP
