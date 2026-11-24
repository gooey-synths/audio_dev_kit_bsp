#include "usb_serial.hpp"
#include <system/interrupts.h>
#include <system/tasks.h>
#include <FreeRTOS.h>
#include <task.h>

#include <cassert>

extern "C" {
    ///
    /// TinyUSB CDC data recieved callback.
    /// @param itf Interface where data was received.
    ///
    void tud_cdc_rx_cb(uint8_t itf) {
        // Notify instance that data is available
        usb::USBSerial::USBCommunication& uitf = usb::USBSerial::getInstance().getInterface(itf);
        uitf.NotifyRxEvent();
    }
}

namespace usb {

///
/// Constructor.
///
USBSerial::USBSerial() :
    mUsbInterfaces{USBCommunication(0, mUsbEventGroup), USBCommunication(1, mUsbEventGroup)},
    mUsbEventGroup(xEventGroupCreate())
{
    NVIC_SetVector(OTG_FS_IRQn, reinterpret_cast<uintptr_t>(usbHandler));
    NVIC_SetPriority(OTG_FS_IRQn, USB_OTG_INT_PRIO);
    tud_init(BOARD_TUD_RHPORT);

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    USBSerial::UsbSerialTask,         /* Function that implements the task. */
                    "USB",                      /* Text name for the task. */
                    USB_TASK_STACK_SIZE,  /* Stack size in words, not bytes. */
                    NULL,                    /* Parameter passed into the task. */
                    USB_TASK_PRIO,          /* Priority at which the task is created. */
                    &xHandle );             /* Used to pass out the created task's handle. */

    assert(xReturned == pdPASS);
}

///
/// Constructor.
/// @param itfIdx Index of USB interface.
///
USBSerial::USBCommunication::USBCommunication(size_t itfIdx, EventGroupHandle_t& eventGroup) :
    mItfIdx(itfIdx),
    mEventGroup(eventGroup)
{
    ; // Do nothing.
}

// Documentation inherited.
size_t USBSerial::USBCommunication::ReadN(char* buf, size_t n) {
    taskENTER_CRITICAL();
    size_t nRead = tud_cdc_n_read(mItfIdx, buf, n);
    if(!Available()) {
        // We have read all of the data cleare RX event flag.
        xEventGroupClearBits(mEventGroup, 1 << mItfIdx);
    }
    taskEXIT_CRITICAL();
    return nRead;
}

// Documentation inherited.
// @note If data is written too fast, data may be lost.
size_t USBSerial::USBCommunication::WriteN(char* buf, size_t n) {
    uint32_t ret = tud_cdc_n_write(mItfIdx, buf, n);
    return ret;
}

// Documentation inherited.
size_t USBSerial::USBCommunication::Available() {
    return tud_cdc_n_available(mItfIdx);
}

// Documentation inherited.
void USBSerial::USBCommunication::Flush() {
    tud_cdc_n_write_flush(mItfIdx);
}

// Documentation inherited.
bool USBSerial::USBCommunication::BlockUntilAvailable(uint32_t timeout) {
    EventBits_t bits = xEventGroupWaitBits(mEventGroup, 1 << mItfIdx, false, true, timeout);

    if(bits & (1 << mItfIdx)) {
        return true;
    }

    return false;

}

} // namespace usb
