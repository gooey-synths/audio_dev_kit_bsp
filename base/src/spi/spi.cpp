#include "spi.hpp"

#include "../util/util.h"

namespace spi {

SpiBusBase *sInstances[NUM_SPI_CONTROLLERS];
static constexpr const char* scIsActive = "SPI controller is active";

///
/// Constructor.
/// @param instance_num SPI bus number.
/// @todo reset for each bus.
///
SpiBusBase::SpiBusBase(size_t instance_num)
    : mIsActive(false), mSpiNum(instance_num), mKerClkFreq(0),
      // SPI 6 is connected to DMA2
      mTxDma(dma::DmaController::getInstance(1 + (size_t)(instance_num > 5))->claimAvailableChannel()),
      mRxDma(dma::DmaController::getInstance(1 + (size_t)(instance_num > 5))->claimAvailableChannel()) {

    errorIfTaken(instance_num);

    // enable and set SPI hardware.

    switch (instance_num) {
    case 1:
        mSpiHw = SPI1;
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        mKerClkFreq = PLL1_DIVQ_TARGET;
        mSpiIrqN = SPI1_IRQn;
        RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi1Isr);
        mTxDma->setRequest(dma::SPI1_TX_REQ);
        mRxDma->setRequest(dma::SPI1_RX_REQ);
        break;
    case 2:
        mSpiHw = SPI2;
        RCC->APB1LENR |= RCC_APB1LENR_SPI2EN;
        mKerClkFreq = PLL1_DIVQ_TARGET;
        mSpiIrqN = SPI2_IRQn;
        RCC->APB1LRSTR |= RCC_APB1LRSTR_SPI2RST;
        RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_SPI2RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi2Isr);
        mTxDma->setRequest(dma::SPI2_TX_REQ);
        mRxDma->setRequest(dma::SPI2_RX_REQ);
        break;
    case 3:
        mSpiHw = SPI3;
        RCC->APB1LENR |= RCC_APB1LENR_SPI3EN;
        mKerClkFreq = PLL1_DIVQ_TARGET; // todo: Update if needed.
        mSpiIrqN = SPI3_IRQn;
        RCC->APB1LRSTR |= RCC_APB1LRSTR_SPI3RST;
        RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_SPI3RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi3Isr);
        mTxDma->setRequest(dma::SPI3_TX_REQ);
        mRxDma->setRequest(dma::SPI3_RX_REQ);
        break;
    case 4:
        mSpiHw = SPI4;
        RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
        mKerClkFreq = 0; // todo: Update if needed.
        mSpiIrqN = SPI4_IRQn;
        RCC->APB2RSTR |= RCC_APB2RSTR_SPI4RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI4RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi4Isr);
        mTxDma->setRequest(dma::SPI4_TX_REQ);
        mRxDma->setRequest(dma::SPI4_RX_REQ);
        break;
    case 5:
        mSpiHw = SPI5;
        RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        mKerClkFreq = 0;
        mSpiIrqN = SPI5_IRQn;
        RCC->APB2RSTR |= RCC_APB2RSTR_SPI5RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI5RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi5Isr);
        mTxDma->setRequest(dma::SPI5_TX_REQ);
        mRxDma->setRequest(dma::SPI5_RX_REQ);
        break;
    case 6:
        mSpiHw = SPI6;
        RCC->APB4ENR |= RCC_APB4ENR_SPI6EN;
        mKerClkFreq = 0; // todo: Update if needed.
        mSpiIrqN = SPI6_IRQn;
        RCC->APB4RSTR |= RCC_APB4RSTR_SPI6RST;
        RCC->APB4RSTR &= ~(RCC_APB4RSTR_SPI6RST);
        set_vector_table_entry(static_cast<int>(mSpiIrqN) + 16, spi6Isr);
        mTxDma->setRequest(dma::SPI6_TX_REQ);
        mRxDma->setRequest(dma::SPI6_RX_REQ);
        break;
    }

    NVIC_DisableIRQ(mSpiIrqN);

    sInstances[mSpiNum - 1] = this;

    mSpiHw->CR1 &= ~SPI_CR1_SPE;                     // disable SPI
    mSpiHw->IER |= (SPI_IER_TXPIE | SPI_IER_RXPIE);  // Enable tx and rx interrupt.
    mSpiHw->CR1 |= SPI_CR1_SSI;                      // Set CS pin high
    mSpiHw->IFCR |= 0x1FF << SPI_IFCR_EOTC_Pos;      // Clear all interrupts
    mSpiHw->CFG2 |= SPI_CFG2_MASTER | SPI_CFG2_SSOE; // SPI master, SS output enable

    mTxDma->setTransferType(dma::MEM2PER, true);
    mRxDma->setTransferType(dma::PER2MEM, true);
}

///
/// Configure the SPI bus.
/// @param conf SPI bus configuration.
/// @note This might undershoot target frequency.
///
void SpiBusBase::configure(SpiBusConfig conf) {
    if(mIsActive) {
        throw scIsActive;
    }

    // Calculate and set divider for target frequency
    for (uint8_t iDiv = 0; iDiv <= 7; iDiv++) {
        mSpiHw->CFG1 &= ~(SPI_CFG1_MBR_Msk);      // Clear baud rate bits
        mSpiHw->CFG1 |= iDiv << SPI_CFG1_MBR_Pos; // Set baud rate bits
        if (mKerClkFreq / (1 << iDiv) <= conf.mFreq / 4) {
            break;
        }
    }

    // Configure phase
    if (conf.mPhase) {
        mSpiHw->CFG2 |= (SPI_CFG2_CPHA);
    } else {
        mSpiHw->CFG2 &= ~(SPI_CFG2_CPHA);
    }

    // Configure polarity
    if (conf.mPolarity) {
        mSpiHw->CFG2 |= (SPI_CFG2_CPOL);
    } else {
        mSpiHw->CFG2 &= ~(SPI_CFG2_CPOL);
    }

    // Configure IO swap
    if (conf.mIoSwap) {
        mSpiHw->CFG2 |= (SPI_CFG2_IOSWP);
    } else {
        mSpiHw->CFG2 &= ~(SPI_CFG2_IOSWP);
    }

    // Configure word size
    mSpiHw->CFG1 &= ~(SPI_CFG1_DSIZE_Msk);                             // Clear word size bits
    mSpiHw->CFG1 |= (conf.mWordSize - 1 & 0x1F) << SPI_CFG1_DSIZE_Pos; // Set word size bits

    // Configure master inter-data-idleness
    mSpiHw->CFG2 &= ~(SPI_CFG2_MIDI_Msk);                      // Clear MIDI
    mSpiHw->CFG2 |= ((conf.mMidi & 0xF) << SPI_CFG2_MIDI_Pos); // Set MIDI
}

///
/// Get the current bus configuration.
/// @return The current bus configuration.
///
SpiBusConfig SpiBusBase::getConfiguration() {
    SpiBusConfig ret;

    uint32_t div = mSpiHw->CFG1 & (SPI_CFG1_MBR_Msk) >> SPI_CFG1_MBR_Pos;
    ret.mFreq = mKerClkFreq / (1 << (div + 1));
    ret.mPhase = !!(mSpiHw->CFG2 & (SPI_CFG2_CPHA));
    ret.mPolarity = !!(mSpiHw->CFG2 & (SPI_CFG2_CPOL));
    ret.mIoSwap = !!(mSpiHw->CFG2 & (SPI_CFG2_IOSWP));
    ret.mWordSize = 1 + (mSpiHw->CFG1 & SPI_CFG1_DSIZE_Msk) >> SPI_CFG1_DSIZE_Pos;
    ret.mMidi = (mSpiHw->CFG2 & SPI_CFG2_MIDI_Msk) >> SPI_CFG2_MIDI_Pos;

    return ret;
}

///
/// Initialize HW control of the CS pin.
///
void HwCsSpiBus::HwCsSpiBusInit() {
    mSpiHw->CFG2 &= ~(SPI_CFG2_SSM); // Clear software management of CS pin.
    mSpiHw->CFG2 |= (SPI_CFG2_SSOM); //  Set SSOM, allows for inter-data idleness
    mSpiHw->CFG2 |= SPI_CFG2_IOSWP;  //  Swap CIPO and COPI
}

///
/// Inherit documentation.
/// @note Configured word size must round up to dataSize e.g. 24bits->4bytes, 8bits->1byte, not 8bit->4byte. 
/// @todo add ability to only receive or transmit.
///
void HwCsSpiBus::prepare(void *txBuff, void *rxBuff, size_t bufLen, size_t cs, size_t dataSize) {
    (void)cs;

    if(mIsActive) {
        throw scIsActive;
    }

    mRxDma->disable();
    mTxDma->disable();

    // Tx and Rx
    mTxDma->setSource(txBuff, dataSize, 1);
    mTxDma->setDest((void *)&mSpiHw->TXDR, dataSize, 0);
    mTxDma->setNumTransfers(bufLen, 0);

    mRxDma->setSource((void *)&mSpiHw->RXDR, dataSize, 0);
    mRxDma->setDest(rxBuff, dataSize, 1);
    mRxDma->setNumTransfers(bufLen, 0);

    mSpiHw->CR2 = bufLen;
}

///
/// Inherit documentation.
///
void HwCsSpiBus::transact() {
    if(mIsActive) {
        throw scIsActive;
    }

    mSpiHw->CR1 &= ~(SPI_CR1_SPE); // Disable SPI

    mSpiHw->IER |= SPI_IER_TXPIE | SPI_IER_RXPIE; // TX and RX interrupt

    mSpiHw->CFG1 &= ~(SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN); // Disable RX and TX DMA requests

    mSpiHw->IFCR |= 0x1FF << SPI_IFCR_EOTC_Pos; // Clear interrupt flags

    mTxDma->begin();
    mRxDma->begin();

    mSpiHw->CFG1 |= SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN; // Enable RX and TX DMA requests

    mSpiHw->CR1 |= SPI_CR1_SPE; // enable SPI

    mSpiHw->CR1 &= ~(SPI_CR1_SSI_Msk); // bring the CS pin low.

    mSpiHw->CR1 |= SPI_CR1_CSTART; // START

    mIsActive = true;
    NVIC_EnableIRQ(mSpiIrqN);
}

}; // namespace spi