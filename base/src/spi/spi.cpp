#include "spi.hpp"

#include "../util/util.h"

namespace spi{

SpiBusBase* sInstances[NUM_SPI_CONTROLLERS];

///
/// Constructor.
/// @param instance_num SPI bus number.
/// @todo reset for each bus.
///
SpiBusBase::SpiBusBase(size_t instance_num):
    mIsActive(false),
    mSpiNum(instance_num),
    mKerClkFreq(0),
    // SPI 6 is connected to DMA2
    mTxDma(dma::DmaController::getInstance(1+(size_t)(instance_num>5))->claimAvailableChannel()),
    mRxDma(dma::DmaController::getInstance(1+(size_t)(instance_num>5))->claimAvailableChannel())
{
    errorIfTaken(instance_num);

    // enable and set SPI hardware.

    switch(instance_num){
        case 1:
            mSpiHw = SPI1;
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            mKerClkFreq = PLL1_DIVQ_TARGET;
            mSpiIrqN = SPI1_IRQn;
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
            RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI1RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi1Isr);
            mTxDma->setRequest(38);
            mRxDma->setRequest(37);
            break;
        case 2:
            mSpiHw = SPI2;
            RCC->APB1LENR |= RCC_APB1LENR_SPI2EN;
            mKerClkFreq = PLL1_DIVQ_TARGET;
            mSpiIrqN = SPI2_IRQn;
            RCC->APB1LRSTR |= RCC_APB1LRSTR_SPI2RST;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_SPI2RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi2Isr);
            mTxDma->setRequest(40);
            mRxDma->setRequest(39);
            break;
        case 3:
            mSpiHw = SPI3;
            RCC->APB1LENR |= RCC_APB1LENR_SPI3EN;
            mKerClkFreq = PLL1_DIVQ_TARGET; // todo: Update if needed.
            mSpiIrqN = SPI3_IRQn;
            RCC->APB1LRSTR |= RCC_APB1LRSTR_SPI3RST;
            RCC->APB1LRSTR &= ~(RCC_APB1LRSTR_SPI3RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi3Isr);
            mTxDma->setRequest(61);
            mRxDma->setRequest(62);
            break;
        case 4:
            mSpiHw = SPI4;
            RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
            mKerClkFreq = 0; // todo: Update if needed.
            mSpiIrqN = SPI4_IRQn;
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI4RST;
            RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI4RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi4Isr);
            mTxDma->setRequest(84);
            mRxDma->setRequest(83);
            break;
        case 5:
            mSpiHw = SPI5;
            RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
            mKerClkFreq = 0;
            mSpiIrqN = SPI5_IRQn;
            RCC->APB2RSTR |= RCC_APB2RSTR_SPI5RST;
            RCC->APB2RSTR &= ~(RCC_APB2RSTR_SPI5RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi5Isr);
            mTxDma->setRequest(86);
            mRxDma->setRequest(85);
            break;
        case 6:
            mSpiHw = SPI6;
            RCC->APB4ENR |= RCC_APB4ENR_SPI6EN;
            mKerClkFreq = 0; // todo: Update if needed.
            mSpiIrqN = SPI6_IRQn;
            RCC->APB4RSTR |= RCC_APB4RSTR_SPI6RST;
            RCC->APB4RSTR &= ~(RCC_APB4RSTR_SPI6RST);
            set_vector_table_entry((int)mSpiIrqN+16, (uint32_t*)spi6Isr);
            mTxDma->setRequest(12);
            mRxDma->setRequest(11);
            break;
    }

    NVIC_DisableIRQ(mSpiIrqN);

    sInstances[mSpiNum-1] = this;

    mSpiHw->CR1 &= ~SPI_CR1_SPE; // disable SPI
    mSpiHw->IER |= (SPI_IER_TXPIE | SPI_IER_RXPIE); // Enable tx and rx interrupt.
    mSpiHw->CR1 |= SPI_CR1_SSI; // Set CS pin high
    mSpiHw->IFCR |= 0x1FF << SPI_IFCR_EOTC_Pos; // Clear all interrupts
    mSpiHw->CFG2 |= SPI_CFG2_MASTER | SPI_CFG2_SSOE; // SPI master, SS output enable

    mTxDma->setTransferType(dma::MEM2PER, true);
    mRxDma->setTransferType(dma::PER2MEM, true);

    mTxDma->setDest((void*)&mSpiHw->TXDR, sizeof(uint32_t), 0);
    mRxDma->setSource((void*)&mSpiHw->RXDR, sizeof(uint32_t), 0);
}



///
/// Configure the SPI bus.
/// @param conf SPI bus configuration.
/// @note This might undershoot target frequency.
///
void SpiBusBase::configure(SpiBusConfig conf){

    // Calculate and set divider for target frequency
    for(uint8_t iDiv = 0; iDiv <= 7; iDiv++){
        mSpiHw->CFG1 &= ~(SPI_CFG1_MBR_Msk);  // Clear baud rate bits
        mSpiHw->CFG1 |= iDiv << SPI_CFG1_MBR_Pos; // Set baud rate bits
        if(mKerClkFreq/(1<<iDiv) <= conf.mFreq/4){
            break;
        }
    }

    // Configure phase
    if(conf.mPhase){
        mSpiHw->CFG2 |= (SPI_CFG2_CPHA);
    }
    else{
        mSpiHw->CFG2 &= ~(SPI_CFG2_CPHA);
    }

    // Configure polarity
    if(conf.mPolarity){
        mSpiHw->CFG2 |= (SPI_CFG2_CPOL);
    }
    else{
        mSpiHw->CFG2 &= ~(SPI_CFG2_CPOL);
    }

    // Configure word size
    mSpiHw->CFG1 &= ~(SPI_CFG1_DSIZE_Msk); // Clear word size bits
    mSpiHw->CFG1 |= (conf.mWordSize-1 & 0x1F) << SPI_CFG1_DSIZE_Pos; // Set word size bits

    // Configure master inter-data-idleness
    mSpiHw->CFG2 &= ~(SPI_CFG2_MIDI_Msk); // Clear MIDI
    mSpiHw->CFG2 |= ((conf.mMidi & 0xF) << SPI_CFG2_MIDI_Pos); // Set MIDI

}


///
/// Initialize HW control of the CS pin.
///
void HwSpiBus::HwSpiInit(){
    mSpiHw->CFG2 &= ~(SPI_CFG2_SSM); // Clear software management of CS pin.
    mSpiHw->CFG2 |= (SPI_CFG2_SSOM); //  Set SSOM, allows for inter-data idleness
    mSpiHw->CFG2 |= SPI_CFG2_IOSWP; //  Swap CIPO and COPI
}


///
/// Inherit documentation.
/// @todo add ability to only receive or transmit.
///
void HwSpiBus::prepare(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize){
    (void)cs;

    mRxDma->disable();
    mTxDma->disable();

    // Tx and Rx
    mTxDma->setSource(txBuff, dataSize, 1);
    mTxDma->setDest((void*)&mSpiHw->TXDR, dataSize, 0);
    mTxDma->setNumTransfers(bufLen, 0);

    mRxDma->setSource((void*)&mSpiHw->RXDR, dataSize, 0);
    mRxDma->setDest(rxBuff, dataSize, 1);
    mRxDma->setNumTransfers(bufLen, 0);

    mSpiHw->CR2 = bufLen/dataSize;

}

///
/// Inherit documentation.
///
void HwSpiBus::transact(){
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