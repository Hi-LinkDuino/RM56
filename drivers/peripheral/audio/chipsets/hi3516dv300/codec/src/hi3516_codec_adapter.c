/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_codec_ops.h"
#include "audio_codec_base.h"
#include "audio_core.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct CodecData g_codecData = {
    .Init = CodecDeviceInit,
    .Read = AudioDeviceReadReg,
    .Write = AudioDeviceWriteReg,
};

struct AudioDaiOps g_codecDaiDeviceOps = {
    .Startup = CodecDaiStartup,
    .HwParams = CodecDaiHwParams,
};

struct DaiData g_codecDaiData = {
    .DaiInit = CodecDaiDeviceInit,
    .ops = &g_codecDaiDeviceOps,
};

/* HdfDriverEntry implementations */
static int32_t CodecDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    struct CodecHost *codecHost = (struct CodecHost *)OsalMemCalloc(sizeof(*codecHost));
    if (codecHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("malloc codecHost fail!");
        return HDF_FAILURE;
    }
    codecHost->device = device;
    device->service = &codecHost->service;

    return HDF_SUCCESS;
}

static int32_t CodecDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (CodecGetConfigInfo(device, &g_codecData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecSetConfigInfo(&g_codecData, &g_codecDaiData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetServiceName(device, &g_codecData.drvCodecName) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetDaiName(device, &g_codecDaiData.drvDaiName) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRegisterCodec(device, &g_codecData, &g_codecDaiData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void CodecDriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    if (device->priv != NULL) {
        OsalMemFree(device->priv);
    }
    struct CodecHost *codecHost = (struct CodecHost *)device->service;
    if (codecHost == NULL) {
        HDF_LOGE("CodecDriverRelease: codecHost is NULL");
        return;
    }
    OsalMemFree(codecHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_codecDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "CODEC_HI3516",
    .Bind = CodecDriverBind,
    .Init = CodecDriverInit,
    .Release = CodecDriverRelease,
};
HDF_INIT(g_codecDriverEntry);
