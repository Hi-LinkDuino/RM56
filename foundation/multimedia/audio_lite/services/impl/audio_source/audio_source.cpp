/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "audio_source.h"
#include "media_log.h"
#include "securec.h"

namespace OHOS {
namespace Audio {
#define AUDIO_RETURN_VAL_IF_NULL(condition)                                            \
    do {                                                                               \
        if ((condition) == nullptr) {                                                  \
            MEDIA_ERR_LOG("" #condition " is NULL");    \
            return ERR_ILLEGAL_STATE;                                                  \
        }                                                                              \
    } while (0)

using namespace OHOS::Media;
static AudioManager *g_audioManager = nullptr;

AudioSource::AudioSource()
    : initialized_(false),
      started_(false),
      audioAdapter_(nullptr),
      audioCapture_(nullptr)
{
    if (g_audioManager == nullptr) {
        g_audioManager = GetAudioManagerFuncs();
        MEDIA_DEBUG_LOG("g_audioManager:%d", reinterpret_cast<void*>(&g_audioManager));
    }
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs = nullptr;
    g_audioManager->GetAllAdapters(g_audioManager, &descs, &size);
    MEDIA_DEBUG_LOG("GetAllAdapters size:%d", size);

    for (int index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        for (int port = 0; (desc != nullptr && port < static_cast<int>(desc->portNum)); port++) {
            if (desc->ports[port].dir == PORT_IN &&
                !(g_audioManager->LoadAdapter(g_audioManager, desc, &audioAdapter_))) {
                (void)audioAdapter_->InitAllPorts(audioAdapter_);
                if (memcpy_s(&capturePort_, sizeof(struct AudioPort),
                    &desc->ports[port], sizeof(struct AudioPort))) {
                    MEDIA_WARNING_LOG("memcpy_s capturePort_ failed");
                }
                break;
            }
        }
    }
    MEDIA_DEBUG_LOG("LoadAdapter audioAdapter_:%d", reinterpret_cast<void*>(&audioAdapter_));
}

AudioSource::~AudioSource()
{
    MEDIA_DEBUG_LOG("in");
    if (initialized_) {
        Release();
    }
    
    if (audioAdapter_ != nullptr) {
        MEDIA_INFO_LOG("UnloadModule audioAdapter_");
        if (g_audioManager == nullptr) {
            MEDIA_ERR_LOG("~AudioSource g_audioManager is nullptr");
            audioAdapter_ = nullptr;
            return;
        }
        g_audioManager->UnloadAdapter(g_audioManager, audioAdapter_);
        audioAdapter_ = nullptr;
    }
}

int32_t AudioSource::InitCheck()
{
    if (!initialized_) {
        MEDIA_ERR_LOG("not initialized");
        return ERR_ILLEGAL_STATE;
    }
    return SUCCESS;
}

bool AudioSource::GetMinFrameCount(int32_t sampleRate, int32_t channelCount,
                                   AudioCodecFormat audioFormat, size_t &frameCount)
{
    if (sampleRate <= 0 || channelCount <= 0 || audioFormat < AUDIO_DEFAULT ||
        audioFormat >= FORMAT_BUTT) {
        MEDIA_ERR_LOG("invalid params sampleRate:%d channelCount:%d audioFormat:%d", sampleRate,
                      channelCount, audioFormat);
        return false;
    }
    frameCount = 0;
    return true;
}

uint64_t AudioSource::GetFrameCount()
{
    int32_t ret;
    if ((ret = InitCheck()) != SUCCESS) {
        return ret;
    }
    AUDIO_RETURN_VAL_IF_NULL(audioCapture_);
    uint64_t frameCount = 0;
    ret = audioCapture_->attr.GetFrameCount(reinterpret_cast<AudioHandle>(audioCapture_), &frameCount);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("attr GetFrameCount failed:0x%x ", ret);
        return ret;
    }
    return frameCount;
}

int32_t AudioSource::EnumDeviceBySourceType(AudioSourceType inputSource, std::vector<AudioDeviceDesc> &devices)
{
    if (inputSource != AUDIO_MIC && inputSource != AUDIO_SOURCE_DEFAULT) {
        MEDIA_ERR_LOG("AudioSource only support AUDIO_MIC");
        return ERR_INVALID_PARAM;
    }
    AUDIO_RETURN_VAL_IF_NULL(audioAdapter_);

    struct AudioPortCapability capability;
    audioAdapter_->GetPortCapability(audioAdapter_, &capturePort_, &capability);
    AudioDeviceDesc deviceDesc;
    deviceDesc.deviceId = capability.deviceId;
    deviceDesc.inputSourceType = AUDIO_MIC;
    devices.push_back(deviceDesc);
    return SUCCESS;
}

static bool ConvertCodecFormatToAudioFormat(AudioCodecFormat codecFormat, AudioFormat *audioFormat)
{
    if (audioFormat == nullptr) {
        MEDIA_ERR_LOG("audioFormat is NULL");
        return false;
    }

    switch (codecFormat) {
        case AUDIO_DEFAULT:
        case PCM:
            *audioFormat = AUDIO_FORMAT_PCM_16_BIT;
            break;
        case AAC_LC:
            *audioFormat = AUDIO_FORMAT_AAC_LC;
            break;
        case AAC_LD:
            *audioFormat = AUDIO_FORMAT_AAC_LD;
            break;
        case AAC_ELD:
            *audioFormat = AUDIO_FORMAT_AAC_ELD;
            break;
        case AAC_HE_V1:
            *audioFormat = AUDIO_FORMAT_AAC_HE_V1;
            break;
        case AAC_HE_V2:
            *audioFormat = AUDIO_FORMAT_AAC_HE_V2;
            break;
        case G711A:
            *audioFormat = AUDIO_FORMAT_G711A;
            break;
        case G711U:
            *audioFormat = AUDIO_FORMAT_G711U;
            break;
        case G726:
            *audioFormat = AUDIO_FORMAT_G726;
            break;
        default: {
            MEDIA_ERR_LOG("not support this codecFormat:%d", codecFormat);
            return false;
        }
    }
    return true;
}

int32_t AudioSource::Initialize(const AudioSourceConfig &config)
{
    AUDIO_RETURN_VAL_IF_NULL(audioAdapter_);
    MEDIA_INFO_LOG("deviceId:0x%x config.sampleRate:%d", config.deviceId, config.sampleRate);
    struct AudioDeviceDescriptor desc;
    struct AudioSampleAttributes attrs;
    if (config.streamUsage == TYPE_MEDIA || config.streamUsage == TYPE_DEFAULT) {
        attrs.type = AUDIO_IN_MEDIA;
    } else if (config.streamUsage == TYPE_VOICE_COMMUNICATION) {
        attrs.type = AUDIO_IN_COMMUNICATION;
    }
    if (config.bitWidth != BIT_WIDTH_16) {
        MEDIA_ERR_LOG("not support bitWidth:%d, only support 16 bit width", config.bitWidth);
        return ERR_INVALID_PARAM;
    }
    if (!ConvertCodecFormatToAudioFormat(config.audioFormat, &(attrs.format))) {
        MEDIA_ERR_LOG("not support audioFormat:%d", config.audioFormat);
        return ERR_INVALID_PARAM;
    }
    attrs.sampleRate = config.sampleRate;
    attrs.channelCount = config.channelCount;
    attrs.interleaved = config.interleaved;
    attrs.capture_dma_size = config.dmaBufferSize;
    attrs.capture_pull_data_cb = (data_pull_push_cb_t)config.dataPullCb;
    MEDIA_DEBUG_LOG("attrs capture_dma_size %d, capture_pull_data_cb %p", attrs.capture_dma_size, attrs.capture_pull_data_cb);
    int32_t ret = audioAdapter_->CreateCapture(audioAdapter_, &desc, &attrs, &audioCapture_);
    if (ret != SUCCESS || audioCapture_ == nullptr) {
        MEDIA_ERR_LOG("CreateCapture failed:0x%x", ret);
        return ret;
    }
    initialized_ = true;
    return SUCCESS;
}

int32_t AudioSource::SetInputDevice(uint32_t deviceId)
{
    (void)deviceId;
    return SUCCESS;
}

int32_t AudioSource::GetCurrentDeviceId(uint32_t &deviceId)
{
    AUDIO_RETURN_VAL_IF_NULL(audioCapture_);
    int32_t ret = audioCapture_->attr.GetCurrentChannelId(reinterpret_cast<AudioHandle>(audioCapture_), &deviceId);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("GetCurrentChannelId failed:0x%x", ret);
        return ret;
    }
    MEDIA_INFO_LOG("deviceId:0x%x", deviceId);
    return SUCCESS;
}

int32_t AudioSource::Start()
{
    int32_t ret;
    if ((ret = InitCheck()) != SUCCESS) {
        return ret;
    }

    AUDIO_RETURN_VAL_IF_NULL(audioCapture_);
    ret = audioCapture_->control.Start(reinterpret_cast<AudioHandle>(audioCapture_));
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioCapture_ Start failed:0x%x", ret);
        return ret;
    }
    started_ = true;
    return SUCCESS;
}

int32_t AudioSource::ReadFrame(AudioFrame &frame, bool isBlockingRead)
{
    if (!started_) {
        MEDIA_ERR_LOG("AudioSource not Start");
        return ERR_ILLEGAL_STATE;
    }
    AUDIO_RETURN_VAL_IF_NULL(audioCapture_);
    uint64_t readlen = ERR_INVALID_READ;
    int32_t ret = audioCapture_->CaptureFrame(audioCapture_, frame.buffer, frame.bufferLen, &readlen);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioCapture_::CaptureFrame failed:0x%x", ret);
        return ERR_INVALID_READ;
    }
    ret = audioCapture_->GetCapturePosition(audioCapture_, &frame.frames, &frame.time);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioCapture_::GetCapturePosition failed:0x%x", ret);
        return ERR_INVALID_READ;
    }
    return readlen;
}

int32_t AudioSource::Stop()
{
    MEDIA_INFO_LOG("AudioSource::Stop");
    if (!started_) {
        MEDIA_ERR_LOG("AudioSource not Start");
        return ERR_ILLEGAL_STATE;
    }

    AUDIO_RETURN_VAL_IF_NULL(audioCapture_);
    int32_t ret = audioCapture_->control.Stop(reinterpret_cast<AudioHandle>(audioCapture_));
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("Stop failed:0x%x", ret);
        return ret;
    }
    started_ = false;
    return SUCCESS;
}

int32_t AudioSource::Release()
{
    int32_t ret;
    if ((ret = InitCheck()) != SUCCESS) {
        return ret;
    }
    if (audioCapture_) {
        audioAdapter_->DestroyCapture(audioAdapter_, audioCapture_);
        audioCapture_ = nullptr;
    }
    initialized_ = false;
    MEDIA_INFO_LOG("AudioSource Released");
    return SUCCESS;
}
}  // namespace Audio
}  // namespace OHOS
