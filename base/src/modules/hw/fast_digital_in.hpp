#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <board/board_interface.hpp>
#include <string>

namespace modules {
namespace hw {

///
/// Class defining a fast digital input.
///
class FastDigitalInput : public module_basics::ModuleBase<0, 1> {
public:
    static constexpr module_basics::ModuleIdType scId = 4; ///< Module ID of the digital input

    ///
    /// Factory method.
    /// @param board Board instance.
    ///
    static FastDigitalInput* factory(board::BoardInterface& board) {
        return new FastDigitalInput(board);
    }

    ///
    /// Constructor.
    /// @param Board instance.
    ///
    FastDigitalInput(board::BoardInterface& board) : mBoard(board) {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~FastDigitalInput() {
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
        mOutputs[0] = mDi->GetValue() ? 0xFFFF : 0;
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        int idx = -1;
        getKeyValue(params, "idx", idx, true);
        mDi = &mBoard.GetDigitalInput(board::IOSpeed::FAST, idx);
    }

protected:
    board::BoardInterface& mBoard;        ///< Board reference.
    board::DigitalInput* mDi = nullptr;   ///< Board digital input.
};

} // namespace hw
} // modules

