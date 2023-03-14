/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "rtc_if.h"

#define HDF_LOG_TAG rtc_core_c

int32_t RtcHostReadTime(struct RtcHost *host, struct RtcTime *time)
{
    if (host == NULL || time == NULL) {
        HDF_LOGE("%s: host or time is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->ReadTime == NULL) {
        HDF_LOGE("%s: method or ReadTime is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->ReadTime(host, time);
}

int32_t RtcHostWriteTime(struct RtcHost *host, const struct RtcTime *time)
{
    if (host == NULL || time == NULL) {
        HDF_LOGE("%s: host or time is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->WriteTime == NULL) {
        HDF_LOGE("%s: method or WriteTime is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->WriteTime(host, time);
}

int32_t RtcHostReadAlarm(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, struct RtcTime *time)
{
    if (host == NULL || time == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->ReadAlarm == NULL) {
        HDF_LOGE("%s: method or ReadAlarm is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->ReadAlarm(host, alarmIndex, time);
}

int32_t RtcHostWriteAlarm(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, const struct RtcTime *time)
{
    if (host == NULL || time == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->WriteAlarm == NULL) {
        HDF_LOGE("%s: method or WriteAlarm is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->WriteAlarm(host, alarmIndex, time);
}

int32_t RtcHostRegisterAlarmCallback(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, RtcAlarmCallback cb)
{
    if (host == NULL || cb == NULL) {
        HDF_LOGE("%s: host or cb is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->RegisterAlarmCallback == NULL) {
        HDF_LOGE("%s: method or RegisterAlarmCallback is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->RegisterAlarmCallback(host, alarmIndex, cb);
}

int32_t RtcHostAlarmInterruptEnable(struct RtcHost *host, enum RtcAlarmIndex alarmIndex, uint8_t enable)
{
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->AlarmInterruptEnable == NULL) {
        HDF_LOGE("%s: method or AlarmInterruptEnable is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->AlarmInterruptEnable(host, alarmIndex, enable);
}

int32_t RtcHostGetFreq(struct RtcHost *host, uint32_t *freq)
{
    if (host == NULL || freq == NULL) {
        HDF_LOGE("%s: host or freq is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->GetFreq == NULL) {
        HDF_LOGE("%s: method or GetFreq is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->GetFreq(host, freq);
}

int32_t RtcHostSetFreq(struct RtcHost *host, uint32_t freq)
{
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->SetFreq == NULL) {
        HDF_LOGE("%s: method or SetFreq is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->SetFreq(host, freq);
}

int32_t RtcHostReset(struct RtcHost *host)
{
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->Reset == NULL) {
        HDF_LOGE("%s: method or Reset is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->Reset(host);
}

int32_t RtcHostReadReg(struct RtcHost *host, uint8_t usrDefIndex, uint8_t *value)
{
    if (host == NULL || value == NULL) {
        HDF_LOGE("%s: host or value is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->ReadReg == NULL) {
        HDF_LOGE("%s: method or ReadReg is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->ReadReg(host, usrDefIndex, value);
}

int32_t RtcHostWriteReg(struct RtcHost *host, uint8_t usrDefIndex, uint8_t value)
{
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (host->method == NULL || host->method->WriteReg == NULL) {
        HDF_LOGE("%s: method or WriteReg is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return host->method->WriteReg(host, usrDefIndex, value);
}

struct RtcHost *RtcHostCreate(struct HdfDeviceObject *device)
{
    struct RtcHost *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL!", __func__);
        return NULL;
    }

    host = (struct RtcHost *)OsalMemCalloc(sizeof(*host));
    if (host == NULL) {
        HDF_LOGE("%s: malloc host fail!", __func__);
        return NULL;
    }

    host->device = device;
    device->service = &(host->service);
    host->method = NULL;
    host->data = NULL;
    host->device->service->Dispatch = RtcIoDispatch;
    return host;
}

void RtcHostDestroy(struct RtcHost *host)
{
    if (host == NULL) {
        HDF_LOGE("%s: host is NULL!", __func__);
        return;
    }

    host->device = NULL;
    host->method = NULL;
    host->data = NULL;
    OsalMemFree(host);
}
