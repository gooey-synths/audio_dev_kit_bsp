#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace module_basics {

///
/// Interface for interacting with a module.
/// @note Modules should not inherit directly from this class. see @ref ModuleBase
///
class ModuleInterface {
public:

    ///
    /// Destructor.
    ///
    virtual ~ModuleInterface() = 0;

    ///
    /// Check if an input index exists in a module.
    /// @param idx Index of the input to check.
    /// @return True if the input exists on the module.
    ///
    virtual bool inputExists(size_t idx) = 0;

    ///
    /// Check if an output index exists in a module.
    /// @param idx Index of the output to check.
    /// @return True if the output exists on the module.
    ///
    virtual bool outputExists(size_t idx) = 0;

    ///
    /// @brief Set the current value of the input of a module.
    /// @param idx Index of the input to set the value of.
    /// @param val Value to set the input to.
    /// @note For speed, bounds checking may not be applied.
    ///
    virtual void setInput(size_t idx, uint16_t val) noexcept = 0;

    ///
    /// Get the current value of the output of a module.
    /// @param idx Index of output to get the value of.
    /// @return The current value of the output of a module.
    /// @note For speed, bounds checking may not be applied.
    ///
    virtual uint16_t getOutput(size_t idx) noexcept = 0;

    ///
    /// Get the index of an input given the input name.
    /// @param name Name of the input to get the index of.
    /// @return The index of the input with the corresponding name.
    ///
  	virtual size_t getInputIdx(std::string name) = 0;

    ///
    /// Get the index of an output given the output name.
    /// @param name Name of the output to get the index of.
    /// @return The index of the output with the corresponding name.
    ///
  	virtual size_t getOutputIdx(std::string name) = 0;

    ///
    /// Run function, performs a single cycle of module processing, reads inputs and sets outputs.
    /// @note This function should be highly optimized for speed.
    ///
    virtual void run() noexcept = 0;

    ///
    /// Initial configuration of module. Will be called once during module instantiation.
    /// @param params User parameters for the module. Keys are parameter names while values are parameter values.
    /// @note Casting to appropriate types should happen inside this function. May throw if parameter is not present or malformed.
    ///
    virtual void configure(std::unordered_map<std::string, std::string> params) = 0;
};

///
/// Module base class.
/// @tparam tNumInupts Number of inputs the module has.
/// @tparam tNumOutputs Number of outputs the module has.
/// @note All modules should inherit from this class.
///
template<size_t tNumInupts, size_t tNumOutputs>
class ModuleBase : public ModuleInterface {
public:
    virtual bool inputExists(size_t idx) {
        return idx < tNumInupts;
    }

    virtual bool outputExists(size_t idx) {
        return idx < tNumOutputs;
    }

    virtual uint16_t getOutput(size_t idx) noexcept {
        return mOutputs[idx];
    }

    virtual void setInput(size_t idx, uint16_t val) noexcept {
        mInputs[idx] = val;
    } 

protected:
    uint16_t mInputs[tNumInupts]; ///< Inputs of the module.
    uint16_t mOutputs[tNumOutputs]; ///< Outputs of the module.
};

} // namespace module_basics