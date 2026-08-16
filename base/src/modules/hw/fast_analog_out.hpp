#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <board/board_interface.hpp>
#include <string>

namespace modules {
namespace hw {

///
/// Class defining a fast analog output.
///
class FastAnalogOutput : public module_basics::ModuleBase<1, 0> {
public:
    static constexpr module_basics::ModuleIdType scId = 1; ///< Module ID of the analog output

    ///
    /// Factory method.
    /// @param board Board instance.
    ///
    static FastAnalogOutput* factory(board::BoardInterface& board) {
        return new FastAnalogOutput(board);
    }

    ///
    /// Constructor.
    /// @param Board instance.
    ///
    FastAnalogOutput(board::BoardInterface& board) : mBoard(board) {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~FastAnalogOutput() {
        if(mAo) {
            // Set value to half way.
            mAo->SetValue(0xFFFF >> 2);
        }
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
        mAo->SetValue(mInputs[0]);
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        size_t idx = -1;
        getKeyValue(params, "idx", idx, true);
        mAo = &mBoard.GetAnalogOutput(board::IOSpeed::FAST, idx);
    }

protected:
    board::BoardInterface& mBoard;        ///< Board reference.
    board::AnalogOutput* mAo = nullptr;   ///< Board analog output.
};

} // namespace hw
} // modules

