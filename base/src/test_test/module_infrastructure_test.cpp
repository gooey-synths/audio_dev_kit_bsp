#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "modules/module_basics.hpp"

// Test double deriving from the actual base class template
template<size_t I, size_t O>
class MockModule : public module_basics::ModuleBase<I, O> {
public:
    MOCK_METHOD(size_t, getInputIdx, (std::string name), (override));
    MOCK_METHOD(size_t, getOutputIdx, (std::string name), (override));
    MOCK_METHOD(void, run, (), (noexcept, override));
    MOCK_METHOD(void, configure, ((std::unordered_map<std::string, std::string>)), (override));

    // Expose protected fields from ModuleBase for verification
    uint16_t inspectInputArray(size_t idx) const { return this->mInputs[idx]; }
    void forceOutputArray(size_t idx, uint16_t val) { this->mOutputs[idx] = val; }
};

// Test suite configured specifically for 6 inputs and 7 outputs
TEST(ModuleBaseCoreTests, VerifiesActualBaseClassWithSixInputsAndSevenOutputs) {
    constexpr size_t numInputs = 6;
    constexpr size_t numOutputs = 7;
    MockModule<numInputs, numOutputs> module;

    // --- Test 1: Verify inputExists() boundaries for 6 inputs ---
    EXPECT_TRUE(module.inputExists(0));   // Lower bound
    EXPECT_TRUE(module.inputExists(5));   // Max valid index (6 - 1)
    EXPECT_FALSE(module.inputExists(6));  // Exact out-of-bounds threshold

    // --- Test 2: Verify outputExists() boundaries for 7 outputs ---
    EXPECT_TRUE(module.outputExists(0));   // Lower bound
    EXPECT_TRUE(module.outputExists(6));   // Max valid index (7 - 1)
    EXPECT_FALSE(module.outputExists(7));  // Exact out-of-bounds threshold
 
    // --- Test 3: Verify setInput() modifies the real 6-element array ---
    module.setInput(0, 10);
    module.setInput(5, 50); // Setting the last valid slot
    EXPECT_EQ(module.inspectInputArray(0), 10);
    EXPECT_EQ(module.inspectInputArray(5), 50);

    // --- Test 4: Verify getOutput() reads the real 7-element array ---
    module.forceOutputArray(0, 100);
    module.forceOutputArray(6, 700); // Forcing the last valid slot
    EXPECT_EQ(module.getOutput(0), 100);
    EXPECT_EQ(module.getOutput(6), 700);
}
