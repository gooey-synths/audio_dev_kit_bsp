#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <modules/hw/fast_analog_out.hpp>
#include <modules/hw/fast_analog_in.hpp>
#include <modules/hw/fast_digital_out.hpp>


class MockAnalogOutput : public board::AnalogOutput {
public:
    virtual void SetValue(board::BoardData data) override { mData = data; }

    board::BoardData getValue() { return mData; }
protected:
    board::BoardData mData;
};

class MockAnalogInput : public board::AnalogInput {
public:
    void setValue(board::BoardData data){ mData = data; }

    board::BoardData GetValue() override { return mData; }
protected:
    board::BoardData mData;
};

class MockDigitalOutput : public board::DigitalOutput {
public:
    virtual void SetValue(bool data) override { mData = data; }

    bool getValue() { return mData; }
protected:
    bool mData;
};

class MockBoard : public board::BoardInterface {
public:
    static constexpr board::BoardConfig scMockBoardConfig = {

    };

    board::BoardConfig GetBoardConfig() override {
        return scMockBoardConfig;
    }

    board::AnalogInput& GetAnalogInput(board::IOSpeed, size_t) override {
        return mAi;
    }

    void SetAnalogInputVal(board::BoardData data) {
        mAi.setValue(data);
    }

    board::AnalogOutput& GetAnalogOutput(board::IOSpeed, size_t) override {
        return mAo;
    }

    board::BoardData GetAnalogOutputVal() {
        return mAo.getValue();
    }

    MOCK_METHOD(board::DigitalInput&, GetDigitalInput, (board::IOSpeed, size_t), (override));

    board::DigitalOutput& GetDigitalOutput(board::IOSpeed, size_t) override {
        return mDo;
    }

    bool GetDigitalOutputVal() {
        return mDo.getValue();
    }

    MOCK_METHOD(board::CommunicationInterface*, GetComm, (size_t), (override));
    MOCK_METHOD((void), UpdateSlowIO, (), (override));
    MOCK_METHOD((void), UpdateFastIO, (), (override));
    MOCK_METHOD(board::Timer&, GetTimer, (size_t idx), (override));

protected:
    MockAnalogOutput mAo;
    MockAnalogInput mAi;
    MockDigitalOutput mDo;
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

TEST(FastAnalogOutputTests, SadPathTests) {
    MockBoard board;
    modules::hw::FastAnalogOutput ao(board);

    std::unordered_map<std::string, std::string> config;

    // Expect key not found.
    EXPECT_THROW(
        try {
            ao.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    config["idx"] = "something";
    // Expect invalid value.
    EXPECT_THROW(
        try {
            ao.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Invalid value");
            throw;
        }, const char*);
}

TEST(FastAnalogInputTests, HappyPathTests) {
    MockBoard board;
    modules::hw::FastAnalogInput ai(board);

    std::unordered_map<std::string, std::string> config;
    config["idx"] = "0";

    ai.configure(config);

    size_t outputIdx = ai.getOutputIdx("in");
    board.SetAnalogInputVal(0x6767);

    ai.run();
    ASSERT_EQ(ai.getOutput(outputIdx), 0x6767);
}

TEST(FastAnalogInputTests, SadPathTests) {
    MockBoard board;
    modules::hw::FastAnalogInput ai(board);

    std::unordered_map<std::string, std::string> config;

    // Expect key not found.
    EXPECT_THROW(
        try {
            ai.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    config["idx"] = "something";
    // Expect invalid value.
    EXPECT_THROW(
        try {
            ai.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Invalid value");
            throw;
        }, const char*);
}

TEST(FastDigitalOutputTests, HappyPathTests) {
    MockBoard board;
    modules::hw::FastDigitalOutput fdo(board);

    std::unordered_map<std::string, std::string> config;
    config["idx"] = "0";

    fdo.configure(config);

    size_t inputIdx = fdo.getInputIdx("out");
    fdo.setInput(inputIdx, 0x6767);
    fdo.run();

    EXPECT_EQ(board.GetDigitalOutputVal(), 0x6767 > 0x7FFF);
}

TEST(FastDigitalOutputTests, SadPathTests) {
    MockBoard board;
    modules::hw::FastDigitalOutput fdo(board);

    std::unordered_map<std::string, std::string> config;

    // Expect key not found.
    EXPECT_THROW(
        try {
            fdo.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    config["idx"] = "something";
    // Expect invalid value.
    EXPECT_THROW(
        try {
            fdo.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Invalid value");
            throw;
        }, const char*);
}
