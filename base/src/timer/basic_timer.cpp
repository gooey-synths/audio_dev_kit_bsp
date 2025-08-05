#include "basic_timer.hpp"
#include "../system/board_defs.h"

using namespace timer;

///
/// Constructor.
/// @param timerNum Timer number.
/// @param freq Target frequency in Hz.
/// @note Timer is disabled at instantiation.
///
BasicTimer::BasicTimer(uint8_t timerNum, uint32_t freq):
mTimerNum(timerNum),
mFreq(freq)
{
    switch(timerNum){
        case 6:
            mTimerHw = TIM6;
            RCC->APB1LENR |= RCC_APB1LENR_TIM6EN; // Enable clock to hw
            RCC->APB1LRSTR |= RCC_APB1LRSTR_TIM6RST; // Set reset bit;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_TIM6RST); // Clear reset bit;
            break;

        case 7:
            mTimerHw = TIM7;
            RCC->APB1LENR |= RCC_APB1LENR_TIM7EN; // Enable clock to hw
            RCC->APB1LRSTR |= RCC_APB1LRSTR_TIM7RST; // Set reset bit;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_TIM7RST); // Clear reset bit;
            break;

        default:
            throw "Invalid timer number";
    }

    stop();
}

///
/// Get the frequency of the timer kernel clock.
/// @return The frequency of the timer kernel clock.
///
uint32_t BasicTimer::getKerFreq() {
    // See table 57 in STM RM0433 for explanation.
    uint8_t d2ppre1 = (RCC->D2CFGR & (RCC_D2CFGR_D2PPRE1_DIV16_Msk)) >> RCC_D2CFGR_D2PPRE1_Pos; // Figure out the APB1 prescaler.
    bool timpre = !!(RCC->CFGR & (1 << RCC_CFGR_TIMPRE_Pos)); // Figure out if timpre bit is set.
    
    uint32_t kerClkFreq = APB1_TARGET;

    if(timpre) {
        switch (d2ppre1) {
        case 0:
        case 1:
        case 2:
        case 3:
            kerClkFreq *= 1; // Timer clock is equal to bus clock.
            break;
        case 4:
            kerClkFreq *= 2; // Timer clock is 2 times faster than bus clock.
            break;
        case 5:
        case 6:
        case 7:
            kerClkFreq *= 4; // Timer clock is 4 times faster than bus clock.
            break;
        }

    } else {
        switch (d2ppre1) {
        case 0:
        case 1:
        case 2:
        case 3:
            kerClkFreq *= 1; // Timer clock is equal to bus clock.
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            kerClkFreq *= 2; // Timer clock is equal to bus clock.
            break;
        }
    }

    return kerClkFreq;
}

///
/// Set the frequency of the timer.
/// @param freq Desired frequency in Hz.
/// @note timer must be stopped before changing the frequency
///
void BasicTimer::setFreq(uint32_t freq) {
    mTimerHw->PSC = getKerFreq() / mFreq;

    mTimerHw->EGR = TIM_EGR_UG;
}

///
/// Get the current freuency of the timer.
/// @return The current freuency of the timer.
///
uint32_t BasicTimer::getFreq() {
    return getKerFreq() / mTimerHw->PSC
}