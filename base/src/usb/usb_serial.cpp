#include "usb_serial.hpp"

namespace usb {

///
/// Constructor.
///
USBSerial::USBSerial() :
    mUsbInterfaces{USBCommunication(0), USBCommunication(1)},
    mTimer(USBSerial::scTimerNum)
{
    set_vector_table_entry(static_cast<int>(OTG_FS_IRQn+16), USBSerial::usbHandler);
    tud_init(BOARD_TUD_RHPORT);

    mTimer.setFreq(USBSerial::scTusbFreq);
    mTimer.setInterrupt(USBSerial::timerHandler);
    mTimer.start(scTusbFreq);
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
/// Read N characters into a buffer.
/// @param buf Buffer to read characters into.
/// @param n Maximum characters to read in.
/// @return Number of characters read.
///
size_t USBSerial::USBCommunication::ReadN(char* buf, size_t n) {
    return tud_cdc_n_read(mItfIdx, buf, n);
}

///
/// Write N characters from a buffer.
/// @param buf Buffer of characters to write.
/// @param n Maximum characters to write.
/// @return Number of characters written.
///
size_t USBSerial::USBCommunication::WriteN(char* buf, size_t n) {
    return tud_cdc_n_write(mItfIdx, buf, n);
}



} // namespace usb