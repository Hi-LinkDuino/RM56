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

#ifndef FRAMEWORKS_AUDIO_CAPTURER_THROUGH_H
#define FRAMEWORKS_AUDIO_CAPTURER_THROUGH_H

#include <mutex>
#include <sys/time.h>

#include "audio_capturer.h"
#include "audio_capturer_impl.h"

using OHOS::Audio::AudioCapturerImpl;

namespace OHOS {
namespace Audio {
class AudioCapturer::AudioCapturerClient {
public:
    AudioCapturerClient();
    virtual ~AudioCapturerClient();
    static bool GetMinFrameCount(int32_t sampleRate, int32_t channelCount, AudioCodecFormat audioFormat,
                                 size_t &frameCount);
    int32_t SetCapturerInfo(const AudioCapturerInfo info);
    int32_t GetCapturerInfo(AudioCapturerInfo &info);
    bool Start();
    bool Stop();
    bool Release();
    int32_t Read(uint8_t *buffer, size_t userSize, bool isBlockingRead);
    uint64_t GetFrameCount();
    State GetStatus();
    bool GetAudioTime(Timestamp &timestamp, Timestamp::Timebase base);

private:
    std::unique_ptr<AudioCapturerImpl> impl_;
};
}  // namespace Audio
}  // namespace OHOS

#endif  // FRAMEWORKS_AUDIO_CAPTURER_THROUGH_H
