/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "tfa9879_accessory_impl.h"
#include "audio_accessory_base.h"
#include "audio_accessory_if.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct AccessoryData g_tfa9879Data = {
    .Init = Tfa9879DeviceInit,
    .Read = AccessoryDeviceRegRead,
    .Write = AccessoryDeviceRegWrite,
};

struct AudioDaiOps g_tfa9879DaiDeviceOps = {
    .Startup = Tfa9879DaiStartup,
    .HwParams = Tfa9879DaiHwParams,
};

struct DaiData g_tfa9879DaiData = {
    .drvDaiName = "accessory_dai",
    .DaiInit = Tfa9879DaiDeviceInit,
    .ops = &g_tfa9879DaiDeviceOps,
};

/* HdfDriverEntry */
static int32_t GetServiceName(const struct HdfDeviceObject *device)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input HdfDeviceObject object is nullptr.");
        return HDF_FAILURE;
    }
    node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("get drs node is nullptr.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("drsOps or drsOps getString is null!");
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "serviceName", &g_tfa9879Data.drvAccessoryName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read serviceName failed.");
        return ret;
    }
    return HDF_SUCCESS;
}

/* HdfDriverEntry implementations */
static int32_t Tfa9879DriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    AUDIO_DRIVER_LOG_INFO("success!");
    return HDF_SUCCESS;
}

static int32_t Tfa9879DriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = AccessoryGetConfigInfo(device, &g_tfa9879Data);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get config info failed.");
        return ret;
    }

    ret = GetServiceName(device);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("GetServiceName failed.");
        return ret;
    }

    ret = AudioRegisterAccessory(device, &g_tfa9879Data, &g_tfa9879DaiData);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AudioRegisterAccessory failed.");
        return ret;
    }
    AUDIO_DRIVER_LOG_INFO("success!");
    return HDF_SUCCESS;
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_tfa9879DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "CODEC_TFA9879",
    .Bind = Tfa9879DriverBind,
    .Init = Tfa9879DriverInit,
    .Release = NULL,
};
HDF_INIT(g_tfa9879DriverEntry);
