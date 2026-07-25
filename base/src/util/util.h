#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "../system/stm32h750xx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*InterruptFunctionPtr)();

extern uint32_t* vector_table_ram[]; // Remapped vector table defined in init.c 

void set_vector_table_entry(int index, InterruptFunctionPtr handler);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
