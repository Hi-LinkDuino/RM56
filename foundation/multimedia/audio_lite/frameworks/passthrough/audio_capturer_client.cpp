/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

// #include "audio_capturer.h"
#include "audio_capturer_client.h"
#include "media_log.h"

namespace OHOS {
namespace Audio {
AudioCapturer::AudioCapturerClient::AudioCapturerClient()
    : impl_(new(std::nothrow) AudioCapturerImpl())
{
}

AudioCapturer::AudioCapturerClient::~AudioCapturerClient()
{
    if (impl_ != nullptr) {
        impl_.reset();
        impl_ = nullptr;
    }
    MEDIA_DEBUG_LOG("ctor");
}

bool AudioCapturer::AudioCapturerClient::GetMinFrameCount(int32_t sampleRate, int32_t channelCount,
                                                          AudioCodecFormat audioFormat, size_t &frameCount)
{
    return AudioCapturerImpl::GetMinFrameCount(sampleRate, channelCount, audioFormat, frameCount);
}

uint64_t AudioCapturer::AudioCapturerClient::GetFrameCount()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return 0;
    }
    return impl_->GetFrameCount();
}

State AudioCapturer::AudioCapturerClient::GetStatus()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return INITIALIZED;
    }
    return impl_->GetStatus();
}

bool AudioCapturer::AudioCapturerClient::GetAudioTime(Timestamp &timestamp, Timestamp::Timebase base)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return false;
    }
    return impl_->GetTimestamp(timestamp, base);
}

int32_t AudioCapturer::AudioCapturerClient::SetCapturerInfo(const AudioCapturerInfo info)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetCapturerInfo(info);
}

int32_t AudioCapturer::AudioCapturerClient::GetCapturerInfo(AudioCapturerInfo &info)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetCapturerInfo(info);
}

bool AudioCapturer::AudioCapturerClient::Start()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return false;
    }
    return impl_->Record();
}

bool AudioCapturer::AudioCapturerClient::Stop()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return false;
    }
    return impl_->Stop();
}

bool AudioCapturer::AudioCapturerClient::Release()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return false;
    }
    return impl_->Release();
}

int32_t AudioCapturer::AudioCapturerClient::Read(uint8_t *buffer, size_t userSize, bool isBlockingRead)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Read(buffer, userSize, isBlockingRead);
}
}  // namespace Audio
}  // namespace OHOS
