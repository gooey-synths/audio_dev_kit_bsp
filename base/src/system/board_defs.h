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
/* PLL targets */
#define PLL1_DIVP_TARGET (D1_TARGET)     //< Frequency of PLL1 div P
#define PLL1_DIVQ_TARGET (XTAL_FREQ*10)  //< Frequency of PLL1 div Q

// For TinyUSB
#define SystemCoreClock D1_TARGET;

/* Pin definitions found in pin_defs.c */

extern GPIOPin  led_pin;
extern GPIOConf led_pin_conf;

extern GPIOPin  mco2_pin;
extern GPIOConf mco2_pin_conf;

extern GPIOPin  uart_1_tx_pin;
extern GPIOConf uart_1_tx_conf;

extern GPIOPin  uart_1_rx_pin;
extern GPIOConf uart_1_rx_conf;

extern GPIOPin  adc1_16_pin;
extern GPIOConf adc1_16_conf;

extern GPIOPin  adc1_3_pin;
extern GPIOConf adc1_3_conf;

extern GPIOPin  adc1_8_pin;
extern GPIOConf adc1_8_conf;

extern GPIOPin  adc1_9_pin;
extern GPIOConf adc1_9_conf;

extern GPIOPin  adc1_10_pin;
extern GPIOConf adc1_10_conf;

extern GPIOPin  adc1_11_pin;
extern GPIOConf adc1_11_conf;

extern GPIOPin  adc1_14_pin;
extern GPIOConf adc1_14_conf;

extern GPIOPin  adc1_15_pin;
extern GPIOConf adc1_15_conf;

extern GPIOPin  adc1_18_pin;
extern GPIOConf adc1_18_conf;

extern GPIOPin  adc1_19_pin;
extern GPIOConf adc1_19_conf;

extern GPIOPin spi2_clk_pin;
extern GPIOConf spi2_clk_conf;

extern GPIOPin spi2_cipo_pin;
extern GPIOConf spi2_cipo_conf;

extern GPIOPin spi2_copi_pin;
extern GPIOConf spi2_copi_conf;

extern GPIOPin spi2_cs0_pin;
extern GPIOConf spi2_cs0_conf;

extern GPIOPin spi3_clk_pin;
extern GPIOConf spi3_clk_conf;

extern GPIOPin spi3_cipo_pin;
extern GPIOConf spi3_cipo_conf;

extern GPIOPin spi3_copi_pin;
extern GPIOConf spi3_copi_conf;

extern GPIOPin spi3_cs0_pin;
extern GPIOConf spi3_cs0_conf;

extern GPIOPin spi3_cs1_pin;
extern GPIOConf spi3_cs1_conf;

extern GPIOPin spi3_cs2_pin;
extern GPIOConf spi3_cs2_conf;

extern GPIOPin spi2_clk_pin;
extern GPIOConf spi2_clk_conf;

extern GPIOPin spi2_cipo_pin;
extern GPIOConf spi2_cipo_conf;

extern GPIOPin spi2_copi_pin;
extern GPIOConf spi2_copi_conf;

extern GPIOPin spi2_cs0_pin;
extern GPIOConf spi2_cs0_conf;

extern GPIOPin spi3_clk_pin;
extern GPIOConf spi3_clk_conf;

extern GPIOPin spi3_cipo_pin;
extern GPIOConf spi3_cipo_conf;

extern GPIOPin spi3_copi_pin;
extern GPIOConf spi3_copi_conf;

extern GPIOPin spi3_cs0_pin;
extern GPIOConf spi3_cs0_conf;

extern GPIOPin spi3_cs1_pin;
extern GPIOConf spi3_cs1_conf;

extern GPIOPin spi3_cs2_pin;
extern GPIOConf spi3_cs2_conf;

extern GPIOPin usb_vbus_id_pin;
extern GPIOConf usb_vbus_id_conf;

extern GPIOPin usb_d_plus_pin;
extern GPIOConf usb_d_plus_conf;

extern GPIOPin usb_d_minus_pin;
extern GPIOConf usb_d_minus_conf;

extern GPIOPin usb_vbus_dect_pin;
extern GPIOConf usb_vbus_dect_conf;

#endif