#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <modules/hw/fast_analog_out.hpp>
#include <modules/hw/fast_digital_out.hpp>
#include <modules/hw/fast_digital_in.hpp>


class MockAnalogOutput : public board::AnalogOutput {
public:
    virtual void SetValue(board::BoardData data) override { mData = data; }

    board::BoardData getValue() { return mData; }
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

class MockDigitalInput : public board::DigitalInput {
public:
    void setValue(bool data) { mData = data; }

    virtual bool GetValue() override { return mData; }
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

    MOCK_METHOD(board::AnalogInput&, GetAnalogInput, (board::IOSpeed, size_t), (override));

    board::AnalogOutput& GetAnalogOutput(board::IOSpeed, size_t) override {
        return mAo;
    }

    board::BoardData GetAnalogOutputVal() {
        return mAo.getValue();
    }

    board::DigitalInput& GetDigitalInput(board::IOSpeed, size_t) override {
        return mDi;
    }

    void SetDigitalInputVal(bool val) {
        mDi.setValue(val);
    }

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
    MockDigitalOutput mDo;
    MockDigitalInput mDi;
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

TEST(FastDigitalInputTests, HappyPathTests) {
    MockBoard board;
    modules::hw::FastDigitalInput fdi(board);

    std::unordered_map<std::string, std::string> config;
    config["idx"] = "0";

    fdi.configure(config);

    size_t outputIdx = fdi.getOutputIdx("in");
    board.SetDigitalInputVal(true);
    fdi.run();

    EXPECT_GT(fdi.getOutput(outputIdx), 0x7FFF);
}

TEST(FastDigitalInputTests, SadPathTests) {
    MockBoard board;
    modules::hw::FastDigitalOutput fdi(board);

    std::unordered_map<std::string, std::string> config;

    // Expect key not found.
    EXPECT_THROW(
        try {
            fdi.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Key not found");
            throw;
        }, const char*);

    config["idx"] = "something";
    // Expect invalid value.
    EXPECT_THROW(
        try {
            fdi.configure(config);
        } catch(const char* c) {
            EXPECT_EQ(c, "Invalid value");
            throw;
        }, const char*);
}
