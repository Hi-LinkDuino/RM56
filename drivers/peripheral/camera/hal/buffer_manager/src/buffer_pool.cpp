/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "buffer_pool.h"
#include <chrono>
#include "buffer_adapter.h"
#include "image_buffer.h"
#include "buffer_tracking.h"

namespace OHOS::Camera {
BufferPool::BufferPool()
{
    CAMERA_LOGI("BufferPool construct, instance = %{public}p", this);
}

BufferPool::~BufferPool()
{
    DestroyBuffer();
}

RetCode BufferPool::Init(const uint32_t width,
                         const uint32_t height,
                         const uint64_t usage,
                         const uint32_t bufferFormat,
                         const uint32_t count,
                         const int32_t bufferSourceType)
{
    bufferWidth_ = width;
    bufferHeight_ = height;
    bufferUsage_ = usage;
    bufferFormat_ = bufferFormat;
    bufferCount_ = count;
    bufferSourceType_ = bufferSourceType;

    if (bufferSourceType_ == CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL) {
        CAMERA_LOGI("buffers are from external source");
        return RC_OK;
    }

    BufferAllocatorFactory* factory = BufferAllocatorFactory::GetInstance();
    if (factory == nullptr) {
        CAMERA_LOGE("buffer allocator factory is null");
        return RC_ERROR;
    }

    bufferAllocator_ = factory->GetBufferAllocator(bufferSourceType_);
    if (bufferAllocator_ == nullptr) {
        CAMERA_LOGI("can't find buffer allocator");
        return RC_ERROR;
    }

    if (bufferAllocator_->Init() != RC_OK) {
        return RC_ERROR;
    }

    return PrepareBuffer();
}

RetCode BufferPool::PrepareBuffer()
{
    if (bufferSourceType_ == CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL) {
        CAMERA_LOGI("no need allocate buffer");
        return RC_OK;
    }

    if (bufferAllocator_ == nullptr) {
        CAMERA_LOGE("bufferAllocator_ is nullptr");
        return RC_ERROR;
    }

    for (uint32_t i = 0; i < bufferCount_; i++) {
        std::shared_ptr<IBuffer> buffer =
            bufferAllocator_->AllocBuffer(bufferWidth_, bufferHeight_, bufferUsage_, bufferFormat_);
        if (buffer == nullptr) {
            CAMERA_LOGE("alloc buffer failed");
            return RC_ERROR;
        }
        if (RC_OK != bufferAllocator_->MapBuffer(buffer)) {
            CAMERA_LOGE("map buffer failed");
            return RC_ERROR;
        }
        buffer->SetIndex(i);
        buffer->SetPoolId(poolId_);

        {
            std::unique_lock<std::mutex> l(lock_);
            idleList_.emplace_back(buffer);
        }
    }

    return RC_OK;
}

RetCode BufferPool::DestroyBuffer()
{
    if (bufferSourceType_ == CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL) {
        std::unique_lock<std::mutex> l(lock_);
        idleList_.clear();
        busyList_.clear();
        return RC_OK;
    }

    if (bufferAllocator_ == nullptr) {
        CAMERA_LOGE("bufferAllocator_ is nullptr");
        return RC_ERROR;
    }

    {
        std::unique_lock<std::mutex> l(lock_);

        for (auto it : idleList_) {
            RetCode ret = bufferAllocator_->UnmapBuffer(it);
            if (ret != RC_OK) {
                CAMERA_LOGE("unmap (%{public}d) buffer failed", it->GetIndex());
            }
            ret = bufferAllocator_->FreeBuffer(it);
            if (ret != RC_OK) {
                CAMERA_LOGE("free (%{public}d) buffer failed", it->GetIndex());
            }
        }
        idleList_.clear();

        if (busyList_.size() > 0) {
            CAMERA_LOGE("%{public}u buffer(s) is/are in use.", busyList_.size());
        }
        for (auto it : busyList_) {
            RetCode ret = bufferAllocator_->UnmapBuffer(it);
            if (ret != RC_OK) {
                CAMERA_LOGE("unmap (%{public}d) buffer failed", it->GetIndex());
            }
            ret = bufferAllocator_->FreeBuffer(it);
            if (ret != RC_OK) {
                CAMERA_LOGE("free (%{public}d) buffer failed", it->GetIndex());
            }
        }
        busyList_.clear();
    }

    return RC_OK;
}

RetCode BufferPool::AddBuffer(std::shared_ptr<IBuffer>& buffer)
{
    std::unique_lock<std::mutex> l(lock_);
    buffer->SetPoolId(poolId_);
    idleList_.emplace_back(buffer);
    cv_.notify_one();
    return RC_OK;
}

std::shared_ptr<IBuffer> BufferPool::AcquireBuffer(int timeout)
{
    std::unique_lock<std::mutex> l(lock_);

    // return buffer immediately, if idle buffer is available;
    if (!idleList_.empty()) {
        auto it = idleList_.begin();
        auto buffer = *it;
        busyList_.splice(busyList_.begin(), idleList_, it);
        CAMERA_LOGV("acquire buffer immediately, index = %{public}d", buffer->GetIndex());
        return *it;
    }

    // wait all the time, till idle list is available.
    if (timeout < 0) {
        cv_.wait(l, [this] {
            return !idleList_.empty() || stop_;
            });
        if (!idleList_.empty()) {
            auto it = idleList_.begin();
            auto buffer = *it;
            busyList_.splice(busyList_.begin(), idleList_, it);
            CAMERA_LOGV("acquire buffer wait all the time, index = %{public}d", buffer->GetIndex());
            return *it;
        }
    }

    // wait for timeout, or idle list is available.
    if (timeout > 0) {
        if (cv_.wait_for(l, std::chrono::seconds(timeout), [this] {
            return !idleList_.empty() || stop_;
            }) == false) {
            CAMERA_LOGE("wait idle buffer timeout");
            return nullptr;
        }
        if (!idleList_.empty()) {
            auto it = idleList_.begin();
            auto buffer = *it;
            busyList_.splice(busyList_.begin(), idleList_, it);
            CAMERA_LOGV("acquire buffer wait %{public}ds, index = %{public}d", timeout, buffer->GetIndex());
            return *it;
        }
    }

    // timeout == 0. return nullptr buffer immediately, although idle buffer is not available.
    return nullptr;
}

RetCode BufferPool::ReturnBuffer(std::shared_ptr<IBuffer>& buffer)
{
    std::unique_lock<std::mutex> l(lock_);

    auto it = std::find(busyList_.begin(), busyList_.end(), buffer);
    if (it == busyList_.end()) {
        CAMERA_LOGE("fatal error, busy list is empty, cannot return buffer.");
        return RC_ERROR;
    }

    if (bufferSourceType_ == CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL) {
        busyList_.erase(it);
        cv_.notify_one();
        return RC_OK;
    }

    if (trackingId_ >= 0) {
        POOL_REPORT_BUFFER_LOCATION(trackingId_, buffer->GetFrameNumber());
    }

    idleList_.splice(idleList_.end(), busyList_, it);
    cv_.notify_one();

    return RC_OK;
}

void BufferPool::EnableTracking(const int32_t id)
{
    trackingId_ = id;
    return;
}

void BufferPool::SetId(const int64_t id)
{
    poolId_ = id;
}

void BufferPool::NotifyStop()
{
    stop_ = true;
    cv_.notify_all();
}

void BufferPool::NotifyStart()
{
    stop_ = false;
    cv_.notify_all();
}

void BufferPool::ClearBuffers()
{
    DestroyBuffer();
}

uint32_t BufferPool::GetIdleBufferCount()
{
    std::unique_lock<std::mutex> l(lock_);
    return idleList_.size();
}
} // namespace OHOS::Camera
