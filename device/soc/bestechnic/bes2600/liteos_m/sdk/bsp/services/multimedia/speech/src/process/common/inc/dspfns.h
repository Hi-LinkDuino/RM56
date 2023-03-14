/* dspfns.h
 *
 * Copyright 2001,2014 ARM Limited. All rights reserved.
 *
 * RCS $Revision$
 * Checkin $Date$
 * Revising $Author$
 */

/* ----------------------------------------------------------------------
 * This header file provides a set of DSP-type primitive
 * operations, such as 16-bit and 32-bit saturating arithmetic. The
 * operations it provides are similar to the ones used by the ITU
 * for publishing specifications of DSP algorithms.
 */

#ifndef ARMDSP_DSPFNS_H
#define ARMDSP_DSPFNS_H
#define __ARMCLIB_VERSION 5060016

#ifdef __cplusplus
#define __STDC_LIMIT_MACROS 1
#define __STDC_FORMAT_MACROS 1
#define __STDC_CONSTANT_MACROS 1
#endif /* __cplusplus */
#include <stdint.h>
#include <assert.h>

#ifndef MAX_16
#define MAX_16 INT16_MAX
#define MIN_16 INT16_MIN
#define MAX_32 INT32_MAX
#define MIN_32 INT32_MIN
#endif /* MAX_16 etc. */

/* ARM architecture for M4 */
#ifndef __CC_ARM
#define __thumb
#define __TARGET_ARCH_ARM 0
#define __TARGET_ARCH_THUMB 4
#define __TARGET_FEATURE_DSPMUL
#define __ARM_DSP_IGNORE_OVERFLOW 0
#endif

/* Keil already include these files */
#ifndef __CC_ARM
#include "cmsis.h"
#endif

#if defined(__ICCARM__)
#include <cmsis_iar.h>
#endif

#if defined(__ICCARM__) || defined(__GNUC__)
#define __qadd8     __QADD8
#define __qsub8     __QSUB8
#define __qadd16    __QADD16
#define __qsub16    __QSUB16
#if defined(__TARGET_FEATURE_DSPMUL)
#define __qadd      __QADD
#define __qsub      __QSUB
#endif
#define __ssat		__SSAT
#define __clz		__CLZ
#endif

#if 0
#ifndef __TARGET_FEATURE_DSPMUL
#error ETSI intrinsics not currently emulated on this platform
#endif /* __TARGET_FEATURE_DSPMUL */

#if defined(__thumb) && (__TARGET_ARCH_THUMB < 4)
#error ETSI intrinsics not available on Thumb-1
#endif /* Thumb but not Thumb-2 */
#endif

#ifdef __cplusplus
//#define __forceinline __attribute__((always_inline))
#define __forceinline inline
#define __ARM_INTRINSIC static __forceinline
#elif defined(__ICCARM__)
#define __ARM_INTRINSIC _Pragma("inline=forced")
#elif defined __GNUC__ || defined _USE_STATIC_INLINE
//#define __forceinline __attribute__((always_inline))
#define __forceinline inline
#define __ARM_INTRINSIC static __forceinline
#elif (defined(__STDC_VERSION__) && 199901L <= __STDC_VERSION__)
#define __ARM_INTRINSIC __forceinline
#else
#define __ARM_INTRINSIC __forceinline
#endif

/* Define this to 1 if you do not need add() etc. to set the saturation flag */
#ifndef __ARM_DSP_IGNORE_OVERFLOW
#define __ARM_DSP_IGNORE_OVERFLOW 0
#endif

/* Define this to 1 if you believe all shift counts are in the range [-255,255] */
#ifndef __ARM_DSP_SMALL_SHIFTS
#define __ARM_DSP_SMALL_SHIFTS 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TARGET_FEATURE_DSPMUL

#ifdef __CC_ARM
#pragma recognize_itu_functions /* enable vectorization of ITU functions */
#endif
  
#if !defined(__ARM_BIG_ENDIAN) && !defined(__BIG_ENDIAN)

typedef union {
  struct {
    int _dnm:27;
    int Q:1;
    int V:1;
    int C:1;
    int Z:1;
    int N:1;
  } b;
  unsigned int word;
} _ARM_PSR;

#else /* defined(__ARM_BIG_ENDIAN) || defined(__BIG_ENDIAN) */

typedef union {
  struct {
    int N:1;
    int Z:1;
    int C:1;
    int V:1;
    int Q:1;
    int _dnm:27;
  } b;
  unsigned int word;
} _ARM_PSR;

#endif /* defined(__ARM_BIG_ENDIAN) || defined(__BIG_ENDIAN) */

#ifdef __CC_ARM
register _ARM_PSR _apsr_for_q __asm("apsr");
#else
extern _ARM_PSR _apsr_for_q;
#endif

//#define Overflow _apsr_for_q.b.Q

#ifndef __CC_ARM
/* maybe we should call this function after every asm call */
__ARM_INTRINSIC void getAPSR(void)
{
    _apsr_for_q.word = __get_APSR();
}

__ARM_INTRINSIC void __set_APSR(uint32_t apsr)
{
    __ASM volatile ("MSR APSR_nzcvq, %0" : : "r" (apsr) : "memory");
}

__ARM_INTRINSIC bool __get_Overflow(void)
{
    _apsr_for_q.word = __get_APSR();
    return _apsr_for_q.b.Q;
}

__ARM_INTRINSIC void __set_Overflow(bool flag)
{
    _apsr_for_q.word = __get_APSR();
    _apsr_for_q.b.Q = flag;
    __set_APSR(_apsr_for_q.word);
}

#if !defined(__ARMCC_VERSION)
__ARM_INTRINSIC int32_t __qdbl(int32_t x)
{
    return __qadd(x, x);
}
#endif
#endif

#else

__ARM_INTRINSIC int *_arm_global_overflow(void) {
    static int v;
    return &v;
}

//#define Overflow (*_arm_global_overflow())

__ARM_INTRINSIC bool __get_Overflow(void)
{
    return *_arm_global_overflow();
}

__ARM_INTRINSIC void __set_Overflow(bool flag)
{
    *_arm_global_overflow() = flag;
}

__ARM_INTRINSIC int32_t __qadd(int32_t x, int32_t y)
{
    int32_t r;
#if __TARGET_ARCH_ARM > 0
    int ov = 0;
    __asm {
        adds r, x, y
        movvs ov, #1
    }
    if (ov) {
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        r = y < 0 ? INT32_MIN : INT32_MAX;
    }
#else
    r = x + y;
    if (y > 0 && r < x) {
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        return INT32_MAX;
    } else if (y < 0 && r > x) { 
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        return INT32_MIN;
    }
#endif
    return r;
}

__ARM_INTRINSIC int32_t __qsub(int32_t x, int32_t y)
{
    int32_t r;
#if __TARGET_ARCH_ARM > 0
    int ov = 0;
    __asm {
        subs r, x, y
        movvs ov, #1
    }
    if (ov) {
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        r = y >= 0 ? INT32_MIN : INT32_MAX;
    }
#else
    r = x - y;
    if (y > 0 && r > x) {
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        return INT32_MIN;
    } else if (y < 0 && r < x) {
#if !__ARM_DSP_IGNORE_OVERFLOW
        __set_Overflow(1); //Overflow = 1;
#endif
        return INT32_MAX;
    }
#endif
    return r;
}

__ARM_INTRINSIC int32_t __qdbl(int32_t x)
{
    return __qadd(x, x);
}

#endif

__ARM_INTRINSIC int *_arm_global_carry(void) {
    static int c;
    return &c;
}

#define Carry (*_arm_global_carry())

/*
 * Convert a 32-bit signed integer into a 16-bit signed integer by
 * saturation.
 */
__ARM_INTRINSIC int16_t saturate(int32_t x)
{
#if (defined(__thumb) && (__TARGET_ARCH_THUMB >= 4)) || (__TARGET_ARCH_ARM >= 6)
    return (int16_t)__ssat(x, 16);
#else
    /* ARM v5E has no SSAT instruction */      
    if (x > INT16_MAX || x < INT16_MIN)
        x = __qdbl(INT32_MAX - ((x) >> 31)) >> 16;   /* Saturate and set Overflow */
    return (int16_t) x;
#endif
}

/*
 * Add two 16-bit signed integers with saturation.
 */
__ARM_INTRINSIC int16_t add(int16_t x, int16_t y)
{
#if __ARM_DSP_IGNORE_OVERFLOW && ((defined(__thumb) && (__TARGET_ARCH_THUMB >= 4)) || (__TARGET_ARCH_ARM >= 6))
    return (int16_t)__qadd16(x, y);
#else
    return (int16_t)(__qadd(x<<16, y<<16) >> 16);
#endif
}

/*
 * Subtract one 16-bit signed integer from another with saturation.
 */
__ARM_INTRINSIC int16_t sub(int16_t x, int16_t y)
{
#if __ARM_DSP_IGNORE_OVERFLOW && ((defined(__thumb) && (__TARGET_ARCH_THUMB >= 4)) || (__TARGET_ARCH_ARM >= 6))
    return (int16_t)__qsub16(x, y);
#else
    return (int16_t)(__qsub(x<<16, y<<16) >> 16);
#endif
}

/*
 * Absolute value of a 16-bit signed integer. Saturating, so
 * abs(-0x8000) becomes +0x7FFF.
 */
__ARM_INTRINSIC int16_t abs_s(int16_t x)
{
    if (x >= 0)
        return x;
#if (defined(__thumb) && (__TARGET_ARCH_THUMB >= 4)) || (__TARGET_ARCH_ARM >= 6)
    return (int16_t)__qsub16(0, x);
#else
    else if (x == INT16_MIN)
        return INT16_MAX;
    else
        return (int16_t) -x;
#endif
}

/*
 * Shift a 16-bit signed integer left (or right, if the shift count
 * is negative). Saturate on overflow.
 */
__ARM_INTRINSIC int16_t shl(int16_t x, int16_t shift)
{
    if (shift <= 0 || x == 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        if (shift < -63) shift = -63;
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return (int16_t) (x >> (-shift));
    }
    if (shift > 15)
        shift = 16;
    return saturate(x << shift);
}

/*
 * Shift a 16-bit signed integer right (or left, if the shift count
 * is negative). Saturate on overflow.
 */
__ARM_INTRINSIC int16_t shr(int16_t x, int16_t shift)
{
    if (shift >= 0 || x == 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        if (shift > 63) shift = 63;
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return (int16_t) (x >> shift);
    }
    if (shift < -15)
        shift = -16;
    return saturate(x << (-shift));
}

/*
 * Multiply two 16-bit signed integers, shift the result right by
 * 15 and saturate it. (Saturation is only necessary if both inputs
 * were -0x8000, in which case the result "should" be 0x8000 and is
 * saturated to 0x7FFF.)
 */
__ARM_INTRINSIC int16_t mult(int16_t x, int16_t y)
{
    return (int16_t)(__qdbl(x*y) >> 16);
}

/*
 * Multiply two 16-bit signed integers to give a 32-bit signed
 * integer. Shift left by one, and saturate the result. (Saturation
 * is only necessary if both inputs were -0x8000, in which case the
 * result "should" be 0x40000000 << 1 = +0x80000000, and is
 * saturated to +0x7FFFFFFF.)
 */
__ARM_INTRINSIC int32_t L_mult(int16_t x, int16_t y)
{
    return __qdbl(x*y);
}

/*
 * Negate a 16-bit signed integer, with saturation. (Saturation is
 * only necessary when the input is -0x8000.)
 */
__ARM_INTRINSIC int16_t negate(int16_t x)
{
#if (defined(__thumb) && (__TARGET_ARCH_THUMB >= 4)) || (__TARGET_ARCH_ARM >= 6)
    return (int16_t)__qsub16(0, x);
#else
    if (x == INT16_MIN)
        return INT16_MAX;
    return (int16_t) -x;
#endif
}

/*
 * Return the top 16 bits of a 32-bit signed integer.
 */
__ARM_INTRINSIC int16_t extract_h(int32_t x)
{
    return (int16_t) (x >> 16);
}

/*
 * Return the bottom 16 bits of a 32-bit signed integer, with no
 * saturation, just coerced into a two's complement 16 bit
 * representation.
 */
__ARM_INTRINSIC int16_t extract_l(int32_t x)
{
    return (int16_t) x;
}

/*
 * Divide a 32-bit signed integer by 2^16, rounding to the nearest
 * integer (round up on a tie). Equivalent to adding 0x8000 with
 * saturation, then shifting right by 16.
 */
__ARM_INTRINSIC int16_t round_etsi(int32_t x)
{
    return extract_h(__qadd(x, 0x8000));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and add to
 * another 32-bit integer with saturation.
 * 
 * Note the intermediate saturation operation in the definition:
 * 
 *    L_mac(-1, -0x8000, -0x8000)
 * 
 * will give 0x7FFFFFFE and not 0x7FFFFFFF:
 *    the unshifted product is:   0x40000000
 *    shift left with saturation: 0x7FFFFFFF
 *    add to -1 with saturation:  0x7FFFFFFE
 */
__ARM_INTRINSIC int32_t L_mac(int32_t accumulator, int16_t x, int16_t y)
{
    return __qadd(accumulator, __qdbl(x*y));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and subtract
 * from another 32-bit integer with saturation.
 * 
 * Note the intermediate saturation operation in the definition:
 * 
 *    L_msu(1, -0x8000, -0x8000)
 * 
 * will give 0x80000002 and not 0x80000001:
 *    the unshifted product is:         0x40000000
 *    shift left with saturation:       0x7FFFFFFF
 *    subtract from 1 with saturation:  0x80000002
 */
__ARM_INTRINSIC int32_t L_msu(int32_t accumulator, int16_t x, int16_t y)
{
    return __qsub(accumulator, __qdbl(x*y));
}

/*
 * Add two 32-bit signed integers with saturation.
 */
__ARM_INTRINSIC int32_t L_add(int32_t x, int32_t y)
{
    return __qadd(x, y);
}

/*
 * Subtract one 32-bit signed integer from another with saturation.
 */
__ARM_INTRINSIC int32_t L_sub(int32_t x, int32_t y)
{
    return __qsub(x, y);
}

/*
 * Add together the Carry variable and two 32-bit signed integers,
 * without saturation.
 * Note: the reference implementation has INT32_MIN + -1 + (Carry=1)
 * set the cumulative overflow flag.  This does not match intuition,
 * or the natural behavior of ARM's ADCS instruction.
 */
__ARM_INTRINSIC int32_t L_add_c(int32_t x, int32_t y)
{
    int32_t result;
#if __TARGET_ARCH_ARM > 0
    int32_t flags;
    __asm {
        movs flags, Carry, lsr #1
        adcs result, x, y;
        mrs flags, CPSR;
    }
#if !__ARM_DSP_IGNORE_OVERFLOW
    if (flags & 0x10000000) __set_Overflow(1); //Overflow = 1;  /* V -> Q */
#endif
    Carry = (flags & 0x20000000) != 0;
#else
    /* Inline assembler not available */
    result = x + y + Carry;
    Carry = (uint32_t)((x & y) | ((x | y) & ~result)) >> 31;
#if !__ARM_DSP_IGNORE_OVERFLOW
    if (((result ^ x) & (result ^ y) & 0x80000000) != 0) __set_Overflow(1); //Overflow = 1;
#endif
#endif
    return result;
}

/*
 * Subtract one 32-bit signed integer, together with the logical
 * negation of the Carry variable, from another 32-bit signed integer,
 * without saturation.
 * N.b. the computation matches that of the ETSI reference function
 * (in basicop2.c).  The comment above the ETSI reference function says
 * that L_sub_c(a,b) = a-b-C, but that does not match their code.
 */
__ARM_INTRINSIC int32_t L_sub_c(int32_t x, int32_t y)
{
    int32_t result;
#if __TARGET_ARCH_ARM > 0
    int32_t flags;
    __asm {
        movs flags, Carry, lsr #1
        sbcs result, x, y;
        mrs flags, CPSR;
    }
#if !__ARM_DSP_IGNORE_OVERFLOW
    if (flags & 0x10000000) __set_Overflow(1); //Overflow = 1;  /* V -> Q */
#endif
    Carry = (flags & 0x20000000) != 0;
#else
    /* Inline assembler not available */
    result = x + ~y + Carry;
    Carry = ((uint32_t)((x & ~y) | ((x | ~y) & ~result)) >> 31);
#if !__ARM_DSP_IGNORE_OVERFLOW
    if (((x ^ y) & (result ^ y) & 0x80000000) != 0) __set_Overflow(1); //Overflow = 1;
#endif
#endif
    return result;
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one _with_ saturation, and add
 * with carry to another 32-bit integer _without_ saturation.
 */
__ARM_INTRINSIC int32_t L_macNs(int32_t accumulator, int16_t x, int16_t y)
{
    return L_add_c(accumulator, L_mult(x, y));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one _with_ saturation, and
 * subtract with carry from another 32-bit integer _without_
 * saturation.
 */
__ARM_INTRINSIC int32_t L_msuNs(int32_t accumulator, int16_t x, int16_t y)
{
    return L_sub_c(accumulator, L_mult(x, y));
}

/*
 * Negate a 32-bit signed integer, with saturation. (Saturation is
 * only necessary when the input is -0x80000000.)
 */
__ARM_INTRINSIC int32_t L_negate(int32_t x)
{
    return __qsub(0, x);
}

/*
 * Multiply two 16-bit signed integers, shift the result right by
 * 15 with rounding, and saturate it. (Saturation is only necessary
 * if both inputs were -0x8000, in which case the result "should"
 * be 0x8000 and is saturated to 0x7FFF.)
 */
__ARM_INTRINSIC int16_t mult_r(int16_t x, int16_t y)
{
    return (int16_t)(__qdbl(x*y + 0x4000) >> 16);
}

/*
 * Return the number of bits of left shift needed to arrange for a
 * 16-bit signed integer to have value >= 0x4000 or <= -0x4001.
 * 
 * Returns 0 if x is zero (following C reference implementation).
 */
__ARM_INTRINSIC int16_t norm_s(int16_t x)
{
    return __clz(x ^ ((int32_t)x << 17)) & 15;
}

/*
 * Return the number of bits of left shift needed to arrange for a
 * 32-bit signed integer to have value >= 0x40000000 (if +ve)
 * or <= -0x40000001 (if -ve).
 * 
 * Returns 0 if x is zero (following C reference implementation).
 */
__ARM_INTRINSIC int16_t norm_l(int32_t x)
{
    return __clz(x ^ (x << 1)) & 31;
}

/*
 * Shift a 32-bit signed integer left (or right, if the shift count
 * is negative). Saturate on overflow.
 */
__ARM_INTRINSIC int32_t L_shl(int32_t x, int16_t shift)
{
    if (shift <= 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        if (shift < -63) shift = -63;
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return x >> (-shift);
    }
    if (shift <= norm_l(x) || x == 0)
        return x << shift;
    return __qdbl((x < 0) ? INT32_MIN : INT32_MAX);
}

/*
 * Shift a 32-bit signed integer right (or left, if the shift count
 * is negative). Saturate on overflow.
 */
__ARM_INTRINSIC int32_t L_shr(int32_t x, int16_t shift)
{
    if (shift >= 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        if (shift > 63) shift = 63;
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return x >> shift;
    }
    if ((-shift) <= norm_l(x) || x == 0)
        return x << (-shift);
    return __qdbl((x < 0) ? INT32_MIN : INT32_MAX);
}

/*
 * Shift a 16-bit signed integer right, with rounding. Shift left
 * with saturation if the shift count is negative.
 */
__ARM_INTRINSIC int16_t shr_r(int16_t x, int16_t shift)
{
    if (shift == 0 || x == 0)
        return (int16_t)x;
    if (shift > 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        if (shift > 32) shift = 32;
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return (int16_t) (((x >> (shift-1)) + 1) >> 1);
    }
    if (shift < -15)
        shift = -16;
    return saturate(x << (-shift));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and add to
 * another 32-bit integer with saturation (like L_mac). Then shift
 * the result right by 15 bits with rounding (like round).
 */
__ARM_INTRINSIC int16_t mac_r(int32_t accumulator, int16_t x, int16_t y)
{
    return round_etsi(L_mac(accumulator, x, y));
}

/*
 * Multiply two 16-bit signed integers together to give a 32-bit
 * signed integer, shift left by one with saturation, and subtract
 * from another 32-bit integer with saturation (like L_msu). Then
 * shift the result right by 15 bits with rounding (like round).
 */
__ARM_INTRINSIC int16_t msu_r(int32_t accumulator, int16_t x, int16_t y)
{
    return round_etsi(L_msu(accumulator, x, y));
}

/*
 * Shift a 16-bit signed integer left by 16 bits to generate a
 * 32-bit signed integer. The bottom 16 bits are zeroed.
 */
__ARM_INTRINSIC int32_t L_deposit_h(int16_t x)
{
    return ((int32_t)x) << 16;
}

/*
 * Sign-extend a 16-bit signed integer by 16 bits to generate a
 * 32-bit signed integer.
 */
__ARM_INTRINSIC int32_t L_deposit_l(int16_t x)
{
    return (int32_t)x;
}

/*
 * Shift a 32-bit signed integer right, with rounding. Shift left
 * with saturation if the shift count is negative.
 */
__ARM_INTRINSIC int32_t L_shr_r(int32_t x, int16_t shift)
{
    if (shift == 0 || x == 0)
        return x;
    if (shift > 0) {
#if !__ARM_DSP_SMALL_SHIFTS
        int32_t x2 = (shift > 32) ? 0 : x >> (shift-1);
#else
        int32_t x2 = x >> (shift-1);
#endif /* __ARM_DSP_SMALL_SHIFTS */
        return (x2 >> 1) + (x2 & 1);
    }
    if (-shift <= norm_l(x) || x == 0)
        return x << (-shift);
    return __qdbl((x < 0) ? INT32_MIN : INT32_MAX);
}

/*
 * Absolute value of a 32-bit signed integer. Saturating, so
 * abs(-0x80000000) becomes +0x7FFFFFFF.
 */
__ARM_INTRINSIC int32_t L_abs(int32_t x)
{
    if (x >= 0)
        return x;
    else
        return __qsub(0, x);
}

/*
 * Return a saturated value appropriate to the most recent carry-
 * affecting operation (L_add_c, L_macNs, L_sub_c, L_msuNs).
 * 
 * In other words: return the argument if the Q flag is clear.
 * Otherwise, return -0x80000000 or +0x7FFFFFFF depending on
 * whether the Carry flag is set or clear (respectively).
 */
__ARM_INTRINSIC int32_t L_sat(int32_t x)
{
    if (__get_Overflow()/*Overflow*/) {
        __set_Overflow(0); //Overflow = 0;
        x = (int32_t)((uint32_t)INT32_MAX + Carry);
        Carry = 0;
    }
    return x;
}

/*
 * Divide one 16-bit signed integer by another, and produce a
 * 15-bit fixed point fractional result (by multiplying the true
 * mathematical result by 0x8000). The divisor (denominator) is
 * assumed to be non-zero and also assumed to be greater or equal
 * to the dividend (numerator). Hence the (unscaled) result is
 * necessarily within the range [0,1].
 * 
 * Both operands are assumed to be positive.
 * 
 * After division, the result is saturated to fit into a 16-bit
 * signed integer. (The only way this can happen is if the operands
 * are equal, so that the result would be 1, i.e. +0x8000 in 15-bit
 * fixed point.)
 */
__ARM_INTRINSIC int16_t div_s(int16_t x, int16_t y)
{
    int32_t quot;
    assert(y > 0);
    assert(x >= 0);
    assert(x <= y);
    quot = 0x8000 * x;
    quot /= y;
    if (quot > INT16_MAX)
        return INT16_MAX;
    else
        return (int16_t)quot;
}

#ifdef __cplusplus
}
#endif

#endif /* ARMDSP_DSPFNS_H */

