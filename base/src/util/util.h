#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "../system/stm32h750xx.h"

typedef void (*InterruptFunctionPtr)();

extern uint32_t* vector_table_ram[]; // Remapped vector table defined in init.c 

void set_vector_table_entry(int index, InterruptFunctionPtr handler);


#endif // UTIL_H
