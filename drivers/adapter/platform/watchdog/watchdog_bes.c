/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "watchdog_bes.h"
#include <stdlib.h>
#include <stdio.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "watchdog_if.h"

static int g_watchdogStart;
static int g_watchdogTimeout;

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr);
static int32_t WatchdogDevStop(struct WatchdogCntlr *watchdogCntlr);
static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds);
static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds);
static int32_t WatchdogDevGetStatus(struct WatchdogCntlr *watchdogCntlr, uint32_t *status);
static int32_t WatchdogDevFeed(struct WatchdogCntlr *watchdogCntlr);

struct WatchdogMethod g_WatchdogCntlrMethod = {
    .getStatus = WatchdogDevGetStatus,
    .setTimeout = WatchdogDevSetTimeout,
    .getTimeout = WatchdogDevGetTimeout,
    .start  = WatchdogDevStart,
    .stop   = WatchdogDevStop,
    .feed   = WatchdogDevFeed,
    .getPriv = NULL, // WatchdogDevGetPriv
    .releasePriv = NULL, // WatchdogDevReleasePriv
};

static void WatchdogIrqHandler(enum HAL_WDT_ID_T id, enum HAL_WDT_EVENT_T event)
{
    HDF_LOGD("%s: id %d event %d\r\n", __func__, id, event);
}

static int InitWatchdogDevice(struct WatchdogDevice *watchdogDevice)
{
    struct WatchdogResource *resource = NULL;
    int32_t watchdogId;
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: invaild parameter\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &watchdogDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    watchdogId = resource->watchdogId;
    hal_wdt_set_irq_callback(watchdogId, WatchdogIrqHandler);
    return HDF_SUCCESS;
}

static uint32_t GetWatchdogDeviceResource(
    struct WatchdogDevice *device, const struct DeviceResourceNode *resourceNode)
{
    struct WatchdogResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (device == NULL || resourceNode == NULL) {
        HDF_LOGE("resource or device is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &device->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (dri->GetUint32(resourceNode, "watchdogId", &resource->watchdogId, 0) != HDF_SUCCESS) {
        HDF_LOGE("read watchdogId fail\r\n");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t AttachWatchdogDevice(struct WatchdogCntlr *watchdogCntlr, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct WatchdogDevice *watchdogDevice = NULL;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: param is NULL\r\n", __func__);
        return HDF_FAILURE;
    }

    watchdogDevice = (struct WatchdogDevice *)OsalMemAlloc(sizeof(struct WatchdogDevice));
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc watchdogDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetWatchdogDeviceResource(watchdogDevice, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(watchdogDevice);
        return HDF_FAILURE;
    }

    watchdogCntlr->priv = watchdogDevice;
    watchdogCntlr->wdtId = watchdogDevice->resource.watchdogId;

    return InitWatchdogDevice(watchdogDevice);
}
/* HdfDriverEntry method definitions */
static int32_t WatchdogDriverBind(struct HdfDeviceObject *device);
static int32_t WatchdogDriverInit(struct HdfDeviceObject *device);
static void WatchdogDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_watchdogDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BES_WATCHDOG_MODULE_HDF",
    .Bind = WatchdogDriverBind,
    .Init = WatchdogDriverInit,
    .Release = WatchdogDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_watchdogDriverEntry);

static int32_t WatchdogDriverBind(struct HdfDeviceObject *device)
{
    struct WatchdogCntlr *watchdogCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("hdfDevice object is null!\r\n");
        return HDF_FAILURE;
    }

    watchdogCntlr = (struct WatchdogCntlr *)OsalMemAlloc(sizeof(struct WatchdogCntlr));
    if (watchdogCntlr == NULL) {
        HDF_LOGE("%s: OsalMemAlloc watchdogCntlr error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    HDF_LOGI("Enter %s\r\n", __func__);
    device->service = &watchdogCntlr->service;
    watchdogCntlr->device = device;
    watchdogCntlr->priv = NULL;
    return HDF_SUCCESS;
}

static int32_t WatchdogDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct WatchdogCntlr *watchdogCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    HDF_LOGI("Enter %s:\r\n", __func__);

    watchdogCntlr = WatchdogCntlrFromDevice(device);
    if (watchdogCntlr == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = AttachWatchdogDevice(watchdogCntlr, device);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(watchdogCntlr);
        HDF_LOGE("%s:attach error\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    watchdogCntlr->ops = &g_WatchdogCntlrMethod;
  
    HDF_LOGE("WatchdogDriverInit success!\r\n");
    return ret;
}

static void WatchdogDriverRelease(struct HdfDeviceObject *device)
{
    struct WatchdogCntlr *watchdogCntlr = NULL;
    struct WatchdogDevice *watchdogDevice = NULL;

    if (device == NULL) {
        HDF_LOGE("device is null\r\n");
        return;
    }

    watchdogCntlr = WatchdogCntlrFromDevice(device);
    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    if (watchdogDevice != NULL) {
        OsalMemFree(watchdogDevice);
    }
    return;
}

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr)
{
    struct WatchdogDevice *watchdogDevice = NULL;
    int32_t watchdogId;
    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: OBJECT is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    watchdogId = watchdogDevice->resource.watchdogId;

    hal_wdt_start(watchdogId);
    g_watchdogStart = 1;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevStop(struct WatchdogCntlr *watchdogCntlr)
{
    int32_t watchdogId;
    struct WatchdogDevice *watchdogDevice = NULL;

    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_FAILURE;
    }

    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: OBJECT is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    watchdogId = watchdogDevice->resource.watchdogId;
    hal_wdt_stop(watchdogId);
    g_watchdogStart = 0;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds)
{
    int32_t watchdogId;
    struct WatchdogDevice *watchdogDevice = NULL;
    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    g_watchdogTimeout = seconds;
    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: OBJECT is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    watchdogId = watchdogDevice->resource.watchdogId;
    hal_wdt_set_timeout(watchdogId, seconds);
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds)
{
    if (watchdogCntlr == NULL || seconds == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    *seconds = g_watchdogTimeout;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetStatus(struct WatchdogCntlr *watchdogCntlr, uint32_t *status)
{
    if (watchdogCntlr == NULL || status == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (g_watchdogStart == 1) {
        *status = WATCHDOG_START;
    } else {
        *status = WATCHDOG_STOP;
    }
    return HDF_SUCCESS;
}

static int32_t WatchdogDevFeed(struct WatchdogCntlr *watchdogCntlr)
{
    struct WatchdogDevice *watchdogDevice = NULL;

    if (watchdogCntlr == NULL || watchdogCntlr->priv == NULL) {
        HDF_LOGE("%s: watchdogCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    watchdogDevice = (struct WatchdogDevice *)watchdogCntlr->priv;
    int32_t watchdogId = watchdogDevice->resource.watchdogId;
    hal_wdt_ping(watchdogId);
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetPriv(struct WatchdogCntlr *watchdogCntlr)
{
    (void)watchdogCntlr;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevReleasePriv(struct WatchdogCntlr *watchdogCntlr)
{
    (void)watchdogCntlr;
    return HDF_SUCCESS;
}
