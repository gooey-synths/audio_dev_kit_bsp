#include "spi_device_example.hpp"
#include "spi_example.hpp"


SpiDevice::SpiDevice(ISpiBus& spiBus) : spiBus_(spiBus) { }

void SpiDevice::sendData(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize) {
    spiBus_.prepare(txBuff, rxBuff, bufLen, cs, dataSize);
    spiBus_.transact();
    spiBus_.waitForCompletion();
}
