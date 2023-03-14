/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "timer_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"

#define HDF_LOG_TAG timer_driver_test_c

static struct TimerTestConfig g_config;

static int32_t TimerTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
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
        HDF_LOGE("%s: cmd not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t TimerTestReadConfig(struct TimerTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *face = NULL;

    face = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (face == NULL) {
        HDF_LOGE("%s: face is null", __func__);
        return HDF_FAILURE;
    }
    if (face->GetUint32 == NULL) {
        HDF_LOGE("%s: GetUint32 not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = face->GetUint32(node, "number", &config->number, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read id fail!", __func__);
        return HDF_FAILURE;
    }

    ret = face->GetUint32(node, "useconds", &config->uSecond, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read useconds fail!", __func__);
        return HDF_FAILURE;
    }

    ret = face->GetUint32(node, "isPeriod", &config->isPeriod, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read isPeriod fail!", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGD("timer test init:number[%u][%u][%d]", config->number, config->uSecond, config->isPeriod);

    return HDF_SUCCESS;
}

static int32_t TimerTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = TimerTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service.Dispatch = TimerTestDispatch;
    device->service = &service;
    return HDF_SUCCESS;
}

static int32_t TimerTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void TimerTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    HDF_LOGI("%s: Done!", __func__);
    return;
}

struct HdfDriverEntry g_timerTestEntry = {
    .moduleVersion = 1,
    .Bind = TimerTestBind,
    .Init = TimerTestInit,
    .Release = TimerTestRelease,
    .moduleName = "PLATFORM_TIMER_TEST",
};
HDF_INIT(g_timerTestEntry);