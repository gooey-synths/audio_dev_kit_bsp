#ifndef BOARD_INTERFACE_HPP
#define BOARD_INTERFACE_HPP

#include <cstdint>
#include <cstddef>

/// @todo Serial IO, File IO, filter engine?
namespace board {

/// Data type for data coming to and from the board.
typedef uint16_t BoardData;

/// Callback function typedef.
typedef void (*CallbackFunc)();

///
/// Enumeration for IO speed.
/// @todo Decide if this is an appropriate separation.
///
enum IOSpeed : uint8_t {
    SLOW, ///< Slow devices not meant to update at audio frequencies.
    FAST ///< Fast devices meant to update at audio frequencies.
};

///
/// Input/Output device base class
///
class IO {
public:
    ///
    /// Get the speed of the IO device.
    /// @return The speed of the IO device.
    ///
    IOSpeed GetSpeed() const { return mSpeed; }

protected:
    IOSpeed mSpeed; ///< Device speed.
};

///
/// Input class for input devices.
/// @tparam DataType Data type the input returns.
///
template <typename DataType>
class Input : public IO {
public:
    ///
    /// Get the value of the input.
    /// @return The value of the input.
    ///
    virtual DataType GetValue() const = 0;
};

///
/// Output class for output devices.
/// @tparam data Data type the output accepts.
///
template <typename DataType>
class Output : public IO {
public:
    ///
    /// Set the value of the output.
    /// @param val The desired value of the output.
    ///
    virtual void SetValue(DataType data) = 0;
};

///
/// Analog input class
///
class AnalogInput : public Input<BoardData> {};

///
/// Analog output class
///
class AnalogOutput : public Output<BoardData> {};

///
/// Digital input class
///
class DigitalInput : public Input<bool> {};

///
/// Digital output class
///
class DigitalOutput : public Output<bool> {};

///
/// Timer class
///
class Timer {
public:
    ///
    /// Set the frequency of the timer.
    /// @param frequency Frequency of the timer.
    ///
    virtual void SetFrequency(float frequency) = 0;

    ///
    /// Set the callback function to execute.
    /// @param callback Callback function to execute. Null for no callback.
    ///
    virtual void SetCallback(CallbackFunc callback) = 0;

    ///
    /// Start
    ///
    virtual void Start() = 0;

    ///
    /// Stop
    ///
    virtual void Stop() = 0;
};

///
/// Structure defining the IO configuration.
///
struct IOConfig {
    size_t numAnalogInputs; ///< Number of analog inputs the board provides.
    size_t numAnalogOutputs; ///< Number of analog outputs the board provides.
    size_t numDigitalInputs; ///< Number of digital inputs the board provides.
    size_t numDigitalOutputs; ///< Number of digital outputs the board provides.
};

///
/// Structure defining the board configuration.
///
struct BoardConfig {
    IOConfig slowIO;  ///< Slow IO configuration.
    IOConfig fastIO;  ///< Fast IO configuration.
    size_t numTimers; ///< Number of timers the board provides.
};

///
/// Interface for a board, provides access to hardware on that board.
///
class BoardInterface {
public:
    ///
    /// Get the board configuration.
    /// @return The board configuration.
    ///
    virtual BoardConfig GetBoardConfig() const = 0;

    ///
    /// Get a reference to an analog input on the board.
    /// @param speed Speed of the analog input to get a reference to.
    /// @param idx Index of the analog input to get.
    /// @return A reference to an analog input on the board.
    ///
    virtual AnalogInput& GetAnalogInput(IOSpeed speed, size_t idx) const = 0;
    
    ///
    /// Get a reference to an analog output on the board.
    /// @param speed Speed of the analog output to get a reference to.
    /// @param idx Index of the analog output to get.
    /// @return A reference to an analog output on the board.
    ///
    virtual AnalogOutput& GetAnalogOutput(IOSpeed speed, size_t idx) const = 0;

    ///
    /// Get a reference to an digital input on the board.
    /// @param speed Speed of the digital input to get a reference to.
    /// @param idx Index of the digital input to get.
    /// @return A reference to an digital input on the board.
    ///
    virtual DigitalInput& GetDigitalInput(IOSpeed speed, size_t idx) const = 0;
    
    ///
    /// Get a reference to an digital output on the board.
    /// @param speed Speed of the digital output to get a reference to.
    /// @param idx Index of the digital output to get.
    /// @return A reference to an digital output on the board.
    ///
    virtual DigitalOutput& GetDigitalOutput(IOSpeed speed, size_t idx) const = 0;

    ///
    /// Get a timer from the board.
    /// @param idx Index of the timer to get.
    /// @return A reference to a timer on the board.
    ///
    virtual Timer& GetTimer(size_t idx) const = 0;
};

} // namespace board

#endif // BOARD_INTERFACE_HPP