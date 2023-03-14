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
 * @file osal_timer.h
 *
 * @brief Declares timer types and interfaces.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OSAL_TIMER_H
#define OSAL_TIMER_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Describes a timer.
 */
typedef struct {
    void *realTimer; /**< Pointer to a timer object */
} OsalTimer;

/**
 * @brief Describes a timer execution function type.
 */
typedef void (*OsalTimerFunc)(uintptr_t arg);

/**
 * @brief Defines a timer macro.
 */
#define OSAL_DECLARE_TIMER(timer) OsalTimer timer

/**
 * @brief Creates a timer.
 *
 * @param timer Indicates the pointer to the timer {@link OsalTimer}.
 * @param interval Indicates the timer interval.
 * @param func Indicates the timer execution function {@link OsalTimerFunc}.
 * @param arg Indicates the argument passed to the timer execution function.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 * HDF_ERR_MALLOC_FAIL | Memory allocation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalTimerCreate(OsalTimer *timer, uint32_t interval, OsalTimerFunc func, uintptr_t arg);

/**
 * @brief Deletes a timer.
 *
 * @param timer Indicates the pointer to the timer {@link OsalTimer}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to delete the timer.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalTimerDelete(OsalTimer *timer);

/**
 * @brief Starts a timer.
 *
 * @param timer Indicates the pointer to the timer {@link OsalTimer}.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to start the timer.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalTimerStartOnce(OsalTimer *timer);

/**
 * @brief Starts a periodic timer.
 *
 * @param timer Indicates the pointer to the timer {@link OsalTimer}.
 * @param interval Indicates the timer interval, in milliseconds.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function to start the timer.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalTimerStartLoop(OsalTimer *timer);

/**
 * @brief Sets the interval of a timer.
 *
 * @param timer Indicates the pointer to the timer {@link OsalTimer}.
 * @param interval Indicates the timer interval, in milliseconds.
 *
 * @return Returns a value listed below: \n
 * HDF_STATUS | Description
 * ----------------------| -----------------------
 * HDF_SUCCESS | The operation is successful.
 * HDF_FAILURE | Failed to invoke the system function.
 * HDF_ERR_INVALID_PARAM | Invalid parameter.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t OsalTimerSetTimeout(OsalTimer *timer, uint32_t interval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_TIMER_H */
/** @} */
