#ifndef USB_SERIAL_HPP
#define USB_SERIAL_HPP

#include <streambuf>
#include "../timer/basic_timer.hpp"
extern "C" {
#include "tusb.h"
#include "device/usbd.h"
}

namespace usb {

///
/// Class for interfacing with a serial port over USB.
///
class USBSerial {
public:

    ///
    /// Enumeration for different USB interfaces.
    ///
    enum Interface {
        CENTRAL = 0,
        EXTRA = 1,
    };

    static const size_t scNumInterfaces = CFG_TUD_CDC; ///< Number of interfaces to manage.
    static const uint32_t scTusbFreq = 10; ///< Rate to call TinyUSB device task.
    static const timer::eBasicTimerNumber scTimerNum = ::timer::eBasicTimerNumber::BASIC_TIMER_6 ///< Timer for calling TinyUSB device task.

    ///
    /// @brief Get an instance of the USB Serial.
    /// @return An instance of the USB serial.
    ///
    static USBSerial& GetInstance() {
        static USBSerial sInstance;
        return sInstance;
    }

    ///
    /// Get an IO stream.
    /// @param itf Interface to get stream of.
    /// @return Referance to stream of requested interface.
    ///
    std::iostream& getStream(Interface itf) {
        if(itf >= scNumInterfaces) {
            throw "Invalid USB interface";
        }
        return mUsbStreams[itf];
    }

    // Delete copy and assignment to preserve singleton pattern.
    USBSerial(USBSerial const&) = delete;
    void operator=(USBSerial const&)  = delete;

private:

    ///
    /// Stream buffer for sending and receiving text over USB.
    ///
    class USBStreamBuf: public std::streambuf {
    public:
        USBStreamBuf(size_t itfIdx);

    private:
        size_t mItfIdx; ///< USB interface index.
        char mRxBuf[CFG_TUD_CDC_RX_BUFSIZE]; ///< Receive buffer.
        char mTxBuf[CFG_TUD_CDC_TX_BUFSIZE]; ///< Transmit buffer.
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

    USBStreamBuf mStreamBufs[scNumInterfaces];  ///< USB buffers.
    std::iostream mUsbStreams[scNumInterfaces]; ///< IO streams.
    timer::BasicTimer mTimer; ///< Timer for calling tinyUSB device task.
};

} // namespace usb

#endif // USB_SERIAL_HPP