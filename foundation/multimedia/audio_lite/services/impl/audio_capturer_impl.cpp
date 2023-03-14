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

#include "audio_capturer_impl.h"

#include <sys/select.h>

#include "audio_source.h"
#include "audio_encoder.h"
#include "media_log.h"

namespace OHOS {
namespace Audio {
using namespace OHOS::Media;

const uint64_t TIME_CONVERSION_US_S = 1000000; /* us to s */
const uint64_t TIME_CONVERSION_NS_US = 1000; /* ns  to us  */

#define CHK_NULL_RETURN(ptr, ret) \
    do { \
        if ((ptr) == nullptr) { \
            MEDIA_ERR_LOG("ptr null"); \
            return (ret); \
        } \
    } while (0)

AudioCapturerImpl::AudioCapturerImpl()
    :audioSource_(new(std::nothrow) AudioSource()),
     audioEncoder_(nullptr)
{
    MEDIA_DEBUG_LOG("ctor");
}

AudioCapturerImpl::~AudioCapturerImpl()
{
    if (status_ != RELEASED) {
        Release();
    }
    MEDIA_ERR_LOG("dtor");
}

bool AudioCapturerImpl::GetMinFrameCount(int32_t sampleRate, int32_t channelCount, AudioCodecFormat audioFormat,
    size_t &frameCount)
{
    return AudioSource::GetMinFrameCount(sampleRate, channelCount, audioFormat, frameCount);
}

uint64_t AudioCapturerImpl::GetFrameCount()
{
    CHK_NULL_RETURN(audioSource_, 0);
    pthread_mutex_lock(&mutex_);
    if (status_ == INITIALIZED || status_ == RELEASED) {
        MEDIA_ERR_LOG("check state:%u failed", status_);
        pthread_mutex_unlock(&mutex_);
        return 0;
    }
    pthread_mutex_unlock(&mutex_);
    return audioSource_->GetFrameCount();
}

State AudioCapturerImpl::GetStatus()
{
    pthread_mutex_lock(&mutex_);
    State state = status_;
    pthread_mutex_unlock(&mutex_);
    return state;
}

bool AudioCapturerImpl::GetTimestamp(Timestamp &timestamp, Timestamp::Timebase base)
{
    pthread_mutex_lock(&mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("check state:%u failed", status_);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    timestamp = timestamp_;
    pthread_mutex_unlock(&mutex_);
    return true;
}

static void FillSourceConfig(AudioSourceConfig &sourceConfig, const AudioCapturerInfo &info, uint32_t deviceId)
{
    sourceConfig.deviceId = deviceId;
    sourceConfig.audioFormat = info.audioFormat;
    sourceConfig.sampleRate = info.sampleRate;
    sourceConfig.channelCount = info.channelCount;
    sourceConfig.interleaved = false;
    sourceConfig.bitWidth = info.bitWidth;
    sourceConfig.streamUsage = info.streamType;

    /* BES specific */
    MEDIA_DEBUG_LOG("info.dmaBufferSize %d", info.dmaBufferSize);
    MEDIA_DEBUG_LOG("info.dataPullCb %p", info.dataPullCb);
    sourceConfig.dmaBufferSize = info.dmaBufferSize;
    sourceConfig.dataPullCb = info.dataPullCb;
    MEDIA_DEBUG_LOG("sourceConfig.dmaBufferSize %d", sourceConfig.dmaBufferSize);
    MEDIA_DEBUG_LOG("sourceConfig.dataPullCb %p", sourceConfig.dataPullCb);
}

static void FillEncConfig(AudioEncodeConfig &encodeConfig, const AudioCapturerInfo &info)
{
    encodeConfig.audioFormat = info.audioFormat;
    encodeConfig.bitRate = info.bitRate;
    encodeConfig.sampleRate = info.sampleRate;
    encodeConfig.channelCount = info.channelCount;
    encodeConfig.bitWidth = info.bitWidth;
}

int32_t AudioCapturerImpl::SetCapturerInfo(const AudioCapturerInfo info)
{
    CHK_NULL_RETURN(audioSource_, ERROR);
    pthread_mutex_lock(&mutex_);
    if (status_ != INITIALIZED) {
        MEDIA_ERR_LOG("check state:%u failed", status_);
        pthread_mutex_unlock(&mutex_);
        return ERR_ILLEGAL_STATE;
    }
    std::vector<AudioDeviceDesc> devices;
    int32_t ret = audioSource_->EnumDeviceBySourceType(info.inputSource, devices);
    if (ret != SUCCESS || devices.empty()) {
        MEDIA_ERR_LOG("EnumDeviceBySourceType failed inputSource:%d", info.inputSource);
        pthread_mutex_unlock(&mutex_);
        return ret;
    }
    MEDIA_INFO_LOG("info.sampleRate:%d", info.sampleRate);
    AudioSourceConfig sourceConfig;
    FillSourceConfig(sourceConfig, info, devices[0].deviceId);
    ret = audioSource_->Initialize(sourceConfig);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("Initialize failed inputSource:%d", info.inputSource);
        pthread_mutex_unlock(&mutex_);
        return ret;
    }
    if (info.audioFormat != PCM && info.audioFormat != AUDIO_DEFAULT) {
        AudioEncodeConfig encodeConfig;
        FillEncConfig(encodeConfig, info);
        MEDIA_INFO_LOG("audioEncoder_ bitRate:%d", info.bitRate);
        std::unique_ptr<AudioEncoder> audioEncoder(new(std::nothrow) AudioEncoder());
        audioEncoder_ = std::move(audioEncoder);
        if (audioEncoder_ == nullptr) {
            MEDIA_ERR_LOG("new AudioEncoder failed inputSource:%d", info.inputSource);
            pthread_mutex_unlock(&mutex_);
            return ERR_UNKNOWN;
        }
        ret = audioEncoder_->Initialize(encodeConfig);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("Initialize failed inputSource:%d", info.inputSource);
            (void)audioSource_->Release();
            pthread_mutex_unlock(&mutex_);
            return ret;
        }
    }
    info_ = info;
    status_ = PREPARED;
    MEDIA_INFO_LOG("Set Capturer Info SUCCESS");
    pthread_mutex_unlock(&mutex_);
    return SUCCESS;
}

int32_t AudioCapturerImpl::GetCapturerInfo(AudioCapturerInfo &info)
{
    pthread_mutex_lock(&mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("check state:%u failed", status_);
        pthread_mutex_unlock(&mutex_);
        return ERR_INVALID_OPERATION;
    }
    info = info_;
    pthread_mutex_unlock(&mutex_);
    return SUCCESS;
}

bool AudioCapturerImpl::Record()
{
    CHK_NULL_RETURN(audioSource_, false);
    pthread_mutex_lock(&mutex_);
    if (status_ != PREPARED && status_ != STOPPED) {
        MEDIA_ERR_LOG("not PREPARED or STOPPED status:%u", status_);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    int32_t ret = audioSource_->Start();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioSource_ Start failed:0x%x", ret);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    if (audioEncoder_ != nullptr) {
        uint32_t deviceId = 0;
        ret = audioSource_->GetCurrentDeviceId(deviceId);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("audioSource_ GetCurrentDevice failed:0x%x", ret);
            pthread_mutex_unlock(&mutex_);
            return false;
        }
        inputDeviceId_ = deviceId;
        ret = audioEncoder_->BindSource(deviceId);
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("audioEncoder_ BindSource failed:0x%x", ret);
            pthread_mutex_unlock(&mutex_);
            return false;
        }
        ret = audioEncoder_->Start();
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("audioEncoder_ Start failed:0x%x", ret);
            pthread_mutex_unlock(&mutex_);
            return false;
        }
    }
    status_ = RECORDING;
    MEDIA_INFO_LOG("Start Audio Capturer SUCCESS");
    pthread_mutex_unlock(&mutex_);
    return true;
}

int32_t AudioCapturerImpl::Read(uint8_t *buffer, size_t userSize, bool isBlockingRead)
{
    if (buffer == nullptr || !userSize) {
        MEDIA_ERR_LOG("Invalid buffer:%d userSize:%u", reinterpret_cast<void*>(buffer), userSize);
        return ERR_INVALID_READ;
    }
    CHK_NULL_RETURN(audioSource_, ERROR);

    if (status_ != RECORDING) {
        MEDIA_ERR_LOG("ILLEGAL_STATE  status_:%u", status_);
        return ERR_INVALID_READ;
    }
    int32_t readLen = ERR_INVALID_READ;
    if (info_.audioFormat == PCM || info_.audioFormat == AUDIO_DEFAULT) {
        AudioFrame frame;
        frame.buffer = buffer;
        frame.bufferLen = userSize;
        readLen = audioSource_->ReadFrame(frame, isBlockingRead);
        if (readLen == ERR_INVALID_READ) {
            MEDIA_ERR_LOG("audioSource_ ReadFrame fail,ret:0x%x", readLen);
            return ERR_INVALID_READ;
        }
        timestamp_.time.tv_sec = frame.time.tvSec;
        timestamp_.time.tv_nsec = frame.time.tvNSec;
    } else {
        AudioStream stream;
        stream.buffer = buffer;
        stream.bufferLen = userSize;

        if (audioEncoder_ == nullptr) {
            MEDIA_ERR_LOG("audioEncoder_ ReadStream fail, audioEncoder_ value is nullptr");
            return ERR_INVALID_READ;
        }

        readLen = audioEncoder_->ReadStream(stream, isBlockingRead);
        if (readLen == ERR_INVALID_READ) {
            MEDIA_ERR_LOG("audioEncoder_ ReadStream fail,ret:0x%x", readLen);
            return ERR_INVALID_READ;
        }
        timestamp_.time.tv_sec = static_cast<time_t>(stream.timeStamp / TIME_CONVERSION_US_S);
        timestamp_.time.tv_nsec = static_cast<time_t>((stream.timeStamp -
            timestamp_.time.tv_sec * TIME_CONVERSION_US_S) * TIME_CONVERSION_NS_US);
    }
    return readLen;
}

bool AudioCapturerImpl::StopInternal()
{
    CHK_NULL_RETURN(audioSource_, false);
    int32_t ret;
    if (audioEncoder_ != nullptr) {
        MEDIA_INFO_LOG("audioEncoder Stop");
        ret = audioEncoder_->Stop();
        if (ret != SUCCESS) {
            MEDIA_DEBUG_LOG("audioEncoder_ stop fail,ret:0x%x", ret);
            return false;
        }
    }
    MEDIA_INFO_LOG("audioSource Stop");
    ret = audioSource_->Stop();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioSource_ stop fail,ret:0x%x", ret);
        return false;
    }
    MEDIA_INFO_LOG("Stop Audio Capturer SUCCESS");
    status_ = STOPPED;
    return true;
}

bool AudioCapturerImpl::Stop()
{
    pthread_mutex_lock(&mutex_);
    if (status_ != RECORDING) {
        MEDIA_ERR_LOG("not RECORDING status:%u", status_);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    pthread_mutex_unlock(&mutex_);
    return StopInternal();
}

bool AudioCapturerImpl::Release()
{
    pthread_mutex_lock(&mutex_);
    if (status_ == RELEASED) {
        MEDIA_ERR_LOG("ILLEGAL_STATE status_:%u", status_);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    if (status_ == INITIALIZED) {
        status_ = RELEASED;
        pthread_mutex_unlock(&mutex_);
        return true;
    }
    if (status_ == RECORDING) {
        if (!StopInternal()) {
            MEDIA_ERR_LOG("StopInternal err");
            pthread_mutex_unlock(&mutex_);
            return false;
        }
    }
    int32_t ret;
    if (audioEncoder_ != nullptr) {
        ret = audioEncoder_->Release();
        if (ret != SUCCESS) {
            MEDIA_ERR_LOG("audioEncoder_ Release failed:0x%x", ret);
            pthread_mutex_unlock(&mutex_);
            return false;
        }
    }
    ret = (audioSource_ != nullptr) ? audioSource_->Release() : SUCCESS;
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("audioSource_ Release failed:0x%x", ret);
        pthread_mutex_unlock(&mutex_);
        return false;
    }
    status_ = RELEASED;
    MEDIA_INFO_LOG("Release Audio Capturer SUCCESS");
    pthread_mutex_unlock(&mutex_);
    return true;
}
}  // namespace Audio
}  // namespace OHOS
