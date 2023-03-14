/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef TIMER_IF_H
#define TIMER_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines a callback that will be invoked when a timer's interrupt involved.
 */
typedef int32_t (*TimerHandleCb)(void);

/**
 * @brief Gets a hardware timer.
 * This function must be called to get its device handle before operating the timer.
 * @param number Indicates a timer id.
 * @return If the operation is successful, a pointer to the timer device handle is returned.
 * @since 1.0
 */
DevHandle HwTimerOpen(const uint32_t number);

/**
 * @brief Close a hardware timer.
 * If you no longer need the timer, call this function to close it
 * @param handle Represents a pointer to the timer device handle.
 * @since 1.0
 */
void HwTimerClose(DevHandle handle);

/**
 * @brief Start a timer.
 * If you need the timer run, call this function to  start it
 * @param handle Represents a pointer to the timer device handle.
 * @return success or fail
 * @since 1.0
 */
int32_t HwTimerStart(DevHandle handle);

/**
 * @brief Stop a timer.
 * If you no longer need the timer run, call this function to  stop it
 * @param handle Represents a pointer to the timer device handle.
 * @return success or fail
 * @since 1.0
 */
int32_t HwTimerStop(DevHandle handle);

/**
 * @brief Set a period timer.
 * If you need the timer run, call this function to set timer info
 * @param handle Represents a pointer to the timer device handle.
 * @param useconds Represents the timer interval.
 * @param cb Represents the timer callback function.
 * @return success or fail
 * @since 1.0
 */
int32_t HwTimerSet(DevHandle handle, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief Set a oneshot timer.
 * If you need the timer run, call this function to set timer info
 * @param useconds Represents the timer interval.
 * @param cb Represents the timer callback function.
 * @return success or fail
 * @since 1.0
 */
int32_t HwTimerSetOnce(DevHandle handle, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief Get the timer info.
 * If you need the timer info, call this function get
 * @param handle Represents a pointer to the timer device handle.
 * @param useconds Represents the timer interval.
 * @param isPeriod Represents whether the timer call once
 * @return success or fail
 * @since 1.0
 */
int32_t HwTimerGet(DevHandle handle, uint32_t *useconds, bool *isPeriod);

/**
 * @brief Enumerates TIMER I/O commands.
 *
 * @since 1.0
 */
enum TimerIoCmd {
    TIMER_IO_OPEN = 0,     /**< Open the TIMER device. */
    TIMER_IO_CLOSE,        /**< Close the TIMER device. */
    TIMER_IO_START,        /**< Start the TIMER. */
    TIMER_IO_STOP,         /**< Stop the TIMER. */
    TIMER_IO_SET,          /**< Set the period TIMER info. */
    TIMER_IO_SETONCE,      /**< Set the once TIMER info. */
    TIMER_IO_GET,          /**< Get the TIMER info. */
};

struct TimerConfig {
    uint32_t number;
    uint32_t useconds;
    bool isPeriod;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* TIMER_IF_H */
/** @} */
