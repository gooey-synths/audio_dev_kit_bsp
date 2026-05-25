#include <usb/usb_serial.hpp>
#include <gpio/gpio.hpp>
#include <cstddef>
#include <cctype>
#include <ArduinoJson.hpp>
#include <vector>

using namespace ArduinoJson;
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

// This is a bit janky
const char newline[] = "\r\n";

///
/// Read input of a USB interface until and EOF character is found
/// @param buf Buffer to place received characters into
/// @param bufLen Length of the buffer
/// @param itf Interface to read from
/// @return True if EOF character was found befor buffer overflow
///
static bool readUntilEOF(char* buf, size_t bufLen, usb::USBSerial::USBCommunication& itf) {

    size_t bufIdx = 0;
    char c;

    while(1) {
        size_t numRead = itf.ReadN(&c, 1);

        if(numRead) {
            // Check for windows or linux EOF character.
            if(c == 0x1A || c == 0x04) {
                itf.WriteN((char*)&newline, sizeof(newline));
                itf.Flush();
                return true;
            } else if(bufIdx >= bufLen) {
                return false;
            } else {
                itf.WriteN(&c, 1);
                itf.Flush();
                buf[bufIdx] = c;
                bufIdx++;
            }
        }

    }
}

///
/// Print statistics about a JSON document
/// @param in_doc JSON document to print statistics of 
/// @param itf USB interface to print statistics to
///
static void printStats(JsonDocument& in_doc, usb::USBSerial::USBCommunication& itf) {
    JsonDocument stats_doc;

    char buf[256];
    memset(buf, 0, sizeof(buf));

    // Traverse JSON tree
    size_t numNodes = 1;
    size_t numLeaves = 0;
    std::vector<JsonObject> stack;

    // Initial check for root object vs root array
    if(in_doc.is<JsonObject>()) {
        stack.push_back(in_doc.as<JsonObject>());
    } else if(in_doc.is<JsonArray>()) {
        for(JsonVariant elem : in_doc.as<JsonArray>()) {
            numNodes++;
            if(elem.is<JsonObject>()) {
                stack.push_back(elem.as<JsonObject>());
            } else {
                numLeaves++;
            }
        }
    }

    while(stack.size()) {
        JsonObject currObj = stack.back();
        stack.pop_back();

        for(JsonPair kv : currObj) {
            numNodes++;
            if(kv.value().is<JsonObject>()) {
                stack.push_back(kv.value().as<JsonObject>());
            } else if(kv.value().is<JsonArray>()) {
                for(JsonVariant elem : kv.value().as<JsonArray>()) {
                    numNodes++;
                    if(elem.is<JsonObject>()) {
                        stack.push_back(elem.as<JsonObject>());
                    } else {
                        numLeaves++;
                    }
                }
            }
            else {
                numLeaves++;
            }
        }
    }

    stats_doc["depth"] = in_doc.nesting();
    stats_doc["numNodes"] = numNodes;
    stats_doc["numLeaves"] = numLeaves;

    serializeJson(stats_doc, buf);

    itf.WriteN(buf, sizeof(buf));
    itf.WriteN((char*)&newline, sizeof(newline));
    itf.Flush();
}

///
/// Test reading and writing JSON using ArduinoJSON
/// @note To check for success enter JSON into one USB COM port on the system
/// statistics about the entered JSON or parsing error messages will appear on the other USB COM port
///
void test_ArduinoJSON() {
    setupPins();
    usb::USBSerial& usbSerial = usb::USBSerial::getInstance();
    usb::USBSerial::USBCommunication& jsonInput = usbSerial.getInterface(0);
    usb::USBSerial::USBCommunication& statsOutput = usbSerial.getInterface(1);
    
    char buf[1024];

    JsonDocument doc;
    
    while(1) {
        memset(buf, 0, sizeof(buf));
        if(readUntilEOF(buf, sizeof(buf), jsonInput)) {
            DeserializationError err = deserializeJson(doc, buf);
            if(err) {
                statsOutput.WriteN((char*)err.c_str(), strlen(err.c_str()));
                statsOutput.WriteN((char*)&newline, sizeof(newline));
                statsOutput.Flush();
            } else {
                printStats(doc, statsOutput);
            }
        }
    }
}