#ifndef SPI_DEVICE_EXAMPLE_H
#define SPI_DEVICE_EXAMPLE_H

#include <cstdlib>
#include <iostream>

class ISpiBus;

// SpiDevice that depends on ISpiBus
class SpiDevice {
public:
    explicit SpiDevice(ISpiBus& spiBussy);
    void sendData(void* txBuff, void* rxBuff, size_t bufLen, size_t cs, size_t dataSize);

private:
    ISpiBus& spiBus_;
};

#endif
