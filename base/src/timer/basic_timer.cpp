#include "basic_timer.hpp"
#include "../system/board_defs.h"

using namespace timer;

/// Message when timer has already been claimed.
const char* const BasicTimer::scAlreadyClaimedMsg = "Timer already claimed";
/// Message when requested timer is invalid.
const char* const BasicTimer::scInvalidTimerMsg = "Invalid timer number";

///
/// Constructor.
/// @param timerNum Timer number.
/// @note Timer is disabled at instantiation.
///
BasicTimer::BasicTimer(eBasicTimerNumber timerNum):
mInstanceIdx(timerNum - 6),
mIntFunc(NULL)
{
    if (mInstanceIdx >= NUM_BASIC_TIMERS) {
        throw scInvalidTimerMsg;
    }

    if(sInstances[mInstanceIdx]) {
        throw scAlreadyClaimedMsg;
    }

    sInstances[mInstanceIdx] = this;

    switch(timerNum){
        case 6:
            mTimerHw = TIM6;
            mBTimIrqN = TIM6_DAC_IRQn;
            set_vector_table_entry(static_cast<int>(mBTimIrqN)+16, timerIsr<0>);
            RCC->APB1LENR |= RCC_APB1LENR_TIM6EN; // Enable clock to hw
            RCC->APB1LRSTR |= RCC_APB1LRSTR_TIM6RST; // Set reset bit;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_TIM6RST); // Clear reset bit;
            break;

        case 7:
            mTimerHw = TIM7;
            mBTimIrqN = TIM7_IRQn;
            set_vector_table_entry(static_cast<int>(mBTimIrqN)+16, timerIsr<1>);
            RCC->APB1LENR |= RCC_APB1LENR_TIM7EN; // Enable clock to hw
            RCC->APB1LRSTR |= RCC_APB1LRSTR_TIM7RST; // Set reset bit;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_TIM7RST); // Clear reset bit;
            break;
    }

    stop();
}

///
/// Destructor.
///
BasicTimer::~BasicTimer() {
    stop();
    sInstances[mInstanceIdx] = NULL;
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

    uint32_t counts = getKerFreq() / freq;

    uint16_t psc = 1;
    while(counts > 0xFFFF) {
        psc *= 2;
        counts /= 2;
    }

    mTimerHw->PSC = psc-1;
    mTimerHw->ARR = counts;
}

///
/// Get the current freuency of the timer.
/// @return The current freuency of the timer.
///
uint32_t BasicTimer::getFreq() {
    return getKerFreq() / ((mTimerHw->PSC+1) * mTimerHw->ARR);
}

///
/// Set timer interrupt.
/// @param func Function pointer to interrupt handler, NULL to disable.
///
void BasicTimer::setInterrupt(InterruptFunctionPtr func) {
    mIntFunc = func;

    if(func) {
        NVIC_EnableIRQ(mBTimIrqN);
        mTimerHw->DIER |= TIM_DIER_UIE;
    } else {
        NVIC_DisableIRQ(mBTimIrqN);
        mTimerHw->DIER &= ~TIM_DIER_UIE;
    }
}

BasicTimer* BasicTimer::sInstances[NUM_BASIC_TIMERS] = {NULL, NULL};