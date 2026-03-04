#include "usb_serial.hpp"

namespace usb {

///
/// Constructor.
///
USBSerial::USBSerial(): 
    mStreamBufs{USBSerial::Interface::CENTRAL, USBSerial::Interface::EXTRA},
    mUsbStreams{mStreamBufs[0], mStreamBufs[1]},
    mTimer(USBSerial::scTimerNum)
{
    set_vector_table_entry(static_cast<int>(OTG_FS_IRQn+16), USBSerial::usbHandler);
    tud_init(BOARD_TUD_RHPORT);
    mTimer.setFreq(USBSerial::scTusbFreq);
    mTimer.setInterrupt(USBSerial::timerHandler);
    mTimer.start();
}

///
/// Constructor.
/// @param itfIdx Index of USB interface.
///
USBSerial::USBStreamBuf::USBStreamBuf(size_t itfIdx):
    mItfIdx(itfIdx)
{
    memset(mRxBuf, 0, sizeof mRxBuf);
    memset(mTxBuf, 0, sizeof mTxBuf);
}

} // namespace usb