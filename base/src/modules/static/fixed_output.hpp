#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"

namespace modules {
namespace stat {

///
/// Class defining a fixed output module.
///
class FixedOutput : public module_basics::ModuleBase<0, 1> {
public:
    static constexpr module_basics::ModuleIdType scId = 102; ///< Module ID of the fixed output
    static constexpr size_t scOutIdx = 0;  ///< Index of the output.

    ///
    /// Factory method.
    ///
    static FixedOutput* factory() {
        return new FixedOutput();
    }


    ///
    /// Constructor.
    ///
    FixedOutput() {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~FixedOutput() {
        ; // Do nothing.
    }

    virtual size_t getInputIdx(std::string name) override {
        return module_basics::scInvalidIOIdx; // Input does not exist.
    }

    virtual size_t getOutputIdx(std::string name) override {
        if(name == "out") {
            return scOutIdx;
        }
        return module_basics::scInvalidIOIdx; // Output does not exist
    }

    virtual void run() noexcept override {
        ; // Do nothing.
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        getKeyValue(params, "val", mOutputs[scOutIdx], true);
    }

};

} // namespace stat
} // namespace modules

