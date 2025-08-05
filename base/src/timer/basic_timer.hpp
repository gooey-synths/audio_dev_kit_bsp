#include <stdint.h>
#include "../system/stm32h750xx.h"

namespace timer{

class BasicTimer{
public:
    BasicTimer(uint8_t timerNum, uint32_t freq);

    ///
    /// Start the timer.
    ///
    inline void start(){
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

private:

    static uint32_t getKerFreq();

    TIM_TypeDef* mTimerHw; // Pointer to timer hw
    uint8_t mTimerNum;
    uint32_t mFreq;
};






}// namespace timer