/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_dai_ops.h"
#include "audio_core.h"
#include "audio_dai_base.h"
#include "audio_driver_log.h"
#include "osal_io.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct AudioDaiOps g_daiDeviceOps = {
    .HwParams = DaiHwParams,
    .Trigger = DaiTrigger,
    .Startup = DaiStartup,
};

struct DaiData g_daiData = {
    .DaiInit = DaiDeviceInit,
    .Read = AudioDeviceReadReg,
    .Write = AudioDeviceWriteReg,
    .ops = &g_daiDeviceOps,
};

/* HdfDriverEntry implementations */
static int32_t DaiDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    struct DaiHost *daiHost = (struct DaiHost *)OsalMemCalloc(sizeof(*daiHost));
    if (daiHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("malloc host fail!");
        return HDF_FAILURE;
    }

    daiHost->device = device;
    device->service = &daiHost->service;
    g_daiData.daiInitFlag = false;

    AUDIO_DRIVER_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

static int32_t DaiGetServiceName(const struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }

    const struct DeviceResourceNode *node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs node is nullptr.");
        return HDF_FAILURE;
    }
    struct DeviceResourceIface *drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("invalid drs ops fail!");
        return HDF_FAILURE;
    }

    int32_t ret = drsOps->GetString(node, "serviceName", &g_daiData.drvDaiName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read serviceName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t DaiDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is nullptr.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (DaiGetConfigInfo(device, &g_daiData) !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get dai data fail.");
        return HDF_FAILURE;
    }

    if (DaiGetServiceName(device) !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get service name fail.");
        return HDF_FAILURE;
    }

    OsalMutexInit(&g_daiData.mutex);

    int32_t ret = AudioSocRegisterDai(device, &g_daiData);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("register dai fail.");
        return ret;
    }

    AUDIO_DRIVER_LOG_DEBUG("success.\n");
    return HDF_SUCCESS;
}

static void DaiDriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    OsalMutexDestroy(&g_daiData.mutex);

    struct DaiHost *daiHost = (struct DaiHost *)device->service;
    if (daiHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("daiHost is NULL");
        return;
    }
    OsalMemFree(daiHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_daiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "DAI_HI3516",
    .Bind = DaiDriverBind,
    .Init = DaiDriverInit,
    .Release = DaiDriverRelease,
};
HDF_INIT(g_daiDriverEntry);
