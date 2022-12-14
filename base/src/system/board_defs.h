#ifndef BOARD_DEFS_H
#define BOARD_DEFS_H

/* Clock related */
#define XTAL_FREQ  (16000000)        //< Hz of on-board oscillator
#define D1_TARGET  (XTAL_FREQ*30)    //< Target Hz of D1 domain
#define AHB_AXI_TARGET (D1_TARGET/2) //< Target Hz for AHB and AXI peripheral clocks
/* APB 1, 2 & 4 are divided from AHB clock. */
#define APB1_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB1 peripherals
#define APB2_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB2 peripherals
#define APB4_TARGET (AHB_AXI_TARGET/4)   //< Target Hz for APB4 peripherals

#endif