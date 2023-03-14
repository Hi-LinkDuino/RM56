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

#include "recorder_video_source.h"
#include "media_log.h"

namespace OHOS {
namespace Media {
constexpr int32_t KEY_IS_SYNC_FRAME = 1; // "is-sync-frame"
constexpr int32_t KEY_TIME_US = 2;       // "timeUs"
constexpr int32_t SURFACE_QUEUE_SIZE = 3;
Surface *g_surface = nullptr;

RecorderVideoSource::RecorderVideoSource()
    : surface_(nullptr),
      frameAvailableCount_(0),
      acquireBuffer_(nullptr),
      started_(false)
{
}

RecorderVideoSource::~RecorderVideoSource()
{
    if (surface_ != nullptr) {
        surface_->UnregisterConsumerListener();
    }
}

std::shared_ptr<OHOS::Surface> RecorderVideoSource::GetSurface()
{
    if (surface_.get() == nullptr) {
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            return nullptr;
        }

        g_surface = surface;
        surface->RegisterConsumerListener(*this);
        surface_.reset(surface);
    }
    MEDIA_INFO_LOG("Get Recorder Surface SUCCESS :%p", surface_.get());
    return surface_;
}

int32_t RecorderVideoSource::SetSurfaceSize(int32_t width, int32_t height)
{
    if (surface_.get() == nullptr) {
        Surface *surface = Surface::CreateSurface();
        if (surface == nullptr) {
            return ERR_UNKNOWN;
        }

        g_surface = surface;
        surface->RegisterConsumerListener(*this);
        surface_.reset(surface);
    }
    Surface *surface = surface_.get();
    MEDIA_INFO_LOG("Get Recorder Surface SUCCESS");
    surface->SetWidthAndHeight(width, height);
    surface->SetQueueSize(SURFACE_QUEUE_SIZE);
    /* 3/4 is the worst case of compression provide by encoder. */
    surface->SetSize(width * height * 3 / 4);
    return SUCCESS;
}

void RecorderVideoSource::OnBufferAvailable()
{
    if (surface_ == nullptr) {
        MEDIA_ERR_LOG("surface is NULL");
        return;
    }

    if (!started_) {
        MEDIA_ERR_LOG("video source is not started");
        acquireBuffer_ = surface_->AcquireBuffer();
        if (acquireBuffer_ == nullptr) {
            MEDIA_INFO_LOG("Acquire buffer failed.");
            return;
        }
        surface_->ReleaseBuffer(acquireBuffer_);
        return;
    }

    std::unique_lock<std::mutex> lock(lock_);
    if (frameAvailableCount_ == 0) {
        frameAvailableCondition_.notify_one();
    }
    frameAvailableCount_++;
}

int32_t RecorderVideoSource::Start()
{
    started_ = true;
    MEDIA_INFO_LOG("Start Recorder Video Source SUCCESS");
    return SUCCESS;
}

int32_t RecorderVideoSource::AcquireBuffer(RecorderSourceBuffer &buffer, bool isBlocking)
{
    if (!started_) {
        return ERR_NOT_STARTED;
    }
    if (isBlocking) {
        std::unique_lock<std::mutex> lock(lock_);
        if (frameAvailableCount_ <= 0) {
            frameAvailableCondition_.wait(lock);
            if (!started_) {
                return ERR_READ_BUFFER;
            }
        }
        frameAvailableCount_--;
    }

    if (surface_ == nullptr) {
        MEDIA_ERR_LOG("surface is NULL");
        return ERR_READ_BUFFER;
    }
    acquireBuffer_ = surface_->AcquireBuffer();
    if (acquireBuffer_ == nullptr) {
        MEDIA_ERR_LOG("Acquire buffer failed.");
        return ERR_READ_BUFFER;
    }
    void *pBase = acquireBuffer_->GetVirAddr();
    if (pBase == nullptr) {
        MEDIA_ERR_LOG("GetVirAddr is nullptr");
        return ERR_READ_BUFFER;
    }
    buffer.dataAddr = (uint8_t *)pBase;
    buffer.dataLen = acquireBuffer_->GetSize();
    int64_t valuets = 0;
    acquireBuffer_->GetInt64(KEY_TIME_US, valuets);
    buffer.timeStamp = valuets;

    int32_t value = 0;
    acquireBuffer_->GetInt32(KEY_IS_SYNC_FRAME, value);
    buffer.keyFrameFlag = (value == 1) ? true : false;
    return SUCCESS;
}

int32_t RecorderVideoSource::ReleaseBuffer(RecorderSourceBuffer &buffer)
{
    surface_->ReleaseBuffer(acquireBuffer_);
    return SUCCESS;
}

int32_t RecorderVideoSource::Stop()
{
    started_ = false;
    std::unique_lock<std::mutex> lock(lock_);
    frameAvailableCondition_.notify_all();
    return SUCCESS;
}

int32_t RecorderVideoSource::Resume()
{
    return SUCCESS;
}

int32_t RecorderVideoSource::Pause()
{
    return SUCCESS;
}

int32_t RecorderVideoSource::Release()
{
    return SUCCESS;
}
}  // namespace Media
}  // namespace OHOS