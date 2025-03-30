#ifndef DACx050y_HPP
#define DACx050y_HPP

#include "spibus_interface.hpp"

namespace spi{

///
/// Enumeration for DACx050y registers.
///
enum eDACx050yRegAddr
{
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
/// Structure defining a single transaction to a DACx050ys
///
struct DACx050yTransaction
{    
    uint8_t mDataLsb; ///< Data LSB.

    uint8_t mDataMsb; ///< Data MSB.
  
    uint8_t mAddr: 4; ///< Register adress.
    uint8_t mReserved: 3; ///< Reserved.
    uint8_t mRW: 1; ///< Read/write bit.

    uint8_t mPadding; ///< Padding to 32 bits.


    ///
    /// @brief Constructor.
    /// @param readWrite Read/write state
    /// @param addr Address portion
    /// @param data Data portion
    ///
    DACx050yTransaction(bool readWrite, uint8_t addr, uint16_t data):
        mRW(readWrite),
        mReserved(0),
        mAddr(addr),
        mPadding(0),
        mDataMsb(data >> 8),
        mDataLsb(data & 0xFF)
    {
        ; // Do nothing.
    }

    DACx050yTransaction():
        DACx050yTransaction(false, 0, 0)
    {
        ; // Do nothing.
    }

    uint16_t getData(){
        return (uint16_t) mDataMsb << 8 | mDataLsb;
    }

    void setData(uint16_t data){
        mDataMsb = data >> 8;
        mDataLsb = data & 0xFF;
    }
};

enum eDACx050yMode{
    DACx050y_REG_MODE = 0,
    DACx050y_STREAM_MODE
};

///
/// Interface for DACx050y
/// @tparam tX first digit in part number, means to resolution / 2 
/// @tparam tY last digit in part, means number of channels 
///
template<size_t tX, size_t tY>
class IDACx050y {
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
template<size_t tX, size_t tY>
class DACx050y : IDACx050y<tX, tY> {
public:
    DACx050y(ISpiBus& spiBus);

    /// Set the current mode of the DACx050y
    /// @param mode 
    virtual void setMode(eDACx050yMode mode) {
        mMode = mode;
    }

    ///
    /// Get the current mode of the DACx050y
    /// @return The current mode of the DACx050y
    ///
    virtual eDACx050yMode getMode() {
        return mMode;
    }

private:
    ///
    static constexpr SpiBusConfig scSpiConf = {
        .mFreq = 6000000U,
        .mPhase = 0,
        .mPolarity = 1,
        .mWordSize = 24,
        .mMidi = 0xF,
        .mIoSwap = false,
    };

    ISpiBus& mSpiBus; ///<
    eDACx050yMode mMode; ///<
    DACx050yTransaction mTransactioBuf[tY]; ///<

};

///
/// Class for interacting with a DAC60508
///
class DAC60508 : DACx050y<6,8> {
public:
    DAC60508(ISpiBus& spiBus)
    : DACx050y(spiBus)
    { ; /* Do nothing */ }
};

} // namespace spi

#endif // DACx050y_HPP