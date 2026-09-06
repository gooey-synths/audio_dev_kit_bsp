#include "usb_serial.hpp"
#include <system/interrupts.h>
#include <system/tasks.h>
#include <FreeRTOS.h>
#include <task.h>

#include <cassert>

namespace usb {

///
/// Constructor.
///
USBSerial::USBSerial() :
    mUsbInterfaces{USBCommunication(0), USBCommunication(1)}
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
USBSerial::USBCommunication::USBCommunication(size_t itfIdx) :
    mItfIdx(itfIdx)
{
    ; // Do nothing.
}

///
/// Documentation inherited.
///
size_t USBSerial::USBCommunication::ReadN(char* buf, size_t n) {
    return tud_cdc_n_read(mItfIdx, buf, n);
}

///
/// Documentation inherited.
/// @note If data is written too fast, data may be lost.
///
size_t USBSerial::USBCommunication::WriteN(char* buf, size_t n) {
    uint32_t ret = tud_cdc_n_write(mItfIdx, buf, n);
    return ret;
}

///
/// Documentation inherited.
///
size_t USBSerial::USBCommunication::Available() {
    return tud_cdc_n_available(mItfIdx);
}

///
/// Documentation inherited.
///
void USBSerial::USBCommunication::Flush() {
    tud_cdc_n_write_flush(mItfIdx);
}

} // namespace usb
