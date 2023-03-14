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

#include "player_audio_sink.h"
#include "player_sync.h"
#include "media_log.h"
#include <unistd.h>
#include <cmath>
#include <sys/time.h>

namespace OHOS {
namespace Media {
#define CHECK_FAILED_RETURN(value, target, ret, printfString) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG(" %s ", (printfString != nullptr) ? (printfString) : " "); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_RETURN(value, ret, printfString) \
do { \
    if ((value) == nullptr) { \
        MEDIA_ERR_LOG(" %s ", (printfString != nullptr) ? (printfString) : " "); \
        return (ret); \
    } \
} while (0)

const int32_t HALF_FACTOR = 2;
const int32_t MAX_QUEUE_BUF_NUM = 32;
const int32_t MAX_AUDIO_ADAPTER_NUM = 2;
const int32_t AUDIO_SAMPLE_WIDTH_BYTE = 2;

AudioSink::AudioSink()
    : started_(false), paused_(false), eosSended_(false), rendFrameCnt_(0), lastRendFrameCnt_(0),
      pauseAfterPlay_(false), syncHdl_(nullptr), renderMode_(RENDER_MODE_NORMAL),
      rendStartTime_(-1), lastRendPts_(AV_INVALID_PTS), lastRendSysTimeMs_(-1), renderDelay_(0),
      leftVolume_(0.0f), rightVolume_(0.0f), eosPts_(AV_INVALID_PTS), receivedEos_(false), audioManager_(nullptr),
      audioAdapter_(nullptr), audioRender_(nullptr), reportedFirstFrame(false), audioStreamType_(0)
{
    ResetRendStartTime();
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
    callBack_.onEventCallback = nullptr;
    callBack_.priv= nullptr;
    attr_.sinkType = SINK_TYPE_BUT;
    attr_.trackId = 0;
    attr_.audAttr.format = 0;
    attr_.audAttr.sampleFmt = 0;
    attr_.audAttr.sampleRate = 0;
    attr_.audAttr.channel = 0;
    attr_.audAttr.volume = 0.0;
    renderPort_.dir = PORT_OUT;
    renderPort_.portId = 0;
    renderPort_.portName = nullptr;
}

AudioSink::~AudioSink()
{
    DeInit();
}

void AudioSink::DeInit()
{
    started_ = false;
    if (frameCacheQue_.size() != 0 || frameReleaseQue_.size()) {
        MEDIA_ERR_LOG("frames should be released before DeInit!!!");
    }
    if (audioAdapter_ != nullptr && audioRender_ != nullptr) {
        audioAdapter_->DestroyRender(audioAdapter_, audioRender_);
    }
    if (audioManager_ != nullptr) {
        audioManager_->UnloadAdapter(audioManager_, audioAdapter_);
    }
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
}

int32_t AudioSink::Init(SinkAttr &atrr)
{
    attr_ = atrr;
    audioManager_ = GetAudioManagerFuncs();
    if (audioManager_ == nullptr) {
        return SINK_GET_DEVICE_FAILED;
    }
    int32_t ret = 0;
    int32_t size = -1;
    struct AudioAdapterDescriptor *descs = nullptr;
    audioManager_->GetAllAdapters(audioManager_, &descs, &size);
    if (size > MAX_AUDIO_ADAPTER_NUM) {
        return SINK_GET_DEVICE_FAILED;
    }
    for (int index = 0; index < size; index++) {
        struct AudioAdapterDescriptor *desc = &descs[index];
        for (int port = 0; (desc != nullptr && port < static_cast<int>(desc->portNum)); port++) {
            if (desc->ports[port].dir == PORT_OUT &&
                (audioManager_->LoadAdapter(audioManager_, desc, &audioAdapter_)) == 0) {
                (void)audioAdapter_->InitAllPorts(audioAdapter_);
                if (memcpy_s(&renderPort_, sizeof(struct AudioPort),
                    &desc->ports[port], sizeof(struct AudioPort)) != 0) {
                    MEDIA_ERR_LOG("memcpy_s failed");
                }
                break;
            }
        }
    }
    if (ret != 0 || audioAdapter_ == nullptr) {
        MEDIA_ERR_LOG("load audiodevice failed");
        return SINK_LAOD_DEVICE_FAILED;
    }

    struct AudioSampleAttributes param;
    param.sampleRate = attr_.audAttr.sampleRate;
    param.format = AUDIO_FORMAT_PCM_16_BIT;
    param.channelCount = attr_.audAttr.channel;
    param.interleaved = false;
    param.type = (audioStreamType_ == 1) ? AUDIO_IN_COMMUNICATION : AUDIO_IN_MEDIA;
    MEDIA_INFO_LOG("sampleRate:%d, channelCount:%d", param.sampleRate, param.channelCount);

    struct AudioDeviceDescriptor deviceDesc;
    deviceDesc.portId = 0;
    deviceDesc.pins = PIN_OUT_SPEAKER;
    deviceDesc.desc = nullptr;
    ret = audioAdapter_->CreateRender(audioAdapter_, &deviceDesc, &param, &audioRender_);
    if (ret != 0 || audioRender_ == nullptr) {
        MEDIA_ERR_LOG("AudioDeviceCreateRender failed");
        return SINK_OPEN_STREAM_FAILED;
    }
    MEDIA_DEBUG_LOG("init success");
    return SINK_SUCCESS;
}

void AudioSink::ResetRendStartTime()
{
    lastRendFrameCnt_ = 0;
    lastRendPts_ = AV_INVALID_PTS;
    rendStartTime_ = AV_INVALID_PTS;
}

void AudioSink::GetStatus(AudioSinkStatus &status)
{
    status.audFrameCount = rendFrameCnt_;
}

void AudioSink::UpdateAudioPts(int64_t lastPts, int64_t &timestamp, OutputInfo &renderFrame)
{
    if (renderFrame.timeStamp == -1 && attr_.audAttr.channel != 0) {
        int32_t sampleCnt = (renderFrame.buffers[0].length / attr_.audAttr.channel) / AUDIO_SAMPLE_WIDTH_BYTE;
        int64_t duration = (static_cast<int64_t>(sampleCnt) * MS_SCALE) / attr_.audAttr.sampleRate;
        renderFrame.timeStamp = lastPts + duration;
    }
    timestamp = renderFrame.timeStamp;
    renderDelay_ = 0;
    if (audioRender_->GetLatency(audioRender_, &renderDelay_) == HI_SUCCESS) {
        timestamp -= renderDelay_;
    }
}

void AudioSink::QueueRenderFrame(const OutputInfo &frame, const bool cacheQueue)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frame.type != AUDIO_DECODER || frame.bufferCnt == 0) {
        return;
    }
    if (cacheQueue) {
        frameCacheQue_.push_back(frame);
    } else {
        frameReleaseQue_.push_back(frame);
    }
}

int32_t AudioSink::GetRenderFrame(OutputInfo &renderFrame, const OutputInfo &frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = SINK_QUE_EMPTY;
    if (frame.type == AUDIO_DECODER &&  frame.bufferCnt != 0) {
        frameCacheQue_.push_back(frame);
    }
    if (frameCacheQue_.size() != 0) {
        renderFrame = frameCacheQue_[0];
        ret = SINK_SUCCESS;
    }
    return ret;
}

void AudioSink::ReleaseQueHeadFrame(void)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameCacheQue_.size() != 0) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
    }
}

void AudioSink::ReleaseQueAllFrame(void)
{
    size_t i;
    size_t queSize;
    std::lock_guard<std::mutex> lock(mutex_);
    queSize = frameCacheQue_.size();
    if (queSize > MAX_QUEUE_BUF_NUM) {
        return;
    }
    for (i = 0; i < queSize; i++) {
        frameReleaseQue_.push_back(frameCacheQue_[i]);
    }
    frameCacheQue_.clear();
}

int32_t AudioSink::DequeReleaseFrame(OutputInfo &frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameReleaseQue_.size() == 0) {
        return SINK_QUE_EMPTY;
    }
    frame = frameReleaseQue_[0];
    frameReleaseQue_.erase(frameReleaseQue_.begin());
    return SINK_SUCCESS;
}

void AudioSink::RenderRptEvent(EventCbType event)
{
    if (callBack_.onEventCallback != nullptr) {
        if (event == EVNET_AUDIO_PLAY_EOS && eosSended_) {
            return;
        }
        callBack_.onEventCallback(callBack_.priv, event, 0, 0);
        if (event == EVNET_AUDIO_PLAY_EOS) {
            eosSended_ = true;
        }
    }
}

int32_t AudioSink::WriteToAudioDevice(OutputInfo &renderFrame)
{
    int32_t ret;
    uint64_t writeLen = 0;
    if ((audioRender_ == nullptr) || (renderFrame.buffers == nullptr)) {
        ReleaseQueHeadFrame();
        return SINK_RENDER_ERROR;
    }

    ret = audioRender_->RenderFrame(audioRender_, renderFrame.buffers[0].addr + renderFrame.buffers[0].offset,
        static_cast<uint64_t>(renderFrame.buffers[0].length), &writeLen);
    if ((unsigned long)renderFrame.buffers[0].length != writeLen) {
        return SINK_RENDER_FULL;
    } else if (ret != HI_SUCCESS) {
        ReleaseQueHeadFrame();
        MEDIA_ERR_LOG("RenderFrame failed ret: %x", ret);
        return SINK_RENDER_ERROR;
    }
    ReleaseQueHeadFrame();
    return HI_SUCCESS;
}

int32_t AudioSink::RenderFrame(OutputInfo &frame)
{
    SyncRet syncRet = SYNC_RET_PLAY;
    int64_t crtPlayPts = 0;
    uint64_t frameCnt;
    OutputInfo renderFrame;
    struct AudioTimeStamp timestamp;

    if (!reportedFirstFrame && renderMode_ == RENDER_MODE_PAUSE_AFTER_PLAY) {
        callBack_.onEventCallback(callBack_.priv, EVNET_FIRST_AUDIO_REND, 0, 0);
        reportedFirstFrame = true;
        MEDIA_INFO_LOG("report first audio frame");
    }

    if (paused_ || renderMode_ == RENDER_MODE_PAUSE_AFTER_PLAY) {
        QueueRenderFrame(frame, true);
        return SINK_SUCCESS;
    }
    if (!started_ || audioRender_ == nullptr) {
        QueueRenderFrame(frame, false);
        MEDIA_ERR_LOG("paused or audio dev not inited");
        return SINK_RENDER_ERROR;
    }

    if (GetRenderFrame(renderFrame, frame) != SINK_SUCCESS) {
        if (receivedEos_) {
            RenderRptEvent(EVNET_AUDIO_PLAY_EOS);
            return SINK_RENDER_EOS;
        }
        return SINK_QUE_EMPTY;
    }
    if (pauseAfterPlay_) {
        return SINK_SUCCESS;
    }

    int32_t ret = audioRender_->GetRenderPosition(audioRender_, &frameCnt, &timestamp);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("GetRenderPosition failed,ret=0x%x", ret);
        return SINK_RENDER_ERROR;
    }

    UpdateAudioPts(lastRendPts_, crtPlayPts, renderFrame);
    ret = (syncHdl_ != nullptr) ? syncHdl_->ProcAudFrame(crtPlayPts, syncRet) : HI_SUCCESS;
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("ProcAudFrame pts: %lld failed", renderFrame.timeStamp);
        ReleaseQueHeadFrame();
        return SINK_RENDER_ERROR;
    }
    if (syncRet == SYNC_RET_PLAY) {
        ret = WriteToAudioDevice(renderFrame);
    } else if (syncRet == SYNC_RET_DROP) {
        ReleaseQueHeadFrame();
        ret = SINK_SUCCESS;
    } else if (syncRet == SYNC_RET_REPEAT) {
        ret = SINK_RENDER_DELAY;
    } else {
        MEDIA_ERR_LOG("aud invalid sync ret: %d", syncRet);
        ReleaseQueHeadFrame();
        ret =  SINK_RENDER_ERROR;
    }

    if (ret == SINK_SUCCESS || ret == SINK_RENDER_ERROR) {
        lastRendPts_ = renderFrame.timeStamp;
        rendFrameCnt_++;
    }
    return ret;
}

void AudioSink::RenderEos(void)
{
    receivedEos_ = true;
    eosPts_ = lastRendPts_;
}

int32_t AudioSink::Start(void)
{
    CHECK_FAILED_RETURN(started_, false, SINK_SUCCESS, "have started");
    CHECK_NULL_RETURN(audioRender_, SINK_INVALID_OP, "audio dev not inited");
    int32_t ret = audioRender_->control.Start((AudioHandle)audioRender_);
    if (ret == 0) {
        started_ = true;
    }
    return ret;
}

int32_t AudioSink::SetVolume(float left, float right)
{
    float volume;
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    if (audioRender_ == nullptr) {
        MEDIA_ERR_LOG("AudioSink::SetVolume failed audioRender_ null");
        return -1;
    }

    leftVolume_ = left;
    rightVolume_ = right;
    if ((leftVolume_ == 0) && (rightVolume_ != 0)) {
        volume = rightVolume_;
    } else if ((leftVolume_ != 0) && (rightVolume_ == 0)) {
        volume = leftVolume_;
    } else {
        volume = (leftVolume_ + rightVolume_) / HALF_FACTOR;
    }
    audioRender_->volume.SetVolume(reinterpret_cast<AudioHandle>(audioRender_), volume);
    return SINK_SUCCESS;
}

int32_t AudioSink::GetVolume(float &left, float &right)
{
    left = leftVolume_;
    right = rightVolume_;
    return SINK_SUCCESS;
}

int32_t AudioSink::Stop(void)
{
    if (started_ && audioRender_ != nullptr) {
        (void)audioRender_->control.Stop(reinterpret_cast<AudioHandle>(audioRender_));
    }
    ReleaseQueAllFrame();
    rendFrameCnt_ = 0;
    started_ = false;
    paused_ = false;
    eosSended_ = false;
    return SINK_SUCCESS;
}

int32_t AudioSink::Pause(void)
{
    if (started_ && audioRender_ != nullptr) {
        (void)audioRender_->control.Pause(reinterpret_cast<AudioHandle>(audioRender_));
    }
    paused_ = true;
    return SINK_SUCCESS;
}

int32_t AudioSink::Resume(void)
{
    CHECK_FAILED_RETURN(paused_, true, -1, "not paused");
    CHECK_NULL_RETURN(audioRender_, -1, "audioRender_ is null");
    (void)audioRender_->control.Resume(reinterpret_cast<AudioHandle>(audioRender_));
    paused_ = false;
    renderMode_ = RENDER_MODE_NORMAL;
    return SINK_SUCCESS;
}

int32_t AudioSink::Reset(void)
{
    ReleaseQueAllFrame();
    if (started_ && audioRender_ != nullptr) {
        (void)audioRender_->control.Flush(reinterpret_cast<AudioHandle>(audioRender_));
    }

    ResetRendStartTime();
    receivedEos_ = false;
    renderDelay_ = 0;
    return SINK_SUCCESS;
}

int32_t AudioSink::RegisterCallBack(PlayEventCallback &callback)
{
    callBack_ = callback;
    return SINK_SUCCESS;
}

void AudioSink::SetSync(PlayerSync *sync)
{
    syncHdl_ = sync;
}

int32_t AudioSink::Flush(void)
{
    if (started_ && audioRender_ != nullptr) {
        (void)audioRender_->control.Flush(reinterpret_cast<AudioHandle>(audioRender_));
    }
    renderDelay_ = 0;
    return 0;
}

void AudioSink::GetRenderPosition(int64_t &position)
{
    position = lastRendPts_;
}

void AudioSink::SetRenderMode(RenderMode mode)
{
    renderMode_ = mode;
}

void AudioSink::SetAudioStreamType(int32_t &type)
{
    audioStreamType_ = type;
}
}
}