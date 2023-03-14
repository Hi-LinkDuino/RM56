/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup OSAL
 * @{
 *
 * @brief Defines the structures and interfaces for the Operating System Abstraction Layer (OSAL) module.
 *
 * The OSAL module OpenHarmony OS interface differences and provides unified OS interfaces externally,
 * including the memory management, thread, mutex, spinlock, semaphore, timer, file, interrupt, time,
 * atomic, firmware, and I/O operation modules.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file osal_atomic.h
 *
 * @brief Declares atomic and bit operation interfaces.
 *
 * This file provides interfaces such as reading and setting an atomic,
 * incrementing and decrementing an atomic counter by 1.
 * This file also provides interfaces such as checking the bit status of a variable,
 * and setting and clearing the bit value of a variable.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OSAL_ATOMIC_H
#define OSAL_ATOMIC_H

#include "osal_atomic_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Describes an atomic.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    volatile int32_t counter;/**< Counter (an atomic variable).
                               * An operation on the atomic is to operate this variable.
                               */
} OsalAtomic;

/**
 * @brief Reads the counter of an atomic.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 *
 * @return Returns the counter.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicRead(v) OsalAtomicReadWrapper(v)

/**
 * @brief Sets the counter for an atomic.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 * @param counter Indicates the counter to set.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicSet(v, counter) OsalAtomicSetWrapper(v, counter)

/**
 * @brief Increments the counter of an atomic by 1.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicInc(v) OsalAtomicIncWrapper(v)

/**
 * @brief Increments the counter of an atomic by 1 and returns the new counter.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 *
 * @return Returns the new counter.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicIncReturn(v) OsalAtomicIncRetWrapper(v)

/**
 * @brief Decrements the counter of an atomic by 1.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicDec(v) OsalAtomicDecWrapper(v)

/**
 * @brief Decrements the counter of an atomic by 1 and returns the new counter.
 *
 * @param v Indicates the pointer to the atomic {@link OsalAtomic}.
 *
 * @return Returns the new counter.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalAtomicDecReturn(v) OsalAtomicDecRetWrapper(v)

/**
 * @brief Tests the value of a specified bit of a variable.
 *
 * @param nr Indicates the bit of the variable. The value ranges from <b>0</b> to <b>31</b>.
 * @param addr Indicates the pointer to the variable.
 *
 * @return Returns the bit value.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalTestBit(nr, addr) OsalTestBitWrapper(nr, addr)

/**
 * @brief Sets the value of a specified bit of the variable and returns the bit value before the setting.
 *
 * @param nr Indicates the bit of the variable. The value ranges from <b>0</b> to <b>31</b>.
 * @param addr Indicates the pointer to the variable.
 *
 * @return Returns the bit value before the setting.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalTestSetBit(nr, addr) OsalTestSetBitWrapper(nr, addr)

/**
 * @brief Clears the value of a specified bit of the variable and returns the bit value before clearing.
 *
 * @param nr Indicates the bit of the variable. The value ranges from <b>0</b> to <b>31</b>.
 * @param addr Indicates the pointer to the variable.
 *
 * @return Returns the bit value before the bit is cleared.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalTestClearBit(nr, addr) OsalTestClearBitWrapper(nr, addr)

/**
 * @brief Clears the value of a specified bit of the variable.
 *
 * @param nr Indicates the bit of the variable. The value ranges from <b>0</b> to <b>31</b>.
 * @param addr Indicates the pointer to the variable.
 *
 * @since 1.0
 * @version 1.0
 */
#define OsalClearBit(nr, addr) OsalClearBitWrapper(nr, addr)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_ATOMIC_H */
/** @} */
