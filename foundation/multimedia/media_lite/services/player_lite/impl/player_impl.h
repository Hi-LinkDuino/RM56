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

#ifndef PLAYER_IMPL_H
#define PLAYER_IMPL_H

#include <memory>
#include <vector>
#include <mutex>
#include <pthread.h>
#include "buffer_source.h"
#include "hi_demuxer.h"
#include "liteplayer.h"
#include "player_define.h"
#include "source.h"
#include "surface.h"
#include "player.h"
#include "player_interface.h"

using namespace std;
using OHOS::Media::PlayerControl;
namespace OHOS {
namespace Media {
typedef enum {
    PAYERADP_EVT_PREPARED = 0,
    PAYERADP_EVT_PLAY_COMPLETE = 1,
    PAYERADP_EVT_REWIND_COMPLETE = 2,
    PAYERADP_EVT_VIDEO_SOLUTION = 3,
    PAYERADP_EVT_VIDEO_PLAYED = 4,
    PAYERADP_EVT_PLAY_TIME = 5,
    PAYERADP_EVT_ERROR           = 6,
    PAYERADP_EVT_INFO            = 7,
} PlayAdapterEvt;

class AdapterStreamCallback : public StreamCallback {
public:
    AdapterStreamCallback(std::shared_ptr<StreamSource> &stream, std::shared_ptr<BufferSource> &buffer);
    virtual ~AdapterStreamCallback();
    uint8_t* GetBuffer(size_t index);
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags);
    void SetParameters(const Format &params);
    int Init(void);
    void DeInit(void);
private:
    static void* IdleBufferProcess(void* arg);

    std::weak_ptr<StreamSource> streamSource_;
    std::shared_ptr<BufferSource> bufferSource_;
    pthread_t streamProcess_;
    pthread_mutex_t mutex_;
    bool isRunning_;
};

class PlayerControl;
class PlayerImpl : public PlayerInterface {
friend class BufferSource;
friend class StreamSource;

public:
    PlayerImpl();
    ~PlayerImpl();
    int32_t SetSource(const Source &source) override;
    int32_t Prepare() override;
    int32_t Play() override;
    bool IsPlaying() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Rewind(int64_t mSeconds, int32_t mode) override;
    int32_t SetVolume(float leftVolume, float rightVolume) override;
    int32_t SetSurface(Surface *surface) override;
    bool IsSingleLooping() override;
    int32_t GetPlayerState(int32_t &state) override;
    int32_t GetCurrentPosition(int64_t &position) override;
    int32_t GetDuration(int64_t &durationMs) override;
    int32_t GetVideoWidth(int32_t &videoWidth) override;
    int32_t GetVideoHeight(int32_t &videoHeight) override;
    int32_t SetPlaybackSpeed(float speed) override;
    int32_t GetPlaybackSpeed(float &speed) override;
    int32_t SetAudioStreamType(int32_t type) override;
    void GetAudioStreamType(int32_t &type) override;
    int32_t Reset(void) override;
    int32_t Release() override;
    int32_t SetLoop(bool loop) override;
    void SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb) override;
    int32_t SetVideoScaleType(int32_t type);
    int32_t Init(void) override;
    int32_t DeInit(void) override;
    int32_t SetParameter(const Format &params) override;
private:
    void NotifySeekComplete(PlayerImpl *curPlayer, int64_t seekToMs);
    void NotifyPlaybackComplete(PlayerImpl *curPlayer);
    int32_t SetUriSource(const Source &source);
    int SetStreamSource(const Source &source);
    int32_t RewindInner(int64_t mSeconds, PlayerSeekMode mode);
    bool IsValidRewindMode(PlayerSeekMode mode);
    void UpdateState(PlayerImpl *curPlayer, PlayerStatus state);
    static void PlayerControlEventCb(void* pPlayer, PlayerControlEvent enEvent, const void* pData);
    int GetPlayer();
    static int32_t GetReadableSize(const void *handle);
    int32_t ReadDataPro(uint8_t *data, int32_t size, DataFlags &flags);
    static int32_t ReadData(void *handle, uint8_t *data, int32_t size, int32_t timeOutMs, DataFlags *flags);
    int CreatePlayerParamCheck(PlayerControlParam &createParam);
    void GetDurationInner(int64_t &durationMs);
    void ResetInner(void);
    int32_t EnablePauseAfterPlay(bool isPauseAfterPlay);
    int32_t SetMediaStream(void);
    void ReportVideoSizeChange(void);

    std::shared_ptr<PlayerControl> player_;
    float speed_;
    FormatFileInfo formatFileInfo_;
    PlayerControlStreamAttr mediaAttr_;
    PlayerStatus playerControlState_;

    bool isSingleLoop_;
    int64_t currentPosition_;
    int64_t rewindPosition_;
    Surface* surface_;
    uint32_t currentState_;
    PlayerSeekMode rewindMode_;
    PlayerSeekMode currentRewindMode_;
    int audioStreamType_;
    std::mutex lock_;
    std::mutex rewindLock_;
    std::shared_ptr<PlayerCallback> callback_;
    bool inited_;
    bool released_;
    bool isStreamSource_;
    QueBuffer buffer_;
    std::shared_ptr<BufferSource> bufferSource_;
    std::shared_ptr<AdapterStreamCallback> streamCallback_;
    bool pauseAfterPlay_;
    bool extraRewind_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_IMPL_H
