#ifndef SPI_EXAMPLE_H
#define SPI_EXAMPLE_H

#include <cstdint>
#include <stdint.h>
#include <cstdlib>

// Struct for SPI bus configuration
struct SpiBusConfig {
    bool mPolarity;     ///< Clock polarity
    bool mPhase;        ///< Clock phase
    bool mIoSwap;       ///< True if IO lines should be swapped
    uint32_t mFreq;     ///< Clock frequency
    uint8_t mWordSize;  ///< Data word size in bits
    uint8_t mMidi;      ///< Master inter-data idleness (HW SPI only)
};

// SPI bus interface
class ISpiBus {
public:
    virtual ~ISpiBus() = default;
    virtual void configure(SpiBusConfig conf) = 0;
    virtual void prepare(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize) = 0;
    virtual void transact() = 0;
    virtual void waitForCompletion() = 0;

protected:
    virtual void interrupt() = 0;
    virtual void cleanup() = 0;

};

// Concrete implementation of ISpiBus
class SpiBusImpl : public ISpiBus {
public:
    void configure(SpiBusConfig conf) override;
    void prepare(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize) override;
    void transact() override;
    void waitForCompletion() override;

protected:
    void interrupt() override;
    void cleanup() override;
};

#endif // SPI_EXAMPLE_H
