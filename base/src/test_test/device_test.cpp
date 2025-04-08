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
    virtual void configure(SpiBusConfig conf) {
        mConf = conf;
    }
    virtual SpiBusConfig getConfiguration() {
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
                (spi::DACx050yTransaction*)mRxBuff[iTransaction] = sim((spi::DACx050yTransaction*)mRxBuff[iTransaction]);
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
                tx |= mTxBuff[iBuf] & mask;
            }
            
            uint32_t rx = sim((spi::DACx050yTransaction)tx);

            for(size_t iBuf = 0; iBuf < mBufLen; iBuf++) {
                mRxBuff[iBuff] = rx & mask;
                rx >> mConf.mWordSize;
            }

        }
    }

private:
    spi::DACx050yTransaction sim(const spi::DACx050yTransaction transaction) {
        spi::DACx050yTransaction ret = mNextTransaction;

        mNextTransaction = transaction;

        if(transaction.mRW) {
            // read.
            mNextTransaction.setData(mRegs[transaction.getAddr()]);
        } else {
            // write.
            mRegs[ret.getAddr()] = transaction.getData();
        }

        return ret;
    }

    uint16_t mRegs[16];
    uint16_t mNextTransaction;
}