/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "timer_if.h"
#include "hdf_log.h"
#include "timer_core.h"

DevHandle HwTimerOpen(const uint32_t number)
{
    return (DevHandle)TimerCntrlOpen(number);
}

void HwTimerClose(DevHandle handle)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL) {
        HDF_LOGE("%s: cntrl is null", __func__);
        return;
    }

    if (TimerCntrlClose(cntrl) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlClose fail", __func__);
        return;
    }
}

int32_t HwTimerSet(DevHandle handle, uint32_t useconds, TimerHandleCb cb)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL || cb == NULL) {
        HDF_LOGE("%s: cntrl is null", __func__);
        return HDF_FAILURE;
    }

    if (TimerCntrlSet(cntrl, useconds, cb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlSet fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HwTimerSetOnce(DevHandle handle, uint32_t useconds, TimerHandleCb cb)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL || cb == NULL) {
        HDF_LOGE("%s: cntrl is null", __func__);
        return HDF_FAILURE;
    }

    if (TimerCntrlSetOnce(cntrl, useconds, cb) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlSetOnce fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HwTimerGet(DevHandle handle, uint32_t *useconds, bool *isPeriod)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL || useconds == NULL || isPeriod == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    if (TimerCntrlGet(cntrl, useconds, isPeriod) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlGet fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HwTimerStart(DevHandle handle)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL) {
        HDF_LOGE("%s: cntrl is null", __func__);
        return HDF_FAILURE;
    }

    if (TimerCntrlStart(cntrl) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlStart fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HwTimerStop(DevHandle handle)
{
    struct TimerCntrl *cntrl = (struct TimerCntrl *)handle;
    if (cntrl == NULL) {
        HDF_LOGE("%s: cntrl is null", __func__);
        return HDF_FAILURE;
    }

    if (TimerCntrlStop(cntrl) != HDF_SUCCESS) {
        HDF_LOGE("%s: TimerCntrlStop fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
