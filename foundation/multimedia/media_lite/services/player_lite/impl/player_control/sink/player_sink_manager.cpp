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

#include "player_sink_manager.h"
#include <sys/time.h>
#include "media_log.h"

namespace OHOS {
namespace Media {

#define CHECK_FAILED_RETURN(value, target, ret, printfString) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG(" %s ", (printfString != nullptr) ? (printfString) : " "); \
        return (ret); \
    } \
} while (0)

SinkManager::SinkManager()
    : speed_(1.0f), direction_(TPLAY_DIRECT_BUTT),
      leftVolume_(0.0f), rightVolume_(0.0f), paused_(false), started_(false),
      pauseAfterPlay_(false), audioSinkNum_(0),
      videoSinkNum_(0), sync_(nullptr), recieveAudioEos_(false), recieveVideoEos_(false)
{
    callBack_.onEventCallback = nullptr;
    callBack_.priv = nullptr;
    for (int32_t i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        audioSinkInfo_[i].trackId = -1;
        audioSinkInfo_[i].sink = nullptr;
        videoSinkInfo_[i].trackId = -1;
        videoSinkInfo_[i].sink = nullptr;
    }
}

SinkManager::~SinkManager()
{
}

int32_t SinkManager::AddNewSink(SinkAttr &attr)
{
    int32_t ret;
    if (sync_ == nullptr) {
        sync_ = std::make_shared<PlayerSync>();
    }
    if (sync_ != nullptr) {
        sync_->Init();
    }

    if (attr.sinkType == SINK_TYPE_AUDIO) {
        if (audioSinkNum_ >= MAX_PIPELINE_SINK_NUM) {
            return -1;
        }
        audioSinkInfo_[audioSinkNum_].trackId = attr.trackId;
        audioSinkInfo_[audioSinkNum_].sink = std::make_shared<AudioSink>();
        ret = audioSinkInfo_[audioSinkNum_].sink->Init(attr);
        if (ret != 0) {
            audioSinkInfo_[audioSinkNum_].sink.reset();
            audioSinkInfo_[audioSinkNum_].sink = nullptr;
            MEDIA_ERR_LOG("audio sink Init failed:%d", ret);
            return -1;
        }
        audioSinkInfo_[audioSinkNum_].sink->SetSync(sync_.get());
        audioSinkNum_++;
    } else if (attr.sinkType == SINK_TYPE_VIDEO) {
        if (videoSinkNum_ >= MAX_PIPELINE_SINK_NUM) {
            return -1;
        }
        videoSinkInfo_[videoSinkNum_].trackId = attr.trackId;
        videoSinkInfo_[videoSinkNum_].sink = std::make_shared<VideoSink>();
        ret = videoSinkInfo_[videoSinkNum_].sink->Init(attr);
        if (ret != 0) {
            videoSinkInfo_[videoSinkNum_].sink.reset();
            videoSinkInfo_[videoSinkNum_].sink = nullptr;
            MEDIA_ERR_LOG("video sink Init failed:%d", ret);
            return -1;
        }
        videoSinkInfo_[videoSinkNum_].sink->SetSync(sync_.get());
        videoSinkNum_++;
    }
    return 0;
}


int32_t SinkManager::Start(void)
{
    if (videoSinkInfo_[0].sink != nullptr) {
        int32_t ret = videoSinkInfo_[0].sink->Start();
        CHECK_FAILED_RETURN(ret, SINK_SUCCESS, ret, "videoSinkInfo_ start failed");
        if (sync_ != nullptr) {
            sync_->Start(SYNC_CHN_VID);
        }
    }

    if (audioSinkInfo_[0].sink != nullptr) {
        int32_t ret = audioSinkInfo_[0].sink->Start();
        CHECK_FAILED_RETURN(ret, SINK_SUCCESS, ret, "audioSinkInfo_ start failed");
        if (sync_ != nullptr) {
            sync_->Start(SYNC_CHN_AUD);
        }
    }

    started_ = true;
    return HI_SUCCESS;
}

int32_t SinkManager::Flush(void)
{
    int32_t i;
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (videoSinkInfo_[i].sink != nullptr) {
            videoSinkInfo_[i].sink->Flush();
        }
    }

    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (audioSinkInfo_[i].sink != nullptr) {
            audioSinkInfo_[i].sink->Flush();
        }
    }
    return 0;
}

int32_t SinkManager::Reset(void)
{
    int32_t i;
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (videoSinkInfo_[i].sink != nullptr) {
            videoSinkInfo_[i].sink->Reset();
        }
    }

    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (audioSinkInfo_[i].sink != nullptr) {
            audioSinkInfo_[i].sink->Reset();
        }
    }

    recieveAudioEos_ = false;
    recieveVideoEos_ = false;
    if (sync_ != nullptr) {
        sync_->Reset(SYNC_CHN_VID);
        sync_->Reset(SYNC_CHN_AUD);
    }

    return 0;
}

int32_t SinkManager::Pause()
{
    if (paused_) {
        MEDIA_WARNING_LOG("sink already paused");
        return HI_SUCCESS;
    }
    if (!started_) {
        MEDIA_ERR_LOG("not in running");
        return -1;
    }

    if (audioSinkInfo_[0].sink != nullptr) {
        audioSinkInfo_[0].sink->Pause();
    }
    if (videoSinkInfo_[0].sink != nullptr) {
        videoSinkInfo_[0].sink->Pause();
    }

    if ((audioSinkInfo_[0].sink == nullptr || recieveAudioEos_ || speed_ != 1.0) &&
        (sync_ != nullptr)) {
        sync_->Reset(SYNC_CHN_VID);
    }
    if ((audioSinkInfo_[0].sink != nullptr) && (sync_ != nullptr)) {
        sync_->Reset(SYNC_CHN_AUD);
    }
    paused_ = true;
    return 0;
}

int32_t SinkManager::Resume()
{
    if (!paused_) {
        MEDIA_WARNING_LOG("sink not in pause");
        return HI_FAILURE;
    }

    if (audioSinkInfo_[0].sink != nullptr) {
        audioSinkInfo_[0].sink->Resume();
    }
    if (videoSinkInfo_[0].sink != nullptr) {
        videoSinkInfo_[0].sink->Resume();
    }

    pauseAfterPlay_ = false;
    paused_ = false;

    return 0;
}

int32_t SinkManager::TplayToNormal(void)
{
    int32_t ret;

    ret = Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("m_render pause  failed Ret: %d", ret);
        return ret;
    }
    ret = sync_->Resume();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_syncHdl Resume  failed Ret: %d", ret);
        return ret;
    }

    ret = sync_->Reset(SYNC_CHN_VID);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_syncHdl  Reset vid Ret: %d", ret);
        return ret;
    }
    ret = sync_->Reset(SYNC_CHN_AUD);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_syncHdl  Reset aud Ret: %d", ret);
        return ret;
    }
    ret = Resume();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_render resume  failed Ret: %d", ret);
        return ret;
    }
    speed_ = 1.0;
    direction_ = TPLAY_DIRECT_BUTT;
    return 0;
}

int32_t SinkManager::Tplay(float speed, TplayDirect  tplayDirect)
{
    int32_t ret;

    ret = Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("m_render pause  failed Ret: %d", ret);
        return ret;
    }
    ret = sync_->SetSpeed(speed, tplayDirect);
    if (ret != 0) {
        MEDIA_ERR_LOG("m_syncHdl  TPlay Ret: %d", ret);
        return ret;
    }

    ret = sync_->Reset(SYNC_CHN_VID);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_syncHdl  Reset vid Ret: %d", ret);
        return ret;
    }

    ret = Resume();
    if (ret != 0) {
        MEDIA_ERR_LOG("m_render resume  failed Ret: %d", ret);
        return ret;
    }
    speed_ = speed;
    direction_ = tplayDirect;
    return 0;
}

int32_t SinkManager::SetSpeed(float speed, TplayDirect  tplayDirect)
{
    if (speed_ == speed && tplayDirect == direction_) {
        return 0;
    }
    if (speed == 1.0) { /* tplay ---> normal */
        TplayToNormal();
    } else { /* tplay / ormal ---> tplay */
        Tplay(speed, tplayDirect);
    }
    return 0;
}

int32_t SinkManager::GetSpeed(float &speed, TplayDirect  &tplayDirect)
{
    speed = speed_;
    tplayDirect = direction_;
    return 0;
}

int32_t SinkManager::RenderFrame(OutputInfo &frame)
{
    int ret;
    if (frame.type == AUDIO_DECODER) {
        ret = audioSinkInfo_[0].sink->RenderFrame(frame);
    } else if (frame.type == VIDEO_DECODER) {
        ret = videoSinkInfo_[0].sink->RenderFrame(frame);
    } else {
        MEDIA_ERR_LOG("RenderFrame not support frame type: %d", frame.type);
        ret = -1;
    }
    return ret;
}

void SinkManager::SetRenderMode(RenderMode mode)
{
    if (videoSinkInfo_[0].sink != nullptr) {
        videoSinkInfo_[0].sink->SetRenderMode(mode);
    }
    if (audioSinkInfo_[0].sink != nullptr) {
        audioSinkInfo_[0].sink->SetRenderMode(mode);
    }
    pauseAfterPlay_ = (mode == RENDER_MODE_PAUSE_AFTER_PLAY) ? true : false;
}

int32_t SinkManager::SetVolume(float left, float right)
{
    int i;

    leftVolume_ = left;
    rightVolume_ = right;
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (audioSinkInfo_[i].sink != nullptr) {
            audioSinkInfo_[i].sink->SetVolume(left, right);
        }
    }
    return HI_SUCCESS;
}

int32_t SinkManager::GetVolume(float &left, float &right)
{
    left = leftVolume_;
    right = rightVolume_;
    return 0;
}


int32_t SinkManager::Stop(void)
{
    int32_t i;

    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (audioSinkInfo_[i].sink != nullptr) {
            audioSinkInfo_[i].sink->Stop();
        }
    }
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (videoSinkInfo_[i].sink != nullptr) {
            videoSinkInfo_[i].sink->Stop();
        }
    }
    if (sync_ != nullptr) {
        sync_->Stop(SYNC_CHN_VID);
        sync_->Stop(SYNC_CHN_AUD);
    }
    started_ = false;
    return 0;
}

int32_t SinkManager::RegisterCallBack(PlayEventCallback &callback)
{
    int32_t i;
    callBack_ = callback;
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (audioSinkInfo_[i].sink != nullptr) {
            audioSinkInfo_[i].sink->RegisterCallBack(callback);
        }
    }
    for (i = 0; i < MAX_PIPELINE_SINK_NUM; i++) {
        if (videoSinkInfo_[i].sink != nullptr) {
            videoSinkInfo_[i].sink->RegisterCallBack(callback);
        }
    }
    return 0;
}

int32_t SinkManager::GetStatus(PlayerStreamInfo &streamInfo)
{
    if (audioSinkInfo_[0].sink != nullptr) {
        AudioSinkStatus audioStatus;
        audioSinkInfo_[0].sink->GetStatus(audioStatus);
        streamInfo.avStatus.audFrameCount = audioStatus.audFrameCount;
    }
    if (videoSinkInfo_[0].sink != nullptr) {
        VideoSinkStatus videoStatus;
        videoSinkInfo_[0].sink->GetStatus(videoStatus);
        streamInfo.avStatus.vidFrameCount = videoStatus.vidFrameCount;
        streamInfo.videoInfo.decHeight = videoStatus.decHeight;
        streamInfo.videoInfo.decWidth = videoStatus.decWidth;
        streamInfo.videoInfo.fpsDecimal = videoStatus.fpsDecimal;
        streamInfo.videoInfo.fpsInteger = videoStatus.fpsInteger;
    }

    streamInfo.avStatus.syncStatus.lastAudPts = AV_INVALID_PTS;
    streamInfo.avStatus.syncStatus.lastVidPts = AV_INVALID_PTS;
    if (sync_ != nullptr) {
        sync_->GetStatus(streamInfo.avStatus.syncStatus);
    }
    return 0;
}

int32_t SinkManager::SetParam(const char *key, dataType type, void* value)
{
    return 0;
}

int32_t SinkManager::GetParam(const char *key, dataType *type, void** value, int32_t *size)
{
    return 0;
}

void SinkManager::RenderEos(bool isAudio)
{
    if (audioSinkInfo_[0].sink != nullptr && isAudio == true) {
        audioSinkInfo_[0].sink->RenderEos();
        recieveAudioEos_ = true;
    }
    if (videoSinkInfo_[0].sink != nullptr && isAudio == false) {
        videoSinkInfo_[0].sink->RenderEos();
        recieveVideoEos_ = true;
    }
}

int32_t SinkManager::DequeReleaseFrame(bool audioSink, OutputInfo &frame)
{
    if (audioSinkInfo_[0].sink != nullptr && audioSink == true) {
        return audioSinkInfo_[0].sink->DequeReleaseFrame(frame);
    }
    if (videoSinkInfo_[0].sink != nullptr && audioSink == false) {
        return videoSinkInfo_[0].sink->DequeReleaseFrame(frame);
    }
    return -1;
}

void SinkManager::GetRenderPosition(int64_t &position)
{
    if (audioSinkInfo_[0].sink == nullptr && videoSinkInfo_[0].sink == nullptr) {
        return;
    }

    /* use video position, if tplay or no audio */
    if ((speed_ != 1.0 && videoSinkInfo_[0].sink != nullptr) || audioSinkInfo_[0].sink == nullptr) {
        videoSinkInfo_[0].sink->GetRenderPosition(position);
        return;
    }
    /* audio not recieve eos, use audio position */
    if (!recieveAudioEos_ || videoSinkInfo_[0].sink == nullptr) {
        audioSinkInfo_[0].sink->GetRenderPosition(position);
        return;
    }

    /* use the max position of audio and video sink, if have recieved auduo eos */
    int64_t audioPos;
    int64_t videoPos;
    audioSinkInfo_[0].sink->GetRenderPosition(audioPos);
    videoSinkInfo_[0].sink->GetRenderPosition(videoPos);
    position = (audioPos >= videoPos) ? audioPos : videoPos;
}

void SinkManager::SetAudioStreamType(int32_t &type)
{
    if (audioSinkInfo_[0].sink == nullptr) {
        MEDIA_ERR_LOG("audioSinkInfo_[0].sink is null");
        return;
    }
    audioSinkInfo_[0].sink->SetAudioStreamType(type);
}
}
}
