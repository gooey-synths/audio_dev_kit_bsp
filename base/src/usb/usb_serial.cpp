#include "usb_serial.hpp"

namespace usb {

///
/// Constructor.
///
USBSerial::USBSerial() : 
    mStreamBufs{USBSerial::Interface::CENTRAL, USBSerial::Interface::EXTRA},
    mUsbStreams{std::iostream(&mStreamBufs[0]), std::iostream(&mStreamBufs[1])},
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
USBSerial::USBStreamBuf::USBStreamBuf(size_t itfIdx) :
    mItfIdx(itfIdx)
{
    memset(mRxBuf, 0, sizeof mRxBuf);
    memset(mTxBuf, 0, sizeof mTxBuf);

    setg(mRxBuf, mRxBuf, mRxBuf+sizeof(mRxBuf));
    setp(mTxBuf, mTxBuf+sizeof(mTxBuf));
}

///
/// Underflow.
/// @return EOF or new character from the buffer.
///
int USBSerial::USBStreamBuf::underflow() {
        // Read in new buffer and size
        uint32_t size = tud_cdc_n_read(mItfIdx, mRxBuf, sizeof(mRxBuf));

        // Reset get buffer
        setg(mRxBuf, mRxBuf, mRxBuf+size);

        if (gptr() >= egptr()) {
            return traits_type::eof(); // Signal end of input
        }

        // Return the character at the current position without advancing the pointer (gptr())
        return traits_type::to_int_type(*gptr());
}

///
/// Overflow function called when buffer is full.
/// @param c Character that is to be placed in the buffer. 
/// @return EOF or character placed in the buffer.
///
int USBSerial::USBStreamBuf::overflow(int c) {
    // Flush the buffer
    (void) tud_cdc_n_write(mItfIdx, mTxBuf, sizeof(mTxBuf));

    // Reset put buffer
    setp(mTxBuf, mTxBuf+sizeof(mTxBuf));

    if(c != traits_type::eof()) {
        // Append new data to buffer if it is not an EOF character.
        *pptr() = traits_type::to_char_type(c);
        // Advance put pointer
        pbump(1);

        return c;
    }

    return traits_type::not_eof(c);
}

///
/// Sync function called when buffer should be flushed.
/// @return EOF.
///
int USBSerial::USBStreamBuf::sync() {
    // Call overflow with EOF to flush the buffer.
    return overflow(traits_type::eof());
}


} // namespace usb