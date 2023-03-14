/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_if.h"
#include "securec.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG watchdog_if_u

#define WATCHDOG_ID_MAX   8
#define WATCHDOG_NAME_LEN 32

static void *WatchdogServiceGetById(int16_t wdtId)
{
    char serviceName[WATCHDOG_NAME_LEN + 1] = {0};
    void *service = NULL;

    if (wdtId < 0 || wdtId >= WATCHDOG_ID_MAX) {
        HDF_LOGE("%s: invalid id:%d", __func__, wdtId);
        return NULL;
    }

    if (snprintf_s(serviceName, WATCHDOG_NAME_LEN + 1, WATCHDOG_NAME_LEN,
        "HDF_PLATFORM_WATCHDOG_%d", wdtId) < 0) {
        HDF_LOGE("%s: format service name fail!", __func__);
        return NULL;
    }

    service = (void *)HdfIoServiceBind(serviceName);
    if (service == NULL) {
        HDF_LOGE("%s: get obj fail!", __func__);
    }

    return service;
}

int32_t WatchdogOpen(int16_t wdtId, DevHandle *handle)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    if (handle == NULL) {
        HDF_LOGE("%s: handle null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = WatchdogServiceGetById(wdtId);
    if (service == NULL) {
        HDF_LOGE("%s: get service fail", __func__);
        return HDF_FAILURE;
    }

    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        HdfIoServiceRecycle(service);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_GET_PRIV, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get priv error, ret %d", __func__, ret);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadInt32(reply, &ret)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    *handle = service;
    return ret;
}

void WatchdogClose(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;

    service = (struct HdfIoService *)handle;
    if (service == NULL ||service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        HdfIoServiceRecycle((struct HdfIoService *)handle);
        return;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_RELEASE_PRIV, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogReleasePriv error, ret %d", __func__, ret);
    }
    HdfIoServiceRecycle((struct HdfIoService *)handle);
}

int32_t WatchdogGetStatus(DevHandle handle, int32_t *status)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    if ((handle == NULL) || (status == NULL)) {
        HDF_LOGE("%s: param is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_GET_STATUS, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to read, ret %d", __func__, ret);
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadInt32(reply, status)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    HdfSbufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t WatchdogStart(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_START, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrStart error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t WatchdogStop(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;
    if (handle == NULL) {
        HDF_LOGE("%s: handle is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct HdfIoService *)handle;
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_STOP, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrStop error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfIoService *service = NULL;

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: failed to obtain data", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufWriteUint32(data, seconds)) {
        HDF_LOGE("%s: sbuf write seconds failed", __func__);
        HdfSbufRecycle(data);
        return HDF_ERR_IO;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_SET_TIMEOUT, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to set timeout, ret %d", __func__, ret);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(data);
    return HDF_SUCCESS;
}

int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;

    if ((handle == NULL) || (seconds == NULL)) {
        HDF_LOGE("%s: param is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_GET_TIMEOUT, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to read, ret %d", __func__, ret);
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadUint32(reply, seconds)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    HdfSbufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t WatchdogFeed(DevHandle handle)
{
    int32_t ret;
    struct HdfIoService *service = NULL;

    service = (struct HdfIoService *)handle;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: service is invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = service->dispatcher->Dispatch(&service->object, WATCHDOG_IO_FEED, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WatchdogCntlrFeed error, ret %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}
