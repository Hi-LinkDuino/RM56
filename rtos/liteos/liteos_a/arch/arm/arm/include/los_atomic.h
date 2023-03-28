/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup los_atomic Atomic
 * @ingroup kernel
 */

#ifndef __LOS_ATOMIC_H__
#define __LOS_ATOMIC_H__

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef volatile INT32 Atomic;
typedef volatile INT64 Atomic64;

/**
 * @ingroup  los_atomic
 * @brief Atomic read.
 *
 * @par Description:
 * This API is used to implement the atomic read and return the result value of the read.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * </ul>
 *
 * @param  v         [IN] The reading pointer.
 *
 * @retval #INT32  The result value of the read.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicRead(const Atomic *v)
{
    return *(volatile INT32 *)v;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic setting.
 *
 * @par Description:
 * This API is used to implement the atomic setting operation.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * </ul>
 *
 * @param  v         [IN] The variable pointer to be setting.
 * @param  setVal    [IN] The value to be setting.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_AtomicSet(Atomic *v, INT32 setVal)
{
    *(volatile INT32 *)v = setVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic addition.
 *
 * @par Description:
 * This API is used to implement the atomic addition and return the result value of the augend.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>If the addtion result is not in the range of representable values for 32-bit signed integer,
 * an int integer overflow may occur to the return value</li>
 * </ul>
 *
 * @param  v         [IN] The augend pointer.
 * @param  addVal    [IN] The addend.
 *
 * @retval #INT32  The result value of the augend.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicAdd(Atomic *v, INT32 addVal)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %1, [%2]\n"
                             "add   %1, %1, %3\n"
                             "strex   %0, %1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(addVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic subtraction.
 *
 * @par Description:
 * This API is used to implement the atomic subtraction and return the result value of the minuend.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>If the subtraction result is not in the range of representable values for 32-bit signed integer,
 * an int integer overflow may occur to the return value</li>
 * </ul>
 *
 * @param  v         [IN] The minuend pointer.
 * @param  subVal    [IN] The subtrahend.
 *
 * @retval #INT32  The result value of the minuend.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicSub(Atomic *v, INT32 subVal)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %1, [%2]\n"
                             "sub   %1, %1, %3\n"
                             "strex   %0, %1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(subVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic addSelf.
 *
 * @par Description:
 * This API is used to implement the atomic addSelf.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT_MAX to avoid integer overflow after adding 1.</li>
 * </ul>
 *
 * @param  v      [IN] The addSelf variable pointer.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_AtomicInc(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "add   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

/**
 * @ingroup  los_atomic
 * @brief Atomic addSelf.
 *
 * @par Description:
 * This API is used to implement the atomic addSelf and return the result of addSelf.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT_MAX to avoid integer overflow after adding 1.</li>
 * </ul>
 *
 * @param  v      [IN] The addSelf variable pointer.
 *
 * @retval #INT32 The return value of variable addSelf.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicIncRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "add   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic auto-decrement.
 *
 * @par Description:
 * This API is used to implementating the atomic auto-decrement.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT_MIN to avoid overflow after reducing 1.</li>
 * </ul>
 *
 * @param  v      [IN] The auto-decrement variable pointer.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_AtomicDec(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "sub   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

/**
 * @ingroup  los_atomic
 * @brief Atomic auto-decrement.
 *
 * @par Description:
 * This API is used to implementating the atomic auto-decrement and return the result of auto-decrement.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT_MIN to avoid overflow after reducing 1.</li>
 * </ul>
 *
 * @param  v      [IN] The auto-decrement variable pointer.
 *
 * @retval #INT32  The return value of variable auto-decrement.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicDecRet(Atomic *v)
{
    INT32 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "sub   %0, %0, #1\n"
                             "strex   %1, %0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 read.
 *
 * @par Description:
 * This API is used to implement the atomic64 read and return the result value of the read.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * </ul>
 *
 * @param  v         [IN] The reading pointer.
 *
 * @retval #INT64  The result value of the read.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_Atomic64Read(const Atomic64 *v)
{
    INT64 val;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%1]"
                             : "=&r"(val)
                             : "r"(v)
                             : "cc");
    } while (0);

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 setting.
 *
 * @par Description:
 * This API is used to implement the atomic64 setting operation.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * </ul>
 *
 * @param  v         [IN] The variable pointer to be setting.
 * @param  setVal    [IN] The value to be setting.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_Atomic64Set(Atomic64 *v, INT64 setVal)
{
    INT64 tmp;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "strexd   %0, %3, %H3, [%2]"
                             : "=&r"(status), "=&r"(tmp)
                             : "r"(v), "r"(setVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 addition.
 *
 * @par Description:
 * This API is used to implement the atomic64 addition and return the result value of the augend.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>If the addtion result is not in the range of representable values for 64-bit signed integer,
 * an int integer overflow may occur to the return value</li>
 * </ul>
 *
 * @param  v         [IN] The augend pointer.
 * @param  addVal    [IN] The addend.
 *
 * @retval #INT64  The result value of the augend.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_Atomic64Add(Atomic64 *v, INT64 addVal)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "adds   %Q1, %Q1, %Q3\n"
                             "adc   %R1, %R1, %R3\n"
                             "strexd   %0, %1, %H1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(addVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 subtraction.
 *
 * @par Description:
 * This API is used to implement the atomic64 subtraction and return the result value of the minuend.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>If the subtraction result is not in the range of representable values for 64-bit signed integer,
 * an int integer overflow may occur to the return value</li>
 * </ul>
 *
 * @param  v         [IN] The minuend pointer.
 * @param  subVal    [IN] The subtrahend.
 *
 * @retval #INT64  The result value of the minuend.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_Atomic64Sub(Atomic64 *v, INT64 subVal)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %1, %H1, [%2]\n"
                             "subs   %Q1, %Q1, %Q3\n"
                             "sbc   %R1, %R1, %R3\n"
                             "strexd   %0, %1, %H1, [%2]"
                             : "=&r"(status), "=&r"(val)
                             : "r"(v), "r"(subVal)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 addSelf.
 *
 * @par Description:
 * This API is used to implement the atomic64 addSelf .
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT64_MAX to avoid integer overflow after adding 1.</li>
 * </ul>
 *
 * @param  v      [IN] The addSelf variable pointer.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_Atomic64Inc(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "adds   %Q0, %Q0, #1\n"
                             "adc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 addSelf.
 *
 * @par Description:
 * This API is used to implement the atomic64 addSelf and return the result of addSelf.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT64_MAX to avoid integer overflow after adding 1.</li>
 * </ul>
 *
 * @param  v      [IN] The addSelf variable pointer.
 *
 * @retval #INT64 The return value of variable addSelf.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_Atomic64IncRet(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "adds   %Q0, %Q0, #1\n"
                             "adc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 auto-decrement.
 *
 * @par Description:
 * This API is used to implementating the atomic64 auto-decrement.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT64_MIN to avoid overflow after reducing 1.</li>
 * </ul>
 *
 * @param  v      [IN] The auto-decrement variable pointer.
 *
 * @retval none.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE VOID LOS_Atomic64Dec(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "subs   %Q0, %Q0, #1\n"
                             "sbc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));
}

/**
 * @ingroup  los_atomic
 * @brief Atomic64 auto-decrement.
 *
 * @par Description:
 * This API is used to implementating the atomic64 auto-decrement and return the result of auto-decrement.
 * @attention
 * <ul>
 * <li>The pointer v must not be NULL.</li>
 * <li>The value which v point to must not be INT64_MIN to avoid overflow after reducing 1.</li>
 * </ul>
 *
 * @param  v      [IN] The auto-decrement variable pointer.
 *
 * @retval #INT64  The return value of variable auto-decrement.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_Atomic64DecRet(Atomic64 *v)
{
    INT64 val;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "subs   %Q0, %Q0, #1\n"
                             "sbc   %R0, %R0, #0\n"
                             "strexd   %1, %0, %H0, [%3]"
                             : "=&r"(val), "=&r"(status), "+m"(*v)
                             : "r"(v)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return val;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 8-bit variable.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 8-bit variable and
 * return the previous value of the atomic variable.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v         [IN] The variable pointer.
 * @param  val       [IN] The exchange value.
 *
 * @retval #INT32       The previous value of the atomic variable
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicXchgByte(volatile INT8 *v, INT32 val)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexb   %0, [%3]\n"
                             "strexb   %1, %4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 16-bit variable.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 16-bit variable and
 * return the previous value of the atomic variable.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v         [IN] The variable pointer.
 * @param  val       [IN] The exchange value.
 *
 * @retval #INT32       The previous value of the atomic variable
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicXchg16bits(volatile INT16 *v, INT32 val)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexh   %0, [%3]\n"
                             "strexh   %1, %4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 32-bit variable.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 32-bit variable
 * and return the previous value of the atomic variable.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v         [IN] The variable pointer.
 * @param  val       [IN] The exchange value.
 *
 * @retval #INT32       The previous value of the atomic variable
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT32 LOS_AtomicXchg32bits(Atomic *v, INT32 val)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex   %0, [%3]\n"
                             "strex   %1, %4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 64-bit variable.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 64-bit variable
 * and return the previous value of the atomic variable.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v         [IN] The variable pointer.
 * @param  val       [IN] The exchange value.
 *
 * @retval #INT64       The previous value of the atomic variable
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE INT64 LOS_AtomicXchg64bits(Atomic64 *v, INT64 val)
{
    INT64 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "strexd   %1, %4, %H4, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 8-bit variable with compare.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 8-bit variable, if the value of variable is equal to oldVal.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v           [IN] The variable pointer.
 * @param  val         [IN] The new value.
 * @param  oldVal      [IN] The old value.
 *
 * @retval TRUE  The previous value of the atomic variable is not equal to oldVal.
 * @retval FALSE The previous value of the atomic variable is equal to oldVal.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE BOOL LOS_AtomicCmpXchgByte(volatile INT8 *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexb %0, [%3]\n"
                             "mov %1, #0\n"
                             "teq %0, %4\n"
                             "strexbeq %1, %5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 16-bit variable with compare.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 16-bit variable, if the value of variable is equal to oldVal.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v           [IN] The variable pointer.
 * @param  val         [IN] The new value.
 * @param  oldVal      [IN] The old value.
 *
 * @retval TRUE  The previous value of the atomic variable is not equal to oldVal.
 * @retval FALSE The previous value of the atomic variable is equal to oldVal.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE BOOL LOS_AtomicCmpXchg16bits(volatile INT16 *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexh %0, [%3]\n"
                             "mov %1, #0\n"
                             "teq %0, %4\n"
                             "strexheq %1, %5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 32-bit variable with compare.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 32-bit variable, if the value of variable is equal to oldVal.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v           [IN] The variable pointer.
 * @param  val         [IN] The new value.
 * @param  oldVal      [IN] The old value.
 *
 * @retval TRUE  The previous value of the atomic variable is not equal to oldVal.
 * @retval FALSE The previous value of the atomic variable is equal to oldVal.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE BOOL LOS_AtomicCmpXchg32bits(Atomic *v, INT32 val, INT32 oldVal)
{
    INT32 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrex %0, [%3]\n"
                             "mov %1, #0\n"
                             "teq %0, %4\n"
                             "strexeq %1, %5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

/**
 * @ingroup  los_atomic
 * @brief Atomic exchange for 64-bit variable with compare.
 *
 * @par Description:
 * This API is used to implement the atomic exchange for 64-bit variable, if the value of variable is equal to oldVal.
 * @attention
 * <ul>The pointer v must not be NULL.</ul>
 *
 * @param  v           [IN] The variable pointer.
 * @param  val         [IN] The new value.
 * @param  oldVal      [IN] The old value.
 *
 * @retval TRUE  The previous value of the atomic variable is not equal to oldVal.
 * @retval FALSE The previous value of the atomic variable is equal to oldVal.
 * @par Dependency:
 * <ul><li>los_atomic.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
STATIC INLINE BOOL LOS_AtomicCmpXchg64bits(Atomic64 *v, INT64 val, INT64 oldVal)
{
    INT64 prevVal;
    UINT32 status;

    do {
        __asm__ __volatile__("ldrexd   %0, %H0, [%3]\n"
                             "mov   %1, #0\n"
                             "teq   %0, %4\n"
                             "teqeq   %H0, %H4\n"
                             "strexdeq   %1, %5, %H5, [%3]"
                             : "=&r"(prevVal), "=&r"(status), "+m"(*v)
                             : "r"(v), "r"(oldVal), "r"(val)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return prevVal != oldVal;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __LOS_ATOMIC_H__ */
