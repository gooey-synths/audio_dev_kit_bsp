#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "../spi/spibus_interface.hpp"
#include "../spi/dacx050y.hpp"

using ::testing::_;
using ::testing::internal::CaptureStderr;
using ::testing::internal::GetCapturedStderr;

class MockSpiBus : public spi::ISpiBus {
public:
    virtual void configure(spi::SpiBusConfig conf) {
        mConf = conf;
    }
    virtual spi::SpiBusConfig getConfiguration() {
        return mConf;
    }
    virtual void prepare(void *txBuff, void *rxBuff, size_t bufLen, size_t cs, size_t dataSize) {
        mTxBuff = txBuff;
        mRxBuff = rxBuff;
        mBufLen = bufLen;
        mDataSize = dataSize;
    }


    MOCK_METHOD(void, interrupt, (), (override));
    MOCK_METHOD(void, cleanup, (), (override));
    MOCK_METHOD(void, waitForCompletion, (), (override));

protected:
    void* mTxBuff;
    void* mRxBuff;
    size_t mBufLen;
    size_t mDataSize;
    spi::SpiBusConfig mConf;
};


class DAC60508Sim : public MockSpiBus {
public:

    DAC60508Sim() {
        // Set default register values.
        memset(mRegs, sizeof(mRegs), 0);
        mRegs[spi::eDACx050yRegAddr::DACx050y_ID] = 10340;
        mRegs[spi::eDACx050yRegAddr::DACx050y_SYNC] = 0xFF00; 
        mRegs[spi::eDACx050yRegAddr::DACx050y_GAIN] = 0x00FF; 
        for(size_t iDac = 0; iDac < 8; iDac++) {
            mRegs[spi::eDACx050yRegAddr::DACx050y_DAC0+iDac] = 0x8000;
        }
    }

    virtual void transact() {
        if(mConf.mMidi > 0) {
            if(mDataSize != 4) {
                return; // Unsupported transaction size.
            }
            if(mConf.mWordSize != 24) {
                return; // Unsupported word size.
            }
            for(size_t iTransaction = 0; iTransaction < mBufLen; iTransaction++) {
                reinterpret_cast<spi::DACx050yTransaction*>(mRxBuff)[iTransaction] =
                    sim(reinterpret_cast<spi::DACx050yTransaction*>(mTxBuff)[iTransaction]);
            }
        } else {
            if(mDataSize != 1) {
                return; // Unsupported data size.
            }
            if(mConf.mWordSize != 8) {
                return; // Unsupported word size.
            }
            if(mBufLen != 3){
                return; // Unsupported buffer length.
            }

            uint32_t tx;
            const size_t mask = (1 << mConf.mWordSize) - 1; 

            for(size_t iBuf = 0; iBuf < mBufLen; iBuf++) {
                tx << mConf.mWordSize;
                tx |= reinterpret_cast<uint32_t*>(mTxBuff)[iBuf] & mask;
            }
            
            spi::DACx050yTransaction rx = (*reinterpret_cast<spi::DACx050yTransaction*>(&tx));

            for(size_t iBuf = 0; iBuf < mBufLen; iBuf++) {
                reinterpret_cast<uint32_t*>(mRxBuff)[iBuf] = *reinterpret_cast<uint32_t*>(&rx) & mask;
                *reinterpret_cast<uint32_t*>(&rx) >> mConf.mWordSize;
            }

        }
    }

private:
    spi::DACx050yTransaction sim(spi::DACx050yTransaction transaction) {
        spi::DACx050yTransaction ret = mNextTransaction;

        mNextTransaction = transaction;

        if(transaction.mRW) {
            // read.
            ret.setData(mRegs[transaction.getAddr()]);
        } else {
            // write.
            mRegs[ret.getAddr()] = transaction.getData();
        }

        return ret;
    }

    uint16_t mRegs[16];
    spi::DACx050yTransaction mNextTransaction;
};


// Test Fixture for SpiDevice
class SpiDeviceTest : public ::testing::Test {
protected:
    DAC60508Sim mockDevice;
    spi::DAC60508 interface;

    SpiDeviceTest() : interface(mockDevice, 0)  {}

};

TEST_F(SpiDeviceTest, checkMode) {
    // Capture the stdout output
    CaptureStderr();

    // Check that the default mode is in reg mode.
    spi::eDACx050yMode mode = interface.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_REG_MODE);

    // Check that we can set and get the mode.
    interface.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);
    mode = interface.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_STREAM_MODE);
    
    interface.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);
    mode = interface.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_REG_MODE);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}