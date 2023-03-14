/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_if.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "watchdog_core.h"

#define HDF_LOG_TAG watchdog_if

#define WATCHDOG_ID_MAX   8
#define WATCHDOG_NAME_LEN 32

static void *WatchdogGetById(int16_t wdtId)
{
    char *serviceName = NULL;
    void *obj = NULL;

    if (wdtId < 0 || wdtId >= WATCHDOG_ID_MAX) {
        HDF_LOGE("WatchdogGetById: invalid id:%d", wdtId);
        return NULL;
    }
    serviceName = OsalMemCalloc(WATCHDOG_NAME_LEN + 1);
    if (serviceName == NULL) {
        return NULL;
    }
    if (snprintf_s(serviceName, WATCHDOG_NAME_LEN + 1, WATCHDOG_NAME_LEN,
        "HDF_PLATFORM_WATCHDOG_%d", wdtId) < 0) {
        HDF_LOGE("WatchdogGetById: format service name fail!");
        OsalMemFree(serviceName);
        return NULL;
    }
    obj = (void *)DevSvcManagerClntGetService(serviceName);
    if (obj == NULL) {
        HDF_LOGE("WatchdogGetById: get obj fail!");
    }
    OsalMemFree(serviceName);
    return obj;
}

int32_t WatchdogOpen(int16_t wdtId, DevHandle *handle)
{
    struct Watchdog *service = NULL;
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s: handle null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = WatchdogGetById(wdtId);
    if (service == NULL) {
        *handle = NULL;
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = WatchdogGetPrivData((struct WatchdogCntlr *)service);
    if (ret == HDF_SUCCESS) {
        *handle = (DevHandle)service;
        return ret;
    }

    return ret;
}

void WatchdogClose(DevHandle handle)
{
    if (handle == NULL) {
        HDF_LOGE("%s handle null", __func__);
        return;
    }
    if (WatchdogReleasePriv((struct WatchdogCntlr *)handle) != HDF_SUCCESS) {
        HDF_LOGE("%s WatchdogReleasePriv fail", __func__);
        return;
    }
}

int32_t WatchdogGetStatus(DevHandle handle, int32_t *status)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrGetStatus((struct WatchdogCntlr *)handle, status);
}

int32_t WatchdogStart(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrStart((struct WatchdogCntlr *)handle);
}

int32_t WatchdogStop(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrStop((struct WatchdogCntlr *)handle);
}

int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrSetTimeout((struct WatchdogCntlr *)handle, seconds);
}

int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrGetTimeout((struct WatchdogCntlr *)handle, seconds);
}

int32_t WatchdogFeed(DevHandle handle)
{
    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return WatchdogCntlrFeed((struct WatchdogCntlr *)handle);
}
