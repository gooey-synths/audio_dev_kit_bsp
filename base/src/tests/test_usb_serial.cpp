#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>

extern "C" {
#include <ctype.h>
}

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
/// The first port will echo back typed characters in lower case.
/// The second port will echo back typed characters in upper case
///
void test_usb_serial_echo() {
    setupPins();
    USBSerial& usbSerial = USBSerial::getInstance();

    board::CommunicationInterface& usbItf0 = usbSerial.getInterface(0);
    board::CommunicationInterface& usbItf1 = usbSerial.getInterface(1);

    uint8_t const caseDiff = 'a' - 'A';

    while(1) {
        char c;
        if(usbItf0.ReadN(&c, 1)) {
            if(isupper(c)) {
                c -= caseDiff;
            }
            usbItf0.WriteN(&c, 1);
        }
        if(usbItf1.ReadN(&c, 1)) {
            if(islower(c)) {
                c += caseDiff;
            }
            usbItf1.WriteN(&c, 1);
        }
    }
}