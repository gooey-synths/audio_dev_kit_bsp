#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"
#include <util/fast_math.h>
#include <cmath>

namespace modules {
namespace stat {

typedef uint16_t (*WaveFunc)(uint16_t);

///
/// Class defining a basic frequency modulated oscillator.
/// @note This is an implemenetation of a phase accumulation oscillator.
/// @note This has a minimum phase resolution of 2pi / 65535
///
class Oscillator : public module_basics::ModuleBase<1, 1> {
public:
    // TODO: figure out ID
    static constexpr module_basics::ModuleIdType scId = 1234; ///< Module ID of the digital input

    ///
    /// Factory method.
    ///
    static Oscillator* factory() {
        return new Oscillator();
    }


    ///
    /// Constructor.
    ///
    Oscillator() {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~Oscillator() {
        ; // Do nothing.
    }

    virtual size_t getInputIdx(std::string name) override {
        if(name == "mod") {
            return 0;
        }
        return module_basics::scInvalidIOIdx; // Input does not exist.
    }

    virtual size_t getOutputIdx(std::string name) override {
        if(name == "out") {
            return 0;
        }
        return module_basics::scInvalidIOIdx; // Output does not exist
    }

    virtual void run() noexcept override {
        ///TODO: Only recalculate phase delta if input changes for optimization
        float freqShift = voltageToFreqShift(mInputs[0], mOctavesPerCount);
        mPhaseAcum += FreqToPhaseDelta(freqShift * mCenterFreq);
        mOutputs[0] = mWaveFunc(mPhaseAcum);
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        std::string waveType;
        getKeyValue(params, "cf", mCenterFreq, false);
        getKeyValue(params, "opc", mOctavesPerCount, false);
        getKeyValue(params, "wav", waveType, false);
 
        // TODO: Add wave type decoding
    }

protected:
    ///
    /// Calculate the target frequency shift based on the input
    /// @param in Input to calculate from.
    /// @param octavesPerCount Octaves per count.
    ///
    static inline float voltageToFreqShift(uint16_t in, float octavesPerCount) {
        float fIn = static_cast<float>(in) - (static_cast<float>(0xFFFF / 2.0F));
        float octaveShift = fIn * octavesPerCount;
        return fast_exp2f(octaveShift);
    }

    ///
    /// Calculate the phase delta for a given frequency.
    /// @freq Frequency to calculate the phase delta for.
    ///
    static inline uint16_t FreqToPhaseDelta(float freq) {
        float phaseRatio = freq / module_basics::scSamplerate;
        return static_cast<uint16_t>(phaseRatio * static_cast<float>(0xFFFF));
    }

    // Wave functions

    ///
    /// Saw tooth wave
    /// @param theta Wave phase to generate.
    /// @return Amplitude for a given theta.
    ///
    static inline uint16_t saw(uint16_t theta) {
        return theta;
    }

    ///
    /// Square wave
    /// @param theta Wave phase to generate.
    /// @return Amplitude for a given theta.
    ///
    static inline uint16_t sqr(uint16_t theta) {
        return theta > 0xFFFF/2 ? 0xFFFF : 0;
    }

    uint16_t mPhaseAcum = 0;                         ///< Phase accumulator
    float mCenterFreq = 261.625565F;                 ///< Center frequency for when the voltage is 0
    float mOctavesPerCount = 24.0f / float(0xFFFF);  ///< Octaves per count.
    WaveFunc mWaveFunc = saw;                        ///< Wave function
};

} // namespace hw
} // stat

