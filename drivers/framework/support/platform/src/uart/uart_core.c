/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_core.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "uart_if.h"

#define HDF_LOG_TAG uart_core_c

int32_t UartHostRequest(struct UartHost *host)
{
    int32_t ret;

    if (host == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%s: ENTER %d>>>>>>>>>>>>", __func__, OsalAtomicReadWrapper(&host->atom));
    if (host->method == NULL || host->method->Init == NULL) {
        HDF_LOGE("%s: method or init is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    if (OsalAtomicIncReturn(&host->atom) > 1) {
        HDF_LOGE("%s: uart device is busy", __func__);
        OsalAtomicDec(&host->atom);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = host->method->Init(host);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host init fail", __func__);
        OsalAtomicDec(&host->atom);
        return ret;
    }
    OsalAtomicDec(&host->atom);
    HDF_LOGI("%s: EXIT %d<<<<<<<<<<", __func__, OsalAtomicReadWrapper(&host->atom));
    return HDF_SUCCESS;
}

int32_t UartHostRelease(struct UartHost *host)
{
    int32_t ret;

    if (host == NULL) {
        HDF_LOGE("%s: host or method is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (host->method == NULL || host->method->Deinit == NULL) {
        HDF_LOGE("%s: method or Deinit is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = host->method->Deinit(host);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: host deinit fail", __func__);
        return ret;
    }

    OsalAtomicDec(&host->atom);
    return HDF_SUCCESS;
}

void UartHostDestroy(struct UartHost *host)
{
    if (host == NULL) {
        return;
    }
    OsalMemFree(host);
}

struct UartHost *UartHostCreate(struct HdfDeviceObject *device)
{
    struct UartHost *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return NULL;
    }

    host = (struct UartHost *)OsalMemCalloc(sizeof(*host));
    if (host == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return NULL;
    }

    host->device = device;
    device->service = &(host->service);
    host->device->service->Dispatch = UartIoDispatch;
    OsalAtomicSet(&host->atom, 0);
    host->priv = NULL;
    host->method = NULL;
    return host;
}
