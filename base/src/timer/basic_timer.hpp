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
/// Enumeration of basic timer numbers.
///
enum eBasicTimerNumber : uint8_t {
    BASIC_TIMER_6 = 6,
    BASIC_TIMER_7 = 7,
};


///
/// Class for operating a basic timer
///
class BasicTimer {
public:
    static const char* const scAlreadyClaimedMsg;
    static const char* const scInvalidTimerMsg;

    /// Number of basic timers available.
    static constexpr size_t NUM_BASIC_TIMERS = 2;

    BasicTimer(eBasicTimerNumber timerNum);

    virtual ~BasicTimer();

    ///
    /// Start the timer.
    /// @param oneShot If the timer should only run once.
    ///
    inline void start(bool oneShot) {
        if(oneShot) {
            mTimerHw->CR1 |= TIM_CR1_OPM;
        } else {
            mTimerHw->CR1 &= ~TIM_CR1_OPM;
        }

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
    /// Get the current timer count.
    ///
    inline uint16_t getCount(){
        return mTimerHw->CNT & 0xFFFF;
    }

    void setFreq(uint32_t freq);

    uint32_t getFreq();

    void setInterrupt(InterruptFunctionPtr func);

private:
    static uint32_t getKerFreq();

    ///
    /// Timer interrupt service routine
    /// @tparam instanceNum instacne number 
    ///
    template<size_t instanceNum>
    static void timerIsr() {
        if(sInstances[instanceNum]) {
            sInstances[instanceNum]->mTimerHw->SR = 0;
            if(sInstances[instanceNum]->mIntFunc) {
                sInstances[instanceNum]->mIntFunc();
            }
        }
    }
    
    static BasicTimer *sInstances[NUM_BASIC_TIMERS]; ///< Instances of the basic timers

    TIM_TypeDef* mTimerHw;         ///< Pointer to timer hw.
    size_t mInstanceIdx;           ///< Instance index.
    InterruptFunctionPtr mIntFunc; ///< Interrupt function.
    IRQn_Type mBTimIrqN;           ///< Interrupt number for the basic timer.

};

} // namespace timer

#endif // BASIC_TIMER_HPP