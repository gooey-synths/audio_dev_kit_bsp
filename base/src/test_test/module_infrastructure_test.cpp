#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <modules/module_basics.hpp>

constexpr size_t numDummyInputs = 6;
constexpr size_t numDummyOutputs = 7;

class DummyModule: public module_basics::ModuleInterface {
public:
    uint16_t getInputVal(size_t idx) {
        return mInputs[idx]; 
    }
    void setOutputVal(size_t idx, uint16_t val) { 
        mOutputs[idx] = val; 
    }

    virtual void setInput(size_t idx, uint16_t val) noexcept override {
        mInputs[idx] = val;
    }

    virtual uint16_t getOutput(size_t idx) noexcept override {
        return mOutputs[idx];
    }

    virtual bool outputExists(size_t idx) override {
        return idx < numDummyOutputs;
    }

    virtual bool inputExists(size_t idx) override {
        return idx < numDummyInputs;
    }

    MOCK_METHOD(size_t, getInputIdx, (std::string name), (override));

    MOCK_METHOD(size_t, getOutputIdx, (std::string name), (override));

    MOCK_METHOD(void, run, (), (noexcept, override));

    MOCK_METHOD(void, configure, ((std::unordered_map<std::string, std::string>)), (override));

protected:
    uint16_t mInputs[numDummyInputs];
    uint16_t mOutputs[numDummyOutputs];

};

// Test setters and getters for ModuleBase
TEST(ModuleBaseTests, BasicAssertions) {
    DummyModule dm;

    // Make sure that out of bounds indices return false
    ASSERT_EQ(dm.inputExists(numDummyInputs+1), false);
    ASSERT_EQ(dm.outputExists(numDummyOutputs+1), false);

    // Set all of the inputs of the module and make sure they exist
    for(size_t iInput = 0; iInput < numDummyInputs; iInput++) {
          ASSERT_EQ(dm.inputExists(iInput), true);
          dm.setInput(iInput, iInput);
    }
    // Make sure that the input we expect is set to the value we expect
    for(size_t iInput = 0; iInput < numDummyInputs; iInput++) {
        ASSERT_EQ(dm.getInputVal(iInput), iInput);
    }

    // Set all of the outputs of the module and make sure they exist
    for(size_t iOutput = 0; iOutput < numDummyOutputs; iOutput++) {
          ASSERT_EQ(dm.outputExists(iOutput), true);
          dm.setOutputVal(iOutput, iOutput);
    }

    // Make sure that the outputs we expect is set to the value we expect
    for(size_t iOutput = 0; iOutput < numDummyOutputs; iOutput++) {
        ASSERT_EQ(dm.getOutput(iOutput), iOutput);
    }
}

int main(int argc, char **argv) {
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
