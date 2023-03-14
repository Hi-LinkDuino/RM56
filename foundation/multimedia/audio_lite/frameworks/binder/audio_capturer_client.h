/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_AUDIO_CAPTURER_CLIENT_H
#define FRAMEWORKS_AUDIO_CAPTURER_CLIENT_H

#include <mutex>
#include "audio_capturer.h"
#include "iproxy_client.h"
#include "media_info.h"
#include "surface.h"

using OHOS::Surface;
using std::mutex;
using OHOS::SurfaceBuffer;
using OHOS::IBufferConsumerListener;
using OHOS::Audio::Timestamp;
using OHOS::Audio::AudioCapturerInfo;

namespace OHOS {
namespace Audio {
class AudioCapturer::AudioCapturerClient : public IBufferConsumerListener {
public:
    AudioCapturerClient();
    ~AudioCapturerClient();
    static bool GetMinFrameCount(int32_t sampleRate, int32_t channelCount, AudioCodecFormat audioFormat,
                                size_t &frameCount);
    uint64_t GetFrameCount();
    int32_t SetCapturerInfo(const AudioCapturerInfo info);
    int32_t GetCapturerInfo(AudioCapturerInfo &info);
    bool Start();
    int32_t Read(uint8_t *buffer, size_t userSize, bool isBlockingRead);
    State GetStatus();
    bool GetAudioTime(Timestamp &timestamp, Timestamp::Timebase base);
    bool Stop();
    bool Release();
    void OnBufferAvailable() override;
    static AudioCapturerClient *GetInstance();

private:
    bool InitAudioCapturerClient();
    IClientProxy *GetIClientProxy();
    int32_t InitSurface(void);
    int32_t DeleteSurface(void);
    void ReleaseAllBuffer();

    SvcIdentity sid_;
    IClientProxy *proxy_ = nullptr;
    std::shared_ptr<Surface> surface_;
    std::mutex lock_;
    Timestamp curTimestamp_;
    bool timeStampValid_ = false;
};
}  // namespace Audio
}  // namespace OHOS

#endif // FRAMEWORKS_AUDIO_CAPTURER_CLIENT_H
