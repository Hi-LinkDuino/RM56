/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_platform_base_test.h"
#include "audio_stream_dispatch.h"
#include "audio_platform_base.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG audio_dsp_base_test

int32_t PlatformDataFromCardTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (PlatformDataFromCard(NULL) != NULL) {
        return HDF_FAILURE;
    }

    if (PlatformDataFromCard(&card) != NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioBytesToFramesTest(void)
{
    if (AudioBytesToFrames(0, 0) != 0) {
        return HDF_FAILURE;
    }

    if (AudioBytesToFrames(1, 1) != 1) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioDataBigEndianChangeTest(void)
{
    const int dataLen = 96; // test data length
    char test[dataLen];
    (void)memset_s(test, dataLen, 1, dataLen);

    if (AudioDataBigEndianChange(NULL, 0, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioDataBigEndianChange(test, dataLen, DATA_BIT_WIDTH8) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioDataBigEndianChange(test, dataLen, DATA_BIT_WIDTH24) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioDataBigEndianChange(test, dataLen, DATA_BIT_WIDTH16) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioFramatToBitWidthTest(void)
{
    unsigned int bitWidth = 0;

    if (AudioFormatToBitWidth(1, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioFormatToBitWidth(AUDIO_FORMAT_PCM_16_BIT, &bitWidth) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (bitWidth != DATA_BIT_WIDTH16) {
        return HDF_FAILURE;
    }

    bitWidth = 0;
    if (AudioFormatToBitWidth(AUDIO_FORMAT_PCM_24_BIT, &bitWidth) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (bitWidth != DATA_BIT_WIDTH24) {
        return HDF_FAILURE;
    }

    if (AudioFormatToBitWidth(AUDIO_FORMAT_AAC_HE_V1, &bitWidth) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioSetPcmInfoTest(void)
{
    struct PlatformData platformData;
    struct AudioPcmHwParams param;
    (void)memset_s(&platformData, sizeof(struct PlatformData), 0, sizeof(struct PlatformData));
    (void)memset_s(&param, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));

    if (AudioSetPcmInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioSetPcmInfo(&platformData, &param) != HDF_SUCCESS) {
        ADM_LOG_DEBUG("AudioSetPcmInfo fail");
    }
    return HDF_SUCCESS;
}

int32_t AudioSetRenderBufInfoTest(void)
{
    struct PlatformData data;
    struct AudioPcmHwParams param;
    (void)memset_s(&data, sizeof(struct PlatformData), 0, sizeof(struct PlatformData));
    (void)memset_s(&param, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));

    if (AudioSetRenderBufInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioSetRenderBufInfo(&data, &param) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioSetCaptureBufInfoTest(void)
{
    struct PlatformData data;
    struct AudioPcmHwParams param;
    (void)memset_s(&data, sizeof(struct PlatformData), 0, sizeof(struct PlatformData));
    (void)memset_s(&param, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));

    if (AudioSetCaptureBufInfo(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioSetCaptureBufInfo(&data, &param) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioPcmWriteTest(void)
{
    struct AudioCard card;
    struct AudioTxData txData;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    (void)memset_s(&txData, sizeof(struct AudioTxData), 0, sizeof(struct AudioTxData));

    if (AudioPcmWrite(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioPcmWrite(&card, &txData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioPcmReadTest(void)
{
    struct AudioCard card;
    struct AudioRxData rxData;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    (void)memset_s(&rxData, sizeof(struct AudioRxData), 0, sizeof(struct AudioRxData));

    if (AudioPcmRead(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioPcmRead(&card, &rxData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioPcmMmapWriteTest(void)
{
    struct AudioCard card;
    struct AudioMmapData txMmapData;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    (void)memset_s(&txMmapData, sizeof(struct AudioMmapData), 0, sizeof(struct AudioMmapData));

    if (AudioPcmMmapWrite(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioPcmMmapWrite(&card, &txMmapData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioPcmMmapReadTest(void)
{
    struct AudioCard card;
    struct AudioMmapData rxMmapData;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    (void)memset_s(&rxMmapData, sizeof(struct AudioMmapData), 0, sizeof(struct AudioMmapData));

    if (AudioPcmMmapRead(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioPcmMmapRead(&card, &rxMmapData) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioRenderOpenTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioRenderOpen(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderOpen(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCaptureOpenTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioCaptureOpen(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureOpen(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioRenderCloseTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioRenderClose(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderClose(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioPcmPointerTest(void)
{
    uint32_t pointer = 0;
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioPcmPointer(NULL, NULL, 0) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioPcmPointer(&card, &pointer, AUDIO_RENDER_STREAM) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCaptureCloseTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioCaptureClose(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureClose(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioHwParamsTest(void)
{
    struct AudioCard card;
    struct AudioPcmHwParams param;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));
    (void)memset_s(&param, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));

    if (AudioHwParams(NULL, NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioHwParams(&card, &param) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioRenderPrepareTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioRenderPrepare(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderPrepare(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioCapturePrepareTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioCapturePrepare(NULL) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCapturePrepare(&card) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioRenderTriggerTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioRenderTrigger(NULL, AUDIO_DRV_PCM_IOCTL_RENDER_START) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderTrigger(&card, AUDIO_DRV_PCM_IOCTL_RENDER_START) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderTrigger(&card, AUDIO_DRV_PCM_IOCTL_RENDER_STOP) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderTrigger(&card, AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioRenderTrigger(&card, AUDIO_DRV_PCM_IOCTL_RENDER_RESUME) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioCaptureTriggerTest(void)
{
    struct AudioCard card;
    (void)memset_s(&card, sizeof(struct AudioCard), 0, sizeof(struct AudioCard));

    if (AudioCaptureTrigger(NULL, AUDIO_DRV_PCM_IOCTL_CAPTURE_START) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureTrigger(&card, AUDIO_DRV_PCM_IOCTL_CAPTURE_START) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureTrigger(&card, AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureTrigger(&card, AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioCaptureTrigger(&card, AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME) == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
