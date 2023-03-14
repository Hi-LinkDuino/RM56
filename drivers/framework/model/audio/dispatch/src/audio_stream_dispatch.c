/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_stream_dispatch.h"
#include "audio_driver_log.h"
#include "audio_platform_base.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

#define BUFF_SIZE_MAX 64

static inline struct StreamHost *StreamHostFromDevice(const struct HdfDeviceObject *device)
{
    return (device == NULL) ? NULL : (struct StreamHost *)device->service;
}

static int32_t HwCpuDaiDispatch(const struct AudioCard *audioCard, const struct AudioPcmHwParams *params)
{
    struct AudioRuntimeDeivces *rtd;
    struct DaiDevice *cpuDai;
    if ((audioCard == NULL) || (params == NULL)) {
        ADM_LOG_ERR("CpuDai input param is NULL.");
        return HDF_FAILURE;
    }

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("CpuDai audioCard rtd is NULL.");
        return HDF_FAILURE;
    }

    cpuDai = rtd->cpuDai;
    if (cpuDai == NULL || cpuDai->devData == NULL || cpuDai->devData->ops == NULL) {
        ADM_LOG_ERR("cpuDai param is NULL.");
        return HDF_FAILURE;
    }

    /* If there are HwParams function, it will be executed directly.
     * If not, skip the if statement and execute in sequence.
     */
    if (cpuDai->devData->ops->HwParams != NULL) {
        int ret = cpuDai->devData->ops->HwParams(audioCard, params);
        if (ret < 0) {
            ADM_LOG_ERR("cpuDai hardware params failed ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t HwCodecDaiDispatch(const struct AudioCard *audioCard, const struct AudioPcmHwParams *params)
{
    struct AudioRuntimeDeivces *rtd;
    struct DaiDevice *codecDai;
    if ((audioCard == NULL) || (params == NULL)) {
        ADM_LOG_ERR("CodecDai input param is NULL.");
        return HDF_FAILURE;
    }

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("CodecDai audioCard rtd is NULL.");
        return HDF_FAILURE;
    }
    codecDai = rtd->codecDai;
    if (codecDai == NULL) {
        codecDai = rtd->accessoryDai;
    }
    if (codecDai == NULL || codecDai->devData == NULL || codecDai->devData->ops == NULL) {
        ADM_LOG_ERR("codecDai param is NULL.");
        return HDF_FAILURE;
    }

    /* If there are HwParams function, it will be executed directly.
     * If not, skip the if statement and execute in sequence.
     */
    if (codecDai->devData->ops->HwParams != NULL) {
        int ret = codecDai->devData->ops->HwParams(audioCard, params);
        if (ret < 0) {
            ADM_LOG_ERR("codecDai hardware params failed ret=%d", ret);
            return HDF_ERR_IO;
        }
    }

    return HDF_SUCCESS;
}

static int32_t HwDspDaiDispatch(const struct AudioCard *audioCard, const struct AudioPcmHwParams *params)
{
    struct AudioRuntimeDeivces *rtd;
    struct DaiDevice *dspDai;
    if ((audioCard == NULL) || (params == NULL)) {
        ADM_LOG_ERR("DspDai input param is NULL.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("DspDai audioCard rtd is NULL.");
        return HDF_FAILURE;
    }
    dspDai = rtd->dspDai;

    if (dspDai != NULL && dspDai->devData != NULL && dspDai->devData->ops != NULL &&
        dspDai->devData->ops->HwParams != NULL) {
        int ret = dspDai->devData->ops->HwParams(audioCard, params);
        if (ret < 0) {
            ADM_LOG_ERR("dspDai hardware params failed ret=%d", ret);
            return HDF_ERR_IO;
        }
    }
    return HDF_SUCCESS;
}

static int32_t HwPlatfromDispatch(const struct AudioCard *audioCard, const struct AudioPcmHwParams *params)
{
    int ret = 0;
    struct AudioRuntimeDeivces *rtd;
    if ((audioCard == NULL) || (params == NULL)) {
        ADM_LOG_ERR("Platform input param is NULL.");
        return HDF_FAILURE;
    }

    rtd = audioCard->rtd;
    if (rtd == NULL) {
        ADM_LOG_ERR("audioCard rtd is NULL.");
        return HDF_FAILURE;
    }

    /* If there are HwParams function, it will be executed directly.
     * If not, skip the if statement and execute in sequence.
     */
    ret = AudioHwParams(audioCard, params);
    if (ret < 0) {
        ADM_LOG_ERR("platform hardware params failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t HwParamsDispatch(const struct AudioCard *audioCard, const struct AudioPcmHwParams *params)
{
    if ((audioCard == NULL) || (params == NULL)) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }

    /* Traverse through each driver method; Enter if you have, if not, exectue in order */
    if (HwCodecDaiDispatch(audioCard, params) != HDF_SUCCESS) {
        ADM_LOG_ERR("codec dai hardware params failed.");
        return HDF_FAILURE;
    }

    if (HwCpuDaiDispatch(audioCard, params) != HDF_SUCCESS) {
        ADM_LOG_ERR("cpu dai hardware params failed.");
        return HDF_FAILURE;
    }

    if (HwPlatfromDispatch(audioCard, params) != HDF_SUCCESS) {
        ADM_LOG_ERR("platform dai hardware params failed.");
        return HDF_FAILURE;
    }

    if (HwDspDaiDispatch(audioCard, params) != HDF_SUCCESS) {
        ADM_LOG_ERR("dsp dai hardware params failed.");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t CpuDaiDevStartup(const struct AudioCard *audioCard, const struct DaiDevice *cpuDai)
{
    int32_t ret;
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("audioCard is null.");
        return HDF_FAILURE;
    }
    if (cpuDai != NULL && cpuDai->devData != NULL && cpuDai->devData->ops != NULL &&
        cpuDai->devData->ops->Startup != NULL) {
        ret = cpuDai->devData->ops->Startup(audioCard, cpuDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("cpuDai Startup failed.");
            return HDF_FAILURE;
        }
    } else {
        ADM_LOG_DEBUG("cpu dai startup is null.");
    }
    return HDF_SUCCESS;
}

static int32_t CodecDaiDevStartup(const struct AudioCard *audioCard, const struct DaiDevice *codecDai)
{
    int32_t ret;
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("audioCard is null.");
        return HDF_FAILURE;
    }
    if (codecDai != NULL && codecDai->devData != NULL && codecDai->devData->ops != NULL &&
        codecDai->devData->ops->Startup != NULL) {
        ret = codecDai->devData->ops->Startup(audioCard, codecDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("codecDai Startup failed.");
            return HDF_FAILURE;
        }
    } else {
        ADM_LOG_DEBUG("codec dai startup is null.");
    }

    return HDF_SUCCESS;
}

static int32_t DspDaiDevStartup(const struct AudioCard *audioCard, const struct DaiDevice *dspDai)
{
    int32_t ret;
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("audioCard is null.");
        return HDF_FAILURE;
    }
    if (dspDai != NULL && dspDai->devData != NULL && dspDai->devData->ops != NULL &&
        dspDai->devData->ops->Startup != NULL) {
        ret = dspDai->devData->ops->Startup(audioCard, dspDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("dspDai Startup failed.");
            return HDF_FAILURE;
        }
    } else {
        ADM_LOG_DEBUG("dsp dai startup is null.");
    }
    return HDF_SUCCESS;
}

static int32_t AccessoryDaiDevStartup(const struct AudioCard *audioCard, const struct DaiDevice *accessoryDai)
{
    int32_t ret;
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("audioCard is null.");
        return HDF_FAILURE;
    }
    if (accessoryDai != NULL && accessoryDai->devData != NULL && accessoryDai->devData->ops != NULL &&
        accessoryDai->devData->ops->Startup != NULL) {
        ret = accessoryDai->devData->ops->Startup(audioCard, accessoryDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("accessoryDai Startup failed.");
            return HDF_FAILURE;
        }
    } else {
        ADM_LOG_DEBUG("accessory dai startup is null.");
    }
    return HDF_SUCCESS;
}

static int32_t AudioDaiDeviceStartup(const struct AudioCard *audioCard)
{
    struct DaiDevice *cpuDai = NULL;
    struct DaiDevice *codecDai = NULL;
    struct DaiDevice *dspDai = NULL;
    struct DaiDevice *accessoryDai = NULL;
    int32_t ret;

    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("audioCard is null.");
        return HDF_FAILURE;
    }
    cpuDai = audioCard->rtd->cpuDai;
    ret = CpuDaiDevStartup(audioCard, cpuDai);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("CpuDaiDevStartup failed.");
        return HDF_FAILURE;
    }
    codecDai = audioCard->rtd->codecDai;
    ret = CodecDaiDevStartup(audioCard, codecDai);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("CodecDaiDevStartup failed.");
        return HDF_FAILURE;
    }
    dspDai = audioCard->rtd->dspDai;
    ret = DspDaiDevStartup(audioCard, dspDai);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("DspDaiDevStartup failed.");
        return HDF_FAILURE;
    }
    accessoryDai = audioCard->rtd->accessoryDai;
    ret = AccessoryDaiDevStartup(audioCard, accessoryDai);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AccessoryDaiDevStartup failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HwParamsDataAnalysis(struct HdfSBuf *reqData, struct AudioPcmHwParams *params)
{
    if ((reqData == NULL) || (params == NULL)) {
        ADM_LOG_ERR(" input param is NULL.");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->streamType)) {
        ADM_LOG_ERR("read request streamType failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->channels)) {
        ADM_LOG_ERR("read request channels failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->rate)) {
        ADM_LOG_ERR("read request rate failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->periodSize)) {
        ADM_LOG_ERR("read request periodSize failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->periodCount)) {
        ADM_LOG_ERR("read request periodCount failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, (uint32_t *)&params->format)) {
        ADM_LOG_ERR("read request format failed!");
        return HDF_FAILURE;
    }
    if (!(params->cardServiceName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("read request cardServiceName failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->period)) {
        HDF_LOGE("read request perid failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->frameSize)) {
        HDF_LOGE("read request frameSize failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, (uint32_t *)&params->isBigEndian)) {
        HDF_LOGE("read request isBigEndian failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, (uint32_t *)&params->isSignedData)) {
        HDF_LOGE("read request isSignedData failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->startThreshold)) {
        HDF_LOGE("read request startThreshold failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->stopThreshold)) {
        HDF_LOGE("read request stopThreshold failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reqData, &params->silenceThreshold)) {
        HDF_LOGE("read request silenceThreshold failed!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t StreamHostHwParams(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioPcmHwParams params;
    struct AudioCard *audioCard = NULL;
    int ret;
    ADM_LOG_DEBUG("entry.");

    if ((client == NULL || client->device == NULL) || (data == NULL)) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    (void)reply;

    (void)memset_s(&params, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));

    ret = HwParamsDataAnalysis(data, &params);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("hwparams data analysis failed ret=%d", ret);
        return HDF_FAILURE;
    }
    audioCard = GetCardInstance(params.cardServiceName);
    if (audioCard == NULL) {
        ADM_LOG_ERR("get card instance failed.");
        return HDF_FAILURE;
    }
    ret = HwParamsDispatch(audioCard, &params);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("hwparams dispatch failed ret=%d", ret);
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static struct AudioCard *StreamHostGetCardInstance(const struct HdfDeviceIoClient *client)
{
    char *cardName = NULL;
    struct StreamHost *streamHost = NULL;
    struct AudioCard *audioCard = NULL;

    if (client == NULL || client->device == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return NULL;
    }

    streamHost = StreamHostFromDevice(client->device);
    if (streamHost == NULL) {
        ADM_LOG_ERR("streamHost is NULL");
        return NULL;
    }

    cardName = (char *)streamHost->priv;
    if (cardName == NULL) {
        ADM_LOG_ERR("streamHost priv is NULL.");
        return NULL;
    }

    audioCard = GetCardInstance(cardName);
    if (audioCard == NULL) {
        ADM_LOG_ERR("get card instance failed.");
        return NULL;
    }

    return audioCard;
}

static int32_t StreamHostCapturePrepare(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("CapturePrepare input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL) {
        ADM_LOG_ERR("CapturePrepare get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = AudioCapturePrepare(audioCard);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform CapturePrepare failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostCaptureOpen(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    char *cardName = NULL;
    const char *carNameTemp = NULL;
    struct StreamHost *streamHost = NULL;
    struct AudioCard *audioCard = NULL;

    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("StreamHostCaptureOpen input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    cardName = (char *)OsalMemCalloc(sizeof(char) * BUFF_SIZE_MAX);
    if (cardName == NULL) {
        ADM_LOG_ERR("malloc cardServiceName failed!");
        return HDF_FAILURE;
    }

    streamHost = StreamHostFromDevice(client->device);
    if (streamHost == NULL) {
        OsalMemFree(cardName);
        ADM_LOG_ERR("renderHost is NULL");
        return HDF_FAILURE;
    }

    if (!(carNameTemp = HdfSbufReadString(data))) {
        OsalMemFree(cardName);
        ADM_LOG_ERR("read request cardServiceName failed!");
        return HDF_FAILURE;
    }

    if (strncpy_s(cardName, BUFF_SIZE_MAX - 1, carNameTemp, strlen(carNameTemp) + 1) != EOK) {
        ADM_LOG_ERR("memcpy cardName failed.");
        OsalMemFree(cardName);
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("card name: %s.", cardName);
    streamHost->priv = cardName;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL) {
        ADM_LOG_ERR("StreamHostCaptureOpen get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    if (AudioCaptureOpen(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("platform CaptureOpen failed");
        return HDF_ERR_IO;
    }

    if (AudioDaiDeviceStartup(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("Dai Device Startup failed.");
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderPrepare(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("RenderPrepare input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderPrepare get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = AudioRenderPrepare(audioCard);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform RenderPrepare failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderOpen(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    char *cardName = NULL;
    const char *carNameTemp = NULL;
    struct StreamHost *streamHost = NULL;
    struct AudioCard *audioCard = NULL;

    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("StreamHostRenderOpen input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    cardName = (char *)OsalMemCalloc(sizeof(char) * BUFF_SIZE_MAX);
    if (cardName == NULL) {
        ADM_LOG_ERR("malloc cardServiceName failed!");
        return HDF_FAILURE;
    }

    streamHost = StreamHostFromDevice(client->device);
    if (streamHost == NULL) {
        ADM_LOG_ERR("renderHost is NULL");
        OsalMemFree(cardName);
        return HDF_FAILURE;
    }

    if (!(carNameTemp = HdfSbufReadString(data))) {
        ADM_LOG_ERR("read request cardServiceName failed!");
        OsalMemFree(cardName);
        return HDF_FAILURE;
    }

    if (strncpy_s(cardName, BUFF_SIZE_MAX - 1, carNameTemp, strlen(carNameTemp) + 1) != EOK) {
        ADM_LOG_ERR("memcpy cardName failed.");
        OsalMemFree(cardName);
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("card name: %s.", cardName);

    streamHost->priv = cardName;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("StreamHostRenderOpen get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    if (AudioRenderOpen(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("platform RenderOpen failed.");
        return HDF_FAILURE;
    }

    if (AudioDaiDeviceStartup(audioCard) != HDF_SUCCESS) {
        ADM_LOG_ERR("Dai Device Startup failed.");
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamTransferWrite(const struct AudioCard *audioCard, struct AudioTxData *transfer)
{
    int32_t ret;

    if (audioCard == NULL || transfer == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }

    ret = AudioPcmWrite(audioCard, transfer);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("pcm write failed ret=%d", ret);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t StreamTransferMmapWrite(const struct AudioCard *audioCard, const struct AudioMmapData *txMmapData)
{
    int32_t ret;
    ADM_LOG_DEBUG("entry.");

    if (audioCard == NULL || txMmapData == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }

    ret = AudioPcmMmapWrite(audioCard, txMmapData);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform write failed ret=%d", ret);
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamTransferMmapRead(const struct AudioCard *audioCard, const struct AudioMmapData *rxMmapData)
{
    int32_t ret;
    ADM_LOG_DEBUG("entry.");

    if (audioCard == NULL || rxMmapData == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }

    ret = AudioPcmMmapRead(audioCard, rxMmapData);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform read failed ret=%d", ret);
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("sucess.");
    return HDF_SUCCESS;
}

static int32_t StreamHostWrite(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioTxData transfer;
    struct AudioCard *audioCard = NULL;
    int32_t ret = HDF_SUCCESS;
    uint32_t dataSize = 0;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    (void)memset_s(&transfer, sizeof(struct AudioTxData), 0, sizeof(struct AudioTxData));
    if (!HdfSbufReadUint32(data, (uint32_t *)&(transfer.frames))) {
        ADM_LOG_ERR("read request frames failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&(transfer.buf), &dataSize)) {
        ADM_LOG_ERR("read request buf failed!");
        return HDF_FAILURE;
    }

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = StreamTransferWrite(audioCard, &transfer);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("write reg value failed ret=%d", ret);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(reply, (uint32_t)(transfer.status))) {
        ADM_LOG_ERR("read response status failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRead(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    struct AudioRxData rxData;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    (void)data;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL) {
        ADM_LOG_ERR("get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = AudioPcmRead(audioCard, &rxData);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("pcm read failed ret=%d", ret);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(reply, (uint32_t)(rxData.status))) {
        ADM_LOG_ERR("write request data status failed!");
        return HDF_FAILURE;
    }

    if (rxData.bufSize != 0) {
        if (!HdfSbufWriteBuffer(reply, rxData.buf, (uint32_t)(rxData.bufSize))) {
            ADM_LOG_ERR("write request data buf failed!");
            return HDF_FAILURE;
        }

        if (!HdfSbufWriteUint32(reply, (uint32_t)(rxData.frames))) {
            ADM_LOG_ERR("write frames failed!");
            return HDF_FAILURE;
        }
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostMmapWrite(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioMmapData txMmapData;
    struct AudioCard *audioCard = NULL;
    uint64_t mAddress = 0;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(data, &mAddress)) {
        ADM_LOG_ERR("render mmap read request memory address failed!");
        return HDF_FAILURE;
    }

    (void)memset_s(&txMmapData, sizeof(struct AudioMmapData), 0, sizeof(struct AudioMmapData));
    txMmapData.memoryAddress = (void *)((uintptr_t)mAddress);
    if (txMmapData.memoryAddress == NULL) {
        ADM_LOG_ERR("txMmapData.memoryAddress is NULL.");
        return HDF_FAILURE;
    }

    if (!HdfSbufReadInt32(data, (uint32_t *)&(txMmapData.memoryFd))) {
        ADM_LOG_ERR("render mmap read request memory fd failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(txMmapData.totalBufferFrames))) {
        ADM_LOG_ERR("render mmap read request total buffer frames failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(txMmapData.transferFrameSize))) {
        ADM_LOG_ERR("render mmap read request transfer frame size failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(txMmapData.isShareable))) {
        ADM_LOG_ERR("render mmap read request is share able failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&(txMmapData.offset))) {
        ADM_LOG_ERR("render mmap read request offset failed!");
        return HDF_FAILURE;
    }
    audioCard = StreamHostGetCardInstance(client);
    if (StreamTransferMmapWrite(audioCard, &txMmapData) != HDF_SUCCESS) {
        ADM_LOG_ERR("render mmap write reg value failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}
static int32_t StreamHostMmapPositionWrite(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    struct PlatformData *platformData;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard instance is NULL.");
        return HDF_FAILURE;
    }
    platformData = PlatformDataFromCard(audioCard);
    if (platformData == NULL) {
        ADM_LOG_ERR("platformHost instance is NULL.");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, platformData->renderBufInfo.framesPosition)) {
        ADM_LOG_ERR("render mmap write position failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostMmapRead(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint64_t mAddress = 0;
    struct AudioCard *audioCard = NULL;
    struct AudioMmapData rxMmapData;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(data, &mAddress)) {
        ADM_LOG_ERR("capture mmap read request memory address failed!");
        return HDF_FAILURE;
    }

    (void)memset_s(&rxMmapData, sizeof(struct AudioMmapData), 0, sizeof(struct AudioMmapData));
    rxMmapData.memoryAddress = (void *)((uintptr_t)mAddress);
    if (rxMmapData.memoryAddress == NULL) {
        ADM_LOG_ERR("rxMmapData.memoryAddress is NULL.");
        return HDF_FAILURE;
    }

    if (!HdfSbufReadInt32(data, (uint32_t *)&(rxMmapData.memoryFd))) {
        ADM_LOG_ERR("capture mmap read request memory fd failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(rxMmapData.totalBufferFrames))) {
        ADM_LOG_ERR("capture mmap read request total buffer frames failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(rxMmapData.transferFrameSize))) {
        ADM_LOG_ERR("capture mmap read request transfer frame size failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (uint32_t *)&(rxMmapData.isShareable))) {
        ADM_LOG_ERR("capture mmap read request is share able failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&(rxMmapData.offset))) {
        ADM_LOG_ERR("capture mmap read request offset failed!");
        return HDF_FAILURE;
    }
    audioCard = StreamHostGetCardInstance(client);
    if (StreamTransferMmapRead(audioCard, &rxMmapData) != HDF_SUCCESS) {
        ADM_LOG_ERR("capture mmap read reg value failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostMmapPositionRead(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    struct PlatformData *platformData;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL || reply == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL) {
        ADM_LOG_ERR("audioCard is NULL.");
        return HDF_FAILURE;
    }
    platformData = PlatformDataFromCard(audioCard);
    if (platformData == NULL) {
        ADM_LOG_ERR("platformHost is NULL.");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, platformData->captureBufInfo.framesPosition)) {
        ADM_LOG_ERR("render mmap write position failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamTriggerRouteImpl(const struct AudioCard *audioCard, const struct AudioRuntimeDeivces *rtd,
    enum StreamDispMethodCmd methodCmd)
{
    int32_t ret = HDF_SUCCESS;
    struct DaiDevice *cpuDai = NULL;
    struct DaiDevice *codecDai = NULL;
    struct DaiDevice *accessoryDai = NULL;
    struct DaiDevice *dspDai = NULL;
    if (audioCard == NULL || rtd == NULL) {
        ADM_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    cpuDai = rtd->cpuDai;
    if (cpuDai != NULL && cpuDai->devData != NULL && cpuDai->devData->ops != NULL &&
        cpuDai->devData->ops->Trigger != NULL) {
        ret = cpuDai->devData->ops->Trigger(audioCard, methodCmd, cpuDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("cpuDai Trigger failed.");
            return HDF_FAILURE;
        }
    }
    codecDai = rtd->codecDai;
    if (codecDai != NULL && codecDai->devData != NULL && codecDai->devData->ops != NULL &&
        codecDai->devData->ops->Trigger != NULL) {
        ret = codecDai->devData->ops->Trigger(audioCard, methodCmd, codecDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("codecDai Trigger failed.");
            return HDF_FAILURE;
        }
    }
    accessoryDai = rtd->accessoryDai;
    if (accessoryDai != NULL && accessoryDai->devData != NULL && accessoryDai->devData->ops != NULL &&
        accessoryDai->devData->ops->Trigger != NULL) {
        ret = accessoryDai->devData->ops->Trigger(audioCard, methodCmd, accessoryDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("accessoryDai Trigger failed.");
            return HDF_FAILURE;
        }
    }
    dspDai = rtd->dspDai;
    if (dspDai != NULL && dspDai->devData != NULL && dspDai->devData->ops != NULL &&
        dspDai->devData->ops->Trigger != NULL) {
        ret = dspDai->devData->ops->Trigger(audioCard, methodCmd, dspDai);
        if (ret != HDF_SUCCESS) {
            ADM_LOG_ERR("dspDai Trigger failed.");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderStart(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL) {
        ADM_LOG_ERR("RenderStart input param is NULL.");
        return HDF_FAILURE;
    }
    (void)data;
    (void)reply;
    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderStart get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    audioCard->standbyMode = AUDIO_SAPM_TURN_STANDBY_LATER;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_RENDER_START);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioRenderTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_RENDER_START);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform render start failed ret=%d", ret);
        return HDF_ERR_IO;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostCaptureStart(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL) {
        ADM_LOG_ERR("CaptureStart input param is NULL.");
        return HDF_FAILURE;
    }
    (void)data;
    (void)reply;
    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("CaptureStart get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    audioCard->standbyMode = AUDIO_SAPM_TURN_STANDBY_LATER;
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_CAPTURE_START);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioCaptureTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_CAPTURE_START);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform capture start failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderStop(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");
    if (client == NULL) {
        ADM_LOG_ERR("RenderStop input param is NULL.");
        return HDF_FAILURE;
    }
    (void)reply;
    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderStop get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &audioCard->standbyMode)) {
        ADM_LOG_ERR("read request streamType failed!");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_RENDER_STOP);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioRenderTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_RENDER_STOP);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform render stop failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderClose(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    int ret;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("RenderClose input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderStop get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = AudioRenderClose(audioCard);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform RenderClose failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}


static int32_t StreamHostCaptureStop(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("CaptureStop input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("CaptureStop get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    if (!HdfSbufReadUint32(data, &audioCard->standbyMode)) {
        ADM_LOG_ERR("read request streamType failed!");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioCaptureTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform capture stop failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostCaptureClose(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioCard *audioCard = NULL;
    int ret;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("CaptureClose input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("CaptureStop get card instance or rtd failed.");
        return HDF_FAILURE;
    }

    ret = AudioCaptureClose(audioCard);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform capture close failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}


static int32_t StreamHostRenderPause(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("RenderPause input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderPause get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioRenderTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform render pause failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostCapturePause(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("CapturePause input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("CapturePause get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioCaptureTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform captur pause failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostRenderResume(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("RenderResume input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("RenderResume get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_RENDER_RESUME);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioRenderTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_RENDER_RESUME);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform RenderResume failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostCaptureResume(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("entry.");

    if (client == NULL) {
        ADM_LOG_ERR("CaptureResume input param is NULL.");
        return HDF_FAILURE;
    }

    (void)data;
    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("CaptureResume get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;
    ret = StreamTriggerRouteImpl(audioCard, rtd, AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("StreamTriggerRouteImpl failed");
        return HDF_FAILURE;
    }
    ret = AudioCaptureTrigger(audioCard, AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("platform CaptureResume failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostDspDecode(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DspDevice *dspDev = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("Dsp Decode Entry.");

    if (client == NULL) {
        ADM_LOG_ERR("DspDecode input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("DspDecode get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;

    dspDev = rtd->dsp;
    if (dspDev == NULL || dspDev->devData == NULL || dspDev->devData->Decode == NULL) {
        ADM_LOG_ERR("audioCard rtd dsp is NULL.");
        return HDF_FAILURE;
    }

    ret = dspDev->devData->Decode(audioCard, (void*)data, dspDev);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("DeCode render pause failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("Decode Success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostDspEncode(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DspDevice *dspDev = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("Dsp Encode Entry.");

    if (client == NULL) {
        ADM_LOG_ERR("DspEncode input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("DspEncode get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;

    dspDev = rtd->dsp;
    if (dspDev == NULL || dspDev->devData == NULL || dspDev->devData->Encode == NULL) {
        ADM_LOG_ERR("audioCard rtd dsp is NULL.");
        return HDF_FAILURE;
    }

    ret = dspDev->devData->Encode(audioCard, (void*)data, dspDev);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("EnCode render pause failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("Encode Success.");
    return HDF_SUCCESS;
}

static int32_t StreamHostDspEqualizer(const struct HdfDeviceIoClient *client, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct AudioRuntimeDeivces *rtd = NULL;
    struct DspDevice *dspDev = NULL;
    struct AudioCard *audioCard = NULL;
    int ret = HDF_SUCCESS;
    ADM_LOG_DEBUG("Dsp Equalizer Entry.");

    if (client == NULL) {
        ADM_LOG_ERR("DspEqualizer input param is NULL.");
        return HDF_FAILURE;
    }

    (void)reply;

    audioCard = StreamHostGetCardInstance(client);
    if (audioCard == NULL || audioCard->rtd == NULL) {
        ADM_LOG_ERR("DspEqualizer get card instance or rtd failed.");
        return HDF_FAILURE;
    }
    rtd = audioCard->rtd;

    dspDev = rtd->dsp;
    if (dspDev == NULL || dspDev->devData == NULL || dspDev->devData->Equalizer == NULL) {
        ADM_LOG_ERR("audioCard rtd dsp is NULL.");
        return HDF_FAILURE;
    }

    ret = dspDev->devData->Equalizer(audioCard, (void*)data, dspDev);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("Equalizer render pause failed ret=%d", ret);
        return HDF_ERR_IO;
    }

    ADM_LOG_DEBUG("Equalizer Success.");
    return HDF_SUCCESS;
}

static struct StreamDispCmdHandleList g_streamDispCmdHandle[] = {
    {AUDIO_DRV_PCM_IOCTL_WRITE, StreamHostWrite},
    {AUDIO_DRV_PCM_IOCTL_READ, StreamHostRead},
    {AUDIO_DRV_PCM_IOCTL_HW_PARAMS, StreamHostHwParams},
    {AUDIO_DRV_PCM_IOCTL_RENDER_PREPARE, StreamHostRenderPrepare},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_PREPARE, StreamHostCapturePrepare},
    {AUDIO_DRV_PCM_IOCTL_RENDER_OPEN, StreamHostRenderOpen},
    {AUDIO_DRV_PCM_IOCTL_RENDER_CLOSE, StreamHostRenderClose},
    {AUDIO_DRV_PCM_IOCTL_RENDER_START, StreamHostRenderStart},
    {AUDIO_DRV_PCM_IOCTL_RENDER_STOP, StreamHostRenderStop},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_OPEN, StreamHostCaptureOpen},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_CLOSE, StreamHostCaptureClose},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_START, StreamHostCaptureStart},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP, StreamHostCaptureStop},
    {AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE, StreamHostRenderPause},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE, StreamHostCapturePause},
    {AUDIO_DRV_PCM_IOCTL_RENDER_RESUME, StreamHostRenderResume},
    {AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME, StreamHostCaptureResume},
    {AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER, StreamHostMmapWrite},
    {AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER_CAPTURE, StreamHostMmapRead},
    {AUDIO_DRV_PCM_IOCTL_MMAP_POSITION, StreamHostMmapPositionWrite},
    {AUDIO_DRV_PCM_IOCTL_MMAP_POSITION_CAPTURE, StreamHostMmapPositionRead},
    {AUDIO_DRV_PCM_IOCTL_DSPDECODE, StreamHostDspDecode},
    {AUDIO_DRV_PCM_IOCTL_DSPENCODE, StreamHostDspEncode},
    {AUDIO_DRV_PCM_IOCTL_DSPEQUALIZER, StreamHostDspEqualizer},
};

static int32_t StreamDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    unsigned int count = sizeof(g_streamDispCmdHandle) / sizeof(g_streamDispCmdHandle[0]);
    unsigned int i = 0;
    for (i = 0; i < count; ++i) {
        if ((cmdId == (int)(g_streamDispCmdHandle[i].cmd)) && (g_streamDispCmdHandle[i].func != NULL)) {
            return g_streamDispCmdHandle[i].func(client, data, reply);
        }
    }
    ADM_LOG_ERR("invalid [cmdId=%d]", cmdId);
    return HDF_FAILURE;
}

static struct StreamHost *StreamHostCreateAndBind(struct HdfDeviceObject *device)
{
    struct StreamHost *streamHost;
    if (device == NULL) {
        return NULL;
    }

    streamHost = (struct StreamHost *)OsalMemCalloc(sizeof(*streamHost));
    if (streamHost == NULL) {
        ADM_LOG_ERR("malloc host failed!");
        return NULL;
    }
    streamHost->device = device;
    device->service = &streamHost->service;
    return streamHost;
}

static int32_t AudioStreamBind(struct HdfDeviceObject *device)
{
    struct StreamHost *streamHost;
    ADM_LOG_DEBUG("entry!");
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    streamHost = StreamHostCreateAndBind(device);
    if (streamHost == NULL || streamHost->device == NULL) {
        ADM_LOG_ERR("StreamHostCreateAndBind failed");
        return HDF_FAILURE;
    }

    streamHost->service.Dispatch = StreamDispatch;

    ADM_LOG_INFO("success!");
    return HDF_SUCCESS;
}

static int32_t AudioStreamInit(struct HdfDeviceObject *device)
{
    struct StreamHost *streamHost = NULL;

    if (device == NULL) {
        ADM_LOG_ERR("device is NULL");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("entry.");

    streamHost = StreamHostFromDevice(device);
    if (streamHost == NULL) {
        ADM_LOG_ERR("renderHost is NULL");
        return HDF_FAILURE;
    }

    ADM_LOG_INFO("Success!");
    return HDF_SUCCESS;
}

static void AudioStreamRelease(struct HdfDeviceObject *device)
{
    struct StreamHost *streamHost;
    if (device == NULL) {
        ADM_LOG_ERR("device is NULL");
        return;
    }

    streamHost = StreamHostFromDevice(device);
    if (streamHost == NULL) {
        ADM_LOG_ERR("renderHost is NULL");
        return;
    }

    if (streamHost->priv != NULL) {
        OsalMemFree(streamHost->priv);
    }
    OsalMemFree(streamHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_audioStreamEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_AUDIO_STREAM",
    .Bind = AudioStreamBind,
    .Init = AudioStreamInit,
    .Release = AudioStreamRelease,
};
HDF_INIT(g_audioStreamEntry);
