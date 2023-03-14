/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef TIMER_CORE_H
#define TIMER_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mutex.h"
#include "timer_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return ret", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

#define CHECK_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if ((ret) != HDF_SUCCESS) { \
        HDF_LOGE("%s:line %d %s fail, ret = %d!", __func__, __LINE__, (str), (ret)); \
        return HDF_FAILURE; \
    } \
} while (0)

struct TimerInfo {
    uint32_t number;
    TimerHandleCb cb;
    uint32_t useconds;
    bool isPeriod;
};

struct TimerCntrl {
    struct TimerInfo info;
    struct DListHead node;
    struct TimerCntrlMethod *ops;
    void *priv;
    struct OsalMutex lock;
};

struct TimerCntrlMethod {
    int32_t (*Remove)(struct TimerCntrl *cntrl);
    int32_t (*Open)(struct TimerCntrl *cntrl);
    int32_t (*Close)(struct TimerCntrl *cntrl);
    int32_t (*Start)(struct TimerCntrl *cntrl);
    int32_t (*Stop)(struct TimerCntrl *cntrl);
    int32_t (*Set)(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);
    int32_t (*SetOnce)(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);
    int32_t (*Get)(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod);
};

/**
 * @brief add a timer controller to manager list
 * @param cntrl Indicates a timer controller.
 * @constraints:
 * @return success or fail
 */
int32_t TimerCntrlAdd(struct TimerCntrl *cntrl);

/**
 * @brief remove a timer controller from manager list
 * @param number Indicates a timer id.
 * @constraints:
 * @return success or fail
 */
int32_t TimerCntrlRemoveByNumber(const uint32_t number);

/**
 * @brief Find and return a timer controller by number
 * @param number Indicates a timer id.
 * @return a timer controller
 */
struct TimerCntrl *TimerCntrlOpen(const uint32_t number);

/**
 * @brief close a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlClose(struct TimerCntrl *cntrl);

/**
 * @brief set a timer controller
 * @param cntrl Indicates a period timer controller.
 * @param useconds Indicates a timer timerout time us.
 * @param cb Indicates a timer callback.
 * @return success or fail
 */
int32_t TimerCntrlSet(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief set a timer controller
 * @param cntrl Indicates a timer controller.
 * @param useconds Indicates a timer timerout time us.
 * @param cb Indicates a timer callback.
 * @return success or fail
 */
int32_t TimerCntrlSetOnce(struct TimerCntrl *cntrl, uint32_t useconds, TimerHandleCb cb);

/**
 * @brief get a timer controller
 * @param cntrl Indicates a timer controller.
 * @param useconds Represents the timer interval.
 * @param isPeriod Represents whether the timer call once
 * @return success or fail
 */
int32_t TimerCntrlGet(struct TimerCntrl *cntrl, uint32_t *useconds, bool *isPeriod);

/**
 * @brief start a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlStart(struct TimerCntrl *cntrl);

/**
 * @brief stop a timer controller
 * @param cntrl Indicates a timer controller.
 * @return success or fail
 */
int32_t TimerCntrlStop(struct TimerCntrl *cntrl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* TIMER_CORE_H */
