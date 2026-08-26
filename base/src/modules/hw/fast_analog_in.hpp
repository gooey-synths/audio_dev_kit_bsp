#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <board/board_interface.hpp>
#include <string>

namespace modules {
namespace hw {

///
/// Class defining a fast analog in.
///
class FastAnalogInput : public module_basics::ModuleBase<0, 1> {
public:
    static constexpr module_basics::ModuleIdType scId = 2; ///< Module ID of the analog input

    ///
    /// Factory method.
    /// @param board Board instance.
    ///
    static FastAnalogInput* factory(board::BoardInterface& board) {
        return new FastAnalogInput(board);
    }

    ///
    /// Constructor.
    /// @param Board instance.
    ///
    FastAnalogInput(board::BoardInterface& board) : mBoard(board) {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~FastAnalogInput() {
        ; // Do nothing
    }

    virtual size_t getInputIdx(std::string name) override {
        return module_basics::scInvalidIOIdx; // Input does not exist.
    }

    virtual size_t getOutputIdx(std::string name) override {
        if(name == "in") {
            return 0;
        }
        return module_basics::scInvalidIOIdx; // Output does not exist
    }

    virtual void run() noexcept override {
        mOutputs[0] = mAi->GetValue();
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        int idx = -1;
        getKeyValue(params, "idx", idx, true);
        mAi = &mBoard.GetAnalogInput(board::IOSpeed::FAST, idx);
    }

protected:
    board::BoardInterface& mBoard;        ///< Board reference.
    board::AnalogInput* mAi = nullptr;    ///< Board analog input.
};

} // namespace hw
} // modules

