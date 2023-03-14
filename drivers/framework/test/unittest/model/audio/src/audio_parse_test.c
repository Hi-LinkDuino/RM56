/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "audio_parse_test.h"
#include "audio_parse.h"
#include "devsvc_manager_clnt.h"

#define HDF_LOG_TAG audio_parse_test

int32_t AudioFillConfigDataTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct AudioConfigData configData;
    HDF_LOGI("enter");

    if (AudioFillConfigData(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("AudioFillConfigData fail");
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    if (AudioFillConfigData(device, &configData) != HDF_SUCCESS) {
        HDF_LOGE("AudioFillConfigData fail");
        return HDF_FAILURE;
    }

    HDF_LOGI("success");
    return HDF_SUCCESS;
}

int32_t CodecGetRegConfigTest(void)
{
    struct HdfDeviceObject *device = NULL;
    struct AudioRegCfgData regCfgData;
    HDF_LOGI("enter");

    if (CodecGetRegConfig(NULL, NULL) == HDF_SUCCESS) {
        HDF_LOGE("AudioFillConfigData fail");
        return HDF_FAILURE;
    }

    device = DevSvcManagerClntGetDeviceObject("hdf_audio_codec_dev0");
    if (CodecGetRegConfig(device, &regCfgData) != HDF_SUCCESS) {
        HDF_LOGE("AudioFillConfigData fail");
    }

    HDF_LOGI("success");
    return HDF_SUCCESS;
}
