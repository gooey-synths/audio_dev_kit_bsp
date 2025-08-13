#include "util.h"

///
/// Set a handler in the vector table
/// @param index Index of vector to set
/// @param handler pointer to interrupt handler function
///
void set_vector_table_entry(int index, InterruptFunctionPtr handler){
	if(index < 0 || index > 166){
		return;
	}
	vector_table_ram[index] = (uint32_t*)handler;
}
