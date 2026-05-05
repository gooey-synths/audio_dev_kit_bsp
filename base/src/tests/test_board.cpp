#include "board/physical_board.hpp"

using namespace board;

///
/// Echo test for board
/// @note To check for success feed an analog signal into one of the analog inputs
/// and see it echoed back on the corresponding analog output.
/// Both USB serial instances should also be echoing.
///
void test_board_echo(){
    static ProtoBoardV1 board;

    char str[] = "Testing ProtoBoard V1\r\n";
    board.GetComm(0)->WriteN(str, sizeof(str));
    board.GetComm(0)->Flush();

    Timer& fastTimer = board.GetTimer(0);
    
    static DigitalOutput& d_out = board.GetDigitalOutput(IOSpeed::FAST, 0);
    static DigitalInput& d_in = board.GetDigitalInput(IOSpeed::FAST, 0);
    gpio::GPIOController::getInstance()->setConfig(&led_pin, &led_pin_conf);

    CallbackFunc fastCallback = [] () {
        gpio::GPIOController::getInstance()->getPin(&led_pin) = true;
        for(size_t i=0; i< board.GetBoardConfig().fastIO.numAnalogInputs; i++) {
            AnalogOutput& a_out = board.GetAnalogOutput(IOSpeed::FAST, i);
            AnalogInput& a_in = board.GetAnalogInput(IOSpeed::FAST, i);
            a_out.SetValue(a_in.GetValue());
        }
        d_out.SetValue(d_in.GetValue());

        board.UpdateFastIO();
        gpio::GPIOController::getInstance()->getPin(&led_pin) = false;
    };

    fastTimer.SetFrequency(44100);

    fastTimer.SetCallback(fastCallback);

    fastTimer.Start();

    char buf [256];

    // Echo USB serial communication as well.
    while(1) {
        for(size_t iComm = 0; iComm < usb::USBSerial::getInstance().numInterfaces(); iComm++) {
            if(board.GetComm(iComm)->Available()) {
                size_t nRead = board.GetComm(iComm)->ReadN(buf, sizeof(buf));
                board.GetComm(iComm)->WriteN(buf, nRead);
                board.GetComm(iComm)->Flush();
            }
        }
    }
}