#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <board/board_interface.hpp>
#include <string>

namespace modules {
namespace hw {

///
/// Class defining a fast digital output.
///
class FastDigitalOutput : public module_basics::ModuleBase<1, 0> {
public:
    static constexpr module_basics::ModuleIdType scId = 3; ///< Module ID of the digital output

    ///
    /// Factory method.
    /// @param board Board instance.
    ///
    static FastDigitalOutput* factory(board::BoardInterface& board) {
        return new FastDigitalOutput(board);
    }

    ///
    /// Constructor.
    /// @param Board instance.
    ///
    FastDigitalOutput(board::BoardInterface& board) : mBoard(board) {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~FastDigitalOutput() {
        ; // Do nothing
    }

    virtual size_t getInputIdx(std::string name) override {
        if(name == "out") {
            return 0;
        }
        return module_basics::scInvalidIOIdx; // Input does not exist.
    }

    virtual size_t getOutputIdx(std::string name) override {
        return module_basics::scInvalidIOIdx; // Output does not exist
    }

    virtual void run() noexcept override {
        mDo->SetValue(mInputs[0] > 0x7FFF);
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        int idx = -1;
        getKeyValue(params, "idx", idx, true);
        mDo = &mBoard.GetDigitalOutput(board::IOSpeed::FAST, idx);
    }

protected:
    board::BoardInterface& mBoard;         ///< Board reference.
    board::DigitalOutput* mDo = nullptr;   ///< Board digital output.
};

} // namespace hw
} // modules

