/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "adc_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"

#define HDF_LOG_TAG adc_test_driver_c

static struct AdcTestConfig g_config;

static int32_t AdcTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;

    if (cmd == 0) {
        if (reply == NULL) {
            HDF_LOGE("%s: reply is null!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        if (!HdfSbufWriteBuffer(reply, &g_config, sizeof(g_config))) {
            HDF_LOGE("%s: write reply failed", __func__);
            return HDF_ERR_IO;
        }
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t AdcTestReadConfig(struct AdcTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "devNum", &config->devNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read devNum failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "channel", &config->channel, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read channel failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "maxChannel", &config->maxChannel, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read maxChannel failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "dataWidth", &config->dataWidth, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read dataWidth failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "rate", &config->rate, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read rate failed", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t AdcTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = AdcTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }
    service.Dispatch = AdcTestDispatch;
    device->service = &service;
    HDF_LOGI("%s: Done!", __func__);
    return HDF_SUCCESS;
}

static int32_t AdcTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    HDF_LOGI("%s: Done!", __func__);
    return HDF_SUCCESS;
}

static void AdcTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    HDF_LOGI("%s: Done!", __func__);
    return;
}

struct HdfDriverEntry g_adcTestEntry = {
    .moduleVersion = 1,
    .Bind = AdcTestBind,
    .Init = AdcTestInit,
    .Release = AdcTestRelease,
    .moduleName = "PLATFORM_ADC_TEST",
};
HDF_INIT(g_adcTestEntry);
