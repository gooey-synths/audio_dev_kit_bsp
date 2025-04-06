#include "../tests/tests.hpp"
#include <malloc.h>
#include <cstdint>

int main() {
    //uint8_t* a = (uint8_t*)malloc(40);
    //free(a);

    try {
        throw "test";
    } catch( ... ) {
        ;
    }
    return 0;
}
