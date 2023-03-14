/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "rtc_base.h"
#include "rtc_core.h"
#include "securec.h"

#define HDF_LOG_TAG rtc_if_c

DevHandle RtcOpen()
{
    struct RtcHost *host = NULL;

    host = (struct RtcHost *)DevSvcManagerClntGetService("HDF_PLATFORM_RTC");
    if (host == NULL) {
        HDF_LOGE("rtc get service name fail");
        return NULL;
    }

    return (DevHandle)host;
}

void RtcClose(DevHandle handle)
{
    (void)handle;
}

int32_t RtcReadTime(DevHandle handle, struct RtcTime *time)
{
    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostReadTime((struct RtcHost *)handle, time);
}

int32_t RtcWriteTime(DevHandle handle, const struct RtcTime *time)
{
    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (RtcIsInvalid(time) == RTC_TRUE) {
        HDF_LOGE("%s: time invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RtcHostWriteTime((struct RtcHost *)handle, time);
}

int32_t RtcReadAlarm(DevHandle handle, enum RtcAlarmIndex alarmIndex, struct RtcTime *time)
{
    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostReadAlarm((struct RtcHost *)handle, alarmIndex, time);
}

int32_t RtcWriteAlarm(DevHandle handle, enum RtcAlarmIndex alarmIndex, const struct RtcTime *time)
{
    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (RtcIsInvalid(time) == RTC_TRUE) {
        HDF_LOGE("%s: time invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RtcHostWriteAlarm((struct RtcHost *)handle, alarmIndex, time);
}

int32_t RtcRegisterAlarmCallback(DevHandle handle, enum RtcAlarmIndex alarmIndex, RtcAlarmCallback cb)
{
    if (handle == NULL || cb == NULL) {
        HDF_LOGE("%s: handle or cb is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostRegisterAlarmCallback((struct RtcHost *)handle, alarmIndex, cb);
}

int32_t RtcAlarmInterruptEnable(DevHandle handle, enum RtcAlarmIndex alarmIndex, uint8_t enable)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostAlarmInterruptEnable((struct RtcHost *)handle, alarmIndex, enable);
}

int32_t RtcGetFreq(DevHandle handle, uint32_t *freq)
{
    if (handle == NULL || freq == NULL) {
        HDF_LOGE("%s: handle or freq is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostGetFreq((struct RtcHost *)handle, freq);
}

int32_t RtcSetFreq(DevHandle handle, uint32_t freq)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostSetFreq((struct RtcHost *)handle, freq);
}

int32_t RtcReset(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostReset((struct RtcHost *)handle);
}

int32_t RtcReadReg(DevHandle handle, uint8_t usrDefIndex, uint8_t *value)
{
    if (handle == NULL ||value == NULL) {
        HDF_LOGE("%s: handle or value is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostReadReg((struct RtcHost *)handle, usrDefIndex, value);
}

int32_t RtcWriteReg(DevHandle handle, uint8_t usrDefIndex, uint8_t value)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return RtcHostWriteReg((struct RtcHost *)handle, usrDefIndex, value);
}