#ifndef UTIL_FAST_MATH_H
#define UTIL_FAST_MATH_H

#include <stdint.h>
#include <string.h>

///
/// Calculate 2^n real fast-like.
/// @param n Exponent to calculate
/// @note Only valid from n=-126 to n=127
/// @note Accurate to ~1e-5 relative error, branchless (aside from clamping)
///
static inline float fast_exp2f(float n) {
    // Clamp to avoid underflow/overflow into NaN/Inf
    if (n < -126.0f) n = -126.0f;
    if (n > 127.0f)  n = 127.0f;

    float offset = (n < 0.0f) ? 1.0f : 0.0f;
    float clipp = n;
    int w = (int)clipp;
    float z = clipp - (float)w + offset;

    uint32_t bits = (uint32_t)((1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z));

    float result;
    memcpy(&result, &bits, sizeof(result)); // Strict-aliasing safe type punning
    return result;
}

#endif // UTIL_FAST_MATH_H

