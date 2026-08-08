#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*InterruptFunctionPtr)();

#ifdef MCU_TARGET // Only use RAM functions when building for the MCU
#define __RAMFUNC __attribute__((section(".ramfunc")))
#else
#define __RAMFUNC
#endif

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
