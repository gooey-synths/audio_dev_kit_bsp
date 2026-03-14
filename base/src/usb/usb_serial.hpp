#ifndef USB_SERIAL_HPP
#define USB_SERIAL_HPP

#include <timer/basic_timer.hpp>
#include <board/board_interface.hpp>
extern "C" {
#include "tusb.h"
#include "device/usbd.h"
}

namespace usb {

///
/// Class for managing USB serial connections.
///
class USBSerial {
public:
    static const size_t scNumInterfaces = CFG_TUD_CDC; ///< Number of interfaces to manage.
    static const uint32_t scTusbFreq = 100; ///< Rate to call TinyUSB device task.
    static const timer::eBasicTimerNumber scTimerNum = ::timer::eBasicTimerNumber::BASIC_TIMER_6; ///< Timer for calling TinyUSB device task.

    ///
    /// @brief Get an instance of the USB Serial.
    /// @return An instance of the USB serial.
    ///
    static USBSerial& getInstance() {
        static USBSerial sInstance;
        return sInstance;
    }

    ///
    /// Get a USB interface.
    /// @param idx Index of USB interface to get.
    /// @return A reference to a USB interface.
    ///
    board::CommunicationInterface& getInterface(size_t idx) {
        if(idx >= scNumInterfaces) {
            throw "Invalid USB interface";
        }
        return mUsbInterfaces[idx];
    }

    // Delete copy and assignment to preserve singleton pattern.
    USBSerial(USBSerial const&) = delete;
    void operator=(USBSerial const&)  = delete;

private:

    ///
    /// Stream buffer for sending and receiving text over USB.
    ///
    class USBCommunication: public board::CommunicationInterface {
    public:
        USBCommunication(size_t itfIdx);

    virtual size_t ReadN(char* buf, size_t n) override;

    virtual size_t WriteN(char* buf, size_t n) override;

    private:
        size_t mItfIdx; ///< USB interface index.
    };

    USBSerial();

    ///
    /// Handler for USB events.
    ///
    static void usbHandler() {
        tud_int_handler(BOARD_TUD_RHPORT);
    }

    ///
    /// Handler for timer.
    ///
    static void timerHandler() {
        tud_task_ext(0, true);
    }

    USBCommunication mUsbInterfaces[scNumInterfaces];  ///< USB buffers.
    timer::BasicTimer mTimer; ///< Timer for calling tinyUSB device task.
};

} // namespace usb

#endif // USB_SERIAL_HPP