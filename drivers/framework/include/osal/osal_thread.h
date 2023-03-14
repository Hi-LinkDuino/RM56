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
 * @file osal_thread.h
 *
 * @brief Declares thread types and interfaces.
 *
 * Threads created by the OSAL module are in the detached state. When releasing threads, the service module must
 * first terminate its own threads and then {@link OsalThreadDestroy} can be called to release applied resources.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_THREAD_H
#define OSAL_THREAD_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumerates thread priorities.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    OSAL_THREAD_PRI_LOW, /**< Low priority */
    OSAL_THREAD_PRI_DEFAULT, /**< Default priority */
    OSAL_THREAD_PRI_HIGH, /**< High priority */
    OSAL_THREAD_PRI_HIGHEST /**< Highest priority */
} OSAL_THREAD_PRIORITY;

/**
 * @brief Describes thread parameters.
 *
 * @since 1.0
 * @version 1.0
 */
struct OsalThreadParam {
    char *name; /**< Thread name */
    size_t stackSize; /**< Thread stack size, which may cause thread stack overflow if it is too small */
    OSAL_THREAD_PRIORITY priority; /**< Thread priority */
};

/**
 * @brief Defines a thread callback function type.
 *
 * @since 1.0
 * @version 1.0
 */
typedef int (*OsalThreadEntry)(void *);

/**
 * @brief Describes a thread.
 *
 * @since 1.0
 * @version 1.0
 */
struct OsalThread {
    void *realThread; /**< Pointer to a created thread object */
};

/**
 * @brief Defines a thread macro.
 *
 * @since 1.0
 * @version 1.0
 */
#define OSAL_DECLARE_THREAD(thread) struct OsalThread thread

/**
 * @brief Creates a thread.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @param threadEntry Indicates the thread callback function {@link OsalThreadEntry}.
 * @param entryPara Indicates the pointer to the parameter passed to the thread callback function.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_MALLOC_FAIL | Memory allocation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadCreate(struct OsalThread *thread, OsalThreadEntry threadEntry, void *entryPara);

/**
 * @brief Binds a thread to a specified CPU.
 *
 * Call this function before {@link OsalThreadStart} and after {@link OsalThreadCreate}
 * if you need to run a created thread on a specified CPU.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @param cpuID Indicates the ID of the specified CPU.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadBind(struct OsalThread *thread, unsigned int cpuID);

/**
 * @brief Starts a thread.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @param param Indicates the pointer to the parameter used to start a thread. For details, see {@link OsalThreadParam}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to start the thread.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadStart(struct OsalThread *thread, const struct OsalThreadParam *param);

/**
 * @brief Destroys a thread.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to destroy the thread.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadDestroy(struct OsalThread *thread);

/**
 * @brief Suspends a thread.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to suspend the thread.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadSuspend(struct OsalThread *thread);

/**
 * @brief Resumes a thread.
 *
 * @param thread Indicates the pointer to the thread {@link OsalThread}.
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to resume the thread.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalThreadResume(struct OsalThread *thread);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_THREAD_H */
/** @} */
