/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
 
#include "uart_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"

static struct UartTestConfig g_config;

static int32_t UartTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
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
        HDF_LOGE("%s: g_config.len is %d ", __func__, g_config.len);
        if (!HdfSbufWriteBuffer(reply, g_config.wbuf, g_config.len)) {
            HDF_LOGE("%s: write config wbuf failed", __func__);
            return HDF_ERR_IO;
        }
    } else {
        HDF_LOGE("%s: cmd is not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t UartTestReadConfig(struct UartTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    int32_t i;
    uint32_t *tmp = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL || drsOps->GetUint32Array == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint32(node, "port", &config->port, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read port fail", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint32(node, "len", &config->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read len fail", __func__);
        return HDF_FAILURE;
    }
    config->wbuf = (uint8_t *)OsalMemCalloc(config->len);
    if (config->wbuf == NULL) {
        HDF_LOGE("%s: wbuf OsalMemCalloc error\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    tmp = (uint32_t *)OsalMemCalloc(config->len * sizeof(uint32_t));
    if (tmp == NULL) {
        HDF_LOGE("%s: tmp OsalMemCalloc error\n", __func__);
        OsalMemFree(config->wbuf);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = drsOps->GetUint32Array(node, "wbuf", tmp, config->len, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read wbuf fail\n", __func__);
        OsalMemFree(config->wbuf);
        OsalMemFree(tmp);
        return HDF_FAILURE;
    }
    for (i = 0; i < config->len; i++) {
        config->wbuf[i] = tmp[i];
    }
    OsalMemFree(tmp);
    config->rbuf = (uint8_t *)OsalMemCalloc(config->len);
    if (config->rbuf == NULL) {
        HDF_LOGE("%s: rbuf OsalMemCalloc error\n", __func__);
        OsalMemFree(config->wbuf);
        return HDF_ERR_MALLOC_FAIL;
    }
    return HDF_SUCCESS;
}

static int32_t UartTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = UartTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service.Dispatch = UartTestDispatch;
    device->service = &service;

    return HDF_SUCCESS;
}

static int32_t UartTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void UartTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    return;
}

struct HdfDriverEntry g_uartTestEntry = {
    .moduleVersion = 1,
    .Bind = UartTestBind,
    .Init = UartTestInit,
    .Release = UartTestRelease,
    .moduleName = "PLATFORM_UART_TEST",
};
HDF_INIT(g_uartTestEntry);
