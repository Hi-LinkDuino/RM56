/**
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
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_adapter.h
 *
 * @brief Declares APIs for operations related to the audio adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_hdi_common.h"

using namespace std;

static int g_frameStatus = 1;
static int g_writeCompleted = 0;
static int g_renderFull = 0;
static int g_flushCompleted = 0;
namespace HMOS {
namespace Audio {
int32_t InitAttrs(struct AudioSampleAttributes &attrs)
{
    attrs.format = AUDIO_FORMAT_PCM_16_BIT;
    attrs.channelCount = CHANNELCOUNT;
    attrs.sampleRate = SAMPLERATE;
    attrs.interleaved = 0;
    attrs.type = AUDIO_IN_MEDIA;
    attrs.period = DEEP_BUFFER_RENDER_PERIOD_SIZE;
    attrs.frameSize = PCM_16_BIT * CHANNELCOUNT / MOVE_LEFT_NUM;
    attrs.isBigEndian = false;
    attrs.isSignedData = true;
    attrs.startThreshold = DEEP_BUFFER_RENDER_PERIOD_SIZE / (PCM_16_BIT * attrs.channelCount / MOVE_LEFT_NUM);
    attrs.stopThreshold = INT_32_MAX;
    attrs.silenceThreshold = BUFFER_LENTH;
    return HDF_SUCCESS;
}
int32_t InitAttrsUpdate(struct AudioSampleAttributes &attrs, enum AudioFormat format, uint32_t channelCount,
    uint32_t sampleRate)
{
    InitAttrs(attrs);
    attrs.format = format;
    attrs.sampleRate = sampleRate;
    attrs.channelCount = channelCount;
    return HDF_SUCCESS;
}
int32_t AudioRenderSetGetSampleAttributes(struct AudioSampleAttributes attrs, struct AudioSampleAttributes &attrsValue,
    struct AudioRender *render)
{
    int32_t ret = -1;
    if (render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = render->attr.SetSampleAttributes(render, &attrs);
    if (ret < 0) {
        return ret;
    }
    ret = render->attr.GetSampleAttributes(render, &attrsValue);
    if (ret < 0) {
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}
int32_t AudioCaptureSetGetSampleAttributes(struct AudioSampleAttributes attrs, struct AudioSampleAttributes &attrsValue,
    struct AudioCapture *capture)
{
    int32_t ret = -1;
    if (capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = capture->attr.SetSampleAttributes(capture, &attrs);
    if (ret < 0) {
        return ret;
    }
    ret = capture->attr.GetSampleAttributes(capture, &attrsValue);
    if (ret < 0) {
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}
uint32_t StringToInt(std::string flag)
{
    uint32_t temp = flag[0];
    for (int i = flag.size() - 1; i >= 0; i--) {
        temp <<= MOVE_LEFT_NUM;
        temp += flag[i];
    }
    return temp;
}

int32_t InitDevDesc(struct AudioDeviceDescriptor &devDesc, const uint32_t portId, enum AudioPortPin pins)
{
    devDesc.portId = portId;
    devDesc.pins = pins;
    devDesc.desc = nullptr;
    return HDF_SUCCESS;
}

int32_t SwitchAdapter(struct AudioAdapterDescriptor *descs, const std::string &adapterNameCase,
    enum AudioPortDirection portFlag, struct AudioPort *&audioPort, int size)
{
    if (descs == nullptr || size > ADAPTER_COUNT) {
        return HDF_FAILURE;
    }

    for (int index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc == nullptr || desc->adapterName == nullptr) {
            continue;
        }
        if (strcmp(desc->adapterName, adapterNameCase.c_str())) {
            continue;
        }
        for (uint32_t port = 0; port < desc->portNum; port++) {
            if (desc->ports[port].dir == portFlag) {
                audioPort = &desc->ports[port];
                return index;
            }
        }
    }
    return HDF_FAILURE;
}

uint32_t PcmFormatToBits(enum AudioFormat format)
{
    switch (format) {
        case AUDIO_FORMAT_PCM_8_BIT:
            return PCM_8_BIT;
        case AUDIO_FORMAT_PCM_16_BIT:
            return PCM_16_BIT;
        case AUDIO_FORMAT_PCM_24_BIT:
            return PCM_24_BIT;
        case AUDIO_FORMAT_PCM_32_BIT:
            return PCM_32_BIT;
        default:
            return PCM_16_BIT;
    }
}

uint32_t PcmFramesToBytes(const struct AudioSampleAttributes attrs)
{
    if (attrs.channelCount < 1 || attrs.channelCount > 2) {
        return 0;
    }
    uint32_t formatBits = PcmFormatToBits(attrs.format);
    if (formatBits < PCM_8_BIT || formatBits > PCM_32_BIT) {
        return 0;
    }
    uint32_t ret = FRAME_SIZE * (attrs.channelCount) * (formatBits >> MOVE_RIGHT_NUM);
    return ret;
}

int32_t WavHeadAnalysis(struct AudioHeadInfo &wavHeadInfo, FILE *file, struct AudioSampleAttributes &attrs)
{
    size_t ret = 0;
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    ret = fread(&wavHeadInfo, sizeof(wavHeadInfo), 1, file);
    if (ret != 1) {
        return HDF_FAILURE;
    }
    uint32_t audioRiffId = StringToInt(AUDIO_RIFF);
    uint32_t audioFileFmt = StringToInt(AUDIO_WAVE);
    uint32_t audioDataId = StringToInt(AUDIO_DATA);
    if (wavHeadInfo.testFileRiffId != audioRiffId || wavHeadInfo.testFileFmt != audioFileFmt ||
        wavHeadInfo.dataId != audioDataId) {
        return HDF_FAILURE;
        }
    attrs.channelCount = wavHeadInfo.audioChannelNum;
    attrs.sampleRate = wavHeadInfo.audioSampleRate;
    switch (wavHeadInfo.audioBitsPerSample) {
        case PCM_8_BIT: {
            attrs.format = AUDIO_FORMAT_PCM_8_BIT;
            break;
        }
        case PCM_16_BIT: {
            attrs.format = AUDIO_FORMAT_PCM_16_BIT;
            break;
        }
        case PCM_24_BIT: {
            attrs.format = AUDIO_FORMAT_PCM_24_BIT;
            break;
        }
        case PCM_32_BIT: {
            attrs.format = AUDIO_FORMAT_PCM_32_BIT;
            break;
        }
        default:
            return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
int32_t GetAdapters(TestAudioManager *manager, struct AudioAdapterDescriptor **descs, int &size)
{
    int32_t ret = -1;
    if (descs == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = manager->GetAllAdapters(manager, descs, &size);
    if (ret < 0) {
        return ret;
    }
    if (*descs == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t GetLoadAdapter(TestAudioManager *manager, enum AudioPortDirection portType,
    const std::string &adapterName, struct AudioAdapter **adapter, struct AudioPort *&audioPort)
{
    int32_t ret = -1;
    int size = 0;
    struct AudioAdapterDescriptor *desc = nullptr;
    struct AudioAdapterDescriptor *descs = nullptr;
    if (adapter == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = GetAdapters(manager, &descs, size);
    if (ret < 0) {
        return ret;
    }
    if (descs == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }

    int index = SwitchAdapter(descs, adapterName, portType, audioPort, size);
    if (index < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    desc = &descs[index];
    if (desc == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = manager->LoadAdapter(manager, desc, adapter);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCreateRender(TestAudioManager *manager, enum AudioPortPin pins, const std::string &adapterName,
    struct AudioAdapter **adapter, struct AudioRender **render)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort *renderPort = nullptr;
    if (adapter == nullptr || render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = GetLoadAdapter(manager, PORT_OUT, adapterName, adapter, renderPort);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr || (*adapter)->CreateRender == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(attrs);
    InitDevDesc(devDesc, renderPort->portId, pins);
    ret = (*adapter)->CreateRender(*adapter, &devDesc, &attrs, render);
    if (ret < 0) {
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    if (*render == nullptr) {
        manager->UnloadAdapter(manager, *adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCreateStartRender(TestAudioManager *manager, struct AudioRender **render, struct AudioAdapter **adapter,
    const std::string &adapterName)
{
    int32_t ret = -1;
    enum AudioPortPin pins = PIN_OUT_SPEAKER;

    if (adapter == nullptr || render == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioCreateRender(manager, pins, adapterName, adapter, render);
    if (ret < 0) {
        return ret;
    }
    if (*render == nullptr || *adapter == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioRenderStartAndOneFrame(*render);
    if (ret < 0) {
        (*adapter)->DestroyRender(*adapter, *render);
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderStartAndOneFrame(struct AudioRender *render)
{
    int32_t ret = -1;
    char *frame = nullptr;
    uint64_t numRead = 0;
    uint64_t replyBytes = 0;
    if (render == nullptr || render->control.Start == nullptr || render->RenderFrame == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = render->control.Start((AudioHandle)render);
    if (ret) {
        return ret;
    }
    ret = RenderFramePrepare(AUDIO_FILE, frame, numRead);
    if (ret < 0) {
        if (frame != nullptr) {
            free(frame);
            frame = nullptr;
        }
        return HDF_FAILURE;
    }
    ret = render->RenderFrame(render, frame, numRead, &replyBytes);
    if (ret < 0) {
        if (frame != nullptr) {
            free(frame);
            frame = nullptr;
        }
        return ret;
    }
    free(frame);
    frame = nullptr;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCreateCapture(TestAudioManager *manager, enum AudioPortPin pins, const std::string &adapterName,
    struct AudioAdapter **adapter, struct AudioCapture **capture)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    struct AudioDeviceDescriptor devDesc = {};
    struct AudioPort *capturePort = nullptr;
    if (adapter == nullptr || capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = GetLoadAdapter(manager, PORT_IN, adapterName, adapter, capturePort);
    if (ret < 0) {
        return ret;
    }
    if (*adapter == nullptr || (*adapter)->CreateCapture == nullptr) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InitAttrs(attrs);
    InitDevDesc(devDesc, capturePort->portId, pins);
    ret = (*adapter)->CreateCapture(*adapter, &devDesc, &attrs, capture);
    if (ret < 0) {
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    if (*capture == nullptr) {
        manager->UnloadAdapter(manager, *adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCreateStartCapture(TestAudioManager *manager, struct AudioCapture **capture,
    struct AudioAdapter **adapter, const std::string &adapterName)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    enum AudioPortPin pins = PIN_IN_MIC;
    if (adapter == nullptr || capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioCreateCapture(manager, pins, adapterName, adapter, capture);
    if (ret < 0) {
        manager->UnloadAdapter(manager, *adapter);
        return ret;
    }
    if (*capture == nullptr || *adapter == nullptr) {
        manager->UnloadAdapter(manager, *adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    FILE *file = fopen(AUDIO_CAPTURE_FILE.c_str(), "wb+");
    if (file == nullptr) {
        (*adapter)->DestroyCapture(*adapter, *capture);
        manager->UnloadAdapter(manager, *adapter);
        return HDF_FAILURE;
    }
    InitAttrs(attrs);
    ret = FrameStartCapture((*capture), file, attrs);
    if (ret < 0) {
        (*adapter)->DestroyCapture(*adapter, *capture);
        manager->UnloadAdapter(manager, *adapter);
        fclose(file);
        return ret;
    }
    fclose(file);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioCaptureStartAndOneFrame(struct AudioCapture *capture)
{
    int32_t ret = -1;
    struct AudioSampleAttributes attrs = {};
    InitAttrs(attrs);
    FILE *file = fopen(AUDIO_CAPTURE_FILE.c_str(), "wb+");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    ret = FrameStartCapture(capture, file, attrs);
    if (ret < 0) {
        fclose(file);
        return ret;
    }
    fclose(file);
    return AUDIO_HAL_SUCCESS;
}

int32_t FrameStart(struct AudioHeadInfo wavHeadInfo, struct AudioRender *render, FILE *file,
    struct AudioSampleAttributes attrs)
{
    uint32_t readSize = 0;
    size_t numRead = 0;
    uint64_t replyBytes = 0;
    int32_t tryNumFrame = 0;
    if (render == nullptr || render->control.Start == nullptr || render->RenderFrame == nullptr || file == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = render->control.Start((AudioHandle)render);
    if (ret) {
        return ret;
    }
    uint32_t remainingDataSize = wavHeadInfo.dataSize;
    uint32_t bufferSize = PcmFramesToBytes(attrs);
    if (bufferSize <= 0) {
        return HDF_FAILURE;
    }
    char *frame = nullptr;
    frame = (char *)calloc(1, bufferSize);
    if (frame == nullptr) {
        return HDF_ERR_MALLOC_FAIL;
    }
    do {
        if (g_frameStatus) {
            readSize = (remainingDataSize) > (bufferSize) ? (bufferSize) : (remainingDataSize);
            numRead = fread(frame, readSize, 1, file);
            if (numRead > 0) {
                ret = render->RenderFrame(render, frame, readSize, &replyBytes);
                if (ret < 0) {
                    if (ret == -1) {
                        if (tryNumFrame > TRY_NUM_FRAME) {
                            free(frame);
                            return ret;
                        }
                        tryNumFrame++;
                        continue;
                    }
                    free(frame);
                    return ret;
                }
                tryNumFrame = 0;
            }
            remainingDataSize -= readSize;
        }
    } while (readSize > 0 && remainingDataSize > 0);
    free(frame);
    return AUDIO_HAL_SUCCESS;
}

int32_t FrameStartCapture(struct AudioCapture *capture, FILE *file, const struct AudioSampleAttributes attrs)
{
    int32_t ret = 0;
    uint32_t bufferSize = 0;
    uint64_t replyBytes = 0;
    uint64_t requestBytes = 0;
    if (capture == nullptr || capture->control.Start == nullptr || capture->CaptureFrame == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = capture->control.Start((AudioHandle)capture);
    if (ret < 0) {
        return ret;
    }
    uint32_t pcmBytes = PcmFramesToBytes(attrs);
    if (pcmBytes < 1024 || pcmBytes > 8192) {
        return HDF_FAILURE;
    }
    bufferSize = FRAME_COUNT * pcmBytes;
    if (bufferSize <= 0) {
        return HDF_FAILURE;
    }
    char *frame = nullptr;
    frame = (char *)calloc(1, bufferSize);
    if (frame == nullptr) {
        return HDF_ERR_MALLOC_FAIL;
    }
    requestBytes = bufferSize;
    ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
    if (ret < 0) {
        ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
        if (ret < 0) {
            free(frame);
            return ret;
        }
    }
    uint32_t requestByte = static_cast<uint32_t>(replyBytes);
    fwrite(frame, requestByte, 1, file);
    free(frame);
    return AUDIO_HAL_SUCCESS;
}

int32_t RenderFramePrepare(const std::string &path, char *&frame, uint64_t &readSize)
{
    int32_t ret = -1;
    size_t numRead = 0;
    uint32_t bufferSize = 4096;
    uint32_t remainingDataSize = 0;
    struct AudioSampleAttributes attrs = {};
    struct AudioHeadInfo headInfo = {};
    InitAttrs(attrs);
    char absPath[PATH_MAX] = {0};
    if (realpath(path.c_str(), absPath) == nullptr) {
        return HDF_FAILURE;
    }
    FILE *file = fopen(absPath, "rb");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    ret = WavHeadAnalysis(headInfo, file, attrs);
    if (ret < 0) {
        fclose(file);
        return HDF_FAILURE;
    }
    frame = (char *)calloc(1, bufferSize);
    if (frame == nullptr) {
        fclose(file);
        return HDF_ERR_MALLOC_FAIL;
    }
    remainingDataSize = headInfo.dataSize;
    readSize = (remainingDataSize) > (bufferSize) ? (bufferSize) : (remainingDataSize);
    size_t readSizes = static_cast<size_t>(readSize);
    numRead = fread(frame, readSizes, 1, file);
    if (numRead < 1) {
        free(frame);
        frame = nullptr;
        fclose(file);
        return HDF_FAILURE;
    }
    fclose(file);
    return HDF_SUCCESS;
}

void FrameStatus(int status)
{
    g_frameStatus = status;
    return;
}

int32_t StartRecord(struct AudioCapture *capture, FILE *file, uint64_t filesize)
{
    uint64_t replyBytes = 0;
    uint64_t requestBytes = BUFFER_LENTH;
    uint64_t totalSize = 0;
    int32_t tryNumFrame = 0;
    if (capture == nullptr || capture->control.Start == nullptr ||
        capture->CaptureFrame == nullptr || file == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = capture->control.Start((AudioHandle)capture);
    if (ret < 0) {
        return ret;
    }
    char *frame = (char *)calloc(1, BUFFER_LENTH);
    if (frame == nullptr) {
        return HDF_ERR_MALLOC_FAIL;
    }
    do {
        if (g_frameStatus) {
            ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
            if (ret < 0) {
                if (ret == -1) {
                    if (tryNumFrame++ > TRY_NUM_FRAME) {
                        free(frame);
                        frame = nullptr;
                        return ret;
                    }
                    continue;
                }
                free(frame);
                frame = nullptr;
                return ret;
            }
            tryNumFrame = 0;
            uint32_t replyByte = static_cast<uint32_t>(replyBytes);
            size_t writeRet = fwrite(frame, replyByte, 1, file);
            if (writeRet < 0) {
                free(frame);
                frame = nullptr;
                return HDF_FAILURE;
            }
            totalSize += replyBytes;
        } else {
            totalSize += 0;
        }
    } while (totalSize <= filesize * MEGABYTE);
    free(frame);
    frame = nullptr;
    return AUDIO_HAL_SUCCESS;
}

int32_t WriteIdToBuf(struct HdfSBuf *sBuf, struct AudioCtlElemId id)
{
    if (!HdfSbufWriteInt32(sBuf, id.iface)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, id.cardServiceName)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, id.itemName)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WriteEleValueToBuf(struct HdfSBuf *sBuf, struct AudioCtlElemValue elemvalue)
{
    int32_t ret = -1;
    if (sBuf == nullptr) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemvalue.value[0])) {
        return HDF_FAILURE;
    }
    ret = WriteIdToBuf(sBuf, elemvalue.id);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t ChangeRegisterStatus(struct AudioCtlElemValue elemValue)
{
    int32_t ret = -1;
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *reply = nullptr;
    struct HdfSBuf *sBuf = nullptr;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    if (service == nullptr || service->dispatcher == nullptr) {
        return HDF_FAILURE;
    }
    sBuf = HdfSbufObtainDefaultSize();
    if (sBuf == nullptr) {
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    ret = WriteEleValueToBuf(sBuf, elemValue);
    if (ret < 0) {
        HdfSbufRecycle(sBuf);
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIODRV_CTRL_IOCTRL_ELEM_WRITE, sBuf, reply);
    if (ret < 0) {
        HdfSbufRecycle(sBuf);
        HdfIoServiceRecycle(service);
        return ret;
    }
    HdfSbufRecycle(sBuf);
    HdfIoServiceRecycle(service);
    return HDF_SUCCESS;
}

int32_t QueryRegisterStatus(struct AudioCtlElemId id, struct AudioCtlElemValue &elemValue)
{
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBuf = nullptr;
    struct HdfSBuf *reply = nullptr;
    int32_t ret = -1;
    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    if (service == nullptr || service->dispatcher == nullptr) {
        return HDF_FAILURE;
    }
    sBuf = HdfSbufObtainDefaultSize();
    if (sBuf == nullptr) {
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    ret = WriteIdToBuf(sBuf, id);
    if (ret < 0) {
        HdfSbufRecycle(sBuf);
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    reply = HdfSbufObtainDefaultSize();
    if (reply == nullptr) {
        HdfSbufRecycle(sBuf);
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, AUDIODRV_CTRL_IOCTRL_ELEM_READ, sBuf, reply);
    if (ret < 0) {
        HdfSbufRecycle(sBuf);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        return ret;
    }
    if (!HdfSbufReadInt32(reply, &elemValue.value[0])) {
        HdfSbufRecycle(sBuf);
        HdfSbufRecycle(reply);
        HdfIoServiceRecycle(service);
        return HDF_FAILURE;
    }
    HdfSbufRecycle(sBuf);
    HdfSbufRecycle(reply);
    HdfIoServiceRecycle(service);
    return HDF_SUCCESS;
}

int32_t PowerOff(struct AudioCtlElemValue firstElemValue, struct AudioCtlElemValue secondElemValue)
{
    int32_t ret = -1;
    ret = ChangeRegisterStatus(firstElemValue);
    if (ret < 0) {
        return ret;
    }
    ret = ChangeRegisterStatus(secondElemValue);
    if (ret < 0) {
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t CheckRegisterStatus(const struct AudioCtlElemId firstId, const struct AudioCtlElemId secondId,
    const int firstStatus, const int secondStatus)
{
    int32_t ret = -1;
    struct AudioCtlElemValue elemValue[2] = {{}, {}};
    ret = QueryRegisterStatus(firstId, elemValue[0]);
    if (ret < 0) {
        return ret;
    }
    if (firstStatus != elemValue[0].value[0]) {
        return HDF_FAILURE;
    }
    ret = QueryRegisterStatus(secondId, elemValue[1]);
    if (ret < 0) {
        return ret;
    }
    if (secondStatus != elemValue[1].value[0]) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t StopAudio(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    if (audiopara.capture != nullptr) {
        ret = audiopara.capture->control.Stop((AudioHandle)(audiopara.capture));
        if (ret < 0) {
            audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
            audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
            audiopara.capture = nullptr;
            audiopara.adapter = nullptr;
            return ret;
        }
        if (audiopara.adapter != nullptr) {
            audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        }
        audiopara.capture = nullptr;
    }
    if (audiopara.render != nullptr) {
        ret = audiopara.render->control.Stop((AudioHandle)(audiopara.render));
        if (ret < 0) {
            audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
            audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
            audiopara.render = nullptr;
            audiopara.adapter = nullptr;
            return ret;
        }
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.render = nullptr;
    }
    if (audiopara.manager != nullptr && audiopara.adapter != nullptr) {
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.adapter = nullptr;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t ThreadRelease(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    pthread_join(audiopara.tids, &audiopara.result);
    ret = (intptr_t)audiopara.result;
    if (ret < 0) {
        StopAudio(audiopara);
        return ret;
    }
    ret = StopAudio(audiopara);
    if (ret < 0) {
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}
int32_t PlayAudioFile(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    char absPath[PATH_MAX] = {0};
    if (realpath(audiopara.path, absPath) == nullptr) {
        return HDF_FAILURE;
    }
    if (audiopara.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    FILE *file = fopen(absPath, "rb");
    if (file == nullptr) {
        return HDF_FAILURE;
    }
    if (WavHeadAnalysis(audiopara.headInfo, file, audiopara.attrs) < 0) {
        fclose(file);
        return HDF_FAILURE;
    }
    ret = AudioCreateRender(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                            &audiopara.render);
    if (ret < 0) {
        fclose(file);
        return ret;
    }
    if (audiopara.render == nullptr) {
        fclose(file);
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = FrameStart(audiopara.headInfo, audiopara.render, file, audiopara.attrs);
    if (ret == AUDIO_HAL_SUCCESS) {
        fclose(file);
    } else {
        audiopara.adapter->DestroyRender(audiopara.adapter, audiopara.render);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.render = nullptr;
        audiopara.adapter = nullptr;
        fclose(file);
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t RecordAudio(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    if (audiopara.manager == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioCreateCapture(audiopara.manager, audiopara.pins, audiopara.adapterName, &audiopara.adapter,
                             &audiopara.capture);
    if (ret < 0) {
        return ret;
    }
    if (audiopara.capture == nullptr) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    bool isMute = false;
    ret = audiopara.capture->volume.SetMute(audiopara.capture, isMute);
    if (ret < 0) {
        audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        return ret;
    }

    FILE *file = fopen(audiopara.path, "wb+");
    if (file == nullptr) {
        audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = StartRecord(audiopara.capture, file, audiopara.fileSize);
    if (ret < 0) {
        audiopara.adapter->DestroyCapture(audiopara.adapter, audiopara.capture);
        audiopara.manager->UnloadAdapter(audiopara.manager, audiopara.adapter);
        audiopara.capture = nullptr;
        audiopara.adapter = nullptr;
        fclose(file);
        return ret;
    }
    fclose(file);
    return AUDIO_HAL_SUCCESS;
}
int32_t InitMmapDesc(FILE *fp, struct AudioMmapBufferDescripter &desc, int32_t &reqSize, bool flag)
{
    if (fp == NULL) {
        return HDF_FAILURE;
    }
    int fd = fileno(fp);
    if (fd == -1) {
        return HDF_FAILURE;
    }
    if (flag) {
        struct AudioHeadInfo wavHeadInfo = {};
        fseek(fp, 0, SEEK_END);
        reqSize = ftell(fp);
        desc.offset = sizeof(wavHeadInfo);
    } else {
        reqSize = FILE_CAPTURE_SIZE;
        ftruncate(fd, FILE_CAPTURE_SIZE);
        desc.offset = 0;
    }
    desc.memoryFd = fd;
    desc.isShareable = 1;
    desc.transferFrameSize = DEEP_BUFFER_RENDER_PERIOD_SIZE / FRAME_COUNT;
    return HDF_SUCCESS;
}

int32_t PlayMapAudioFile(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    int32_t reqSize = 0;
    bool isRender = true;
    FrameStatus(1);
    struct AudioMmapBufferDescripter desc = {};
    if (audiopara.render == nullptr) {
        return HDF_FAILURE;
    }
    char absPath[PATH_MAX] = {0};
    if (realpath(audiopara.path, absPath) == nullptr) {
        return HDF_FAILURE;
    }
    FILE *fp = fopen(absPath, "rb+");
    if (fp == nullptr) {
        return HDF_FAILURE;
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    if (ret < 0) {
        fclose(fp);
        return HDF_FAILURE;
    }
    ret = audiopara.render->control.Start((AudioHandle)(audiopara.render));
    if (ret < 0) {
        fclose(fp);
        return ret;
    }
    ret = audiopara.render->attr.ReqMmapBuffer((AudioHandle)(audiopara.render), reqSize, &desc);
    if (ret == 0) {
        munmap(desc.memoryAddress, reqSize);
    }
    fclose(fp);
    return ret;
}
int32_t RecordMapAudio(struct PrepareAudioPara &audiopara)
{
    int32_t ret = -1;
    int32_t reqSize = 0;
    bool isRender = false;
    struct AudioMmapBufferDescripter desc = {};
    if (audiopara.capture == nullptr) {
        return HDF_FAILURE;
    }
    FILE *fp = fopen(audiopara.path, "wb+");
    if (fp == NULL) {
        return HDF_FAILURE;
    }
    ret = InitMmapDesc(fp, desc, reqSize, isRender);
    if (ret < 0) {
        fclose(fp);
        return HDF_FAILURE;
    }
    ret = audiopara.capture->control.Start((AudioHandle)(audiopara.capture));
    if (ret < 0) {
        fclose(fp);
        return ret;
    }
    ret = audiopara.capture->attr.ReqMmapBuffer((AudioHandle)(audiopara.capture), reqSize, &desc);
    fclose(fp);
    if (ret == 0) {
        munmap(desc.memoryAddress, reqSize);
    }
    return ret;
}
int32_t AudioRenderCallback(enum AudioCallbackType type, void *reserved, void *cookie)
{
    switch (type) {
        case AUDIO_NONBLOCK_WRITE_COMPELETED:
            g_writeCompleted = AUDIO_WRITE_COMPELETED_VALUE;
            return HDF_SUCCESS;
        case AUDIO_RENDER_FULL:
            g_renderFull = AUDIO_RENDER_FULL_VALUE;
            return HDF_SUCCESS;
        case AUDIO_FLUSH_COMPLETED:
            g_flushCompleted = AUDIO_FLUSH_COMPLETED_VALUE;
            return HDF_SUCCESS;
        case AUDIO_ERROR_OCCUR:
            return HDF_FAILURE;
        case AUDIO_DRAIN_COMPELETED:
            return HDF_FAILURE;
        default:
            return HDF_FAILURE;
    }
}
int32_t CheckWriteCompleteValue()
{
    if (g_writeCompleted == AUDIO_WRITE_COMPELETED_VALUE)
        return HDF_SUCCESS;
    else
        return HDF_FAILURE;
}
int32_t CheckRenderFullValue()
{
    if (g_renderFull == AUDIO_RENDER_FULL_VALUE)
        return HDF_SUCCESS;
    else
        return HDF_FAILURE;
}
int32_t CheckFlushValue()
{
    if (g_flushCompleted == AUDIO_FLUSH_COMPLETED_VALUE)
        return HDF_SUCCESS;
    else
        return HDF_FAILURE;
}
}
}