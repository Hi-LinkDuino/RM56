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
 * @file osal_sem.h
 *
 * @brief Declares semaphore structures and interfaces.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_SEM_H
#define OSAL_SEM_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OSAL_WAIT_FOREVER 0xFFFFFFFF

/**
 * @brief Describes a semaphore.
 */
struct OsalSem {
    void *realSemaphore; /**< Pointer to a semaphore to operate */
};

/**
 * @brief Defines a semaphore.
 */
#define OSAL_DECLARE_SEMAPHORE(sem) struct OsalSem sem

/**
 * @brief Initializes a semaphore.
 *
 * @param sem Indicates the pointer to the semaphore {@link OsalSem}.
 * @param value Indicates the initial value of the semaphore.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to initialize the semaphore.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSemInit(struct OsalSem *sem, uint32_t value);

/**
 * @brief Waits for a semaphore.
 *
 * @param sem Indicates the pointer to the semaphore {@link OsalSem}.
 * @param ms Indicates the timeout interval.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to wait for the semaphore.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_TIMEOUT | Timeout occurs.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSemWait(struct OsalSem *sem, uint32_t ms);

/**
 * @brief Releases a semaphore.
 *
 * @param sem Indicates the pointer to the semaphore {@link OsalSem}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to release the semaphore.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSemPost(struct OsalSem *sem);

/**
 * @brief Destroys a semaphore.
 *
 * @param sem Indicates the pointer to the semaphore {@link OsalSem}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to destroy the semaphore.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalSemDestroy(struct OsalSem *sem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_SEM_H */
/** @} */
