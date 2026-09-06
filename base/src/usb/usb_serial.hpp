#ifndef USB_SERIAL_HPP
#define USB_SERIAL_HPP

#include <board/board_interface.hpp>
extern "C" {
#include "tusb.h"
#include <FreeRTOS.h>
#include <event_groups.h>

#include "device/usbd.h"
}

namespace usb {

///
/// Class for managing USB serial connections.
///
class USBSerial {
public:
    ///
    /// Stream buffer for sending and receiving text over USB.
    ///
    class USBCommunication: public board::CommunicationInterface {
    public:
        virtual size_t ReadN(char* buf, size_t n) override;

        virtual size_t WriteN(char* buf, size_t n) override;

        virtual size_t Available() override;

        virtual void Flush() override;

        virtual bool BlockUntilAvailable(uint32_t timeout) override;


        // Delete copy and assignment.
        USBCommunication(USBCommunication const&) = delete;
        void operator=(USBCommunication const&)  = delete;

    private:
        USBCommunication(size_t itfIdx, EventGroupHandle_t& eventGroup);

        ///
        /// Notify RTOS that data is available.
        ///
        void NotifyRxEvent() {
            // Have to do this to avoid priority inversion
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xEventGroupSetBitsFromISR(mEventGroup, 1 << mItfIdx, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        // Only allow USB Serial class to construct interfaces.
        friend class USBSerial;
        // Allow TinyUSB RX callback to access event bits while maintaining encapsulation.
        // Note: I don't feel great about this but the alternatives feel worse to me.
        friend void ::tud_cdc_rx_cb(uint8_t itf);

        size_t mItfIdx; ///< USB interface index.
        EventGroupHandle_t& mEventGroup; ///< Event group reference for USB events.
    };

    static const size_t scNumInterfaces = CFG_TUD_CDC; ///< Number of interfaces to manage.

    ///
    /// @brief Get an instance of the USB Serial.
    /// @return An instance of the USB serial.
    ///
    static USBSerial& getInstance() {
        static USBSerial sInstance;
        return sInstance;
    }

    ///
    /// Get the number of USB Serial interfaces.
    /// @return Number of USB Serial interfaces.
    ///
    size_t numInterfaces() const {
        return scNumInterfaces;
    }

    ///
    /// Get a USB interface.
    /// @param idx Index of USB interface to get.
    /// @return A reference to a USB interface.
    ///
    USBCommunication& getInterface(size_t idx) {
        if(idx >= scNumInterfaces) {
            throw "Invalid USB interface";
        }
        return mUsbInterfaces[idx];
    }

    // Delete copy and assignment to preserve singleton pattern.
    USBSerial(USBSerial const&) = delete;
    void operator=(USBSerial const&)  = delete;

private:
    USBSerial();

    ///
    /// FreeRTOS task for calling TinyUSB device task
    /// @param pvParameters Initialization parameters (unused).
    ///
    static void UsbSerialTask(void *pvParameters) {
        (void) pvParameters;
        while(1) {
            tud_task();
        }
    }

    ///
    /// Handler for USB interrupt.
    ///
    static void usbHandler() {
        tud_int_handler(BOARD_TUD_RHPORT);
    }

    USBCommunication mUsbInterfaces[scNumInterfaces];  ///< USB buffers.
    EventGroupHandle_t mUsbEventGroup;                 ///< FreeRTOS event group.
};

} // namespace usb

#endif // USB_SERIAL_HPP