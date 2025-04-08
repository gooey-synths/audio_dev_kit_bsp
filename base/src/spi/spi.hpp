#ifndef SPI_HPP
#define SPI_HPP

#include "../dma/dma.hpp"
#include "../gpio/gpio.hpp"
#include "../system/board_defs.h"
#include "../system/stm32h750xx.h"
#include "spibus_interface.hpp"

#include <cassert>
#include <stdint.h>

namespace spi {

#define NUM_SPI_CONTROLLERS 6 ///< Number of spi controllers on the stm32h750.

class SpiBusBase; ///< forward reference.

extern SpiBusBase *sInstances[NUM_SPI_CONTROLLERS]; ///< instances of the spi buses

///
/// Base class for SPI buses.
/// @todo CRC?
///
class SpiBusBase : ISpiBus {

  public:
    SpiBusBase(size_t);

    ///
    /// Destructor.
    ///
    virtual ~SpiBusBase() { sInstances[mSpiNum] = NULL; }

    void configure(SpiBusConfig conf) override;

    SpiBusConfig getConfiguration() override;

    ///
    /// Wait for the SPI bus to complete.
    ///
    void waitForCompletion() override {
        while (mIsActive) {
            ; // Do nothing
        }
    }

  protected:
    ///
    /// SPI interrupt.
    ///
    void interrupt() {

        // transfer complete flag
        if (mSpiHw->SR & SPI_SR_EOT) {
            cleanup();

            NVIC_DisableIRQ(mSpiIrqN);

            mIsActive = false;

            mSpiHw->IFCR |= 0x1FF << 3; // clear interrupt flags
        }
    }

    static void spi1Isr() {
        if (sInstances[0] != NULL) {
            sInstances[0]->interrupt();
        }
    }
    static void spi2Isr() {
        if (sInstances[1] != NULL) {
            sInstances[1]->interrupt();
        }
    }
    static void spi3Isr() {
        if (sInstances[2] != NULL) {
            sInstances[2]->interrupt();
        }
    }
    static void spi4Isr() {
        if (sInstances[3] != NULL) {
            sInstances[3]->interrupt();
        }
    }
    static void spi5Isr() {
        if (sInstances[4] != NULL) {
            sInstances[4]->interrupt();
        }
    }
    static void spi6Isr() {
        if (sInstances[5] != NULL) {
            sInstances[5]->interrupt();
        }
    }

    uint8_t mSpiNum;                        ///< Number of the SPI bus.
    volatile bool mIsActive;                ///< True if the SPI is currently active.
    IRQn_Type mSpiIrqN;                     ///< Interrupt number for the SPI controller.
    SPI_TypeDef *mSpiHw;                    ///< SPI peripheral pointer.
    SpiBusConfig mConfig;                   ///< SPI bus configuration.
    uint32_t mKerClkFreq;                   ///< Frequency of the SPI_KER_CLK.
    dma::DmaController::DmaChannel *mTxDma; ///< TX DMA channel
    dma::DmaController::DmaChannel *mRxDma; ///< RX DMA channel

  private:
    ///
    /// Throw an exception if multiple instances are decalared of the same SPI bus.
    /// @param instance_num Spi bus number
    ///
    static void errorIfTaken(size_t instanceNum) {
        instanceNum -= 1; // 1 index to 0 index conversion
        if (instanceNum > NUM_SPI_CONTROLLERS || sInstances[instanceNum]) {
            throw "Invalid SPI instance";
        }
    }
};

extern SpiBusBase *sInstances[NUM_SPI_CONTROLLERS]; ///< instances of the spi buses

///
/// Hw controlled chip select spi bus
/// @note Can only control 1 chip select line
///
class HwCsSpiBus : public SpiBusBase {

  public:
    ///
    /// Constructor.
    /// @param instance_num SPI bus number.
    ///
    HwCsSpiBus(size_t instance_num) : SpiBusBase(instance_num) { HwCsSpiBusInit(); }

    ///
    /// Inherit documentation.
    ///
    void prepare(void *txBuff, void *rxBuff, size_t bufLen, size_t cs, size_t dataSize = sizeof(uint8_t)) override;

    ///
    /// Inherit documentation.
    ///
    void transact() override;

  protected:
    ///
    /// Inherit documentation.
    ///
    virtual void cleanup() override {
        mSpiHw->CR1 |= SPI_CR1_SSI; // Set chip select high.
    }

  private:
    void HwCsSpiBusInit();
};

///
/// Sw controlled chip select spi bus
/// @todo fill this in.
///
class SwCsSpiBus : SpiBusBase {};

} // namespace spi

#endif // SPI_HPP