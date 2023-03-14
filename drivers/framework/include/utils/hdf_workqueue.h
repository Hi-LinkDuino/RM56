/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup DriverUtils
 * @{
 *
 * @brief Defines common macros and interfaces of the driver module.
 *
 * This module provides interfaces such as log printing, doubly linked list operations, and work queues.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file hdf_workqueue.h
 *
 * @brief Declares work queue structures and interfaces.
 *
 * This file provides interfaces such as initializing a work queue, a work item, and a delayed work item,
 * adding a work or delayed work item to a work queue, and destroying a work queue, a work item,
 * and a delayed work item. You need to define a work queue, a work item, and a delayed work item,
 * and then call the initialization function to initialize them. The work item, delayed work item,
 * and work queue must be destroyed when they are no longer used.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef HDF_WORKQUEUE_H
#define HDF_WORKQUEUE_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Describes a work execution function type.
 *
 * The thread of the work queue executes this function after the work item is added to the work queue.
 */
typedef void (*HdfWorkFunc)(void *);

/**
 * @brief Enumerates statuses of a work item or a delayed work item.
 */
enum {
    HDF_WORK_BUSY_PENDING = 1 << 0, /**< The work item or delayed work item is pending. */
    HDF_WORK_BUSY_RUNNING = 1 << 1, /**< The work item or delayed work item is running. */
};
/**
 * @brief Describes a work item and a delayed work item.
 * This structure defines the work and delayed work items, and then calls the initialization
 * function {@link HdfWorkInit} or {@link HdfDelayedWorkInit} to perform initialization.
 * The <b>HdfAddWork()</b> function is to add a work item to a work queue immediately,
 * and the <b>HdfAddDelayedWork()</b> function is to add a work item to a work queue after the configured delayed time.
 */
typedef struct {
    void *realWork; /**< Pointer to a work item and a delayed work item */
} HdfWork;

/**
 * @brief Describes a work queue.
 */
typedef struct {
    void *realWorkQueue; /**< Pointer to a work queue */
} HdfWorkQueue;

/**
 * @brief Initializes a work queue.
 *
 * When a work queue is initialized, a thread is created. The thread cyclically executes the work items
 * in the work queue, that is, executes their functions.
 *
 * @param queue Indicates the pointer to the work queue {@link OsalWorkQueue}.
 * @param name Indicates the pointer to the work queue name.
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
int32_t HdfWorkQueueInit(HdfWorkQueue *queue, char *name);

/**
 * @brief Initializes a work item.
 *
 * This function uses <b>func</b> and <b>arg</b> to initialize a work item.
 * After the work item is added to a work queue, the thread of the work queue executes this function,
 * and <b>arg</b> is passed to <b>func</b>.
 *
 * @param work Indicates the pointer to the work item {@link HdfWork}.
 * @param func Indicates the work execution function.
 * @param arg Indicates the pointer to the argument of the work execution function.
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
int32_t HdfWorkInit(HdfWork *work, HdfWorkFunc func, void *arg);

/**
 * @brief Initializes a delayed work item.
 *
 * This function uses <b>func</b> and <b>arg</b> to initialize a work item.
 * The work item is added to a work queue after the configured delayed time.
 * The thread of the work queue executes this function, and <b>arg</b> is passed to <b>func</b>.
 *
 * @param work Indicates the pointer to the delayed work item {@link HdfWork}.
 * @param func Indicates the work execution function.
 * @param arg Indicates the pointer to the argument of the work execution function.
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
int32_t HdfDelayedWorkInit(HdfWork *work, HdfWorkFunc func, void *arg);

/**
 * @brief Destroys a work item.
 *
 * @param work Indicates the pointer to the work item {@link HdfWork}.
 * @since 1.0
 * @version 1.0
 */
void HdfWorkDestroy(HdfWork *work);

/**
 * @brief Destroys a work queue.
 *
 * @param queue Indicates the pointer to the work queue {@link HdfWorkQueue}.
 * @since 1.0
 * @version 1.0
 */
void HdfWorkQueueDestroy(HdfWorkQueue *queue);

/**
 * @brief Destroys a delayed work item.
 *
 * @param work Indicates the pointer to the delayed work item {@link HdfWork}.
 * @since 1.0
 * @version 1.0
 */
void HdfDelayedWorkDestroy(HdfWork *work);

/**
 * @brief Adds a work item to a work queue.
 *
 * After a work item is added to a work queue, the thread of the work queue executes the function of the work item.
 *
 * @param queue Indicates the pointer to the work queue {@link HdfWorkQueue}.
 * @param work Indicates the pointer to the work item {@link HdfWork}.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
bool HdfAddWork(HdfWorkQueue *queue, HdfWork *work);

/**
 * @brief Adds a delayed work item to a work queue.
 *
 * A delayed work item is added to a work queue after the configured delayed time (ms),
 * and the thread of the work queue executes the work function.
 *
 * @param queue Indicates the pointer to the work queue {@link HdfWorkQueue}.
 * @param work Indicates the pointer to the delayed work item {@link HdfWork}.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
bool HdfAddDelayedWork(HdfWorkQueue *queue, HdfWork *work, uint32_t ms);

/**
 * @brief Obtains the status of a work item or delayed work item.
 *
 * @param work Indicates the pointer to the work item or delayed work item {@link HdfWork}.
 * @return Returns <b>HDF_WORK_BUSY_PENDING</b> if the work item is pending;
 *         returns <b>HDF_WORK_BUSY_RUNNING</b> if the work item is running.
 * @since 1.0
 * @version 1.0
 */
unsigned int HdfWorkBusy(HdfWork *work);

/**
 * @brief Cancels a work item. This function waits until the work item is complete.
 *
 * @param work Indicates the pointer to the work item {@link HdfWork}.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
bool HdfCancelWorkSync(HdfWork *work);

/**
 * @brief Cancels a delayed work item.
 *
 * @param work Indicates the pointer to the delayed work item {@link HdfWork}.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 * @since 1.0
 * @version 1.0
 */
bool HdfCancelDelayedWorkSync(HdfWork *work);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_WORKQUEUE_H */
/** @} */
