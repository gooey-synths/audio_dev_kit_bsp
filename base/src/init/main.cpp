#include "../tests/tests.hpp"
#include <cstdlib>
#include <cstdint>

int main(){
    uint8_t* test = (uint8_t*)malloc(20);
    free(test);
    test_adc_continuous_conversion();
    return 0;
}
