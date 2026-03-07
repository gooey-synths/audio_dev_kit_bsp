#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>

using namespace usb;

///
/// Setup USB pins.
///
void setupPins() {
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
    USBSerial& usbSerial = USBSerial::GetInstance();

    std::iostream& usbStream0 = usbSerial.getStream(USBSerial::Interface::CENTRAL);
    std::iostream& usbStream1 = usbSerial.getStream(USBSerial::Interface::EXTRA);

    uint8_t const case_diff = 'a' - 'A';

    while(1) {
        char c;
        if(usbStream0 >> c) {
            usbStream0 << c-case_diff;
        }
        if(usbStream1 >> c) {
            usbStream1 << c+case_diff;
        }
    }
}