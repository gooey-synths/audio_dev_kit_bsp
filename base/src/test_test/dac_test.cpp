#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "../spi/spibus_interface.hpp"
#include "../spi/dacx050y.hpp"

using ::testing::_;
using ::testing::internal::CaptureStderr;
using ::testing::internal::GetCapturedStderr;

constexpr uint16_t cDefaultId = 10340;
constexpr uint16_t cDefaultSync = 0xFF00;
constexpr uint16_t cDefaultGain = 0x00FF;
constexpr uint16_t cDefaultDac = 0x8000;

const char* const cSpiBusy = "Spi bus";

class MockSpiBus : public spi::ISpiBus {
public:
    virtual void configure(spi::SpiBusConfig conf) {
        if(mBusy) {
            throw cSpiBusy;
        }
        mConf = conf;
    }

    virtual spi::SpiBusConfig getConfiguration() {
        return mConf;
    }

    virtual void prepare(void *txBuff, void *rxBuff, size_t bufLen, size_t cs, size_t dataSize) {
        if(mBusy) {
            throw cSpiBusy;
        }
        mTxBuff = txBuff;
        mRxBuff = rxBuff;
        mBufLen = bufLen;
        mDataSize = dataSize;
    }

    virtual void transact() {
        if(mBusy) {
            throw cSpiBusy;
        }
    }

    MOCK_METHOD(void, interrupt, (), (override));
    MOCK_METHOD(void, cleanup, (), (override));
    MOCK_METHOD(void, waitForCompletion, (), (override));

    size_t getBufLen() { return mBufLen; }
    size_t getDataSize() { return mDataSize; }
    void* getTxBuff() { return mTxBuff; }
    void* getRxBuff() { return mRxBuff; }

    void setBusy(bool busy) { mBusy = busy; }

protected:
    bool mBusy = false;
    void* mTxBuff;
    void* mRxBuff;
    size_t mBufLen = 0;
    size_t mDataSize = 1;
    spi::SpiBusConfig mConf;
};

#if 0
class DAC60508Sim : public MockSpiBus {
public:

    DAC60508Sim() {
        // Set default register values.
        memset(mRegs, sizeof(mRegs), 0);
        mRegs[spi::eDACx050yRegAddr::DACx050y_ID] = cDefaultId;
        mRegs[spi::eDACx050yRegAddr::DACx050y_SYNC] = cDefaultSync; 
        mRegs[spi::eDACx050yRegAddr::DACx050y_GAIN] = cDefaultGain; 
        for(size_t iDac = 0; iDac < 8; iDac++) {
            mRegs[spi::eDACx050yRegAddr::DACx050y_DAC0+iDac] = cDefaultDac;
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
            return; // Unsupported midi setting.
        }
    }

private:
    spi::DACx050yTransaction sim(spi::DACx050yTransaction transaction) {
        spi::DACx050yTransaction ret = mNextTransaction;

        mNextTransaction = transaction;

        if(transaction.mRW) {
            // read.
            mNextTransaction.setData(mRegs[transaction.getAddr()]);
            mNextTransaction.setAddr(transaction.getAddr());
        } else {
            // write.
            mRegs[transaction.getAddr()] = transaction.getData();
        }

        return ret;
    }

    uint16_t mRegs[16];
    spi::DACx050yTransaction mNextTransaction;
};
#endif


// Test Fixture for DAC60508
class DacTest : public ::testing::Test {
protected:
    MockSpiBus mockBus;
    spi::DAC60508 dac;

    DacTest() : dac(mockBus, 0)  {}

};

TEST_F(DacTest, testDacMode) {
    // Check that the default mode is in reg mode.
    spi::eDACx050yMode mode = dac.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_REG_MODE);

    // Check that we can set and get the mode.
    dac.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);
    mode = dac.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_STREAM_MODE);
    ASSERT_EQ(mockBus.getBufLen(), 8);
    
    dac.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);
    mode = dac.getMode();
    ASSERT_EQ(mode, spi::eDACx050yMode::DACx050y_REG_MODE);
    ASSERT_EQ(mockBus.getBufLen(), 1);
}

TEST_F(DacTest, testDacSetup) {
    // Check that the configuration is set when calling setup
    dac.setup();
    spi::SpiBusConfig conf = mockBus.getConfiguration();
    ASSERT_EQ(conf, spi::DAC60508::scSpiConf);
}

TEST_F(DacTest, testInvalidMode) {
    // Check that we can't perform a register transaction in stream mode.
    dac.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);
    EXPECT_THROW({
        try {
            dac.WriteReg(spi::eDACx050yRegAddr::DACx050y_NOP, 0);
        } catch (const char* e) {
            ASSERT_EQ(e, spi::DAC60508::scInvalidMode);
            throw;
        }
    }, const char*);
}

TEST_F(DacTest, testInvalidIdx) {
    // Check that we can't set an invalid DAC idx
    dac.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);    
    EXPECT_THROW({
        try {
            dac.setStreamVal(255, 0);
        } catch (const char* e) {
            ASSERT_EQ(e, spi::DAC60508::scInvalidIdx);
            throw;
        }
    }, const char*);
}

TEST_F(DacTest, testRegisterWrite) {
    dac.setup();
    dac.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);
    uint16_t testGain = 0x1F;

    EXPECT_CALL(mockBus, waitForCompletion());
    dac.WriteReg(spi::eDACx050yRegAddr::DACx050y_GAIN, testGain);

    spi::DACx050yTransaction* txBuff = reinterpret_cast<spi::DACx050yTransaction*>(mockBus.getTxBuff());

    ASSERT_EQ(txBuff->getAddr(), spi::eDACx050yRegAddr::DACx050y_GAIN);
    ASSERT_EQ(txBuff->getData(), testGain);
}

TEST_F(DacTest, testRegisterRead) {
    dac.setup();
    dac.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);

    EXPECT_CALL(mockBus, waitForCompletion()).Times(2);
    dac.ReadReg(spi::eDACx050yRegAddr::DACx050y_ID);

    spi::DACx050yTransaction* rxBuff = reinterpret_cast<spi::DACx050yTransaction*>(mockBus.getRxBuff());

    ASSERT_EQ(rxBuff->getAddr(), spi::eDACx050yRegAddr::DACx050y_ID);
    ASSERT_EQ(rxBuff->getData(), 10340);
}

TEST_F(DacTest, testStream) {
    dac.setup();
    dac.setMode(spi::eDACx050yMode::DACx050y_STREAM_MODE);

    for(size_t idx = 0; idx < 8; idx++) {
        dac.setStreamVal(idx, idx);
    }

    dac.updateStream();

    dac.setMode(spi::eDACx050yMode::DACx050y_REG_MODE);

    for(size_t idx = 0; idx < 8; idx++) {
        EXPECT_CALL(mockBus, waitForCompletion()).Times(2);

        uint16_t dacData = dac.ReadReg(static_cast<spi::eDACx050yRegAddr>(spi::eDACx050yRegAddr::DACx050y_DAC0 + idx));
        ASSERT_EQ(dacData, idx);
    }
}

TEST_F(DacTest, testBusy) {
    mockBus.setBusy(true);

    EXPECT_THROW({
        try {
            dac.setup();
        } catch (const char* e) {
            ASSERT_EQ(e, cSpiBusy);
            throw;
        }
    }, const char*);

    EXPECT_THROW({
        try {
            dac.WriteReg(spi::eDACx050yRegAddr::DACx050y_NOP, 0);
        } catch (const char* e) {
            ASSERT_EQ(e, cSpiBusy);
            throw;
        }
    }, const char*);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}