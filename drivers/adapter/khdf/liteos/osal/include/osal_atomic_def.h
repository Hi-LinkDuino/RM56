/*
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

#ifndef OSAL_ATOMIC_DEF_H
#define OSAL_ATOMIC_DEF_H

#include <los_atomic.h>
#include <los_hwi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OsalAtomicReadWrapper(v) LOS_AtomicRead((const Atomic *)&(v)->counter)
#define OsalAtomicSetWrapper(v, value) LOS_AtomicSet((Atomic *)&(v)->counter, value)
#define OsalAtomicIncWrapper(v) LOS_AtomicInc((Atomic *)&(v)->counter)
#define OsalAtomicIncRetWrapper(v) LOS_AtomicIncRet((Atomic *)&(v)->counter)
#define OsalAtomicDecWrapper(v) LOS_AtomicDec((Atomic *)&(v)->counter)
#define OsalAtomicDecRetWrapper(v) LOS_AtomicDecRet((Atomic *)&(v)->counter)

#define OSAL_BITS_PER_LONG 32
#define OSAL_BIT_MASK(nr) (1UL << ((nr) % OSAL_BITS_PER_LONG))
#define OSAL_BIT_WORD(nr) ((nr) / OSAL_BITS_PER_LONG)
#define OSAL_BITOP_WORD(nr) ((nr) / OSAL_BITS_PER_LONG)

static inline int32_t OsalTestBitWrapper(unsigned long nr, const volatile unsigned long *addr)
{
    return (1UL & (addr[OSAL_BIT_WORD(nr)] >> (nr & (OSAL_BITS_PER_LONG - 1))));
}

static inline int32_t OsalTestSetBitWrapper(unsigned long nr, volatile unsigned long *addr)
{
    uint32_t intSave = LOS_IntLock();

    const unsigned long mask = OSAL_BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + OSAL_BIT_WORD(nr);
    unsigned long old = *p;
    *p = old | mask;

    LOS_IntRestore(intSave);
    return ((old & mask) != 0);
}

static inline int32_t OsalTestClearBitWrapper(unsigned long nr, volatile unsigned long *addr)
{
    uint32_t intSave = LOS_IntLock();

    const unsigned long mask = OSAL_BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + OSAL_BIT_WORD(nr);
    unsigned long old = *p;
    *p = old & ~mask;

    LOS_IntRestore(intSave);
    return ((old & mask) != 0);
}

static inline void OsalClearBitWrapper(unsigned long nr, volatile unsigned long *addr)
{
    uint32_t intSave = LOS_IntLock();

    const unsigned long mask = OSAL_BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + OSAL_BIT_WORD(nr);
    *p &= ~mask;

    LOS_IntRestore(intSave);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_ATOMIC_DEF_H */

