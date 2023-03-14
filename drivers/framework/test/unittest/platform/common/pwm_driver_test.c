/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"

#define HDF_LOG_TAG pwm_driver_test_c

static struct PwmTestConfig g_config;

static int32_t PwmTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
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
        HDF_LOGE("%s: cmd no support\n", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t PwmTestReadConfig(struct PwmTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;
    uint32_t temp;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL) {
        HDF_LOGE("%s: drsOps is null.", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: GetUint32 not support.", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = drsOps->GetUint32(node, "num", &(config->num), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read num fail.", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "period", &(config->cfg.period), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read period fail.", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "duty", &(config->cfg.duty), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read duty fail.", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "polarity", &(temp), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read polarity fail.", __func__);
        return ret;
    }
    config->cfg.polarity = temp;

    ret = drsOps->GetUint32(node, "status", &(temp), 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read status fail", __func__);
        return ret;
    }
    config->cfg.status = temp;

    return HDF_SUCCESS;
}

static int32_t PwmTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = PwmTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }
    
    service.Dispatch = PwmTestDispatch;
    device->service = &service;
    return HDF_SUCCESS;
}

static int32_t PwmTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void PwmTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    HDF_LOGI("%s: Done!", __func__);
    return;
}

struct HdfDriverEntry g_pwmTestEntry = {
    .moduleVersion = 1,
    .Bind = PwmTestBind,
    .Init = PwmTestInit,
    .Release = PwmTestRelease,
    .moduleName = "PLATFORM_PWM_TEST",
};
HDF_INIT(g_pwmTestEntry);