#include "board/physical_board.hpp"

using namespace board;

void test_board_echo(){
    static ProtoBoardV1 board;

    Timer& fastTimer = board.GetTimer(0);
    
    static AnalogOutput& a_out = board.GetAnalogOutput(IOSpeed::FAST, 0);
    static AnalogInput& a_in = board.GetAnalogInput(IOSpeed::FAST, 0);

    static DigitalOutput& d_out = board.GetDigitalOutput(IOSpeed::FAST, 0);
    static DigitalInput& d_in = board.GetDigitalInput(IOSpeed::FAST, 0);

    CallbackFunc fastCallback = [] () {
        a_out.SetValue(a_in.GetValue());
        d_out.SetValue(d_in.GetValue());

        board.UpdateFastIO();
    };

    fastTimer.SetFrequency(44100);

    fastTimer.SetCallback(fastCallback);

    fastTimer.Start();
}