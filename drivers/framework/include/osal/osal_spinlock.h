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
 * @file osal_spinlock.h
 *
 * @brief Declares spinlock types and interfaces.
 *
 * This file provides the interfaces for initializing, destroying, obtaining, and releasing a spinlock,
 * the interfaces for obtaining a spinlock and disabling the interrupt request (IRQ), releasing a spinlock
 * and enabling the IRQ, obtaining a spinlock, disabling the IRQ, and saving its status, and releasing a spinlock,
 * enabling the IRQ, and restoring the saved IRQ status. The spinlock needs to be destroyed when it is no longer used.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_SPINLOCK_H
#define OSAL_SPINLOCK_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Describes a spinlock.
 */
typedef struct {
    void *realSpinlock; /**< Pointer to a spinlock */
} OsalSpinlock;

/**
 * @brief Defines a spinlock.
 */
#define OSAL_DECLARE_SPINLOCK(spinlock) OsalSpinlock spinlock

/**
 * @brief Initializes a spinlock.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to initialize the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_MALLOC_FAIL | Memory allocation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinInit(OsalSpinlock *spinlock);

/**
 * @brief Destroys a spinlock.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to destroy the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinDestroy(OsalSpinlock *spinlock);

/**
 * @brief Obtains a spinlock.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to obtain the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinLock(OsalSpinlock *spinlock);

/**
 * @brief Releases a spinlock.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to release the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinUnlock(OsalSpinlock *spinlock);

/**
 * @brief Obtains a spinlock and disables the IRQ.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to obtain the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinLockIrq(OsalSpinlock *spinlock);

/**
 * @brief Releases a spinlock and enables the IRQ.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to release the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinUnlockIrq(OsalSpinlock *spinlock);

/**
 * @brief Obtains a spinlock, disables the IRQ, and saves its status.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 * @param flags Indicates the pointer to the status of the IRQ register.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to obtain the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinLockIrqSave(OsalSpinlock *spinlock, uint32_t *flags);

/**
 * @brief Releases a spinlock, enables the IRQ, and restores the saved IRQ status.
 *
 * @param spinlock Indicates the pointer to the spinlock {@link OsalSpinlock}.
 * @param flags Indicates the pointer to the value used to restore the IRQ register.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to release the spinlock.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSpinUnlockIrqRestore(OsalSpinlock *spinlock, uint32_t *flags);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_SPINLOCK_H */
/** @} */
