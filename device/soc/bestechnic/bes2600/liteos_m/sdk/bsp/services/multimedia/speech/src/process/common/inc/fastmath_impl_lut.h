#ifndef FASTMATH_IMPL_LUT_H
#define FASTMATH_IMPL_LUT_H

#include <limits.h>
#include "ae_math.h"
#include "ae_common.h"

#define FASTMATH_LOG2_TABLE_MIN (1.f)
#define FASTMATH_LOG2_TABLE_MAX (2.f)
#define FASTMATH_LOG2_TABLE_SIZE (33)
#define FASTMATH_LOG2_TABLE_STEP ((FASTMATH_LOG2_TABLE_MAX - FASTMATH_LOG2_TABLE_MIN) / (FASTMATH_LOG2_TABLE_SIZE - 1))
#define FASTMATH_LOG2_TABLE_STEP_INV (1.f / FASTMATH_LOG2_TABLE_STEP)

#define FASTMATH_POW2_TABLE_MIN (0.f)
#define FASTMATH_POW2_TABLE_MAX (1.f)
#define FASTMATH_POW2_TABLE_SIZE (33)
#define FASTMATH_POW2_TABLE_STEP ((FASTMATH_POW2_TABLE_MAX - FASTMATH_POW2_TABLE_MIN) / (FASTMATH_POW2_TABLE_SIZE - 1))
#define FASTMATH_POW2_TABLE_STEP_INV (1.f / FASTMATH_POW2_TABLE_STEP)

extern float log2_table[FASTMATH_LOG2_TABLE_SIZE];
extern float pow2_table[FASTMATH_POW2_TABLE_SIZE];

static inline float pow2_0_1(float x)
{
    AE_ASSERT(x >= 0 && x <= 1, "input must be [0, 1], but input is %s%d.%06d", AE_SIGN(x), AE_INT(x), AE_FRAC(x));

    float i = (x - FASTMATH_POW2_TABLE_MIN) * FASTMATH_POW2_TABLE_STEP_INV;
    int index = (int)AE_FLOOR(i);
    float fract = i - index;
    float a = pow2_table[index];
    float b = pow2_table[index + 1];

    return (b - a) * fract + a;
}

static inline float fastpow2(float x)
{
    if (x >= 31)
        return (float)INT_MAX; //power(2, 31) - 1;
    if (x < -23)
        return 1.1921e-7f; // 2^(-23)

    float y;
    int a = (int)AE_FLOOR(x);
    float b = x - a;
    if (a < 0) {
        y = pow2_0_1(b) / (1 << (-a));
    } else {
        y = pow2_0_1(b) * (1 << a);
    }

    return y;
}

static inline float log2_1_2(float x)
{
    AE_ASSERT(x >= 1 && x <= 2, "input must be [1, 2], but input is %s%d.%06d", AE_SIGN(x), AE_INT(x), AE_FRAC(x));

    float i = (x - FASTMATH_LOG2_TABLE_MIN) * FASTMATH_LOG2_TABLE_STEP_INV;
    int index = (int)AE_FLOOR(i);
    float fract = i - index;
    float a = log2_table[index];
    float b = log2_table[index + 1];

    return (b - a) * fract + a;
}

static float myfrexpf(float x, int *pw2)
{
    union {float f, long l} fl;
    long int i;

    fl.f=x;
    /* Find the exponent (power of 2) */
    i  = ( fl.l >> 23) & 0x000000ff;
    i -= 0x7e;
    *pw2 = i;
    fl.l &= 0x807fffff; /* strip all exponent bits */
    fl.l |= 0x3f000000; /* mantissa between 0.5 and 1 */
    return(fl.f);
}

static inline float fastlog2(float x)
{
    float y;
    if (x < 1.1921e-7f) {
        y = -23.f; // log2(1e-7)
    } else {
        float a;
        int b;
        a = myfrexpf(x, &b);
        //TRACE(0, "[%s] x - %s%d.%06d, a - %s%d.%06d, b - %s%d.%06d", __FUNCTION__,
        //    AE_SIGN(x), AE_INT(x), AE_FRAC(x),
        //    AE_SIGN(a), AE_INT(a), AE_FRAC(a),
        //    AE_SIGN(b), AE_INT(b), AE_FRAC(b));
        y = log2_1_2(a * 2) + b - 1;
    }

    return y;
}

#endif