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

#ifndef PLAYER_AUDIO_SINK_H
#define PLAYER_AUDIO_SINK_H

#include <memory>
#include <vector>
#include <mutex>
#include "player_sink_type.h"
#include "player_define.h"
#include "codec_type.h"
#include "player_sync.h"
#include "audio_manager.h"

using namespace std;

namespace OHOS {
namespace Media {
class PlayerSync;
class AudioSink {
public:
    AudioSink();
    ~AudioSink();
    int32_t Init(SinkAttr &atrr);
    void DeInit(void);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Flush(void);
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t SetSpeed(float speed);
    int32_t GetSpeed(float &speed);
    int32_t GetState(int32_t &state);
    int32_t RenderFrame(OutputInfo &frame);
    void SetRenderMode(RenderMode mode);
    int32_t SetVolume(float left, float right);
    int32_t GetVolume(float &left, float &right);
    int32_t SetMute(bool mute);
    int32_t GetMute(bool &mute);
    void SetSync(PlayerSync *sync);
    int32_t SetParam(const char *key, dataType type, void* value);
    int32_t GetParam(const char *key, dataType type, void* value);
    int32_t RegisterCallBack(PlayEventCallback &callback);
    void GetStatus(AudioSinkStatus &status);
    void RenderEos(void);
    int DequeReleaseFrame(OutputInfo &frame);
    void GetRenderPosition(int64_t &position);
    void SetAudioStreamType(int32_t &type);

private:
    void ResetRendStartTime();
    void SendAudioEndOfStream();
    void UpdateAudioPts(int64_t lastPts, int64_t& timestamp, OutputInfo &renderFrame);
    int GetRenderFrame(OutputInfo &renderFrame, const OutputInfo &frame);
    void ReleaseQueHeadFrame(void);
    void ReleaseQueAllFrame(void);
    void RenderRptEvent(EventCbType event);
    int32_t WriteToAudioDevice(OutputInfo &renderFrame);
    void QueueRenderFrame(const OutputInfo &frame, bool cacheQueue);

    bool started_;
    bool paused_;
    bool eosSended_;
    uint64_t rendFrameCnt_;
    uint64_t lastRendFrameCnt_;
    bool pauseAfterPlay_;
    SinkAttr attr_;
    PlayerSync *syncHdl_;
    RenderMode renderMode_;
    int64_t rendStartTime_;
    int64_t lastRendPts_;
    int64_t lastRendSysTimeMs_;
    uint32_t renderDelay_;
    PlayEventCallback callBack_;
    float leftVolume_;
    float rightVolume_;
    int64_t eosPts_;
    bool receivedEos_;
    std::mutex mutex_;
    std::vector<OutputInfo> frameCacheQue_;
    std::vector<OutputInfo> frameReleaseQue_;
    struct AudioManager *audioManager_;
    struct AudioAdapter *audioAdapter_;
    struct AudioRender *audioRender_;
    AudioPort renderPort_;
    bool reportedFirstFrame;
    int32_t audioStreamType_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SINK_H
