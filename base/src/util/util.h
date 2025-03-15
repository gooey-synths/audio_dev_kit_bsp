#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "../system/stm32h750xx.h"

extern uint32_t* vector_table_ram[]; // Remapped veector table defined in init.c

///
/// Set a handler in the vector table
/// @param index Index of vector to set
/// @param handler pointer to interrupt handler function
///
void set_vector_table_entry(int index, uint32_t* handler){
	if(index < 0 || index > 166){
		return;
	}
	vector_table_ram[index] = handler;
}


#endif // UTIL_H
