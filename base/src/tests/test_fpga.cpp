#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>
#include <cstddef>
#include <cctype>
#include <spi/spi.hpp>

static constexpr size_t scFpgaProgUsbItf = 1;
static constexpr size_t scFpgaCsPin = 1;

static const uint8_t HEADER[3] = {0x15,0x15,0x15};

static constexpr size_t scSpiBuffSize = 512;
static char txBuff[scSpiBuffSize];
static char rxBuff[scSpiBuffSize];

///
/// Setup pins.
///
static void setupPins() {
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();

    gpio_controller->setConfig(&usb_d_minus_pin, &usb_d_minus_conf);
    gpio_controller->setConfig(&usb_d_plus_pin, &usb_d_plus_conf);
    gpio_controller->setConfig(&usb_vbus_dect_pin, &usb_vbus_dect_conf);
    gpio_controller->setConfig(&usb_vbus_id_pin, &usb_vbus_id_conf);

    gpio_controller->setConfig(&spi3_clk_pin,  &spi3_clk_conf);
    gpio_controller->setConfig(&spi3_cipo_pin, &spi3_cipo_conf);
    gpio_controller->setConfig(&spi3_copi_pin, &spi3_copi_conf);
    gpio_controller->setConfig(&spi3_cs0_pin,  &spi3_cs0_conf);
    gpio_controller->setConfig(&spi3_cs1_pin,  &spi3_cs1_conf);
    gpio_controller->setConfig(&spi3_cs2_pin,  &spi3_cs2_conf);

    gpio_controller->setConfig(&fpga_cdone_pin,  &fpga_cdone_conf);
    gpio_controller->setConfig(&fpga_hard_reset_pin,  &fpga_hard_reset_conf);
}


void wait_for_header(usb::USBSerial::USBCommunication& comm) {
    size_t counter = 0;

    while(1) {
        char c;
        while(!comm.Available());
        size_t n = comm.ReadN(&c, sizeof c);

        if(n == 0) {
            continue;
        }

        if(c == HEADER[counter]) {
            counter++;
        } else {
            counter = 0;
        }

        if(counter >= sizeof HEADER) {
            break;
        }
    }
}

void test_fpga_prog() {
    // Setup hardware
    setupPins();

    usb::USBSerial::USBCommunication& fpgaProgItf = usb::USBSerial::getInstance().getInterface(scFpgaProgUsbItf);

    gpio::Pin cs_pins[] = {
        gpio::GPIOController::getInstance()->getPin(&spi3_cs0_pin),
        gpio::GPIOController::getInstance()->getPin(&spi3_cs1_pin),
        gpio::GPIOController::getInstance()->getPin(&spi3_cs2_pin),
    };

    gpio::Pin fpgaHardReset = gpio::GPIOController::getInstance()->getPin(&fpga_hard_reset_pin);
    gpio::Pin fpgaCDone = gpio::GPIOController::getInstance()->getPin(&fpga_cdone_pin);
    gpio::Pin& fpgaCS = cs_pins[scFpgaCsPin];

    spi::SwCsSpiBus spiBus(3, cs_pins, sizeof cs_pins / sizeof *cs_pins);

    spi::SpiBusConfig conf;
    conf.mFreq = 30000U;
    conf.mPhase = 0;
    conf.mPolarity = 1;
    conf.mWordSize = 8;
    conf.mIoSwap = true;
    conf.mMidi = 0;
    spiBus.configure(conf);

    // Wait for header
    wait_for_header(fpgaProgItf);

    // Start FPGA in slave mode
    fpgaHardReset = false;
    fpgaCS = false;
    for(volatile int i = 0; i < 0xFFFF; i++); // lazy delay
    fpgaHardReset = true;
    for(volatile int i = 0; i < 0xFFFFFF; i++); // lazy delay
    fpgaCS = true;

    // Send Dummy Clocks
    uint8_t dummyTx = 0x67;
    uint8_t dummyRx = 0x00;
    spiBus.prepare(&dummyTx, &dummyRx, 1, spi::SpiBusBase::NO_CS_SELECTED);
    spiBus.transact();
    spiBus.waitForCompletion();

    fpgaCS = false;

    while(1) {
        while(!fpgaProgItf.Available());

        size_t n = fpgaProgItf.ReadN(txBuff, scSpiBuffSize);

        spiBus.prepare(txBuff, rxBuff, scSpiBuffSize, spi::SpiBusBase::NO_CS_SELECTED);
        spiBus.transact();
        spiBus.waitForCompletion();
    }
}

