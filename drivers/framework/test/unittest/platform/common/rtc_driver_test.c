/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "rtc_test.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_time.h"
#include "rtc_base.h"
#include "rtc_if.h"

#define HDF_LOG_TAG hdf_rtc_driver_test_c

static struct RtcTestConfig g_config;

static int32_t RtcTestDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
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
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t RtcTestReadConfig(struct RtcTestConfig *config, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint32(node, "time", &config->time, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read time failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "maxYear", &config->maxYear, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read maxYear failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "year", &config->year, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read year failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "month", &config->month, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read month failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "day", &config->day, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read day failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "hour", &config->hour, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read hour failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "minute", &config->minute, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read minute failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "second", &config->second, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read second failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "frequency", &config->frequency, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read frequency failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "userValue", &config->userValue, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read userValue failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "userMaxIndex", &config->userMaxIndex, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read userMaxIndex failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "waitTimeSecond", &config->waitTimeSecond, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read waitTimeSecond failed", __func__);
        return ret;
    }
    ret = drsOps->GetUint32(node, "writeWaitMillisecond", &config->writeWaitMillisecond, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read writeWaitMillisecond failed", __func__);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t RtcTestBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    static struct IDeviceIoService service;

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or config is null!", __func__);
        return HDF_ERR_IO;
    }

    ret = RtcTestReadConfig(&g_config, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed", __func__);
        return ret;
    }

    service.Dispatch = RtcTestDispatch;
    device->service = &service;

    return HDF_SUCCESS;
}

static int32_t RtcTestInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void RtcTestRelease(struct HdfDeviceObject *device)
{
    if (device != NULL) {
        device->service = NULL;
    }
    return;
}

struct HdfDriverEntry g_rtcTestEntry = {
    .moduleVersion = 1,
    .Bind = RtcTestBind,
    .Init = RtcTestInit,
    .Release = RtcTestRelease,
    .moduleName = "PLATFORM_RTC_TEST",
};
HDF_INIT(g_rtcTestEntry);
