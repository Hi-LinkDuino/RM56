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

#include <unistd.h>
#include <sys/time.h>
#include "player_video_sink.h"
#include "media_log.h"

namespace OHOS {
namespace Media {

const int32_t MAX_VIDEO_QUEUE_BUF_NUM = 32;
const float VIDEO_FRAME_RATE_MAX = 120.0f;
const int32_t FLOAT_INT_SCALE = 1000;
const int32_t SS2US = 1000000;
const int32_t US2MS = 1000;
const int32_t DEFUALT_REGION_WIDTH = 480;
const int32_t DEFUALT_REGION_HEIGHT = 480;

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

static int64_t GetCurTimeUs()
{
    struct timeval ts;
    ts.tv_sec = 0;
    ts.tv_usec = 0;
    gettimeofday(&ts, nullptr);
    return (((int64_t)ts.tv_sec) * SS2US) + ((int64_t)ts.tv_usec);
}

static int64_t GetCurTimeMs()
{
    int64_t curTimeUs = GetCurTimeUs();
    return (int64_t)(curTimeUs / US2MS);
}

VideoSink::VideoSink(void)
    :speed_(1.0f), paused_(false), started_(false), syncHdl_(nullptr),
    renderFrameCnt_(0), renderMode_(RENDER_MODE_NORMAL), rendStartTime_(-1), lastRendPts_(AV_INVALID_PTS),
    recievedEos_(false), EosPts_(AV_INVALID_PTS), pauseAfterPlay_(false), firstVidRend_(false),
    lastRendCnt_(0), vidRendStartTime_(AV_INVALID_PTS), eosSended_(false), lastConfigRegionX_(-1),
    lastConfigRegionY_(-1), lastConfigRegionW_(-1), lastConfigRegionH_(-1)
{
    ResetRendStartTime();
    attr_.sinkType = SINK_TYPE_BUT;
    attr_.trackId = 0;
    attr_.vidAttr.format = 0;
    attr_.vidAttr.width = 0;
    attr_.vidAttr.height = 0;
    attr_.vidAttr.frameRate = 0.0;
    attr_.vidAttr.surface = nullptr;
    lastRendSysTimeMs_ = GetCurTimeMs();
    callBack_.onEventCallback = nullptr;
    callBack_.priv = nullptr;
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
}

VideoSink::~VideoSink()
{
    (void)DeInit();
}

int32_t VideoSink::DeInit()
{
    int32_t ret = HI_SUCCESS;
    layerFuncs_->DeinitDisplay(0);
    return ret;
}

int32_t VideoSink::Init(SinkAttr &attr)
{
    attr_ = attr;
    (void)LayerInitialize(&layerFuncs_);
    layerFuncs_->InitDisplay(0);
    return 0;
}

void VideoSink::GetStatus(VideoSinkStatus &status)
{
    status.vidFrameCount = renderFrameCnt_;
    status.decHeight = attr_.vidAttr.height;
    status.decWidth = static_cast<uint32_t>(attr_.vidAttr.width);
    if (vidRendStartTime_ != AV_INVALID_PTS) {
        int64_t diffTimeMs = GetCurTimeMs() - vidRendStartTime_;
        if (diffTimeMs > MS_SCALE) {
            double frameCnt = lastRendCnt_;
            double time = diffTimeMs;
            double frameRate = frameCnt / time;
            if (frameRate >= VIDEO_FRAME_RATE_MAX || frameRate < 0) {
                status.fpsInteger = 0;
                status.fpsDecimal = 0;
                return;
            }

            uint32_t tmp = frameRate * FLOAT_INT_SCALE;
            status.fpsInteger = (tmp / FLOAT_INT_SCALE);
            status.fpsDecimal = (tmp % FLOAT_INT_SCALE);
            return;
        }
    }
    status.fpsInteger = 0;
    status.fpsDecimal = 0;
}

void VideoSink::SetDefaultDisplayRegionInfo(void)
{
    lastConfigRegionX_ = 0;
    lastConfigRegionY_ = 0;
    lastConfigRegionW_ = DEFUALT_REGION_WIDTH;
    lastConfigRegionH_ = DEFUALT_REGION_HEIGHT;
}

void VideoSink::UpdateDisplayRegionInfo(int32_t x, int32_t y, int32_t w, int32_t h)
{
    lastConfigRegionX_ = x;
    lastConfigRegionY_ = y;
    lastConfigRegionW_ = w;
    lastConfigRegionH_ = h;
}

void VideoSink::CreateAndConfigLayer(void)
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    IRect attr;
    uint32_t devId = 0;
    uint32_t layerId = 0;
    int32_t right = lastConfigRegionX_ + lastConfigRegionW_ - 1;
    int32_t botttom = lastConfigRegionY_ + lastConfigRegionH_ - 1;
    /* Make sure the coordinates are even */
    x = lastConfigRegionX_ - lastConfigRegionX_ % 0x2;
    y = lastConfigRegionY_ - lastConfigRegionY_ % 0x2;
    w = right - x + 1;
    h = botttom - y + 1;
    w = w + w % 2;
    h = h + h % 2;
    attr.x = x;
    attr.y = y;
    attr.w = w;
    attr.h = h;
    LayerInfo lInfo;
    lInfo.width = w;
    lInfo.height = h;
    if (layerFuncs_ != nullptr) {
        layerFuncs_->CreateLayer(devId, &lInfo, &layerId);
        layerFuncs_->SetLayerSize(devId, layerId, &attr);
    }

}

void VideoSink::CheckConfigVideoOutput(void)
{
    Surface *surface = attr_.vidAttr.surface;
    /* set default region if not set */
    if (surface == nullptr) {
        SetDefaultDisplayRegionInfo();
        return CreateAndConfigLayer();
    }
    int32_t x = std::stoi(surface->GetUserData("region_position_x"));
    int32_t y = std::stoi(surface->GetUserData("region_position_y"));
    int32_t w = std::stoi(surface->GetUserData("region_width"));
    int32_t h = std::stoi(surface->GetUserData("region_height"));
    UpdateDisplayRegionInfo(x, y, w, h);

    CreateAndConfigLayer();
}

int32_t VideoSink::Start()
{
    vidRendStartTime_ = GetCurTimeMs();
    CheckConfigVideoOutput();
    if (syncHdl_ != nullptr) {
        syncHdl_->Start(SYNC_CHN_VID);
    }
    started_ = true;
    return HI_SUCCESS;
}

void VideoSink::SetRenderMode(RenderMode mode)
{
    // use to control report first video frame
    renderMode_ = mode;
}

int32_t VideoSink::Stop()
{
    ReleaseQueAllFrame();
    ResetRendStartTime();
    renderFrameCnt_ = 0;
    if (layerFuncs_ != nullptr) {
        layerFuncs_->CloseLayer(0, 0);
    }
    started_ = false;
    return HI_SUCCESS;
}

int32_t VideoSink::Pause()
{
    if (paused_) {
        MEDIA_WARNING_LOG("vsink already paused");
        return HI_SUCCESS;
    }
    if (!started_) {
        MEDIA_ERR_LOG("vsink not in running");
        return HI_FAILURE;
    }

    ResetRendStartTime();
    paused_ = true;
    return HI_SUCCESS;
}

int32_t VideoSink::Resume(void)
{
    renderMode_ = RENDER_MODE_NORMAL;
    if (!paused_) {
        MEDIA_WARNING_LOG("vsink not in pause");
        return HI_FAILURE;
    }

    vidRendStartTime_ = GetCurTimeMs();
    lastRendSysTimeMs_ = GetCurTimeMs();
    paused_ = false;
    return HI_SUCCESS;
}

int32_t VideoSink::Flush(void)
{
    if (!started_) {
        MEDIA_ERR_LOG("vsink not in started");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

void VideoSink::ResetRendStartTime()
{
    lastRendCnt_ = 0;
    lastRendPts_ = AV_INVALID_PTS;
    vidRendStartTime_ = AV_INVALID_PTS;
}

int32_t VideoSink::Reset()
{
    ReleaseQueAllFrame();
    Flush();
    ResetRendStartTime();
    recievedEos_ = false;
    firstVidRend_ = false;
    return HI_SUCCESS;
}


int32_t VideoSink::RegisterCallBack(PlayEventCallback &callback)
{
    callBack_ = callback;
    return 0;
}

void VideoSink::QueueRenderFrame(OutputInfo &frame, bool cacheQueue)
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

int32_t VideoSink::GetRenderFrame(OutputInfo &renderFrame, OutputInfo &frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t ret = SINK_QUE_EMPTY;
    if (frame.type == VIDEO_DECODER && frame.bufferCnt != 0) {
        frameCacheQue_.push_back(frame);
    }
    if (frameCacheQue_.size() != 0) {
        renderFrame = frameCacheQue_[0];
        ret = SINK_SUCCESS;
    }
    return ret;
}

void VideoSink::ReleaseQueHeadFrame(void)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameCacheQue_.size() != 0) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
    }
}

void VideoSink::ReleaseQueAllFrame(void)
{
    size_t i;
    size_t queSize;
    std::lock_guard<std::mutex> lock(mutex_);
    queSize = frameCacheQue_.size();
    if (queSize > MAX_VIDEO_QUEUE_BUF_NUM) {
        return;
    }
    for (i = 0; i < queSize; i++) {
        frameReleaseQue_.push_back(frameCacheQue_[i]);
    }
    frameCacheQue_.clear();
}

int32_t VideoSink::DequeReleaseFrame(OutputInfo &frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (frameReleaseQue_.size() == 0) {
        return SINK_QUE_EMPTY;
    }
    frame = frameReleaseQue_[0];
    frameReleaseQue_.erase(frameReleaseQue_.begin());
    return SINK_SUCCESS;
}

void VideoSink::RenderRptEvent(EventCbType event)
{
    if (callBack_.onEventCallback != nullptr) {
        if (event == EVNET_VIDEO_PLAY_EOS && eosSended_ == true) {
            return;
        }
        callBack_.onEventCallback(callBack_.priv, event, 0, 0);
        if (event == EVNET_VIDEO_PLAY_EOS) {
            eosSended_ = true;
        }
    }
}

int32_t VideoSink::WriteToVideoDevice(OutputInfo &renderFrame)
{
    if (layerFuncs_ != nullptr) {
        LayerBuffer layerBuf;
        layerBuf.data.virAddr = renderFrame.vendorPrivate;
        layerFuncs_->Flush(0, 0, &layerBuf);
    }
    ReleaseQueHeadFrame();
    return SINK_SUCCESS;
}

int32_t VideoSink::RenderFrame(OutputInfo &frame)
{
    SyncRet syncRet = SYNC_RET_PLAY;
    int64_t crtPlayPts = 0;
    OutputInfo renderFrame;

    /* the frame should be save to queue at state paused and none-started */
    if (!started_ || paused_ || (renderMode_ == RENDER_MODE_PAUSE_AFTER_PLAY && renderFrameCnt_ == 1)) {
        QueueRenderFrame(frame, started_ ? true : false);
        return SINK_SUCCESS;
    }

    if (GetRenderFrame(renderFrame, frame) != SINK_SUCCESS) {
        if (recievedEos_ == true) {
            RenderRptEvent(EVNET_VIDEO_PLAY_EOS);
            return SINK_RENDER_EOS;
        }
        return SINK_QUE_EMPTY;
    }

    crtPlayPts = renderFrame.timeStamp;
    int32_t ret = (syncHdl_ != nullptr) ? syncHdl_->ProcVidFrame(crtPlayPts, syncRet) : HI_SUCCESS;
    if (ret != HI_SUCCESS) {
        ReleaseQueHeadFrame();
        MEDIA_ERR_LOG("ProcVidFrame pts: %llu failed", renderFrame.timeStamp);
        return SINK_RENDER_ERROR;
    }

    if (syncRet == SYNC_RET_PLAY) {
        ret = WriteToVideoDevice(renderFrame);
        if (renderFrameCnt_ == 0) {
            callBack_.onEventCallback(callBack_.priv, EVNET_FIRST_VIDEO_REND, 0, 0);
        }
        renderFrameCnt_++;
    } else if (syncRet == SYNC_RET_DROP) {
        MEDIA_INFO_LOG("too late, drop, pts: %lld", renderFrame.timeStamp);
        ReleaseQueHeadFrame();
        ret = SINK_SUCCESS;
    } else if (syncRet == SYNC_RET_REPEAT) {
        ret = SINK_RENDER_DELAY;
    } else {
        MEDIA_ERR_LOG("video invalid sync ret: %d", syncRet);
        ReleaseQueHeadFrame();
        ret =  SINK_RENDER_ERROR;
    }

    /* render pts update after the frame that have been processed */
    if (ret == SINK_SUCCESS || ret == SINK_RENDER_ERROR) {
        lastRendPts_ = (renderFrame.timeStamp > lastRendPts_) ? renderFrame.timeStamp : lastRendPts_;
    }
    return ret;
}

void VideoSink::SetSync(PlayerSync *sync)
{
    syncHdl_ = sync;
}

void VideoSink::RenderEos(void)
{
    recievedEos_ = true;
    EosPts_ = lastRendPts_;
}

void VideoSink::GetRenderPosition(int64_t &position)
{
    position = lastRendPts_;
}
}
}