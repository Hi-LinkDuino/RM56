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
 * @file osal_mutex.h
 *
 * @brief Declares mutex types and interfaces.
 *
 * This file provides interfaces for initializing and destroying a mutex, locking a mutex,
 * locking a mutex upon timeout, and unlocking a mutex. The mutex must be destroyed after being used.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_MUTEX_H
#define OSAL_MUTEX_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Describes a mutex.
 */
struct OsalMutex {
    void *realMutex; /**< Pointer to a mutex object to operate */
};

/**
 * @brief Defines a mutex.
 */
#define OSAL_DECLARE_MUTEX(mutex) struct OsalMutex mutex

/**
 * @brief Initializes a mutex.
 *
 * @param mutex Indicates the pointer to the mutex {@link OsalMutex}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to initialize the mutex.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalMutexInit(struct OsalMutex *mutex);

/**
 * @brief Destroys a mutex.
 *
 * @param mutex Indicates the pointer to the mutex {@link OsalMutex}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to destroy the mutex.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalMutexDestroy(struct OsalMutex *mutex);

/**
 * @brief Locks a mutex.
 *
 * @param mutex Indicates the pointer to the mutex {@link OsalMutex}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to lock the mutex.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalMutexLock(struct OsalMutex *mutex);

/**
 * @brief Locks a mutex with a specified timeout duration.
 *
 * @param mutex Indicates the pointer to the mutex {@link OsalMutex}.
 * @param ms Indicates the timeout duration, in milliseconds.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to lock the mutex.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_TIMEOUT | Timeout occurs.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalMutexTimedLock(struct OsalMutex *mutex, uint32_t ms);

/**
 * @brief Unlocks a mutex.
 *
 * @param mutex Indicates the pointer to the mutex {@link OsalMutex}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to unlock the mutex.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalMutexUnlock(struct OsalMutex *mutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_MUTEX_H */
/** @} */
