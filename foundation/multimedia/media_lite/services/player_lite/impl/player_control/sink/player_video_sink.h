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

#ifndef PLAYER_VIDEO_SINK_H
#define PLAYER_VIDEO_SINK_H

#include <memory>
#include <vector>
#include "player_sink_type.h"
#include "player_define.h"
#include "codec_type.h"
#include "player_sync.h"
#include <mutex>
#include "display_layer.h"

using namespace std;

namespace OHOS {
namespace Media {
class VideoSink {
public:
    VideoSink();
    ~VideoSink();
    int32_t Init(SinkAttr &attr);
    int32_t DeInit(void);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t Flush(void);
    int32_t SetSpeed(float speed);
    int32_t GetSpeed(float &speed);
    int32_t GetState(int32_t &state);
    int32_t RenderFrame(OutputInfo &frame);
    void SetRenderMode(RenderMode mode);
    void SetSync(PlayerSync *sync);
    int32_t SetParam(const char *key, dataType type, void* value);
    int32_t GetParam(const char *key, dataType type, void* value);
    int32_t RegisterCallBack(PlayEventCallback &callback);
    void GetStatus(VideoSinkStatus &status);
    void RenderEos(void);
    int DequeReleaseFrame(OutputInfo &frame);
    void GetRenderPosition(int64_t &position);

private:
    void ResetRendStartTime();
    int GetRenderFrame(OutputInfo &renderFrame, OutputInfo &frame);
    void ReleaseQueHeadFrame(void);
    void ReleaseQueAllFrame(void);
    void RenderRptEvent(EventCbType event);
    int32_t WriteToVideoDevice(OutputInfo &renderFrame);
    void QueueRenderFrame(OutputInfo &frame, bool cacheQueue);
    void CheckConfigVideoOutput(void);
    void SetDefaultDisplayRegionInfo(void);
    void UpdateDisplayRegionInfo(int32_t x, int32_t y, int32_t w, int32_t h);
    void CreateAndConfigLayer(void);

    SinkAttr attr_;
    float speed_;
    bool paused_;
    bool started_;
    PlayerSync *syncHdl_;
    uint64_t renderFrameCnt_;
    RenderMode renderMode_;
    int64_t rendStartTime_;
    int64_t lastRendPts_;
    int64_t lastRendSysTimeMs_;
    int32_t recievedEos_;
    int64_t EosPts_;
    bool pauseAfterPlay_;
    bool firstVidRend_;
    uint64_t lastRendCnt_;
    int64_t vidRendStartTime_;
    PlayEventCallback callBack_;
    bool eosSended_;
    std::mutex mutex_;
    LayerFuncs *layerFuncs_;
    std::vector<OutputInfo> frameCacheQue_;
    std::vector<OutputInfo> frameReleaseQue_;
    int32_t lastConfigRegionX_;
    int32_t lastConfigRegionY_;
    int32_t lastConfigRegionW_;
    int32_t lastConfigRegionH_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SINK_H
