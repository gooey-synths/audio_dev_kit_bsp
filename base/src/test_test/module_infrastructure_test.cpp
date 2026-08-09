#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <modules/module_basics.hpp>
#include <modules/graph_runner.hpp>
#include <modules/graph_loader.hpp>

static constexpr size_t scMockPortInputIdx = 0;
static constexpr size_t scMockPortOutputIdx = 0;
// Test double deriving from the actual base class template
template<size_t I, size_t O>
class MockModule : public module_basics::ModuleBase<I, O> {
public:
    size_t getInputIdx(std::string name) override {
        if(name == "mock") {
            return scMockPortInputIdx;
        }
        return scMockPortInputIdx+1;
    }
    size_t getOutputIdx(std::string name) override {
        if(name == "mock") {
            return scMockPortOutputIdx;
        }
        return scMockPortOutputIdx+1;
    }

    MOCK_METHOD(void, run, (), (noexcept, override));

    MOCK_METHOD(void, configure, ((std::unordered_map<std::string, std::string>&)), (override));

    // Expose protected fields from ModuleBase for verification
    uint16_t inspectInputArray(size_t idx) const { return this->mInputs[idx]; }
    void forceOutputArray(size_t idx, uint16_t val) { this->mOutputs[idx] = val; }

    // Expose number of times run() has been called
    size_t numRuns() { return mNumRuns; }

protected:
    size_t mNumRuns = 0;

};

class MockTimer : public board::Timer {
public:
    MOCK_METHOD(void, SetFrequency, (float), (override));
    MOCK_METHOD(void, Start, (), (override));
    MOCK_METHOD(void, Stop, (), (override));

    void SetCallback(board::CallbackFunc callback) override {
        mCallback = callback;
    }

    void tick() {
        if(mCallback) {
            mCallback();
        }
    }

protected:
    board::CallbackFunc mCallback = NULL;
};

template <size_t numTimers>
class MockBoard : public board::BoardInterface {
public:

    MockBoard() { ; /* Do nothing */ }

    board::BoardConfig GetBoardConfig() override {
        return board::BoardConfig {
            .numTimers = numTimers
        };
    }

    MOCK_METHOD(board::AnalogInput&, GetAnalogInput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::AnalogOutput&, GetAnalogOutput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::DigitalInput&, GetDigitalInput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::DigitalOutput&, GetDigitalOutput, (board::IOSpeed, size_t), (override));
    MOCK_METHOD(board::CommunicationInterface*, GetComm, (size_t), (override));
    MOCK_METHOD((void), UpdateSlowIO, (), (override));
    MOCK_METHOD((void), UpdateFastIO, (), (override));

    board::Timer& GetTimer(size_t idx) override {
        return mTimer;
    }

    MockTimer mTimer;
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

TEST(GraphRunnerCoreTests, VerifiesGraphRunnerErrorConditions) {

    // --- Test 1: Verify invalid board error ---
    MockBoard<0> invalidBoard;
    EXPECT_THROW(graph_infrastructure::GraphRunner invalidRunner(invalidBoard), const char*);

    // --- Test 2: Verify multiple instances error ---
    MockBoard<1> board;
    graph_infrastructure::GraphRunner runner(board);
    EXPECT_THROW(graph_infrastructure::GraphRunner invalidRunner(board), const char*);

    // --- Test 3: Verify invalid graph error ---
    runner.setGraph(NULL);
    EXPECT_THROW(runner.start(), const char*);

    // --- Test 4: Verify graph already running error ---
    graph_infrastructure::Graph g;
    runner.setGraph(&g);
    runner.start();
    EXPECT_THROW(runner.setGraph(&g), const char*);

    // --- Test 5: Verify graph double start error ---
    EXPECT_THROW(runner.start(), const char*);
}

// Test fixture for GraphRunner functionality
class GraphRunnerFunctionalityTestFixture : public ::testing::Test {
protected:
    static constexpr size_t numModules = 10;
    static constexpr size_t numInputs = 6;
    static constexpr size_t numOutputs = 7;

    MockBoard<1> board;
    graph_infrastructure::Graph graph;

    void SetUp() override {
        for(size_t i = 0; i < numModules; i++) {
            graph.mods.push_back(new MockModule<numInputs, numOutputs>);
        }
    }

    void TearDown() override {
        for(module_basics::ModuleInterface*& m : graph.mods) {
            delete m;
        }
    }
};
TEST(GraphRunnerCoreTests, SurvivesTimerFiringWhenInstanceIsDestroyed) {
    MockBoard<1> board;
 
    // Create an artificial scope so the GraphRunner is created and then destroyed
    {
        graph_infrastructure::GraphRunner runner(board);
        graph_infrastructure::Graph g;
        runner.setGraph(&g);

        // The timer now holds a pointer to GraphRunner::fastCallBackCallBack.
        // We can tick it here while it's stopped to ensure it safely ignores the tick.
        EXPECT_NO_THROW(board.mTimer.tick());
    }
    // runner goes out of scope here. The destructor is called.
    // sInstance is now nullptr (or NULL in your original code).

    // The hardware timer doesn't know the runner was destroyed, so it fires anyway!
    // If your static wrapper function is written correctly, this will safely do nothing.
    // If it's missing the null check, this will result in a segmentation fault.
    EXPECT_NO_THROW(board.mTimer.tick());
}

// Test suite to check that GraphRunner calls run() on modules
TEST_F(GraphRunnerFunctionalityTestFixture, VerifiesGraphRunnerCallsModuleRun) {
    graph_infrastructure::GraphRunner runner(board);
    runner.setGraph(&graph);
    runner.start();

    // 1. Set up our expectations: We expect every module's run() to be called exactly once.
    for(module_basics::ModuleInterface* mod : graph.mods) {
        auto* mockMod = dynamic_cast<MockModule<numInputs, numOutputs>*>(mod);
        EXPECT_CALL(*mockMod, run()).Times(1);
    }

    // 2. Trigger the action
    board.mTimer.tick();
 
    // 3. gmock will automatically fail the test here if run() wasn't called exactly once!
}

class MockModuleLoader: public graph_infrastructure::ModuleLoaderInterface {
public:
    static constexpr module_basics::ModuleIdType scMockModuleId = 0;
    static constexpr size_t scMockModuleNumInputs = 1;
    static constexpr size_t scMockModuleNumOutputs = 1;

    virtual module_basics::ModuleInterface* loadModule(module_basics::ModuleIdType modId) override {
        if(modId == scMockModuleId) {
            return new MockModule<scMockModuleNumInputs, scMockModuleNumOutputs>();
        }
        return NULL;
    }
};

TEST(GraphLoaderCoreTests, GraphLoaderHappyPath) {
    const char* validGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                },\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                }\
            ],\
            \"connections\": [\
                {\
                    \"input_mod\": 0,\
                    \"input_port_name\": \"mock\",\
                    \"output_mod\": 1,\
                    \"output_port_name\": \"mock\"\
                }\
            ]\
        }\
    ";
    std::cout << validGraphJson << std::endl;

    MockModuleLoader moduleLoader;

    graph_infrastructure::GraphLoader graphLoader(moduleLoader);
    graph_infrastructure::Graph* g = graphLoader.load(const_cast<char*>(validGraphJson), strlen(validGraphJson));

    // Make sure that graph is not NULL
    EXPECT_NE(g, nullptr);

    // Make sure that 2 modules were added to the graph
    EXPECT_EQ(g->mods.size(), 2);

    // Make sure that all modules are not NULL
    for(module_basics::ModuleInterface* mod : g->mods) {
        EXPECT_NE(mod, nullptr);
    }

    // Make sure there is one connection
    EXPECT_EQ(g->cons.size(), 1);

    // Make sure that both input and output of that connection point to the mock port
    EXPECT_EQ(g->cons[0].inPortIdx, scMockPortInputIdx);
    EXPECT_EQ(g->cons[0].outPortIdx, scMockPortOutputIdx);

    EXPECT_EQ(g->cons[0].inModIdx, 0);
    EXPECT_EQ(g->cons[0].outModIdx, 1);
}

// NOTE: This does not test all the ways that JSON can be malformed
// That is too much work for what it is worth
TEST(GraphLoaderCoreTests, GraphLoaderSadPaths) {
    MockModuleLoader moduleLoader;
    graph_infrastructure::GraphLoader graphLoader(moduleLoader);

    // Test invalid module id
    const char* invalidModuleIdGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 6,\
                    \"args\": {}\
                }\
           ]\
        }\
    ";
    EXPECT_THROW({
        try {
            graphLoader.load(const_cast<char*>(invalidModuleIdGraphJson), strlen(invalidModuleIdGraphJson));
        } catch(const char* e) {
            // Ensure that it throws an error referencing the module
            EXPECT_THAT(e, ::testing::HasSubstr("Module"));
            throw;
        }
    }, const char*);

    // Test invalid input module
    const char* invalidInputModuleGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                },\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                }\
            ],\
            \"connections\": [\
                {\
                    \"input_mod\": 9,\
                    \"input_port_name\": \"mock\",\
                    \"output_mod\": 1,\
                    \"output_port_name\": \"mock\"\
                }\
            ]\
        }\
    ";
    EXPECT_THROW({
        try {
            graphLoader.load(const_cast<char*>(invalidInputModuleGraphJson), strlen(invalidInputModuleGraphJson));
        } catch(const char* e) {
            // Ensure that it throws an error referencing the input module
            EXPECT_THAT(e, ::testing::HasSubstr("input module"));
            throw;
        }
    }, const char*);

    // Test invalid output module
    const char* invalidOutputModuleGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                },\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                }\
            ],\
            \"connections\": [\
                {\
                    \"input_mod\": 0,\
                    \"input_port_name\": \"mock\",\
                    \"output_mod\": 9,\
                    \"output_port_name\": \"mock\"\
                }\
            ]\
        }\
    ";
    EXPECT_THROW({
        try {
            graphLoader.load(const_cast<char*>(invalidOutputModuleGraphJson), strlen(invalidOutputModuleGraphJson));
        } catch(const char* e) {
            // Ensure that it throws an error referencing the output module
            EXPECT_THAT(e, ::testing::HasSubstr("output module"));
            throw;
        }
    }, const char*);

    // Test invalid input port
    const char* invalidInputPortGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                },\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                }\
            ],\
            \"connections\": [\
                {\
                    \"input_mod\": 0,\
                    \"input_port_name\": \"invalid\",\
                    \"output_mod\": 1,\
                    \"output_port_name\": \"mock\"\
                }\
            ]\
        }\
    ";
    EXPECT_THROW({
        try {
            graphLoader.load(const_cast<char*>(invalidInputPortGraphJson), strlen(invalidInputPortGraphJson));
        } catch(const char* e) {
            // Ensure that it throws an error referencing the input port
            EXPECT_THAT(e, ::testing::HasSubstr("input port"));
            throw;
        }
    }, const char*);

    // Test invalid output port
    const char* invalidOutputPortGraphJson = "\
        {\
            \"modules\": [\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                },\
                {\
                    \"id\": 0,\
                    \"args\": {}\
                }\
            ],\
            \"connections\": [\
                {\
                    \"input_mod\": 0,\
                    \"input_port_name\": \"mock\",\
                    \"output_mod\": 1,\
                    \"output_port_name\": \"invalid\"\
                }\
            ]\
        }\
    ";
    EXPECT_THROW({
        try {
            graphLoader.load(const_cast<char*>(invalidOutputPortGraphJson), strlen(invalidOutputPortGraphJson));
        } catch(const char* e) {
            std::cout << e << std::endl;
            // Ensure that it throws an error referencing the output port
            EXPECT_THAT(e, ::testing::HasSubstr("output port"));
            throw;
        }
    }, const char*);
}

TEST(GraphLoaderCoreTests, GraphLoaderDoubleBufferingTest) {
    //TODO: Test double buffering system of graph loader
}
