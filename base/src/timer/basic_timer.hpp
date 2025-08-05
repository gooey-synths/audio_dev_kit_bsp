#include <stdint.h>
#include "../system/stm32h750xx.h"

namespace timer{

class BasicTimer{
public:
    BasicTimer(TIM_TypeDef* timer_hw, uint8_t timer_num, uint32_t freq);

    ///
    /// Start the timer.
    ///
    inline void start(){
        mTimerHw->CR1 |= 1<<0; // Enable the timer.
    }
    
    ///
    /// Stop the timer.
    ///
    inline void stop(){
        mTimerHw->CR1 &= ~(1<<0); // Disable the timer.
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
    inline uint16_t getTime(){
        return mTimerHw->CNT & 0xFFFF;
    }

private:
    TIM_TypeDef* mTimerHw;
    uint8_t mTimerNum;
    uint32_t mFreq;


};






}// namespace timer