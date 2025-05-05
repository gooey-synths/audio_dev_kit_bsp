#include "../system/stm32h750xx.h"
#include "../system/board_defs.h"
#include <stdint.h>

// prototypes
void reset_handler();
void halt();
extern int main(); // defined in main.c

#define ISR_NOT_IMPL ((uint32_t*) halt)

 /* These are defined in the linker script */
extern uint32_t _stext;  //< Start of text section
extern uint32_t _etext;  //< End of text section
extern uint32_t _sbss;   //< Start of BSS section
extern uint32_t _ebss;   //< End of BSS section
extern uint32_t _sdata;  //< Start of data section
extern uint32_t _edata;  //< End of data section
extern uint32_t _sstack; //< Start of stack section
extern uint32_t _estack; //< End of stack section
extern uint32_t _sidata; //< Startup initialization data

__attribute__ ((section(".isr_vector")))
uint32_t* vector_table[] = {
    (uint32_t*) &_estack,
    (uint32_t*) reset_handler,
    ISR_NOT_IMPL, /* NMI */
    ISR_NOT_IMPL, /* Hard fault */
    ISR_NOT_IMPL, /* Memory management fault */
    ISR_NOT_IMPL, /* Bus fault */
    ISR_NOT_IMPL, /* Usage fault */
    ISR_NOT_IMPL, /* Reserved */
    ISR_NOT_IMPL, /* Reserved */
    ISR_NOT_IMPL, /* Reserved */
    ISR_NOT_IMPL, /* Reserved */
    ISR_NOT_IMPL, /* SVCall */
    ISR_NOT_IMPL, /* Reserved for debug */
    ISR_NOT_IMPL, /* Reserved */
    ISR_NOT_IMPL, /* PendSV */
    ISR_NOT_IMPL, /* Systick */
    ISR_NOT_IMPL, /* Window WatchDog              */
    ISR_NOT_IMPL, /* PVD/AVD through EXTI Line detection */
    ISR_NOT_IMPL, /* Tamper and TimeStamps through the EXTI */
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

uint32_t* vector_table_ram[166]__attribute__((aligned (512))); // Remapped vector table must be aligned to rounded up power of 2

///
/// Enable the FPU coprocessor
///
void enable_fpu(){
    /* enable FPU if available and used */
    SCB->CPACR |= ((3UL << 10*2) |             /* set CP10 Full Access               */
                (3UL << 11*2));           /* set CP11 Full Access               */
}

///
/// Caluculate number of times to devide by 2 to get from source to targer
/// @param source Source frequency
/// @param target Target frequency
/// @return Divider amount
/// @note For correct results make sure that target is a power of 2 division of source
///
uint8_t calc_divider(uint32_t source, uint32_t target){
    uint8_t divider = 0;

    while(source > target){
        source >>= 1;
        divider++;
    }
    return divider;
}

///
/// Set VOS power scale
/// @param scale Scale to set
///
void set_VOS_scale(uint8_t scale){

    uint8_t scale_bits = 4-scale;
    scale_bits &= 0x3;

    if(scale_bits != 0){
        PWR->D3CR |= scale_bits << 14;
        while (!(PWR->D3CR & 1 << 13)){
            ; // Wait for the VOSRDY bit
        }
    }
    else{ // Requested VOS scale 0
        PWR->D3CR |= 3 << 14; // Shift to VOS scale 1 before 0
        while (!(PWR->D3CR & 1 << 13)){
            ; // Wait for the VOSRDY bit
        }

        /// Activate VOS0
        SYSCFG->PWRCR = 1; // Maximum overdrive!
        while (!(PWR->D3CR & 1 << 13)){
            ; // Wait for the VOSRDY bit
        }
    }

}

///
/// Set flash parameters
/// @param latency Number of wait states per read
/// @param prog_delay Programming delay
///
void set_flash_params(uint8_t latency, uint8_t prog_delay){
    if(latency > 4){
        latency = 4; // clip to 4 wait states
    }
    if(prog_delay > 2){
        prog_delay = 2; // clip to 2 latency.
    }

    FLASH->ACR = prog_delay << 4 | latency;
    if(FLASH->ACR != (prog_delay << 4 | latency)){
        halt(); // Failed to set flash parameters!
    }
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

    set_VOS_scale(0); // Set to highest power state 

    // Configure flash (based on table 17, VOS0 column in STM RM0433)
    if(AHB_AXI_TARGET > 0 && AHB_AXI_TARGET <= 70e6){
        set_flash_params(0, 0);
    }
    else if(AHB_AXI_TARGET > 70e6 && AHB_AXI_TARGET <= 140e6){
        set_flash_params(1, 1);
    }
    else if(AHB_AXI_TARGET > 140e6 && AHB_AXI_TARGET <= 185e6){
        set_flash_params(2, 1);
    }
    else if(AHB_AXI_TARGET > 185e6 && AHB_AXI_TARGET <= 210e6){
        set_flash_params(2, 2);
    }
    else if(AHB_AXI_TARGET > 210e6 && AHB_AXI_TARGET <= 185e6){
        set_flash_params(3, 2);
    }
    else if(AHB_AXI_TARGET > 225e6 && AHB_AXI_TARGET <= 240e6){
        set_flash_params(4, 2);
    }
    else{
        halt(); // Invalid frequency... 
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
    uint16_t pll1_divn = (uint32_t)D1_TARGET/XTAL_FREQ; // This sets our VCO frequency.
    uint8_t  pll1_divp = (uint32_t)D1_TARGET/PLL1_DIVP_TARGET; // We will be using DIVP as our D1 target.
    uint8_t  pll1_divq = (uint32_t)D1_TARGET/PLL1_DIVQ_TARGET;

    RCC->PLLCFGR = 
        //1 << 18 | // ENABLE pll1 divr
        1 << 17 | // ENABLE pll1 divq
        1 << 16 | // ENABLE pll1 divp
        3 << 2  | // clock rate frequency is between 8 an 16MHz TODO: calculate clock range beforehand
        0 << 1  | // Wide VCO range 192 to 836 MHz
        0 << 0;  // No fractional divider.

    RCC->PLL1DIVR =
        ((pll1_divn - 1) & 0x1FF) << 0 | // Set PLL1 divN divider
        ((pll1_divp - 1) & 0x7F)  << 9 | // Set PLL1 divP divider
        ((pll1_divq - 1) & 0x7F) << 16;  // Set PLL1 divQ divider

    RCC->CR |= 1 << 24; // Enable PLL1

    while(!(RCC->CR & 1 << 25)){
        ; // Wait for PLL1 ready flag
    }

    // Configure cbus prescalers    
    uint8_t hpre = 0; // 0 means no division
    if(D1_TARGET != AHB_AXI_TARGET){
        hpre = 8 + calc_divider(D1_TARGET, AHB_AXI_TARGET)-1;
        //assert(hpre < 16);
    }

    RCC->D1CFGR = 
        0 << 8 |    // Do not divide D1 clock, PLL is set to appropriate range
        hpre << 0;  // Divide for AHB

    uint8_t apb1_pre = 0; // 0 means no division
    uint8_t apb2_pre = 0; // 0 means no division
    uint8_t apb4_pre = 0; // 0 means no division

    if(APB1_TARGET != AHB_AXI_TARGET){
        apb1_pre = 4 + calc_divider(AHB_AXI_TARGET, APB1_TARGET) - 1;
    }

    if(APB2_TARGET != AHB_AXI_TARGET){
        apb2_pre = 4 + calc_divider(AHB_AXI_TARGET, APB2_TARGET) - 1;
    }

    RCC->D2CFGR =
        apb2_pre << 8 |
        apb1_pre << 4;

    if(APB4_TARGET != AHB_AXI_TARGET){
        apb4_pre = 4 + calc_divider(AHB_AXI_TARGET, APB4_TARGET) - 1;
    }

    RCC->D3CFGR =
        apb4_pre << 4;

    // Set system clock
    RCC->CFGR |= 3; // Select PLL1p as system clock 
    
    while (!((RCC->CFGR & 3<<3) == 3<<3)){
        ; // Wait for PLL1 to be selected for CPU clock.
    }

    // By default MCO2 is hooked up to sysck, leave it be.
    RCC->CFGR &= ~(0xF << 25); // clear MCO2 prescaler
    RCC->CFGR |= (10 << 25); // prescale by 10


}

///
/// Reset handler and initial entry point. 
/// @todo Remap vector table for dynamic interrupts
///
__attribute__ ((noreturn)) void reset_handler(){

    /* Copy init values from text to data */
    uint32_t *init_values_ptr = &_sidata;
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

    // Copy vector table to RAM location
    for(unsigned int i = 0; i < sizeof(vector_table)/sizeof(*vector_table); i++){
        vector_table_ram[i] = vector_table[i];
    }

    // Remap vector table to RAM location
    SCB->VTOR = (uint32_t)vector_table_ram; //NOLINT

    enable_fpu();

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