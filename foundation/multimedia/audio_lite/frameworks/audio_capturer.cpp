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

#include "audio_capturer.h"
#include "audio_capturer_client.h"
#include "media_log.h"

namespace OHOS {
namespace Audio {
#define CHK_NULL_RETURN(ptr, ret) \
    do { \
        if ((ptr) == nullptr) { \
            MEDIA_ERR_LOG("ptr null"); \
            return (ret); \
        } \
    } while (0)

AudioCapturer::AudioCapturer()
    : impl_(new(std::nothrow) AudioCapturerClient())
{
}

AudioCapturer::~AudioCapturer()
{
}

bool AudioCapturer::GetMinFrameCount(int32_t sampleRate, int32_t channelCount, AudioCodecFormat audioFormat,
                                     size_t &frameCount)
{
    return AudioCapturerClient::GetMinFrameCount(sampleRate, channelCount, audioFormat, frameCount);
}

uint64_t AudioCapturer::GetFrameCount()
{
    CHK_NULL_RETURN(impl_, 0);
    return impl_->GetFrameCount();
}

State AudioCapturer::GetStatus()
{
    CHK_NULL_RETURN(impl_, INITIALIZED);
    return impl_->GetStatus();
}

bool AudioCapturer::GetAudioTime(Timestamp &timestamp, Timestamp::Timebase base)
{
    CHK_NULL_RETURN(impl_, false);
    return impl_->GetAudioTime(timestamp, base);
}

int32_t AudioCapturer::SetCapturerInfo(const AudioCapturerInfo info)
{
    CHK_NULL_RETURN(impl_, -1);
    return impl_->SetCapturerInfo(info);
}

int32_t AudioCapturer::GetCapturerInfo(AudioCapturerInfo &info)
{
    CHK_NULL_RETURN(impl_, -1);
    return impl_->GetCapturerInfo(info);
}

bool AudioCapturer::Start()
{
    CHK_NULL_RETURN(impl_, false);
    return impl_->Start();
}

bool AudioCapturer::Stop()
{
    CHK_NULL_RETURN(impl_, false);
    return impl_->Stop();
}

bool AudioCapturer::Release()
{
    CHK_NULL_RETURN(impl_, false);
    return impl_->Release();
}

int32_t AudioCapturer::Read(uint8_t *buffer, size_t userSize, bool isBlockingRead)
{
    CHK_NULL_RETURN(impl_, -1);
    return impl_->Read(buffer, userSize, isBlockingRead);
}

}  // namespace Audio
}  // namespace OHOS
