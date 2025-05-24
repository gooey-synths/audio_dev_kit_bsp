#ifndef ADC_INTERFACE_HPP
#define ADC_INTERFACE_HPP

#include <cstdint>

namespace adc {

///
/// Interface for the on chip ADC
///
class IOnChipADC {
  public:

    virtual void stop() = 0;

    virtual void setSequence(uint8_t *sequence, uint8_t len) = 0;

    virtual void beginConversion(bool continuous) = 0;

    virtual uint16_t getConversion(uint8_t convIdx) = 0;

};

} // namespace adc
#endif // ADC_INTERFACE_HPP