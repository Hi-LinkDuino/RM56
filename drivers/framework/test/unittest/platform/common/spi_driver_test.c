/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "osal_mem.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "spi_test.h"

#define HDF_LOG_TAG spi_test_driver_c

static struct SpiTestConfig g_config;

static int32_t SpiTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGD("%s: enter!", __func__);
    if (cmd != 0) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (reply == NULL) {
        HDF_LOGE("%s: reply is null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGE("%s: sizeof(g_config): %d, len: %d", __func__, sizeof(g_config), g_config.len);
    if (!HdfSbufWriteBuffer(reply, &g_config, sizeof(g_config))) {
        HDF_LOGE("%s: write config failed", __func__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteBuffer(reply, g_config.wbuf, g_config.len)) {
        HDF_LOGE("%s: write config failed", __func__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t SpiTestInitFromHcs(struct SpiTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *face = NULL;

    face = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (face == NULL) {
        HDF_LOGE("%s: face is null", __func__);
        return HDF_FAILURE;
    }
    if (face->GetUint32 == NULL || face->GetUint8Array == NULL) {
        HDF_LOGE("%s: GetUint32 or GetUint32Array not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = face->GetUint32(node, "bus", &config->bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail", __func__);
        return HDF_FAILURE;
    }
    ret = face->GetUint32(node, "cs", &config->cs, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read cs fail", __func__);
        return HDF_FAILURE;
    }
    ret = face->GetUint32(node, "len", &config->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read len fail", __func__);
        return HDF_FAILURE;
    }
    config->wbuf = (uint8_t *)OsalMemCalloc(config->len);
    if (config->wbuf == NULL) {
        HDF_LOGE("%s: wbuf OsalMemCalloc error\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = face->GetUint8Array(node, "wbuf", g_config.wbuf, config->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read wbuf fail", __func__);
        OsalMemFree(config->wbuf);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SpiTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct IDeviceIoService *service = NULL;

    service = (struct IDeviceIoService *)OsalMemCalloc(sizeof(*service));
    if (service == NULL) {
        HDF_LOGE("%s: malloc service failed!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = SpiTestInitFromHcs(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service->Dispatch = SpiTestDispatch;
    device->service = service;

    return HDF_SUCCESS;
}

static int32_t SpiTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void SpiTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL && device != NULL) {
        OsalMemFree(device->service);
        device->service = NULL;
    }
    return;
}

struct HdfDriverEntry g_spiTestEntry = {
    .moduleVersion = 1,
    .Bind = SpiTestBind,
    .Init = SpiTestInit,
    .Release = SpiTestRelease,
    .moduleName = "PLATFORM_SPI_TEST",
};
HDF_INIT(g_spiTestEntry);
