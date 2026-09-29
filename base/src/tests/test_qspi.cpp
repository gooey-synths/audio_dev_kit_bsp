#include <qspi/qspi.hpp>
#include <gpio/gpio.hpp>
#include <system/board_defs.h>
#include <usb/usb_serial.hpp>

#include <stdio.h>
#include <inttypes.h>

#include <FreeRTOS.h>
#include <task.h>

///
/// Class for interfacing with a W25Q128JV NOR flash memory.
///
class W25Q128JV {
public:
    W25Q128JV(qspi::QSpi& q)
        : mQ(q)
    {
        mDevConf.devSel = qspi::eDeviceSelection::DEV_1;
        mDevConf.freq = 2500000;
        mDevConf.nAddrBits = 6;

        mHeader.address = 0;
        mHeader.alternate = 0;

        mQ.setDeviceConfiguration(mDevConf);
    }

    void waitForCompletion() {
        while(mQ.isBusy()) {
            vTaskDelay(10);
        }
    }

    uint64_t readUniqueId() {
        uint64_t id;

        mCommConf.addressMode = qspi::eNumLines::DISABLED;
        mCommConf.addressSize = qspi::eCommandSize::EIGHT;
        mCommConf.alternateMode = qspi::eNumLines::ONE;
        mCommConf.dataMode = qspi::eNumLines::ONE;
        mCommConf.dummyCycles = 24;
        mCommConf.instruction = 0x4B;
        mCommConf.instructionMode = qspi::eNumLines::ONE;
        mQ.setCommunicationConfiguration(mCommConf);

        // Use alternate byte to make up the last dummy byte
        mHeader.alternate = 0;
        mQ.setHeader(mHeader);

        mQ.startIndirectRead((uint8_t*)&id, sizeof(id));

        waitForCompletion();
        return id;
    }

    uint32_t readJEDEC() {
        uint32_t id;

        mCommConf.addressMode = qspi::eNumLines::DISABLED;
        mCommConf.addressSize = qspi::eCommandSize::EIGHT;
        mCommConf.alternateMode = qspi::eNumLines::DISABLED;
        mCommConf.dataMode = qspi::eNumLines::ONE;
        mCommConf.dummyCycles = 0;
        mCommConf.instruction = 0x9F;
        mCommConf.instructionMode = qspi::eNumLines::ONE;
        mQ.setCommunicationConfiguration(mCommConf);

        mQ.startIndirectRead((uint8_t*)&id, 3);

        waitForCompletion();
        return id;
    }

    void writeStatusReg(uint8_t inst, uint8_t data) {
        mCommConf.addressMode = qspi::eNumLines::DISABLED;
        mCommConf.addressSize = qspi::eCommandSize::EIGHT;
        mCommConf.alternateMode = qspi::eNumLines::DISABLED;
        mCommConf.dataMode = qspi::eNumLines::ONE;
        mCommConf.dummyCycles = 0;
        mCommConf.instruction = inst;
        mCommConf.instructionMode = qspi::eNumLines::ONE;
        mQ.setCommunicationConfiguration(mCommConf);

        mQ.startIndirectWrite(&data, 1);

        waitForCompletion();
    }

    void erase() {
        mCommConf.addressMode = qspi::eNumLines::DISABLED;
        mCommConf.addressSize = qspi::eCommandSize::EIGHT;
        mCommConf.alternateMode = qspi::eNumLines::DISABLED;
        mCommConf.dataMode = qspi::eNumLines::DISABLED;
        mCommConf.dummyCycles = 0;
        mCommConf.instruction = 0xC7;
        mCommConf.instructionMode = qspi::eNumLines::ONE;
        mQ.setCommunicationConfiguration(mCommConf);

        // We do not need to start an indirect write if there is no data.
        // mQ.startIndirectWrite(nullptr, 0);

        waitForCompletion();

        // Begin status polling for erase finish
        qspi::StatusPollingConfigurtion conf;
        conf.dataLength = 1;
        conf.interval = 1000;
        conf.mask = 0x80;
        conf.match = 0x80;
        conf.orMode = false;

        beginStatusPolling(0x00, conf);

        while(!mQ.statusPollingMatch()) {
            vTaskDelay(10);
        }
    }

private:


    void beginStatusPolling(uint8_t inst, qspi::StatusPollingConfigurtion& conf) {
        mCommConf.addressMode = qspi::eNumLines::DISABLED;
        mCommConf.addressSize = qspi::eCommandSize::EIGHT;
        mCommConf.alternateMode = qspi::eNumLines::DISABLED;
        mCommConf.dataMode = qspi::eNumLines::ONE;
        mCommConf.dummyCycles = 0;
        mCommConf.instruction = inst;
        mCommConf.instructionMode = qspi::eNumLines::ONE;
        mQ.setCommunicationConfiguration(mCommConf);

        mQ.startStatusPolling(conf);
    }

    qspi::QSpi& mQ;
    qspi::CommunicationConfiguration mCommConf;
    qspi::DeviceConfiguration mDevConf;
    qspi::Header mHeader;
};

///
/// Setup the pins for testing
///
void setup_pins(){

    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();

    // Configure QSPI pins
    gpio_controller->setConfig(&qspi0_pin, &qspi0_pin_conf);
    gpio_controller->setConfig(&qspi1_pin, &qspi1_pin_conf);
    gpio_controller->setConfig(&qspi2_pin, &qspi2_pin_conf);
    gpio_controller->setConfig(&qspi3_pin, &qspi3_pin_conf);
    gpio_controller->setConfig(&qspi_clk_pin, &qspi_clk_pin_conf);
    gpio_controller->setConfig(&qspi_cs_pin, &qspi_cs_pin_conf);

    gpio_controller->setConfig(&usb_d_minus_pin, &usb_d_minus_conf);
    gpio_controller->setConfig(&usb_d_plus_pin, &usb_d_plus_conf);
    gpio_controller->setConfig(&usb_vbus_dect_pin, &usb_vbus_dect_conf);
    gpio_controller->setConfig(&usb_vbus_id_pin, &usb_vbus_id_conf);
}

void test_qspi_W25Q128JV() {
    setup_pins();

    qspi::QSpi q;
    W25Q128JV flash(q);

    usb::USBSerial::USBCommunication& comm = usb::USBSerial::getInstance().getInterface(0);

    char buffer[32];

    while(1) {
        // Print unique ID
        uint64_t uid = flash.readUniqueId();
        sprintf(buffer, "UID: 0x%llx", uid);
        comm.WriteN(buffer, strlen(buffer));
        comm.Flush();

        // Print JEDEC
        uint32_t jid = flash.readJEDEC();
        sprintf(buffer, "JID: 0x%xll", jid);
        comm.WriteN(buffer, strlen(buffer));
        comm.Flush();

        // Enable Quad
        flash.writeStatusReg(0x31, 1<<1);


        // Chip erase
        flash.erase();

        vTaskDelay(100);
    }
}
