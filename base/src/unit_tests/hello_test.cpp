#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../spi/example/spi_example.hpp"
#include "../spi/example/spi_device_example.hpp"

using ::testing::_;
using ::testing::internal::CaptureStderr;
using ::testing::internal::GetCapturedStderr;

class MockSpiBus : public ISpiBus {
public:
    MOCK_METHOD(void, configure, (SpiBusConfig conf), (override));
    MOCK_METHOD(void, prepare, (void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize), (override));
    MOCK_METHOD(void, transact, (), (override));
    MOCK_METHOD(void, waitForCompletion, (), (override));
    MOCK_METHOD(void, interrupt, (), (override));
    MOCK_METHOD(void, cleanup, (), (override));
};


// Test Fixture for SpiDevice
class SpiDeviceTest : public ::testing::Test {
protected:
    MockSpiBus mockBus;
    SpiDevice device;

    SpiDeviceTest() : device(mockBus) {}

    // Helper function to simulate a valid SpiBusConfig
    SpiBusConfig createValidConfig() {
        SpiBusConfig config;
        config.mPolarity = false;  // 2 MHz
        return config;
    }
};

TEST_F(SpiDeviceTest, SendDataCallsPrepareTransactAndWait) {
    
    char txData[10] = {};
    char rxData[10] = {};
    size_t bufLen = sizeof(txData);
    size_t cs = 0;
    size_t dataSize = 8;
    
    EXPECT_CALL(mockBus, prepare(txData, rxData, bufLen, cs, dataSize)).Times(1);
    EXPECT_CALL(mockBus, transact()).Times(1);
    EXPECT_CALL(mockBus, waitForCompletion()).Times(1);

    device.sendData(txData, rxData, bufLen, cs, dataSize);
}

TEST_F(SpiDeviceTest, SendData_InvalidBuffers) {
    // Capture the stdout output
    CaptureStderr();

    // Verify that no SPI functions were called
    EXPECT_CALL(mockBus, prepare(_, _, _, _, _)).Times(0);

    device.sendData(nullptr, nullptr, 128, 0, 8);

    std::string output = GetCapturedStderr();
    ASSERT_EQ(output, "Error: Buffer pointers cannot be null.\n");

}

TEST_F(SpiDeviceTest, SendData_InvalidBufferLength) {
    // Capture the stdout output
    CaptureStderr();
    char txData[128] = {0};
    char rxData[128] = {0};

    EXPECT_CALL(mockBus, prepare(_, _, _, _, _)).Times(0);

    // Try sending data with bufLen = 0
    device.sendData(txData, rxData, 0, 0, 8);

    // Assert that the correct error message is printed
    std::string output = GetCapturedStderr();
    ASSERT_EQ(output, "Error: Buffer length and data size must be greater than 0.\n");
}

TEST_F(SpiDeviceTest, SendData_InvalidChipSelect) {
    char txData[128] = {0};
    char rxData[128] = {0};
    size_t invalidCs = 4;  // Chip select should be between 0 and 3
    // Capture the stdout output
    CaptureStderr();
    
    // Verify that no SPI functions were called
    EXPECT_CALL(mockBus, prepare(_, _, _, _, _)).Times(0);

    device.sendData(txData, rxData, 128, invalidCs, 8);


    // Assert that the correct error message is printed
    std::string output = GetCapturedStderr();
    ASSERT_EQ(output, "Error: Invalid chip select value.\n");

};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
