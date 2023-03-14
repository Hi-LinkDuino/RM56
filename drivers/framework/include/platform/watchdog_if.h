/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup WATCHDOG
 * @{
 *
 * @brief Provides watchdog APIs, such as setting the watchdog timeout duration and feeding a watchdog (resetting
 * a watchdog timer).
 *
 * If an error occurs in the main program of the system, for example, if the program crashes or the watchdog timer
 * is not reset in time, the watchdog timer generates a reset signal, and the system restores from the suspending
 * state to the normal state.
 *
 * @since 1.0
 */

/**
 * @file watchdog_if.h
 *
 * @brief Declares standard watchdog APIs.
 *
 * @since 1.0
 */

#ifndef WATCHDOG_IF_H
#define WATCHDOG_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates watchdog statuses.
 *
 * To obtain the watchdog status, call the {@link WatchdogGetStatus} function.
 *
 * @since 1.0
 */
enum WatchdogStatus {
    WATCHDOG_STOP,  /**< Stopped */
    WATCHDOG_START, /**< Started */
};

/**
 * @brief Enumerates WATCHDOG I/O commands.
 *
 * @since 1.0
 */
enum WatchdogIoCmd {
    WATCHDOG_IO_GET_PRIV = 0,    /**< Get the WATCHDOG device priv. */
    WATCHDOG_IO_RELEASE_PRIV,    /**< Release the WATCHDOG device priv. */
    WATCHDOG_IO_GET_STATUS,      /**< Get status. */
    WATCHDOG_IO_START,           /**< Start. */
    WATCHDOG_IO_STOP,            /**< Stop */
    WATCHDOG_IO_SET_TIMEOUT,     /**< Set timrout. */
    WATCHDOG_IO_GET_TIMEOUT,     /**< Get timeout. */
    WATCHDOG_IO_FEED,            /**< Feed. */
};

/**
 * @brief Opens a watchdog.
 *
 * Before operating a watchdog, you must call this function to open it and obtain its device handle.
 *
 * @param wdtId Indicates the watchdog ID.
 *
 * @return Returns the pointer to the device handle of the watch dog if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
int32_t WatchdogOpen(int16_t wdtId,  DevHandle *handle);

 /**
 * @brief Closes a watchdog.
 *
 * If you no longer need a watchdog, call this function to close it and release its device handle to prevent
 * unnecessary use of memory resources.
 *
 * @param handle Indicates the pointer to the watchdog device handle.
 *
 * @since 1.0
 */
void WatchdogClose(DevHandle handle);

/**
 * @brief Obtains the watchdog status.
 *
 * For the available watchdog statuses, see {@link WatchdogStatus}.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 * @param status Indicates the pointer to the watchdog status.
 *
 * @return Returns <b>0</b> if the watchdog status is obtained; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t WatchdogGetStatus(DevHandle handle, int32_t *status);

/**
 * @brief Starts a watchdog.
 *
 * This function starts the watchdog timer. You must feed the watchdog periodically; otherwise, the watchdog hardware
 * will reset the system upon a timeout.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 *
 * @return Returns <b>0</b> if the watchdog is successfully started; returns a negative value otherwise.
 * @attention If the watchdog timer has started before this function is called, calling this function will succeed;
 * however, the watchdog hardware determines whether to reset the timer.
 *
 * @since 1.0
 */
int32_t WatchdogStart(DevHandle handle);

/**
 * @brief Stops a watchdog.
 *
 * If the watchdog has stopped before this function is called, calling this function will succeed.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 *
 * @return Returns <b>0</b> if the watchdog is successfully stopped; returns a negative value otherwise.
 * @since 1.0
 */
int32_t WatchdogStop(DevHandle handle);

/**
 * @brief Sets the watchdog timeout duration.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 * @param seconds Indicates the timeout duration, in seconds.
 *
 * @return Returns <b>0</b> if the setting is successful; returns a negative value otherwise.
 * @since 1.0
 */
int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds);

/**
 * @brief Obtains the watchdog timeout duration.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 * @param seconds Indicates the pointer to the timeout duration, in seconds.
 *
 * @return Returns <b>0</b> if the watchdog timeout duration is obtained; returns a negative value otherwise.
 * @since 1.0
 */
int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds);

/**
 * @brief Feeds a watchdog, that is, resets a watchdog timer.
 *
 * After a watchdog is started, you must feed it to reset the watchdog timer periodically.
 * If you do not do so, the watchdog hardware will reset the system upon a timeout.
 *
 * @param handle Indicates the pointer to the watchdog handle, which is obtained via {@link WatchdogOpen}.
 *
 * @return Returns <b>0</b> if the watchdog is fed; returns a negative value otherwise.
 * @since 1.0
 */
int32_t WatchdogFeed(DevHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* WATCHDOG_IF_H */
/** @} */
