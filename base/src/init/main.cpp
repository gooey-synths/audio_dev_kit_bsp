#include "../tests/tests.hpp"
#include <cstdlib>
#include <cstdint>

int main() {
    test_basic_timer_exceptions();
    test_basic_timer_accuracy();
    return 0;
}
