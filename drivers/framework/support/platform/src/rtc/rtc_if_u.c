/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_if.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG rtc_if_u_c

DevHandle RtcOpen()
{
    struct HdfIoService *service = NULL;

    service = HdfIoServiceBind("HDF_PLATFORM_RTC");
    if (service == NULL) {
        HDF_LOGE("%s: rtc service bind fail", __func__);
        return NULL;
    }

    return (DevHandle)service;
}

void RtcClose(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return;
    }

    HdfIoServiceRecycle((struct HdfIoService *)handle);
}

int32_t RtcReadTime(DevHandle handle, struct RtcTime *time)
{
    int32_t ret;
    uint32_t len = 0;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    struct RtcTime *temp = NULL;

    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: fail to obtain reply!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_READTIME, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadBuffer(reply, (const void **)&temp, &len) || temp == NULL) {
        HDF_LOGE("%s: read buffer fail", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (len != sizeof(*time)) {
        HDF_LOGE("%s: read error, len: %u, size: %zu", __func__, len, sizeof(*time));
        ret = HDF_FAILURE;
        goto EXIT;
    }

    if (memcpy_s(time, sizeof(*time), temp, len) != EOK) {
        HDF_LOGE("%s: memcpy time fail!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

EXIT:
    HdfSbufRecycle(reply);
    return ret;
}

int32_t RtcWriteTime(DevHandle handle, const struct RtcTime *time)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteBuffer(data, time, sizeof(*time))) {
        HDF_LOGE("%s: write rtc time fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_WRITETIME, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return ret;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

int32_t RtcReadAlarm(DevHandle handle, enum RtcAlarmIndex alarmIndex, struct RtcTime *time)
{
    int32_t ret;
    uint32_t len = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct RtcTime *temp = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: fail to obtain reply!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)alarmIndex)) {
        HDF_LOGE("%s: write alarmIndex fail!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_READALARM, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadBuffer(reply, (const void **)&temp, &len) || temp == NULL) {
        HDF_LOGE("%s: read buffer fail", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    if (len != sizeof(*time)) {
        HDF_LOGE("%s: read error, len: %u, size: %zu", __func__, len, sizeof(*time));
        ret = HDF_FAILURE;
        goto EXIT;
    }

    if (memcpy_s(time, sizeof(*time), temp, len) != EOK) {
        HDF_LOGE("%s: memcpy time fail!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t RtcWriteAlarm(DevHandle handle, enum RtcAlarmIndex alarmIndex, const struct RtcTime *time)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || time == NULL) {
        HDF_LOGE("%s: handle or time is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)alarmIndex)) {
        HDF_LOGE("%s: write rtc time fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteBuffer(data, time, sizeof(*time))) {
        HDF_LOGE("%s: write rtc time fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_WRITEALARM, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return ret;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

int32_t RtcRegisterAlarmCallback(DevHandle handle, enum RtcAlarmIndex alarmIndex, RtcAlarmCallback cb)
{
    (void)alarmIndex;
    if (handle == NULL || cb == NULL) {
        HDF_LOGE("%s: handle or cb is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

int32_t RtcAlarmInterruptEnable(DevHandle handle, enum RtcAlarmIndex alarmIndex, uint8_t enable)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)alarmIndex)) {
        HDF_LOGE("%s: write alarmIndex fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint8(data, enable)) {
        HDF_LOGE("%s: write enable fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_ALARMINTERRUPTENABLE, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return ret;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

int32_t RtcGetFreq(DevHandle handle, uint32_t *freq)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || freq == NULL) {
        HDF_LOGE("%s: handle or freq is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_GETFREQ, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadUint32(reply, freq)) {
        HDF_LOGE("%s: read buffer fail", __func__);
        goto EXIT;
    }

EXIT:
    HdfSbufRecycle(reply);
    return ret;
}

int32_t RtcSetFreq(DevHandle handle, uint32_t freq)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, freq)) {
        HDF_LOGE("%s: write freq fail", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_SETFREQ, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return ret;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

int32_t RtcReset(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_RESET, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: rtc reset fail, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t RtcReadReg(DevHandle handle, uint8_t usrDefIndex, uint8_t *value)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL || value == NULL) {
        HDF_LOGE("%s: handle or value is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: fail to obtain reply!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint8(data, usrDefIndex)) {
        HDF_LOGE("%s: write usrDefIndex fail!", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        ret = HDF_ERR_MALLOC_FAIL;
        goto EXIT;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_READREG, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        goto EXIT;
    }

    if (!HdfSbufReadUint8(reply, value)) {
        HDF_LOGE("%s: read value fail", __func__);
        ret = HDF_ERR_IO;
        goto EXIT;
    }

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

int32_t RtcWriteReg(DevHandle handle, uint8_t usrDefIndex, uint8_t value)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle is NULL.", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: fail to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint8(data, usrDefIndex)) {
        HDF_LOGE("%s: write usrDefIndex fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    if (!HdfSbufWriteUint8(data, value)) {
        HDF_LOGE("%s: write value fail!", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, RTC_IO_WRITEREG, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail, ret is %d", __func__, ret);
        HdfSbufRecycle(data);
        return ret;
    }

    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}
