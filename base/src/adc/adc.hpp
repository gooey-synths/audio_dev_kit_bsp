#ifndef ADC_HPP
#define ADC_HPP

#include "adc_interface.hpp"
#include "../dma/dma.hpp"
#include "../system/stm32h750xx.h"
#include <cassert>

namespace adc {

///
/// Class for controlling the ADC on the STM32H750.
///
class OnChipADC : IOnChipADC {

    static const uint8_t NUM_CONVERSIONS = 16; ///< Maximum number of conversion that can be done on the ADC.

  public:
    OnChipADC(uint8_t adcNum);

    ///
    /// Stop any ongoing conversions.
    ///
    void stop() {
        mDmaChannel->disable();
        
        mControllerHw->CR |= (ADC_CR_ADSTP); // Stop the ADC
        while (mControllerHw->CR & ADC_CR_ADSTP) {
            ; // Wait for stop command to process
        }
    }

    void setSequence(uint8_t *sequence, uint8_t len);

    void beginConversion(bool continuous);

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
    void setBoostBits();

    void calibrate();

    ///
    /// Disable the ADC.
    ///
    void disable() {
        stop();
        mControllerHw->CR |= (ADC_CR_ADDIS); // Disable the ADC
        while (mControllerHw->CR & ADC_CR_ADDIS) {
            ; // Wait for disable command to process.
        }
    }

    ///
    /// Enable the ADC.
    ///
    void enable() {
        mControllerHw->ISR |= ADC_ISR_ADRDY; // clear the ADC ready flag by writing a 1
        mControllerHw->CR |= ADC_CR_ADEN;    // enable the ADC
        while (!(mControllerHw->ISR & ADC_ISR_ADRDY)) {
            ; // wait for ADC to enable
        }
        mControllerHw->ISR |= ADC_ISR_ADRDY; // clear the ADC ready flag by writing a 1
    }

    static bool sAdc12Reset; ///< True if ADCs 1 and 2 have been reset. They are reset together.
    uint8_t mSeqLen; ///< Current set sequence length
    uint8_t mControllerNum; ///< ADC controller number.
    ADC_TypeDef *mControllerHw; ///< Pointer to the controller hw registers

    dma::DmaController::DmaChannel *mDmaChannel;

    uint16_t mConversions[NUM_CONVERSIONS];
};

} // namespace adc
#endif // ADC_HPP