#if !defined(FIXED_H)
#define FIXED_H

#if defined(__arm__)
#include "arm/m533_arm.h"
#endif

#ifndef FUNCTION_smmul
#define FUNCTION_smmul
static inline int smmul(const int a, const int b) {
    return (int)(((long long)a * b) >> 31);
}
#endif

#endif