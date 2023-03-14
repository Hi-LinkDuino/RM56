/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "i2c_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"

static struct I2cTestConfig g_config;

static int32_t I2cTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
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

static int32_t I2cTestReadConfig(struct I2cTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "bus_num", &config->busNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus num failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "dev_addr", &config->devAddr, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read dev addr failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "reg_len", &config->regLen, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read reg len failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "reg_addr", &config->regAddr, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read reg addr failed", __func__);
        return ret;
    }

    ret = drsOps->GetUint16(node, "buf_size", &config->bufSize, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read buf size failed", __func__);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t I2cTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = I2cTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service.Dispatch = I2cTestDispatch;
    device->service = &service;

    return HDF_SUCCESS;
}

static int32_t I2cTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void I2cTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    return;
}

struct HdfDriverEntry g_i2cTestEntry = {
    .moduleVersion = 1,
    .Bind = I2cTestBind,
    .Init = I2cTestInit,
    .Release = I2cTestRelease,
    .moduleName = "PLATFORM_I2C_TEST",
};
HDF_INIT(g_i2cTestEntry);
