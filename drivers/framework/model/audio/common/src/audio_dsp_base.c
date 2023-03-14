/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_dsp_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

int32_t DspGetServiceName(const struct HdfDeviceObject *device, const char **drvDspName)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL || drvDspName == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("device property is NULL.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("from resource get drsops failed!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "serviceName", drvDspName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read DspServiceName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t DspGetDaiName(const struct HdfDeviceObject *device, const char **drvDaiName)
{
    const struct DeviceResourceNode *node = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (device == NULL || drvDaiName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is null pointer.");
        return HDF_FAILURE;
    }

    node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs node is null pointer.");
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs ops fail!");
        return HDF_FAILURE;
    }

    ret = drsOps->GetString(node, "dspDaiName", drvDaiName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read dspDaiName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}
