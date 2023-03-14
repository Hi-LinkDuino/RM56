/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdf_device_desc.h"
#include "osal_mem.h"
#include "rtc_core.h"
#include "rtc_if.h"

#define HDF_LOG_TAG rtc_service_c

static int32_t RtcServiceIoReadTime(struct RtcHost *host, struct HdfSBuf *reply)
{
    int32_t ret;
    struct RtcTime time;

    ret = RtcHostReadTime(host, &time);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host read time fail!", __func__);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &time, sizeof(time))) {
        HDF_LOGE("%s: write buffer fail!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoWriteTime(struct RtcHost *host, struct HdfSBuf *data)
{
    int32_t ret;
    uint32_t len;
    struct RtcTime *time = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&time, &len) || sizeof(*time) != len) {
        HDF_LOGE("%s: read buffer fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostWriteTime(host, time);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host write time fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoReadAlarm(struct RtcHost *host, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t alarmIndex = 0;
    struct RtcTime time;

    if (!HdfSbufReadUint32(data, &alarmIndex)) {
        HDF_LOGE("%s: read alarmIndex fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostReadAlarm(host, (enum RtcAlarmIndex)alarmIndex, &time);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host read alarm fail!", __func__);
        return ret;
    }

    if (!HdfSbufWriteBuffer(reply, &time, sizeof(time))) {
        HDF_LOGE("%s: write buffer fail!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoWriteAlarm(struct RtcHost *host, struct HdfSBuf *data)
{
    int32_t ret;
    uint32_t len;
    uint32_t alarmIndex = 0;
    struct RtcTime *time = NULL;

    if (!HdfSbufReadUint32(data, &alarmIndex)) {
        HDF_LOGE("%s: read alarmIndex fail!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&time, &len) || sizeof(*time) != len) {
        HDF_LOGE("%s: read buffer fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostWriteAlarm(host, (enum RtcAlarmIndex)alarmIndex, time);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host write alarm fail!", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoRegisterAlarmCallback(struct RtcHost *host, struct HdfSBuf *data)
{
    (void)host;
    (void)data;
    return HDF_SUCCESS;
}

static int32_t RtcServiceIoInterruptEnable(struct RtcHost *host, struct HdfSBuf *data)
{
    int32_t ret;
    uint32_t alarmIndex = 0;
    uint8_t enable = 0;

    if (!HdfSbufReadUint32(data, &alarmIndex))  {
        HDF_LOGE("%s: read alarmIndex fail!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint8(data, &enable)) {
        HDF_LOGE("%s: read enable fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostAlarmInterruptEnable(host, (enum RtcAlarmIndex)alarmIndex, enable);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host alarm interrupt enable fail! ret :%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoGetFreq(struct RtcHost *host, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t freq = 0;

    ret = RtcHostGetFreq(host, &freq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host get freq fail! ret :%d", __func__, ret);
        return ret;
    }

    if (!HdfSbufWriteUint32(reply, freq)) {
        HDF_LOGE("%s: write freq fail!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoSetFreq(struct RtcHost *host, struct HdfSBuf *data)
{
    int32_t ret;
    uint32_t freq = 0;

    if (!HdfSbufReadUint32(data, &freq)) {
        HDF_LOGE("%s: read freq fail", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostSetFreq(host, freq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host set freq fail! ret :%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoReset(struct RtcHost *host)
{
    int32_t ret;

    ret = RtcHostReset(host);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host reset fail! ret :%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoReadReg(struct RtcHost *host, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t usrDefIndex = 0;
    uint8_t value = 0;

    if (!HdfSbufReadUint8(data, &usrDefIndex)) {
        HDF_LOGE("%s: read usrDefIndex fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostReadReg(host, usrDefIndex, &value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host read reg fail! ret :%d", __func__, ret);
        return ret;
    }

    if (!HdfSbufWriteUint8(reply, value)) {
        HDF_LOGE("%s: write value fail!", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t RtcServiceIoWriteReg(struct RtcHost *host, struct HdfSBuf *data)
{
    int32_t ret;
    uint8_t usrDefIndex = 0;
    uint8_t value = 0;

    if (!HdfSbufReadUint8(data, &usrDefIndex)) {
        HDF_LOGE("%s: read usrDefIndex fail!", __func__);
        return HDF_ERR_IO;
    }

    if (!HdfSbufReadUint8(data, &value)) {
        HDF_LOGE("%s: read value fail!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcHostWriteReg(host, usrDefIndex, value);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host write reg fail! ret :%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t RtcIoDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct RtcHost *host = NULL;

    if (client == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device == NULL) {
        HDF_LOGE("%s: client->device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device->service == NULL) {
        HDF_LOGE("%s: client->device->service is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    host = (struct RtcHost *)client->device->service;

    switch (cmd) {
        case RTC_IO_READTIME:
            return RtcServiceIoReadTime(host, reply);
        case RTC_IO_WRITETIME:
            return RtcServiceIoWriteTime(host, data);
        case RTC_IO_READALARM:
            return RtcServiceIoReadAlarm(host, data, reply);
        case RTC_IO_WRITEALARM:
            return RtcServiceIoWriteAlarm(host, data);
        case RTC_IO_REGISTERALARMCALLBACK:
            return RtcServiceIoRegisterAlarmCallback(host, data);
        case RTC_IO_ALARMINTERRUPTENABLE:
            return RtcServiceIoInterruptEnable(host, data);
        case RTC_IO_GETFREQ:
            return RtcServiceIoGetFreq(host, reply);
        case RTC_IO_SETFREQ:
            return RtcServiceIoSetFreq(host, data);
        case RTC_IO_RESET:
            return RtcServiceIoReset(host);
        case RTC_IO_READREG:
            return RtcServiceIoReadReg(host, data, reply);
        case RTC_IO_WRITEREG:
            return RtcServiceIoWriteReg(host, data);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
}