#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>
#include <cstddef>
#include <cctype>

using namespace usb;

///
/// Setup USB pins.
///
static void setupPins() {
    gpio::GPIOController* gpio_controller = gpio::GPIOController::getInstance();

    gpio_controller->setConfig(&usb_d_minus_pin, &usb_d_minus_conf);
    gpio_controller->setConfig(&usb_d_plus_pin, &usb_d_plus_conf);
    gpio_controller->setConfig(&usb_vbus_dect_pin, &usb_vbus_dect_conf);
    gpio_controller->setConfig(&usb_vbus_id_pin, &usb_vbus_id_conf);
}

///
/// Test USB serial connection.
/// @note To check for success observe 2 new USB COM ports on the system.
/// The first port will echo back typed characters in inverted case.
/// The second port will echo back typed characters without inverting the case.
///
void test_usb_serial_echo() {
    setupPins();
    USBSerial& usbSerial = USBSerial::getInstance();

    board::CommunicationInterface& usbItf0 = usbSerial.getInterface(0);
    board::CommunicationInterface& usbItf1 = usbSerial.getInterface(1);

    uint8_t const caseDiff = 'a' - 'A';

    char buf[128];
    size_t numRead = 0;

    while(1) {
        for(size_t itf = 0; itf < 2; itf++) {
            numRead = usbSerial.getInterface(itf).ReadN(buf, sizeof buf / sizeof *buf);
            if(numRead) {
                // Only invert case on interface 0
                if(itf == 0) {
                    for(char& c : buf) {
                        if(islower(c)) {
                            c -= caseDiff;
                        } else if(isupper(c)) {
                            c += caseDiff;
                        }
                    }
                }
                usbSerial.getInterface(itf).WriteN(buf, numRead);
            }
        }
    }
}