#ifndef BASIC_TIMER_HPP
#define BASIC_TIMER_HPP

extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include "../system/stm32h750xx.h"
#include "../util/util.h"
}

namespace timer {

///
/// Class for operating a basic timer
///
class BasicTimer {
public:

    /// Number of basic timers available.
    static constexpr size_t NUM_BASIC_TIMERS = 2;

    BasicTimer(uint8_t timerNum);

    virtual ~BasicTimer();

    ///
    /// Start the timer.
    /// @param oneShot If the timer should only run once
    ///
    inline void start(bool oneShot) {
        mTimerHw->SR = 0;
        mTimerHw->CR1 |= TIM_CR1_CEN; // Enable the timer.
    }
    
    ///
    /// Stop the timer.
    ///
    inline void stop(){
        mTimerHw->CR1 &= ~(TIM_CR1_CEN); // Disable the timer.
    }

    ///
    /// Set the timer to 0.
    ///
    inline void reset(){
        mTimerHw->CNT = 0;
    }

    ///
    /// Get the current timer count.
    ///
    inline uint16_t getCount(){
        return mTimerHw->CNT & 0xFFFF;
    }

    void setFreq(uint32_t freq);

    uint32_t getFreq();

    void SetInterrupt(InterruptFunctionPtr func);

private:
    static uint32_t getKerFreq();

    template<size_t instanceNum>
    static void timerIsr() {
        if(sInstances[instanceNum]) {
            sInstances[instanceNum]->mTimerHw->SR = 0;
            if(sInstances[instanceNum]->mIntFunc) {
                sInstances[instanceNum]->mIntFunc();
            }
        }
    }
    
    #if 0

    static void basicTimer6Isr() {         
        if (sInstances[0]) {
            sInstances[0]->mTimerHw->SR = 0;
            if(sInstances[0]->mIntFunc) {
                sInstances[0]->mIntFunc();
            }
        }
    };

    static void basicTimer7Isr() {
        if (sInstances[1]) {
            sInstances[1]->mTimerHw->SR = 0;
            if(sInstances[1]->mIntFunc) {
                sInstances[1]->mIntFunc();
            }
        }
    }

    #endif

    static BasicTimer *sInstances[NUM_BASIC_TIMERS]; ///< Instances of the basic timers

    TIM_TypeDef* mTimerHw;         ///< Pointer to timer hw
    InterruptFunctionPtr mIntFunc; ///< Interrupt function.
    IRQn_Type mBTimIrqN;           ///< Interrupt number for the basic timer.

};

}// namespace timer

#endif // BASIC_TIMER_HPP