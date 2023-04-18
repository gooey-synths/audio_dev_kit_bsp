#ifndef BOARD_DEFS_H
#define BOARD_DEFS_H

#include "../gpio/gpio_defs.h"

/* Clock related */
#define XTAL_FREQ  (16000000)        //< Hz of on-board oscillator
#define D1_TARGET  (XTAL_FREQ*30)    //< Target Hz of D1 domain
#define AHB_AXI_TARGET (D1_TARGET/2) //< Target Hz for AHB and AXI peripheral clocks
/* APB 1, 2 & 4 are divided from AHB clock. */
#define APB1_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB1 peripherals
#define APB2_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB2 peripherals
#define APB4_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB4 peripherals

/* Pin definitions found in pin_defs.c */

extern GPIOPin  led_pin;
extern GPIOConf led_pin_conf;

extern GPIOPin  mco2_pin;
extern GPIOConf mco2_pin_conf;

extern GPIOPin  uart_1_tx_pin;
extern GPIOConf uart_1_tx_conf;

extern GPIOPin  uart_1_rx_pin;
extern GPIOConf uart_1_rx_conf;

#endif