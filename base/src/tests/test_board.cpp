#include "board/physical_board.hpp"

using namespace board;

void test_board_echo(){
    static ProtoBoardV1 board;

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

    fastTimer.SetFrequency(40000);

    fastTimer.SetCallback(fastCallback);

    fastTimer.Start();

    while(1);
}