#include "../system/stm32h750xx.h"
#include "../system/board_defs.h"
#include <stdint.h>

// prototypes
void reset_handler();
void halt();
extern int main(void); // defined in main.c

#define ISR_NOT_IMPL ((uint32_t*) halt)

 /* These are defined in the linker script */
extern uint32_t _stext;  //< Start of text section
extern uint32_t _etext;  //< End of text section
extern uint32_t _sbss;   //< Start of BSS section
extern uint32_t _ebss;   //< End of BSS section
extern uint32_t _sdata;  //< Start of data section
extern uint32_t _edata;  //< End of data section
extern uint32_t _sstack; //< Start of stack
extern uint32_t _estack; //< End of stack section

#define STACK_START_ADDR   0x20020000

__attribute__ ((section(".isrvectors")))
uint32_t* vector_table[] = {
    (uint32_t*) STACK_START_ADDR,   
    (uint32_t*) reset_handler,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL,
    ISR_NOT_IMPL, /* Window WatchDog              */
    ISR_NOT_IMPL, /* PVD/AVD through EXTI Line detection */
    ISR_NOT_IMPL, /* Tamper and TimeStamps through the EXT */
    ISR_NOT_IMPL, /* RTC Wakeup through the EXTI line */
    ISR_NOT_IMPL, /* FLASH                        */
    ISR_NOT_IMPL, /* RCC                          */
    ISR_NOT_IMPL, /* EXTI Line0                   */
    ISR_NOT_IMPL, /* EXTI Line1                   */
    ISR_NOT_IMPL, /* EXTI Line2                   */
    ISR_NOT_IMPL, /* EXTI Line3                   */
    ISR_NOT_IMPL, /* EXTI Line4                   */
    ISR_NOT_IMPL, /* DMA1 Stream 0                */
    ISR_NOT_IMPL, /* DMA1 Stream 1                */
    ISR_NOT_IMPL, /* DMA1 Stream 2                */
    ISR_NOT_IMPL, /* DMA1 Stream 3                */
    ISR_NOT_IMPL, /* DMA1 Stream 4                */
    ISR_NOT_IMPL, /* DMA1 Stream 5                */
    ISR_NOT_IMPL, /* DMA1 Stream 6                */
    ISR_NOT_IMPL, /* ADC1, ADC2 and ADC3s         */
    ISR_NOT_IMPL, /* FDCAN1 interrupt line 0      */
    ISR_NOT_IMPL, /* FDCAN2 interrupt line 0      */
    ISR_NOT_IMPL, /* FDCAN1 interrupt line 1      */
    ISR_NOT_IMPL, /* FDCAN2 interrupt line 1      */
    ISR_NOT_IMPL, /* External Line[9:5]s          */
    ISR_NOT_IMPL, /* TIM1 Break interrupt         */
    ISR_NOT_IMPL, /* TIM1 Update interrupt        */
    ISR_NOT_IMPL, /* TIM1 Trigger and Commutation interrupt*/
    ISR_NOT_IMPL, /* TIM1 Capture Compare         */
    ISR_NOT_IMPL, /* TIM2                         */
    ISR_NOT_IMPL, /* TIM3                         */
    ISR_NOT_IMPL, /* TIM4                         */
    ISR_NOT_IMPL, /* I2C1 Event                   */
    ISR_NOT_IMPL, /* I2C1 Error                   */
    ISR_NOT_IMPL, /* I2C2 Event                   */
    ISR_NOT_IMPL, /* I2C2 Error                   */
    ISR_NOT_IMPL, /* SPI1                         */
    ISR_NOT_IMPL, /* SPI2                         */
    ISR_NOT_IMPL, /* USART1                       */
    ISR_NOT_IMPL, /* USART2                       */
    ISR_NOT_IMPL, /* USART3                       */
    ISR_NOT_IMPL, /* External Line[15:10]s        */
    ISR_NOT_IMPL, /* RTC Alarm (A and B) through EXTI Line */
    ISR_NOT_IMPL, /* Reserved                     */
    ISR_NOT_IMPL, /* TIM8 Break and TIM12         */
    ISR_NOT_IMPL, /* TIM8 Update and TIM13        */
    ISR_NOT_IMPL, /* TIM8 Trigger and Commutation and TIM1 */
    ISR_NOT_IMPL, /* TIM8 Capture Compare         */
    ISR_NOT_IMPL, /* DMA1 Stream7                 */
    ISR_NOT_IMPL, /* FMC                          */
    ISR_NOT_IMPL, /* SDMMC1                       */
    ISR_NOT_IMPL, /* TIM5                         */
    ISR_NOT_IMPL, /* SPI3                         */
    ISR_NOT_IMPL, /* UART4                        */
    ISR_NOT_IMPL, /* UART5                        */
    ISR_NOT_IMPL, /* TIM6 and DAC1&2 underrun errors */
    ISR_NOT_IMPL, /* TIM7                         */
    ISR_NOT_IMPL, /* DMA2 Stream 0                */
    ISR_NOT_IMPL, /* DMA2 Stream 1                */
    ISR_NOT_IMPL, /* DMA2 Stream 2                */
    ISR_NOT_IMPL, /* DMA2 Stream 3                */
    ISR_NOT_IMPL, /* DMA2 Stream 4                */
    ISR_NOT_IMPL, /* Ethernet                     */
    ISR_NOT_IMPL, /* Ethernet Wakeup through EXTI line */
    ISR_NOT_IMPL, /* FDCAN calibration unit interrupt*/
    ISR_NOT_IMPL, /* Reserved                     */
    ISR_NOT_IMPL, /* Reserved                     */
    ISR_NOT_IMPL, /* Reserved                     */
    ISR_NOT_IMPL, /* Reserved                     */
    ISR_NOT_IMPL, /* DMA2 Stream 5                */
    ISR_NOT_IMPL, /* DMA2 Stream 6                */
    ISR_NOT_IMPL, /* DMA2 Stream 7                */
    ISR_NOT_IMPL, /* USART6                       */
    ISR_NOT_IMPL, /* I2C3 event                   */
    ISR_NOT_IMPL, /* I2C3 error                   */
    ISR_NOT_IMPL, /* USB OTG HS End Point 1 Out   */
    ISR_NOT_IMPL, /* USB OTG HS End Point 1 In    */
    ISR_NOT_IMPL, /* USB OTG HS Wakeup through EXTI */
    ISR_NOT_IMPL, /* USB OTG HS                   */
    ISR_NOT_IMPL, /* DCMI                         */
    ISR_NOT_IMPL, /* Crypto                       */
    ISR_NOT_IMPL, /* Hash and Rng                 */
    ISR_NOT_IMPL, /* FPU                          */
    ISR_NOT_IMPL, /* UART7                        */
    ISR_NOT_IMPL, /* UART8                        */
    ISR_NOT_IMPL, /* SPI4                         */
    ISR_NOT_IMPL, /* SPI5                         */
    ISR_NOT_IMPL, /* SPI6                         */
    ISR_NOT_IMPL, /* SAI1                         */
    ISR_NOT_IMPL, /* LTDC                         */
    ISR_NOT_IMPL, /* LTDC error                   */
    ISR_NOT_IMPL, /* DMA2D                        */
    ISR_NOT_IMPL, /* SAI2                         */
    ISR_NOT_IMPL, /* QUADSPI                      */
    ISR_NOT_IMPL, /* LPTIM1                       */
    ISR_NOT_IMPL, /* HDMI_CEC                     */
    ISR_NOT_IMPL, /* I2C4 Event                   */
    ISR_NOT_IMPL, /* I2C4 Error                   */
    ISR_NOT_IMPL, /* SPDIF_RX                     */
    ISR_NOT_IMPL, /* USB OTG FS End Point 1 Out   */
    ISR_NOT_IMPL, /* USB OTG FS End Point 1 In    */
    ISR_NOT_IMPL, /* USB OTG FS Wakeup through EXTI */
    ISR_NOT_IMPL, /* USB OTG FS                   */
    ISR_NOT_IMPL, /* DMAMUX1 Overrun interrupt    */
    ISR_NOT_IMPL, /* HRTIM Master Timer global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Timer A global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Timer B global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Timer C global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Timer D global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Timer E global Interrupt */
    ISR_NOT_IMPL, /* HRTIM Fault global Interrupt   */
    ISR_NOT_IMPL, /* DFSDM Filter0 Interrupt        */
    ISR_NOT_IMPL, /* DFSDM Filter1 Interrupt        */
    ISR_NOT_IMPL, /* DFSDM Filter2 Interrupt        */
    ISR_NOT_IMPL, /* DFSDM Filter3 Interrupt        */
    ISR_NOT_IMPL, /* SAI3 global Interrupt          */
    ISR_NOT_IMPL, /* Serial Wire Interface 1 global interrupt */
    ISR_NOT_IMPL, /* TIM15 global Interrupt      */
    ISR_NOT_IMPL, /* TIM16 global Interrupt      */
    ISR_NOT_IMPL, /* TIM17 global Interrupt      */
    ISR_NOT_IMPL, /* MDIOS Wakeup  Interrupt     */
    ISR_NOT_IMPL, /* MDIOS global Interrupt      */
    ISR_NOT_IMPL, /* JPEG global Interrupt       */
    ISR_NOT_IMPL, /* MDMA global Interrupt       */
    ISR_NOT_IMPL, /* Reserved                    */
    ISR_NOT_IMPL, /* SDMMC2 global Interrupt     */
    ISR_NOT_IMPL, /* HSEM1 global Interrupt      */
    ISR_NOT_IMPL, /* Reserved                    */
    ISR_NOT_IMPL, /* ADC3 global Interrupt       */
    ISR_NOT_IMPL, /* DMAMUX Overrun interrupt    */
    ISR_NOT_IMPL, /* BDMA Channel 0 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 1 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 2 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 3 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 4 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 5 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 6 global Interrupt */
    ISR_NOT_IMPL, /* BDMA Channel 7 global Interrupt */
    ISR_NOT_IMPL, /* COMP1 global Interrupt     */
    ISR_NOT_IMPL, /* LP TIM2 global interrupt   */
    ISR_NOT_IMPL, /* LP TIM3 global interrupt   */
    ISR_NOT_IMPL, /* LP TIM4 global interrupt   */
    ISR_NOT_IMPL, /* LP TIM5 global interrupt   */
    ISR_NOT_IMPL, /* LP UART1 interrupt         */
    ISR_NOT_IMPL, /* Reserved                   */
    ISR_NOT_IMPL, /* Clock Recovery Global Interrupt */
    ISR_NOT_IMPL, /* ECC diagnostic Global Interrupt */
    ISR_NOT_IMPL, /* SAI4 global interrupt      */
    ISR_NOT_IMPL, /* Reserved                   */
    ISR_NOT_IMPL, /* Reserved                   */
    ISR_NOT_IMPL  /* Interrupt for all 6 wake-up pins */
};

///
/// Caluculate bits for prescalers
/// @param source Source frequency
/// @param target Target frequency
/// @return bits for prescaler
/// @note For correct results make sure that target is a power of 2 division of source
///
uint8_t calc_prescaler_bits(uint32_t source, uint32_t target){
    uint8_t scale_bits = 0;

    while(source > source){
        target >>= 2;
        scale_bits++;
    }
    return scale_bits;
}


///
/// Configure and start clocks.
/// @todo FLASH, adjust wait states based on frequency target.
/// @todo calculate actual frequency after dividers
/// @todo PWR, adjust VOS state based on frequency target
///
void start_clocks(){

    /// Get power supply ready for higher frequencies
    /// Note: must shift into VOS1 before shifting into VOS0
    PWR->CR3 |= 1 << 1; // Enable internal LDO
    
    RCC->APB4ENR |= 1 << 1; // Enable clock to SYSCFG peripheral

    PWR->D3CR |= 2 << 14; // Set VOS to scale 2

    while (!(PWR->D3CR & 1 << 13)){
        ; // Wait for the VOSRDY bit
    }

    PWR->D3CR |= 1 << 14; // Set VOS to scale 1
    while (!(PWR->D3CR & 1 << 13)){
        ; // Wait for the VOSRDY bit
    }

    /// Activate VOS0 (overdrive!)
    SYSCFG->PWRCR = 1; // Maximum overdrive!

    while (!(PWR->D3CR & 1 << 13)){
        ; // Wait for the VOSRDY bit
    }

    // Start up external oscllator
    RCC->CR |= 1 << 16; // Enable HSE

    while(!(RCC->CR & 1 << 17)){
        ; // Wait for HSE ready flag
    }

    // Start PLL1
    RCC->PLLCKSELR =
        2 << 0 |  // HSE as PLL clock
        1 << 4;   // Prescale PLL1 by 1
    uint8_t pll1_divn = (uint32_t)D1_TARGET/XTAL_FREQ;

    RCC->PLLCFGR = 
        //1 << 18 | // ENABLE pll1 divr
        //1 << 17 | // ENABLE pll1 divq
        1 << 16 | // ENABLE pll1 divp
        3 << 2  | // clock rate frequency is between 8 an 16MHz TODO: calculate clock range beforehand
        0 << 1  | // Wide VCO range 192 to 836 MHz
        0 << 0;  // No fractional divider.

    RCC->PLL1DIVR = 
        pll1_divn -1 << 0; // Set PLL1 divN divider

    RCC->CR |= 1 << 24; // Enable PLL1

    while(!(RCC->CR & 1 << 25)){
        ; // Wait for PLL1 ready flag
    }

    // Configure cbus prescalers    
    uint8_t hpre = 0;
    if(D1_TARGET != AHB_TARGET){
        hpre = 8 + calc_prescaler_bits(D1_TARGET, AHB_TARGET); //TODO: use this!
        //assert(hpre < 16);
    }

    RCC->D1CFGR = 
        0 << 8 |    // Do not divide D1 clock
        hpre << 0;  // Divide for AHB

    uint8_t apb1_pre = 0;
    uint8_t apb2_pre = 0;
    uint8_t apb4_pre = 0;

    if(APB1_TARGET != AHB_TARGET){
        apb1_pre = 4 + calc_prescaler_bits(AHB_TARGET, APB1_TARGET);
        //assert(apb1_pre < 8);
    }

    if(APB2_TARGET != AHB_TARGET){
        apb2_pre = 4 + calc_prescaler_bits(AHB_TARGET, APB2_TARGET);
        //assert(apb2_pre < 8);
    }

    RCC->D2CFGR =
        apb2_pre << 8 |
        apb1_pre << 4;

    if(APB4_TARGET != AHB_TARGET){
        apb4_pre = 4 + calc_prescaler_bits(AHB_TARGET, APB4_TARGET);
        //assert(apb4_pre < 8);
    }

    RCC->D3CFGR =
        apb4_pre << 4;

    // Configure flash

    uint8_t flash_latency = 4; // 5 flash clock cycle latency
    uint8_t program_delay = 2; // 2?
    FLASH->ACR = program_delay << 4 | flash_latency;
    if(FLASH->ACR != (program_delay << 4 | flash_latency)){
        halt(); // Failed to set flash params!
    }
    // Set system clock
    RCC->CFGR |= 3; // Select PLL1p as system clock 
    
    while (!((RCC->CFGR & 3<<3) == 3<<3)){
        ; // Wait for PLL1 to be selected for CPU clock.
    }
    volatile uint32_t debug = FLASH->ACR;

}

///
/// Reset handler and initial entry point. 
/// @todo Remap vector table for dynamic interrupts
///
__attribute__ ((noreturn)) void reset_handler(){

    /* Copy init values from text to data */
    uint32_t *init_values_ptr = &_etext  + (&_sdata - &_edata);
    uint32_t *data_ptr = &_sdata;

    if (init_values_ptr != data_ptr) {
        for (; data_ptr < &_edata;) {
            *data_ptr++ = *init_values_ptr++;
        }
    }

    /* Clear the zero segment */
    for (uint32_t *bss_ptr = &_sbss; bss_ptr < &_ebss;) {
        *bss_ptr++ = 0;
    }

    start_clocks();

    main();

    halt(); // Should never get here! 
}

///
/// Halt the system due to error.
///
__attribute__ ((noreturn)) void halt(){
    while(1){;}
}
