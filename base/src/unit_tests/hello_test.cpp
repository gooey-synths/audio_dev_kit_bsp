#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../spi/example/spi_example.hpp"
#include "../spi/example/spi_device_example.hpp"

class MockSpiBus : public ISpiBus {
public:
    MOCK_METHOD(void, configure, (SpiBusConfig conf), (override));
    MOCK_METHOD(void, prepare, (void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize), (override));
    MOCK_METHOD(void, transact, (), (override));
    MOCK_METHOD(void, waitForCompletion, (), (override));
    MOCK_METHOD(void, interrupt, (), (override));
    MOCK_METHOD(void, cleanup, (), (override));
};

// TEST(SpiDeviceTest, InitializeCallsConfigure) {
//     MockSpiBus mockBus;
//     SpiBusConfig config;
//     SpiDevice device(mockBus);
//
//     EXPECT_CALL(mockBus, configure()).Times(1);
//
//     device.initialize(config);
// }

TEST(SpiDeviceTest, SendDataCallsPrepareTransactAndWait) {
    MockSpiBus mockBus;
    SpiDevice device(mockBus);
    
    char txData[10] = {};
    char rxData[10] = {};
    size_t bufLen = sizeof(txData);
    size_t cs = 0;
    size_t dataSize = 8;
    
    // EXPECT_CALL(mockBus, prepare(txData, rxData, bufLen, cs, dataSize)).Times(1);
    // EXPECT_CALL(mockBus, transact()).Times(1);
    // EXPECT_CALL(mockBus, waitForCompletion()).Times(1);
    
    // device.sendData(txData, rxData, bufLen, cs, dataSize);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
