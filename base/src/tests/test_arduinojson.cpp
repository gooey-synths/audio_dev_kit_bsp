#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>
#include <cstddef>
#include <cctype>
#include <ArduinoJson.h>

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

static bool readUntilEOF(char* buf, size_t bufLen, usb::USBSerial::USBCommunication& itf) {

    size_t bufIdx;
    char c;

    while(1) {
        size_t numRead = itf.ReadN(&c, 1);

        if(numRead) {
            if(c == EOF) {
                return true;
            } else if(bufIdx >= bufLen) {
                return false;
            } else {
                buf[bufIdx] = c;
                bufIdx++;
            }
        }

    }
}

static void printStats(ArduinoJson::JsonDocument& in_doc, usb::USBSerial::USBCommunication& itf) {
    ArduinoJson::JsonDocument stats_doc;

    char buf[256];
    memset(buf, 0, sizeof(buf));

    stats_doc["depth"] = in_doc.nesting();

    ArduinoJson::serializeJson(stats_doc, buf);

    itf.WriteN(buf, sizeof(buf));

    itf.Flush();
}


void test_ArduinoJSON() {
    setupPins();
    usb::USBSerial& usbSerial = usb::USBSerial::getInstance();
    usb::USBSerial::USBCommunication& jsonInput = usbSerial.getInterface(0);
    usb::USBSerial::USBCommunication& statsOutput = usbSerial.getInterface(1);
    
    char buf[1024];

    ArduinoJson::JsonDocument doc;
    
    while(1) {
        memset(buf, 0, sizeof(buf));
        if(readUntilEOF(buf, sizeof(buf), jsonInput)) {
            ArduinoJson::deserializeJson(doc, buf);

            printStats(doc, statsOutput);
        }
    }

}