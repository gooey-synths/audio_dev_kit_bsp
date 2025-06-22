#ifndef DACx050y_HPP
#define DACx050y_HPP

#include "spibus_interface.hpp"

namespace spi {

///
/// Enumeration for DACx050y register addresses.
///
enum eDACx050yRegAddr {
    DACx050y_NOP = 0,
    DACx050y_ID,
    DACx050y_SYNC,
    DACx050y_CONF,
    DACx050y_GAIN,
    DACx050y_TRIG,
    DACx050y_CAST,
    DACx050y_STAT,
    DACx050y_DAC0,
    DACx050y_DAC1,
    DACx050y_DAC2,
    DACx050y_DAC3,
    DACx050y_DAC4,
    DACx050y_DAC5,
    DACx050y_DAC6,
    DACx050y_DAC7,
};

///
/// Structure defining a single transaction to a DACx050y
///
struct DACx050yTransaction {
    uint32_t mData : 16;    ///< Register data.

    uint32_t mAddr : 4;     ///< Register adress.
    uint32_t mReserved : 3; ///< Reserved.
    uint32_t mRW : 1;       ///< Read/write bit.

    uint32_t mPadding : 8; ///< Padding to 32 bits.

    ///
    /// @brief Constructor.
    /// @param readWrite Read/write state
    /// @param addr Address portion
    /// @param data Data portion
    ///
    DACx050yTransaction(bool readWrite, uint8_t addr, uint16_t data)
        : mRW(readWrite), mReserved(0), mAddr(addr), mPadding(0), mData(data) {
        ; // Do nothing.
    }

    DACx050yTransaction() : DACx050yTransaction(false, 0, 0) {
        ; // Do nothing.
    }

    uint8_t getAddr() { return mAddr; }

    void setAddr(uint8_t addr) { mAddr = addr; }

    uint16_t getData() { return mData; }

    void setData(uint16_t data) {
        mData = data;
    }
};

enum eDACx050yMode { DACx050y_REG_MODE = 0, DACx050y_STREAM_MODE };

///
/// Interface for DACx050y
/// @tparam tX first digit in part number, means to resolution / 2
/// @tparam tY last digit in part, means number of channels
///
template <size_t tX, size_t tY> class IDACx050y {
  public:
    virtual void setup() = 0;

    virtual void setMode(eDACx050yMode mode) = 0;
    virtual eDACx050yMode getMode() = 0;

    virtual uint16_t ReadReg(eDACx050yRegAddr addr) = 0;
    virtual void WriteReg(eDACx050yRegAddr addr, uint16_t value) = 0;

    virtual void setStreamVal(uint8_t dacIdx, uint16_t val) = 0;
    virtual void updateStream() = 0;
};

///
/// Class for interacting with a DACx050y
/// @tparam tX first digit in part number, means to resolution / 2
/// @tparam tY last digit in part, means number of channels
///
template <size_t tX, size_t tY> class DACx050y : public IDACx050y<tX, tY> {
  public:
    /// SPI bus configuration for the DACX050Y
    static constexpr SpiBusConfig scSpiConf = {
        .mPolarity = true,
        .mPhase = false,
        .mIoSwap = true,
        .mFreq = 300000000U,
        .mWordSize = 24,
        .mMidi = 0x7
    };

    static constexpr const char* const scInvalidIdx = "Invalid DAC index";
    static constexpr const char* const scInvalidMode = "Invalid DAC mode";

    ///
    /// Constructor.
    /// @param spiBus SpiBus that the DAC is connected to.
    /// @param cs Chip select the the DAC is connected to.
    ///
    DACx050y(ISpiBus &spiBus, size_t cs) : mSpiBus(spiBus), mCs(cs), mMode(DACx050y_REG_MODE) {
        setMode(mMode);
    }

    ///
    /// Set the current mode of the DACx050y
    /// @param mode Mode for the DAC to operate in
    /// @note Calling functions without setting the proper mode may result in undefined behavoir!
    ///
    virtual void setMode(eDACx050yMode mode) {
        size_t bufLen = 0;
        switch (mode) {
        case eDACx050yMode::DACx050y_REG_MODE:
            bufLen = 1;
            break;

        case eDACx050yMode::DACx050y_STREAM_MODE:
            bufLen = tY;
            for (size_t iDac = 0; iDac < tY; iDac++) {
                mTxBuf[iDac].mAddr = DACx050y_DAC0 + iDac;
            }
            break;

        default:
            throw scInvalidMode;
        };

        mSpiBus.prepare(reinterpret_cast<void*>(&mTxBuf[0]), reinterpret_cast<void*>(&mRxBuf[0]), bufLen, mCs, sizeof(mTxBuf[0]));
        mMode = mode;
    }

    ///
    /// Setup the SPI bus for transaction.
    /// It is the users responsability to call this correctly
    /// when in a multi SPI device environment.
    ///
    virtual void setup() { mSpiBus.configure(scSpiConf); }

    ///
    /// Get the current mode of the DACx050y
    /// @return The current mode of the DACx050y
    ///
    virtual eDACx050yMode getMode() { return mMode; }

    ///
    /// Read a register of the DACx050y.
    /// @param addr Address of the register to read.
    /// @return The value of that register.
    ///
    virtual uint16_t ReadReg(eDACx050yRegAddr addr) {
        // Send initial read command.
        regTransaction(addr, 0x0000, true);

        // Send NOP command to get data back.
        regTransaction(eDACx050yRegAddr::DACx050y_NOP, 0x0420, false);

        return mRxBuf[0].getData();
    }

    ///
    /// Write a register of the DACx050y.
    /// @param addr Address of the register to write.
    /// @param value Value of register to write.
    ///
    virtual void WriteReg(eDACx050yRegAddr addr, uint16_t value) { regTransaction(addr, value, false); }

    ///
    /// Set DAC value.
    /// @param dacIdx Dac index to set value of.
    /// @param val Value to set the DAC to.
    ///
    virtual void setStreamVal(uint8_t dacIdx, uint16_t val) {
        if (dacIdx > tY) {
            throw scInvalidIdx;
        }
        mTxBuf[dacIdx].setData(val);
    }

    ///
    /// Update DAC values.
    /// @note This does NOT block until transaction is complete.
    ///
    virtual void updateStream() { mSpiBus.transact(); }

  private:
    ///
    /// Perform a register mode transaction.
    /// @param addr Address
    /// @param value Data
    /// @param read True if read bit should be set.
    ///
    inline void regTransaction(eDACx050yRegAddr addr, uint16_t value, bool read) {
        if (mMode != eDACx050yMode::DACx050y_REG_MODE) {
            throw scInvalidMode;
        }

        mTxBuf[0].mRW = read;
        mTxBuf[0].setAddr(addr);
        mTxBuf[0].setData(value);

        mSpiBus.transact();
        mSpiBus.waitForCompletion();
    }

    ISpiBus &mSpiBus;               ///< Reference to the SPI bus that the DAC60508 is connected to.
    size_t mCs;                     ///< Chip select the DAC60508 uses.
    eDACx050yMode mMode;            ///< Current operating mode.
    DACx050yTransaction mTxBuf[tY]; ///< Buffer to use for SPI transmit
    DACx050yTransaction mRxBuf[tY]; ///< Buffer to use for SPI receive
};

///
/// Class for interacting with a DAC60508
///
class DAC60508 : public DACx050y<6, 8> {
  public:
    DAC60508(ISpiBus &spiBus, size_t cs) : DACx050y(spiBus, cs) { ; /* Do nothing */ }
};

} // namespace spi

#endif // DACx050y_HPP