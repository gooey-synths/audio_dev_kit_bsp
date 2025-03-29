#include "adc.hpp"

namespace adc {

bool OnChipADC::sAdc12Reset = false;

///
/// Constructor
/// @param adcNum ADC number (1..3)
/// @note This assumes a rev. V microcontroller.
/// @note This function will reset the ADC controller. ADC 1 and 2 are tied
/// together so they will be reset together.
/// @todo probably clean this into an ADC12 or 3 controller, subclasses?
///
OnChipADC::OnChipADC(uint8_t adcNum)
    : mAdcNum(adcNum), mDmaChannel(dma::DmaController::getInstance(1)->claimAvailableChannel()) {
    assert(adcNum > 0 && adcNum < 4);
    assert(mDmaChannel);
    
    RCC->D3CCIPR |= RCC_D3CCIPR_ADCSEL_1;
    // Select per_ck as kernel clock. Currently per_ck is set to be HSI
    // This only matters if ADCx_CCR CKMODE is 0.
    // This does mean that bus and conversion clocks are not synchronized.
    // Change ADCx_CCR CKMODE > 0 if they should be. I don't think they do?

    uint8_t dmaReq = 0;

    switch (adcNum) {
    case 1:
        mControllerHw = ADC1;
        if (!sAdc12Reset) {
            RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;    // enable ADC 1 and 2
            RCC->AHB1RSTR |= RCC_AHB1RSTR_ADC12RST; // reset ADC 1 and 2
            RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_ADC12RST);
            ADC12_COMMON->CCR &= ~(ADC_CCR_PRESC_Msk); // clear prescaler bits
            // TODO: make this more dynamic.
            ADC12_COMMON->CCR |= 0x00 << ADC_CCR_PRESC_Pos; // set 1 prescaler
            sAdc12Reset = true;
        }
        dmaReq = 9;
        break;

    case 2:
        mControllerHw = ADC2;
        if (!sAdc12Reset) {
            RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;    // enable ADC 1 and 2
            RCC->AHB1RSTR |= RCC_AHB1RSTR_ADC12RST; // reset ADC 1 and 2
            RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_ADC12RST);
            ADC12_COMMON->CCR &= ~(ADC_CCR_PRESC_Msk); // clear prescaler bits
            // TODO: make this more dynamic.
            ADC12_COMMON->CCR |= 0x00 << ADC_CCR_PRESC_Pos; // set 1 prescaler
            sAdc12Reset = true;
        }
        dmaReq = 10;
        break;

    case 3:
        mControllerHw = ADC3;
        RCC->AHB1ENR |= RCC_AHB4ENR_ADC3EN;    // enable ADC 3
        RCC->AHB1RSTR |= RCC_AHB4RSTR_ADC3RST; // reset ADC 3
        RCC->AHB1RSTR &= ~(RCC_AHB4RSTR_ADC3RST);
        ADC3_COMMON->CCR &= ~(ADC_CCR_PRESC_Msk); // clear prescaler bits
        // TODO: make this more dynamic.
        ADC3_COMMON->CCR |= 0x00 << ADC_CCR_PRESC_Pos; // set 1 prescaler
        sAdc12Reset = true;
        dmaReq = 115;
        break;
    }

    disable();
    calibrate();

    // Zero conversion buffer
    for (uint16_t& iConversion : mConversions) {
        iConversion = 0;
    }

    // Configure DMA channel
    mDmaChannel->setTransferType(dma::PER2MEM, true);
    mDmaChannel->setDest((void *)mConversions, sizeof(*mConversions), true);
    mDmaChannel->setSource((void *)&mControllerHw->DR, sizeof(uint16_t), 0);
    mDmaChannel->setRequest(dmaReq);
}

///
/// Set the sequence to convert
/// @param sequence pointer to sequence
/// @param len Length of the sequence, max 16
///
void OnChipADC::setSequence(uint8_t *sequence, uint8_t len) {
    if (len > 16) {
        return;
    }

    mControllerHw->SQR1 &= ~(ADC_SQR1_L_Msk);           // Clear sequence length bits
    mControllerHw->SQR1 |= (len - 1) << ADC_SQR1_L_Pos; // Set sequence length bits

    const uint8_t offsetDelta = (ADC_SQR1_SQ2_Pos - ADC_SQR1_SQ1_Pos);
    uint8_t offsetCounter = ADC_SQR1_SQ1_Pos;
    uint8_t registerCounter = 1;
    for (uint8_t iChannel = 0; iChannel < len; iChannel++) {
        volatile uint32_t *reg = NULL;

        switch (registerCounter) {
        case 1:
            reg = &(mControllerHw->SQR1);
            break;
        case 2:
            reg = &(mControllerHw->SQR2);
            break;
        case 3:
            reg = &(mControllerHw->SQR3);
            break;
        case 4:
            reg = &(mControllerHw->SQR4);
            break;
        }

        *reg &= ~(0x1F << offsetCounter);                     // clear channel bits
        *reg |= (sequence[iChannel] & 0x1F) << offsetCounter; // set channel bits
        mControllerHw->PCSEL |= 1 << (sequence[iChannel] & 0x1F);
        offsetCounter += offsetDelta;

        // offset counter overflow, reset and go to the next register.
        if (offsetCounter > 24) {
            offsetCounter = 0;
            registerCounter++;
        }
    }

    mSeqLen = len;
}

///
/// Begin a single sequence conversion
///
void OnChipADC::beginSingleConversion() {
    disable();

    mControllerHw->ISR = 0x3FF << ADC_ISR_ADRDY_Pos; // Clear interrupts
    mControllerHw->CFGR &= ~(ADC_CFGR_CONT);         // Clear continous bit

    mControllerHw->CFGR &= ~(ADC_CFGR_DMNGT_Msk); // clear DMNGT bits
    mControllerHw->CFGR |= ADC_CFGR_DMNGT_0;      // set DMNGT bits for one shot mode

    mControllerHw->CFGR |= ADC_CFGR_AUTDLY; // enable AUTODLY

    mDmaChannel->disable();

    mDmaChannel->setNumTransfers(mSeqLen, false);

    mDmaChannel->begin();

    enable();

    mControllerHw->CR |= ADC_CR_ADSTART; // set start adc bit
}

///
/// Begin continuously converting a sequence
///
void OnChipADC::beginContinuousConversion() {
    disable();

    mControllerHw->ISR = 0x3FF << ADC_ISR_ADRDY_Pos; // Clear interrupts
    mControllerHw->CFGR |= ADC_CFGR_CONT;            // Set continous bit

    mControllerHw->CFGR |= 0x03 << ADC_CFGR_DMNGT_Pos; // Set DMNGT bits DMA circulare mode.

    mControllerHw->CFGR |= ADC_CFGR_AUTDLY; // enable AUTODLY

    mDmaChannel->disable();

    mDmaChannel->setNumTransfers(mSeqLen, true);

    mDmaChannel->begin();

    enable();
    mControllerHw->CR |= ADC_CR_ADSTART; // set start adc bit
}

///
/// Calibrate the ADC pg. 925 RM0433
/// @todo Save calibration factors and load that instead of calibrating each
/// time.
///
void OnChipADC::calibrate() {
    disable();

    static bool linearityCal = false; // linearity calibration must be done only once

    mControllerHw->CR &= ~(ADC_CR_DEEPPWD); // Disable DEEP PWD
    mControllerHw->CR |= ADC_CR_ADVREGEN;   // Enable ADC vreg

    while (!(mControllerHw->ISR & ADC_ISR_LDORDY)) {
        ; // Wait for the LDO to be enabled
    }

    for (int i = 0; i < 0x1FFFFF; i++) {
        ; // Wait for transfer to happen, not sure if this is needed.
    }

    mControllerHw->CR &=
        ~(1 << ADC_CR_ADCALDIF_Pos); // Clear differential calibration bit for single ended calibration.
    if (!linearityCal) {
        mControllerHw->CR |= ADC_CR_ADCALLIN; // Enable linearity calibration
    }

    mControllerHw->CR |= ADC_CR_ADCAL; // set ADC calibration bit

    while ((mControllerHw->CR & ADC_CR_ADCAL)) {
        ; // Wait for the calibration to complete
    }
    if (!linearityCal) {
        linearityCal = true; // Linearity calibration must be done only once
    }
}

} // namespace adc