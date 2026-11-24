#ifdef __cplusplus
extern "C" {
#endif

#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*InterruptFunctionPtr)();

#define NON_CACHEABLE __attribute__((section(".noncache_ram"), aligned(32)))
#define NON_CACHEABLE_POOL_SIZE 1024

void* allocate_non_cacheable(size_t size);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H

#ifdef __cplusplus
}
#endif
