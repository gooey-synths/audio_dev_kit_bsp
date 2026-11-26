#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <util/fast_math.h>
#include <cmath>

namespace modules {
namespace stat {

///
/// Class defining a basic voltage controlled attenuator.
///
class VCA : public module_basics::ModuleBase<2, 1> {
public:
    static constexpr module_basics::ModuleIdType scId = 101; ///< Module ID of the oscillator
    static constexpr size_t scCvIdx = 0;
    static constexpr size_t scInIdx = 1;
    static constexpr size_t scOutIdx = 0;

    ///
    /// Factory method.
    ///
    static VCA* factory() {
        return new VCA();
    }


    ///
    /// Constructor.
    ///
    VCA() {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~VCA() {
        ; // Do nothing.
    }

    virtual size_t getInputIdx(std::string name) override {
        if(name == "cv") {
            return scCvIdx;
        } else if(name == "in") {
            return scInIdx;
        }
        return module_basics::scInvalidIOIdx; // Input does not exist.
    }

    virtual size_t getOutputIdx(std::string name) override {
        if(name == "out") {
            return scOutIdx;
        }
        return module_basics::scInvalidIOIdx; // Output does not exist
    }

    virtual void run() noexcept override {
        float attenuation = mMaxAttenuation * (float)mInputs[scCvIdx];
        float attenuated = (float)mInputs[scInIdx] * attenuated / float(0xFFFF);
        mOutputs[scOutIdx]  = (uint16_t)attenuated;
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        float attenuation;
        getKeyValue(params, "at", attenuation, false);
        if(attenuation < 0.0 || attenuation > 1.0) {
            throw "Invalid attenuation";
        }
        mMaxAttenuation = attenuation;
    }

protected:
    float mMaxAttenuation = 1.0;
};



} // namespace stat
} // namespace modules

