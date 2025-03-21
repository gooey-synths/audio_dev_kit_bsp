#ifndef SPI_HPP
#define SPI_HPP

#include "../system/stm32h750xx.h"
#include "../system/board_defs.h"
#include "../dma/dma.hpp"
#include "../gpio/gpio.hpp"

#include <cassert>
#include <stdint.h>


namespace spi{

#define NUM_SPI_CONTROLLERS 6 ///< Number of spi controllers on the stm32h750.


class SpiBusBase; ///< forward reference.

extern SpiBusBase* sInstances[NUM_SPI_CONTROLLERS]; ///< instances of the spi buses 

///
/// Configuration for SPI buses.
///
struct SpiBusConfig{
    bool mPolarity;             ///< Clock polarity
    bool mPhase;                ///< Clock phase
    bool mIoSwap;               ///< True if IO lines should be swapped
    uint32_t mFreq;             ///< Clock frequency
    uint8_t mWordSize;          ///< Data word size in bits
    uint8_t mMidi;              ///< Master inter-data idleness. Only valid for HW spi bus
};



///
/// Base class for SPI buses.
/// @todo CRC?
///
class SpiBusBase {

public:

    SpiBusBase(size_t);

    ///
    /// Destructor.
    ///
    virtual ~SpiBusBase() override {
        sInstances[mSpiNum] = NULL;
    }

    ///
    /// Configure the bus
    /// @param conf SPI bus configuration to set.
    ///
    void configure(SpiBusConfig conf);

    ///
    /// Prepare for a transaction.
    /// @param txBuff Pointer to transmission bufffer.
    /// @param rxBuff Pointer to reception buffer.
    /// @param buffLen BuffLen Length of buffer in bytes.
    /// @param cs Chip select index to assert.
    /// @param dataSize Size of a single data packet. 1,2,4 bytes.
    /// @note Buffers must stay valid until transaction is finished.
    ///
    virtual void prepare(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize) = 0;

    ///
    /// Perform a transaction on the SPI bus
    /// @note The bus must have been previously prepared using @ref prepare. This method can be called multiple times for the same transaction.
    ///
    virtual void transact() = 0;


    ///
    /// Wait for the SPI bus to complete.
    ///
    void waitForCompletion(){
        while(mIsActive){
            ; // Do nothing
        }
    }

protected:

    ///
    /// SPI interrupt.
    ///
    void interrupt(){

        // transfer complete flag
        if(mSpiHw->SR & SPI_SR_EOT){
            cleanup();

            NVIC_DisableIRQ(mSpiIrqN);

            mIsActive = false;

            mSpiHw->IFCR |= 0x1FF << 3; // clear interrupt flags

        }

    }

    ///
    /// This function is called at the end of a transaction
    ///
    virtual void cleanup() = 0;

    static void spi1Isr(){if(sInstances[0] != NULL){sInstances[0]->interrupt();}}
    static void spi2Isr(){if(sInstances[1] != NULL){sInstances[1]->interrupt();}}
    static void spi3Isr(){if(sInstances[2] != NULL){sInstances[2]->interrupt();}}
    static void spi4Isr(){if(sInstances[3] != NULL){sInstances[3]->interrupt();}}
    static void spi5Isr(){if(sInstances[4] != NULL){sInstances[4]->interrupt();}}
    static void spi6Isr(){if(sInstances[5] != NULL){sInstances[5]->interrupt();}}

    uint8_t mSpiNum;         ///< Number of the SPI bus.
    volatile bool mIsActive; ///< True if the SPI is currently active.
    IRQn_Type mSpiIrqN;      ///< Interrupt number for the SPI controller.
    SPI_TypeDef* mSpiHw;     ///< SPI peripheral pointer.
    SpiBusConfig mConfig;    ///< SPI bus configuration.
    uint32_t mKerClkFreq;    ///< Frequency of the SPI_KER_CLK.
    dma::DmaController::DmaChannel* mTxDma; ///< TX DMA channel
    dma::DmaController::DmaChannel* mRxDma; ///< RX DMA channel


private:

    ///
    /// Throw an exception if multiple instances are decalared of the same SPI bus.
    /// @param instance_num Spi bus number
    /// @todo Add exception.
    ///
    static void errorIfTaken(size_t instanceNum){
        instanceNum -= 1; // 1 index to 0 index conversion
        if(instanceNum > NUM_SPI_CONTROLLERS || sInstances[instanceNum]){
            assert(false);
        }
    }
};


extern SpiBusBase* sInstances[NUM_SPI_CONTROLLERS]; ///< instances of the spi buses 

///
/// Hw controlled chip select spi bus
/// @note Can only control 1 chip select line
///
class HwSpiBus : public SpiBusBase{

public:
    ///
    /// Constructor.
    /// @param instance_num SPI bus number.
    ///
    HwSpiBus(size_t instance_num):
        SpiBusBase(instance_num)
    {
        HwSpiInit();
    }

    ///
    /// Inherit documentation.
    ///
    virtual void prepare(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize=sizeof(uint8_t));

    ///
    /// Inherit documentation.
    ///
    virtual void transact();


protected:
    ///
    /// Inherit documentation.
    ///
    virtual void cleanup(){
        mSpiHw->CR1 |= SPI_CR1_SSI; // Set chip select high.
    }

private:

    void HwSpiInit();

};

} // namespace spi

#endif // SPI_HPP
