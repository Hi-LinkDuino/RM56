/*
 * Copyright (c) 2022 Jiangsu Hoperun Software Co., Ltd.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "watchdog_if.h"
#include "watchdog_core.h"
#include "wm_regs.h"
#include "wm_cpu.h"

struct WatchdogResource {
    int32_t watchdogId;
};

struct WatchdogDevice {
    struct WatchdogResource resource;
};
    
static int g_watchdogStart;
static int g_watchdogTimeout;

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr);
static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds);
static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds);
static int32_t WatchdogDevGetStatus(struct WatchdogCntlr *watchdogCntlr, uint32_t *status);
static int32_t WatchdogDevFeed(struct WatchdogCntlr *watchdogCntlr);

struct WatchdogMethod g_WatchdogCntlrMethod = {
    .getStatus = WatchdogDevGetStatus,
    .setTimeout = WatchdogDevSetTimeout,
    .getTimeout = WatchdogDevGetTimeout,
    .start  = WatchdogDevStart,
    .stop   = NULL,  // WatchdogDevStop
    .feed   = WatchdogDevFeed,
    .getPriv = NULL, // WatchdogDevGetPriv
    .releasePriv = NULL, // WatchdogDevReleasePriv
};

static int32_t WatchdogDevStart(struct WatchdogCntlr *watchdogCntlr)
{
    tls_watchdog_start_cal_elapsed_time();
    g_watchdogStart = 1;
    return HDF_SUCCESS;
}

static int32_t WatchdogDevSetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t seconds)
{
    g_watchdogTimeout = seconds;
    tls_watchdog_init(seconds);
    return HDF_SUCCESS;
}

static int32_t WatchdogDevGetTimeout(struct WatchdogCntlr *watchdogCntlr, uint32_t *seconds)
{
    if (watchdogCntlr == NULL || seconds == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    g_watchdogTimeout = tls_watchdog_stop_cal_elapsed_time();

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
    tls_sys_clk sysclk;

    tls_sys_clk_get(&sysclk);
    if (g_watchdogStart == 1) {
        tls_sys_reset();
    }
    return HDF_SUCCESS;
}

static int InitWatchdogDevice(struct WatchdogDevice *watchdogDevice)
{
    struct WatchdogResource *resource = NULL;
    if (watchdogDevice == NULL) {
        HDF_LOGE("%s: invaild parameter\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &watchdogDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is NULL\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }

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

    return InitWatchdogDevice(watchdogDevice);
}

/* HdfDriverEntry method definitions */
static int32_t WatchdogDriverBind(struct HdfDeviceObject *device);
static int32_t WatchdogDriverInit(struct HdfDeviceObject *device);
static void WatchdogDriverRelease(struct HdfDeviceObject *device);

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

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_watchdogDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "WM_WATCHDOG_MODULE_HDF",
    .Bind = WatchdogDriverBind,
    .Init = WatchdogDriverInit,
    .Release = WatchdogDriverRelease,
};

// Initialize HdfDriverEntry
HDF_INIT(g_watchdogDriverEntry);
