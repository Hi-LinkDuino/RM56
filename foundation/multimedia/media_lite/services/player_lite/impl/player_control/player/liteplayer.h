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

#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include <string>
#include "hi_liteplayer.h"
#include "liteplayer_state_machine.h"
#include "player_define.h"
#include "player_source.h"
#include "player_sink_manager.h"
#include "hi_demuxer.h"
#include "decoder.h"
#include "surface.h"

namespace OHOS {
namespace Media {
class PlayerSource;
class SinkManager;
class Decoder;

struct PalayControlEventItem{
    EventCbType event;
};

class PlayerControl : public PlayerControlHandle {
public:
    friend class PlayerControlState;
    friend class PlayerControlSMObserver;

    PlayerControl();
    ~PlayerControl() override;

    int32_t Init(const PlayerControlParam &createParam);  // playercontrol create

    int32_t Deinit(void);  // playercontrol destroy

    PlayerStatus GetState(void);

    int32_t RegCallback(PlayerCtrlCallbackParam &eventCallback);

    int32_t SetDataSource(const std::string filepath);
    int32_t SetDataSource(const int32_t fd);
    int32_t SetDataSource(BufferStream &stream);

    int32_t Prepare(void);

    int32_t Play(void);

    int32_t SetVolume(VolumeAttr &volumeAttr);

    int32_t Stop(void);

    int32_t Pause(void);

    int32_t Seek(int64_t timeInMs);

    int32_t GetFileInfo(FormatFileInfo &formatInfo);

    int32_t SetMedia(PlayerControlStreamAttr &mediaAttr);

    int32_t SetSurface(Surface *surface);

    int32_t TPlay(TplayAttr tplayAttr);

    int32_t Invoke(PlayerInvoke invokeId, void *param);

    int32_t OnPlayControlEvent(void *priv, const EventCbType event);

    int32_t OnSwitchTPlay2Play(void);

    void OnVideoEndOfStream(void);
    void StateChangeCallback(PlayerStatus state) override;
    int32_t SetAudioStreamType(int32_t type);

protected:
    int32_t DoRegCallback(PlayerCtrlCallbackParam &observer) override;

    int32_t DoSetDataSource(const std::string filepath) override;
    int32_t DoSetDataSource(const int32_t fd) override;
    int32_t DoSetDataSource(BufferStream &stream)  override;
    int32_t DoPrepare(void) override;

    int32_t DoPlay(void) override;

    int32_t DoSetVolume(VolumeAttr &volumeAttr) override;

    int32_t ReadPacketAndPushToDecoder(void) override;

    int32_t DoStop(void) override;

    int32_t DoSeek(int64_t timeInMs) override;

    int32_t DoPause(void) override;

    int32_t DoGetFileInfo(FormatFileInfo &fileInfo) override;

    int32_t DoSetMedia(PlayerControlStreamAttr &mediaAttr) override;

    int32_t DoTPlay(TplayAttr& trickPlayAttr) override;

    int32_t DoInvoke(InvokeParameter& invokeParam) override;
    int32_t DoSetAudioStreamType(int32_t type) override;
    void ClearCachePacket(void);

    void NotifyError(PlayerControlError playerError) override;

    void EventCallback(PlayerControlEvent event, const void* data);

private:
    int32_t InitAttr(const PlayerControlParam &createParam);
    bool IsPlayEos(void);
    int32_t CheckMediaType(FormatFileInfo &fmtFileInfo);
    int32_t SyncPrepare(void);
    int32_t AyncSeek(int64_t seekTime);
    int32_t TPlayAfterFrameRead(FormatFrame& packet);
    int32_t TPlayBeforeFrameRead(void);
    int32_t TPlayResetBuffer(void);
    int32_t TPlayGetSeekOffset(float speed, TplayDirect direction);
    int32_t IsRepeatTplayReq(TplayAttr &tplayAttr, bool &isRepeat);
    PlayerTplayMode TPlayGetPlayMode(void);
    int32_t TPlayCheckContinueLost(void);
    bool TPlayIsFileReadEnd(void);
    int32_t ReadPacket(void);
    int32_t ReadTplayData(void);
    int32_t SeekInTplayMode(int64_t seekTimeInMs, FormatSeekMode seekFlag);
    void UpdateProgressNotify(void);
    void DealPlayEnd(void);
    int32_t ReadFrameFromSource(FormatFrame &fmtFrame);
    int32_t GetVideoResolution(int32_t streamIdx, StreamResolution &resolution);
    void DestroyDecoder(void);
    void StopSinkAndDecoder(void);
    void GetPlayElementEventCallBack(PlayEventCallback &callback);
    int32_t PauseResume(void);
    int32_t TPlayResume(void);
    int32_t DecoderAndSinkReset(void);
    int32_t GetStreamInfo(PlayerStreamInfo &streamInfo);
    int32_t SinkStart(void);
    int32_t DecoderStart(void);
    int32_t VideoDecoderStart(void);
    int32_t AudioDecoderStart(void);
    int32_t SetDecoderAndStreamAttr(void);
    int32_t CheckMediaInfo(void);
    int32_t AddAudioSink(void);
    int32_t AddVideoSink();
    int32_t SetVDecAttr(void);
    int32_t SetADecAttr(void);
    int32_t SetStreamAttr();
    void ReleaseADecoderOutputFrame(void);
    void ReleaseVDecoderOutputFrame(void);
    void RenderAudioFrame(void);
    void RenderVideoFrame(void);
    static void *DataSchProcess(void *priv);
    void ReortRenderPosition(void);
    void PushPacketToADecoder(void);
    void PushPacketToVDecoder(void);
    int32_t DoSeekIfNeed(void);
    void FlushDecoder(void);
    int32_t EnablePauseAfterPlay(bool pauseAfterPlay);
    void EventProcess(EventCbType event);
    void EventQueueProcess(void);

private:
    PlayerControlStateMachine *stateMachine_;
    PlayerControlSMObserver *observer_;
    bool isInited_;
    FormatFileInfo fmtFileInfo_;
    PlayerCtrlCallbackParam eventCallback_; /* the regeisted call back function */
    bool isNeedPause_; /* need to pause after first frame render */
    bool pauseMode_;
    bool isPlayEnd_;
    bool isVidPlayEos_;
    bool isAudPlayEos_;
    PlayerControlParam playerParam_;
    FormatFrame formatPacket_;
    bool cachedPacket_;
    int64_t lastRendPos_;
    uint64_t lastNotifyTime_;
    /* TPlay */
    PlayerTplayMode tplayMode_;
    TplayAttr tplayAttr_;
    uint32_t lastReadPktStrmIdx_;
    int64_t lastReadPktPts_;
    int64_t lastSendPktPts_;
    int32_t curSeekOffset_;
    bool isVideoStarted_;
    bool isAudioStarted_;
    bool isVidContinueLost_;
    bool strmReadEnd_;
    bool isPlayErr_;
    bool isTplayLastFrame_;
    bool isTplayStartRead_;
    int64_t lastSendVdecPts_;
    int64_t lastSendAdecPts_;
    int64_t currentPosition_;
    PlayControlStreamAttr streamAttr_;
    PlayControlVdecAttr vdecAttr_;
    PlayControlAdecAttr adecAttr_;
    bool paused_;
    bool schThreadExit_;
    bool loop_;
    bool hasRenderAudioEos_;
    bool hasRenderVideoEos_;
    uint32_t renderSleepTime_;
    float leftVolume_;
    float rightVolume_;
    pthread_mutex_t schMutex_;
    pthread_cond_t schCond_;
    pthread_t schProcess_;
    int64_t seekToTimeMs_;
    bool firstAudioFrameAfterSeek_;
    bool firstVideoFrameAfterSeek_;
    SourceType sourceType_;
    int32_t fd_;
    std::string filePath_;
    BufferStream stream_;
    std::shared_ptr<PlayerSource> playerSource_;
    std::shared_ptr<SinkManager> sinkManager_;
    std::shared_ptr<Decoder> audioDecoder_;
    std::shared_ptr<Decoder> videoDecoder_;
    Surface *surface_;
    std::vector<PalayControlEventItem> eventQueue;
    int32_t audioStreamType_;
    int64_t seekTabel_[0x2] = {-1, -1};
private:
    PlayerControl(const PlayerControl &);
    PlayerControl &operator=(const PlayerControl &);
};
}
}
#endif  // LITEPLAYER_H__
