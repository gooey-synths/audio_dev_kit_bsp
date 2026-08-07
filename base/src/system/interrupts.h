#ifndef SYSTEM_INTERRUPTS_H
#define SYSTEM_INTERRUPTS_H

// File for defining system wide interrupt priorities

#define CM_MAX_PRIORITY   0 // Do not try to set a priority above this.
#define TIM6_INT_PRIO     0
#define TIM7_INT_PRIO     1
#define SPI1_INT_PRIO     3
#define SPI2_INT_PRIO     3
#define SPI3_INT_PRIO     3
#define SPI4_INT_PRIO     3
#define SPI5_INT_PRIO     3
#define SPI6_INT_PRIO     3
#define USB_OTG_INT_PRIO  7
#define CM_MIN_PRIORITY   7 // Do not try to set a priority below this.

#endif // SYSTEM_INTERRUPTS_H
