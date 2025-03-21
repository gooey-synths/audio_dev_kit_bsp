#include "spi_device_example.hpp"
#include "spi_example.hpp"


SpiDevice::SpiDevice(ISpiBus& spiBussy) : spiBus_(spiBussy) { }

void SpiDevice::sendData(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize) {
    if (txBuff == nullptr || rxBuff == nullptr) {
        std::cerr << "Error: Buffer pointers cannot be null." << std::endl;
        return;
    }

    if (bufLen == 0 || dataSize == 0) {
        std::cerr << "Error: Buffer length and data size must be greater than 0." << std::endl;
        return;
    }

    if (cs > 3) {
        std::cerr << "Error: Invalid chip select value." << std::endl;
        return;
    }

    spiBus_.prepare(txBuff, rxBuff, bufLen, cs, dataSize);
    spiBus_.transact();
    spiBus_.waitForCompletion();
}
