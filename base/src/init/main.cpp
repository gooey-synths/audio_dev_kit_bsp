#include <stdint.h>

uint16_t b = 42;
char message[] = "Hello World!\r\n";

int main(){
    while(true){
        b++; // display b in gdb to make sure the firmware is running.
    }

}