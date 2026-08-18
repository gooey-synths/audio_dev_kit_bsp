#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <modules/hw/fast_analog_out.hpp>


class MockAnalogOutput : public board::AnalogOutput {
public:
    virtual void SetValue(board::BoardData data) { mData = data; }

    board::BoardData getValue() { return mData; }
protected:
    board::BoardData mData;
};


class MockBoard : public board::BoardInterface {
public:
    static constexpr board::BoardConfig scMockBoardConfig = {

    };

    board::BoardConfig GetBoardConfig() override {
        return scMockBoardConfig;
    }

    MOCK_METHOD(board::AnalogInput&, GetAnalogInput, (board::IOSpeed, size_t), (override));

    board::AnalogOutput& GetAnalogOutput(board::IOSpeed, size_t) override {
        return mAo;
    }

    board::BoardData GetAnalogOutputVal() {
        return mAo.getValue();
    }

    MOCK_METHOD(board::DigitalInput&, GetDigitalInput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::DigitalOutput&, GetDigitalOutput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::CommunicationInterface*, GetComm, (size_t), (override));
    MOCK_METHOD((void), UpdateSlowIO, (), (override));
    MOCK_METHOD((void), UpdateFastIO, (), (override));
    MOCK_METHOD(board::Timer&, GetTimer, (size_t idx), (override));

protected:
    MockAnalogOutput mAo;
};


TEST(FastAnalogOutputTests, HappyPathTests) {
    MockBoard board;
    modules::hw::FastAnalogOutput ao(board);

    std::unordered_map<std::string, std::string> config;
    config["idx"] = "0";

    ao.configure(config);

    size_t inputIdx = ao.getInputIdx("out");
    ao.setInput(inputIdx, 0x6767);
    ao.run();

    EXPECT_EQ(board.GetAnalogOutputVal(), 0x6767);
}
