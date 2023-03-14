/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_codec_base_test.h"
#include "audio_codec_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_codec_base_test

int32_t CodecGetServiceNameTest(void)
{
    struct HdfDeviceObject device;
    const char *drvCodecName = "";

    if (CodecGetServiceName(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetServiceName(&device, &drvCodecName) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CodecGetDaiNameTest(void)
{
    struct HdfDeviceObject device;
    const char *drvCodecName = "";

    if (CodecGetDaiName(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetDaiName(&device, &drvCodecName) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CodecGetConfigInfoTest(void)
{
    struct HdfDeviceObject device;
    struct CodecData codecData;

    if (CodecGetConfigInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetConfigInfo(&device, &codecData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CodecSetConfigInfoTest(void)
{
    struct DaiData device;
    struct CodecData codecData;

    (void)memset_s(&device, sizeof(struct DaiData), 0, sizeof(struct DaiData));
    (void)memset_s(&codecData, sizeof(struct CodecData), 0, sizeof(struct CodecData));
    if (CodecSetConfigInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecSetConfigInfo(&codecData, &device) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CodecSetCtlFuncTest(void)
{
    struct CodecData codeData;

    if (CodecSetCtlFunc(NULL, NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecSetCtlFunc(&codeData, NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
