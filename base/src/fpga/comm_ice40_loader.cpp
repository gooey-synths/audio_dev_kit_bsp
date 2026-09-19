#include "comm_ice40_loader.hpp"
#include "ice40_loader.hpp"

#include <system/tasks.h>

namespace fpga {

const char CommIce40Loader::scHeader[3] = {0xAB,0xCD,0xEF};
CommIce40Loader* CommIce40Loader::sInstance = NULL;

CommIce40Loader::CommIce40Loader(board::CommunicationInterface& commItf,
        gpio::Pin& csPin, gpio::Pin& cDonePin, gpio::Pin& resetPin, spi::ISpiBus& spi)
    : mSpiBus(spi), mCsPin(csPin), mCDonePin(cDonePin), mResetPin(resetPin), mCommItf(commItf) {

    assert(sInstance == nullptr);

    sInstance = this;

    BaseType_t xReturned;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    CommIce40Loader::loaderTask,         /* Function that implements the task. */
                    "ICE40 loader",                      /* Text name for the task. */
                    ICE40_LOAD_TASK_STACK_SIZE,  /* Stack size in words, not bytes. */
                    NULL,                    /* Parameter passed into the task. */
                    ICE40_LOAD_PRIO,          /* Priority at which the task is created. */
                    &mTaskHandle);             /* Used to pass out the created task's handle. */

    assert(xReturned == pdPASS);

}

///
/// Destructor
///
CommIce40Loader::~CommIce40Loader() {
    sInstance = nullptr;
    vTaskDelete(mTaskHandle);
}

///
/// Block until header is sent over
///
void CommIce40Loader::waitForHeader() {
    size_t counter = 0;

    while(1) {
        char c;
        while(!mCommItf.BlockUntilAvailable(portMAX_DELAY));

        size_t n = mCommItf.ReadN(&c, sizeof c);

        if(n == 0) {
            continue;
        }

        if(c == scHeader[counter]) {
            counter++;
        } else {
            counter = 0;
        }

        if(counter >= sizeof(scHeader)) {
            break;
        }
    }
}

///
/// FreeRTOS task for peforming the loading.
/// @param v Task initialization argument, unused.
///
void CommIce40Loader::loaderTask(void* v) {
    (void) v;

    while(1) {
        // This could technically be optimized to pend on some signal, but I won't bother.
        if(sInstance) {
            sInstance->loaderTaskBody();
        } else {
            vTaskDelay(100);
        }
    }
}

///
/// Body of the loading task, should be run in a loop.
///
void CommIce40Loader::loaderTaskBody() {
    char buffer[512];

    waitForHeader();

    ice40_init(mResetPin, mCsPin);

    while(!ice40_is_done(mCDonePin)) {
        mCommItf.BlockUntilAvailable(portMAX_DELAY);
        size_t nRead = mCommItf.ReadN(buffer, sizeof buffer);
        if(nRead) {
            ice40_write_bytes(mSpiBus, buffer, nRead);
        }
    }

    ice40_finish(mSpiBus, mCsPin);
}

} // namespace fpga
