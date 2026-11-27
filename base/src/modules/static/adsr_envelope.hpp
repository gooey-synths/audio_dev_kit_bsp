#pragma once

#include "../module_basics.hpp"
#include "../util.hpp"

namespace modules {
namespace stat {

///
/// Class defining a basic ADSR envelope generator.
///
class AdsrEnvelope: public module_basics::ModuleBase<1, 1> {
public:
    static constexpr module_basics::ModuleIdType scId = 103; ///< Module ID of the envelope.
    static constexpr size_t scGateIdx = 0; ///< Index of gate input
    static constexpr size_t scOutIdx = 0; ///< Index of output

    ///
    /// State of the envelope.
    ///
    enum eState {
        IDLE,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE,
    };

    ///
    /// Factory method.
    ///
    static AdsrEnvelope* factory() {
        return new AdsrEnvelope();
    }


    ///
    /// Constructor.
    ///
    AdsrEnvelope() {
        ; // Do nothing.
    }

    ///
    /// Destructor.
    ///
    virtual ~AdsrEnvelope() {
        ; // Do nothing.
    }

    virtual size_t getInputIdx(std::string name) override {
        if(name == "gate") {
            return scGateIdx;
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
        // State processing
        switch(mState) {
            case ATTACK:
                handleAttack();
                break;
            case DECAY:
                handleDecay();
                break;
            case SUSTAIN:
                handleSustain();
                break;
            case RELEASE:
                handleRelease();
                break;
            case IDLE:
                handleIdle();
                break;
            default:
                // Default to the idle state.
                nextState(IDLE);
                handleIdle();
                break;
        }
        // Always have the release slope calculated.
        // Although less efficient it keeps the timing consistent.
        if(mState != RELEASE) {
            mReleaseSlope = mOutputFloat / module_basics::scSamplerate;
        }
        // Update output and timer.
        mOutputs[scOutIdx] = (uint16_t) mOutputFloat;
        mTimer += 1.0f / module_basics::scSamplerate;
    }

    virtual void configure(std::unordered_map<std::string, std::string>& params) override {
        getKeyValue(params, "a", mAttack, true);
        getKeyValue(params, "d", mDecay, true);
        getKeyValue(params, "s", mSustain, true);
        getKeyValue(params, "r", mRelease, true);

        checkMinMax(mAttack, (float)0, (float)0xFFFF, true);
        checkMinMax(mDecay, (float)0, (float)0xFFFF, true);
        checkMinMax(mSustain, (float)0, (float)0xFFFF, true);
        checkMinMax(mRelease, (float)0, (float)0xFFFF, true);

        mAttackSlope = (float)0xFFFF / module_basics::scSamplerate;
        mDecaySlope = ((float)0xFFFF - mSustain) / module_basics::scSamplerate;
    }

protected:

    ///
    /// Check if the gate input is active.
    /// @return true if the gate is active.
    ///
    inline bool gate() {
        return mInputs[scGateIdx] > 0xFFFF/2;
    }

    ///
    /// Transition to another state.
    /// @param state State for next tick.
    ///
    inline void nextState(eState state) {
        mState = state;
        mTimer = 0;
    }

    ///
    /// Handle the attack state.
    ///
    inline void handleAttack() {
     if(!gate()) {
        nextState(RELEASE);
    }

    mOutputFloat += mOutputFloat;

    if(mTimer >= mAttack) {
        nextState(DECAY);
    }
    }

    ///
    /// Handle the attack state.
    ///
    inline void handleDecay() {
        if(!gate()) {
            nextState(RELEASE);
        }

        mOutputFloat -= mDecaySlope;

        if(mTimer >= mDecay) {
            nextState(SUSTAIN);
        }
    }

    ///
    /// Handle the sustain state.
    ///
    inline void handleSustain() {
        if(!gate()) {
            nextState(RELEASE);
        }
        mOutputFloat = mSustain;
    }

    ///
    /// Handle the Release state.
    ///
    inline void handleRelease() {
        if(gate()) {
            // Retrigger.
            nextState(ATTACK);
        }

        mOutputFloat -= mReleaseSlope;
 
        if(mTimer >= mRelease) {
            nextState(IDLE);
        }
    }

    ///
    /// Handle the idle state.
    ///
    inline void handleIdle() {
        if(gate()) {
            nextState(ATTACK);
        }
        mOutputFloat = 0;
    }

    float mAttack;        ///< Attack setting.
    float mAttackSlope;   ///< Attack slope in counts/tick.
    float mDecay;         ///< Decay setting.
    float mDecaySlope;    ///< Decay slope in counts/tick.
    float mSustain;       ///< Sustain setting.
    float mRelease;       ///< Release setting.
    float mReleaseSlope;  ///< Release slope in counts/tick.

    float mOutputFloat;    ///< Float representation of output.
    eState mState = IDLE;  ///< ADSR state.
    float mTimer = 0;      ///< Time keeping.
};

} // namespace stat
} // namespace modules

