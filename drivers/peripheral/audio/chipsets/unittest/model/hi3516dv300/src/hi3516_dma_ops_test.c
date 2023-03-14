/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_dma_ops_test.h"
#include "audio_platform_base.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hi3516_common_func.h"
#include "hi3516_dma_ops.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

int32_t TestAudioDmaDeviceInit(void)
{
    struct AudioCard *card = NULL;
    struct PlatformDevice *platformDevice = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = AudioDmaDeviceInit(card, platformDevice);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: AudioDmaDeviceInit fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaBufAlloc(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaBufAlloc(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaBufAlloc fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaBufFree(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaBufFree(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaBufFree fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaRequestChannel(void)
{
    HDF_LOGI("%s: success", __func__);
    return Hi3516DmaRequestChannel(NULL, AUDIO_CAPTURE_STREAM);
}

int32_t TestHi3516DmaConfigChannel(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaConfigChannel(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaConfigChannel fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaPrep(void)
{
    HDF_LOGI("%s: success", __func__);
    return Hi3516DmaPrep(NULL, AUDIO_CAPTURE_STREAM);
}

int32_t TestHi3516DmaSubmit(void)
{
    HDF_LOGI("%s: success", __func__);
    return Hi3516DmaSubmit(NULL, AUDIO_CAPTURE_STREAM);
}

int32_t TestHi3516DmaPending(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaPending(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaPending fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaPause(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaPause(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaPause fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaResume(void)
{
    struct PlatformData *data = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaResume(data, AUDIO_CAPTURE_STREAM);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaResume fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestHi3516DmaPointer(void)
{
    struct PlatformData *data = NULL;
    uint32_t *pointer = NULL;
    int ret;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516DmaPointer(data, AUDIO_CAPTURE_STREAM, pointer);
    if (ret == HDF_SUCCESS) {
        HDF_LOGE("%s: Hi3516DmaPointer fail ret = %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
