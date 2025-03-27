#ifndef ADC_HPP
#define ADC_HPP

#include "../dma/dma.hpp"
#include "../system/stm32h750xx.h"
#include <cassert>

namespace adc {

enum eDivider {

};

///
/// Class for controlling the ADC on the STM32H750.
///
class OnChipADC {

    static const uint8_t NUM_CONVERSIONS = 16; ///< Maximum number of conversion that can be done on the 

  public:
    OnChipADC(uint8_t adcNum);

    ///
    /// Stop any ongoing conversions.
    ///
    void stop() {
        mControllerHw->CR |= (ADC_CR_ADSTP); // Stop the ADC
        while (mControllerHw->CR & ADC_CR_ADSTP) {
            mControllerHw->CR |= (ADC_CR_ADSTP); // Stop the ADC
            ; // wait for ADC to stop
        }
    }

    void setSequence(uint8_t *sequence, uint8_t len);

    void beginSingleConversion();

    void beginContinuousConversion();

    ///
    /// Get a conversion result for a certain conversion number.
    /// @param convIdx Index of conversion to return.
    /// @return The conversion result
    /// @note To correlate the conversion number to the channel, remember your
    /// set sequence
    ///
    uint16_t getConversion(uint8_t convIdx) {
        if (convIdx >= NUM_CONVERSIONS) {
            return 0;
        }
        return mConversions[convIdx];
    }

  private:
    void calibrate();

    ///
    /// Disable the ADC.
    ///
    void disable(){
        stop();
        mControllerHw->CR |= (ADC_CR_ADDIS); // Disable the ADC
        while (mControllerHw->CR & ADC_CR_ADDIS) {
            ; // wait for ADC to disable
        }
    }

    ///
    /// Enable the ADC.
    ///
    void enable(){
        mControllerHw->ISR |= ADC_ISR_ADRDY; // clear the ADC ready flag by writing a 1
        mControllerHw->CR |= ADC_CR_ADEN; // enable the ADC
        while (!(mControllerHw->ISR & ADC_ISR_ADRDY)) {
            ; // wait for ADC to enable
        }
        mControllerHw->ISR |= ADC_ISR_ADRDY; // clear the ADC ready flag by writing a 1
    }

    static bool sAdc12Reset; ///< True if ADCs 1 and 2 have been reset. They are reset together.
    uint8_t mSeqLen;
    uint8_t mAdcNum;            ///< Number of the ADC channel: 1, 2, or 3
    ADC_TypeDef *mControllerHw; ///< Pointer to the controller hw registers

    dma::DmaController::DmaChannel *mDmaChannel;

    uint16_t mConversions[NUM_CONVERSIONS];
};

} // namespace adc
#endif // ADC_HPP