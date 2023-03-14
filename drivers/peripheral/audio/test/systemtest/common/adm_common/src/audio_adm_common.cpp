/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio ADM test-related APIs, including data types and functions for writting data
to buffer.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_adm_common.h
 *
 * @brief Declares APIs for operations related to the audio ADM testing.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_adm_common.h"

using namespace std;

namespace HMOS {
namespace Audio {
int32_t WriteHwParamsToBuf(struct HdfSBuf *sBuf, struct AudioPcmHwParams hwParams)
{
    if (sBuf == nullptr) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)hwParams.streamType)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.channels)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.rate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.periodSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.periodCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(hwParams.format))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, hwParams.cardServiceName)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.frameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(hwParams.isBigEndian))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(hwParams.isSignedData))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.startThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.stopThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, hwParams.silenceThreshold)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AdmRenderFramePrepare(const std::string& path, char *&frame, uint32_t& readSize, uint32_t& frameSize)
{
    int32_t ret = -1;
    size_t numRead = 0;
    uint32_t bufferSize = 0;

    struct AudioSampleAttributes attrs = {};
    InitAttrs(attrs);
    char absPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), absPath) == nullptr) {
        return HDF_FAILURE;
    }
    FILE *file = fopen(absPath, "rb");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    struct AudioHeadInfo headInfo = {};
    ret = WavHeadAnalysis(headInfo, file, attrs);
    if (ret < 0) {
        fclose(file);
        return HDF_FAILURE;
    }
    bufferSize = PcmFramesToBytes(attrs);
    if (bufferSize <= 0) {
        fclose(file);
        return HDF_FAILURE;
    }
    frame = (char *)calloc(1, bufferSize);
    if (frame == nullptr) {
        fclose(file);
        return HDF_FAILURE;
    }
    uint32_t remainingDataSize = 0;
    remainingDataSize = headInfo.dataSize;
    readSize = (remainingDataSize) > (bufferSize) ? (bufferSize) : (remainingDataSize);
    numRead = fread(frame, readSize, 1, file);
    if (numRead < 1) {
        fclose(file);
        free(frame);
        frame = nullptr;
        return HDF_FAILURE;
    }
    frameSize = readSize / (attrs.channelCount * (PcmFormatToBits(attrs.format) >> MOVE_RIGHT_NUM));
    fclose(file);
    return HDF_SUCCESS;
}

int32_t WriteFrameToSBuf(struct HdfSBuf *&sBufT, const std::string& path)
{
    int32_t ret = -1;
    sBufT = HdfSbufObtainDefaultSize();
    char *buf = nullptr;
    uint32_t bufsize = 0;
    uint32_t frameSize = 0;
    if (sBufT == NULL) {
        return HDF_FAILURE;
    }

    ret = AdmRenderFramePrepare(path, buf, bufsize, frameSize);
    if (ret < 0) {
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(sBufT, frameSize)) {
        free(buf);
        buf = nullptr;
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(sBufT, buf, bufsize)) {
        free(buf);
        buf = nullptr;
        return HDF_FAILURE;
    }
    free(buf);
    buf = nullptr;
    return HDF_SUCCESS;
}

int32_t WriteToSBuf(struct HdfSBuf *&sBufT)
{
    sBufT = HdfSbufObtainDefaultSize();
    if (sBufT == NULL) {
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(sBufT, 0)) {
        HdfSbufRecycle(sBufT);
        sBufT = nullptr;
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t ObtainBuf(struct HdfSBuf *&readBuf, struct HdfSBuf *&readReply)
{
    readBuf = HdfSbufObtainDefaultSize();
    if (readBuf == nullptr) {
        return HDF_FAILURE;
    }
    readReply = HdfSbufObtainDefaultSize();
    if (readReply == nullptr) {
        HdfSbufRecycle(readBuf);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void RecycleBuf(struct HdfSBuf *&readBuf, struct HdfSBuf *&readReply)
{
    HdfSbufRecycle(readBuf);
    HdfSbufRecycle(readReply);
}

int32_t WriteCtrlInfo(struct HdfIoService *service, struct AudioCtlElemValue writeElemValue)
{
    int32_t ret = -1;
    struct HdfSBuf *writeBuf = nullptr;
    if (service == nullptr || service->dispatcher == nullptr) {
        return HDF_FAILURE;
    }

    writeBuf = HdfSbufObtainDefaultSize();
    if (writeBuf == nullptr) {
        return HDF_FAILURE;
    }
    ret = WriteEleValueToBuf(writeBuf, writeElemValue);
    if (ret < 0) {
        HdfSbufRecycle(writeBuf);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIODRV_CTRL_IOCTRL_ELEM_WRITE, writeBuf, nullptr);
    if (ret < 0) {
        HdfSbufRecycle(writeBuf);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(writeBuf);
    return HDF_SUCCESS;
}

int32_t ReadCtrlInfo(struct HdfIoService *service, struct AudioCtlElemId id, int32_t expectValue)
{
    int32_t ret = -1;
    struct HdfSBuf *readBuf = nullptr;
    struct HdfSBuf *readReply = nullptr;
    struct AudioCtlElemValue readElemValue = {};
    if (service == nullptr || service->dispatcher == nullptr) {
        return HDF_FAILURE;
    }
    ret = ObtainBuf(readBuf, readReply);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    ret = WriteIdToBuf(readBuf, id);
    if (ret < 0) {
        RecycleBuf(readBuf, readReply);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIODRV_CTRL_IOCTRL_ELEM_READ, readBuf, readReply);
    if (ret < 0) {
        RecycleBuf(readBuf, readReply);
        return HDF_FAILURE;
    }
    ret = HdfSbufReadInt32(readReply, &readElemValue.value[0]);
    if (ret < 0 || expectValue != readElemValue.value[0]) {
        RecycleBuf(readBuf, readReply);
        return HDF_FAILURE;
    }
    RecycleBuf(readBuf, readReply);
    return HDF_SUCCESS;
}
int32_t WriteHwParams(string serviceName, struct HdfIoService *&service, struct AudioPcmHwParams hwParams)
{
    int32_t ret = -1;
    int32_t cmdid = -1;
    struct HdfSBuf *writeBuf = nullptr;
    struct HdfSBuf *writeReply = nullptr;
    if (!strcmp(serviceName.c_str(), HDF_CAPTURE_SERVICE.c_str())) {
        cmdid = AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN;
    } else {
        cmdid = AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN;
    }
    service = HdfIoServiceBind(serviceName.c_str());
    if (service == nullptr || service->dispatcher == nullptr) {
        return HDF_FAILURE;
    }
    writeBuf = HdfSbufObtainDefaultSize();
    if (writeBuf == nullptr) {
        HdfIoServiceRecycle(service);
        service = nullptr;
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(writeBuf, CARD_SEVICE_NAME.c_str())) {
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdid, writeBuf, writeReply);
    if (ret < 0) {
        HdfSbufRecycle(writeBuf);
        HdfIoServiceRecycle(service);
        service = nullptr;
        return HDF_FAILURE;
    }
    HdfSbufFlush(writeBuf);
    ret = WriteHwParamsToBuf(writeBuf, hwParams);
    if (ret < 0) {
        HdfSbufRecycle(writeBuf);
        HdfIoServiceRecycle(service);
        service = nullptr;
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIO_DRV_PCM_IOCTRL_HW_PARAMS, writeBuf, writeReply);
    if (ret < 0) {
        HdfSbufRecycle(writeBuf);
        HdfIoServiceRecycle(service);
        service = nullptr;
        return HDF_FAILURE;
    }
    HdfSbufRecycle(writeBuf);
    return HDF_SUCCESS;
}
}
}
