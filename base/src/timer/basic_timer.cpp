#include "basic_timer.hpp"
#include "../system/board_defs.h"

using namespace timer;


///
/// Constructor.
/// @param timer_hw Pointer to the timer register struct.
/// @param timer_num Timer Number
/// @param freq Target frequency in Hz
///
BasicTimer::BasicTimer(TIM_TypeDef* timer_hw, uint8_t timer_num, uint32_t freq):
mTimerHw(timer_hw),
mTimerNum(timer_num),
mFreq(freq)
{

    switch(timer_num){
        case 6:
            RCC->APB1LENR |= 1 << 4; // Enable clock to hw
            RCC->APB1LRSTR |= 1 << 4; // Set reset bit;
            RCC->APB1LRSTR &= ~(1 << 4); // Clear reset bit;
            break;

        case 7:
            RCC->APB1LENR |= 1 << 5; // Enable clock to hw
            RCC->APB1LRSTR |= 1 << 5; // Set reset bit;
            RCC->APB1LRSTR &= ~(1 << 5); // Clear reset bit;
            break;
    }

    // See table 56 in STM RM0433 for explanation.
    uint8_t d2ppre1 = (RCC->D2CFGR & (7 << 4)) >> 4; // Figure out the APB1 prescaler.
    uint8_t timpre = (RCC->CFGR & (1 << 15)) >> 15; // Figure out if timpre bit is set.

    uint32_t ker_clk_freq = APB1_TARGET;
    if(d2ppre1 > 3){
        ker_clk_freq *= 2;
        if(timpre && d2ppre1 > 4){
            ker_clk_freq *= 2;
        }
    }

    mTimerHw->PSC = ker_clk_freq / mFreq;

    mTimerHw->EGR = 1;
}