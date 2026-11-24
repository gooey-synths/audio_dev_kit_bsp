#include "util.h"
#include "stdint.h"
#include "assert.h"


NON_CACHEABLE
static uint8_t non_cacheable_pool[NON_CACHEABLE_POOL_SIZE];

uint8_t* pool_ptr = non_cacheable_pool;

///
/// Allocate a non-cacheable section of the memory pool
/// @note: Don't use if you don't know what you are doing!
///
void* allocate_non_cacheable(size_t size) {
    assert(pool_ptr + size <= non_cacheable_pool + NON_CACHEABLE_POOL_SIZE);
    void* ret = pool_ptr;
    pool_ptr += size;
    return ret;
}

