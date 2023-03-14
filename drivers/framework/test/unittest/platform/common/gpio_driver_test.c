/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"

static struct GpioTestConfig g_config;

static int32_t GpioTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGD("%s: enter!", __func__);

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

static int32_t GpioTestReadConfig(struct GpioTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    uint16_t tmp;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "gpio", &tmp, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read gpio fail!", __func__);
        return ret;
    }
    config->gpio = (uint16_t)tmp;

    ret = drsOps->GetUint16(node, "gpioIrq", &tmp, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read gpioIrq fail!", __func__);
        return ret;
    }
    config->gpioIrq = (uint16_t)tmp;

    ret = drsOps->GetUint16(node, "testUserApi", &tmp, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("%s: read gpioIrq fail, using 0 as default", __func__);
        config->testUserApi = 0;
    }
    config->testUserApi = (uint16_t)tmp;

    return HDF_SUCCESS;
}

static int32_t GpioTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = GpioTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service.Dispatch = GpioTestDispatch;
    device->service = &service;

    return HDF_SUCCESS;
}

static int32_t GpioTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void GpioTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    return;
}

struct HdfDriverEntry g_gpioTestEntry = {
    .moduleVersion = 1,
    .Bind = GpioTestBind,
    .Init = GpioTestInit,
    .Release = GpioTestRelease,
    .moduleName = "PLATFORM_GPIO_TEST",
};
HDF_INIT(g_gpioTestEntry);
