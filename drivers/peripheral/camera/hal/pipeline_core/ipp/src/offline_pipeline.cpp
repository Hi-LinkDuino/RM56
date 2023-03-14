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

#include "offline_pipeline.h"
#include "buffer_manager.h"
#include "ibuffer_pool.h"
#include <vector>

namespace OHOS::Camera {
OfflinePipeline::OfflinePipeline() {}

OfflinePipeline::~OfflinePipeline()
{
    StopProcess();
    bufferCache_.clear();
}

RetCode OfflinePipeline::StartProcess()
{
    running_ = true;
    processThread_ = new std::thread([this]() {
        prctl(PR_SET_NAME, "offlinepipeline");
        while (running_) {
            HandleBuffers();
        }
    });
    if (processThread_ == nullptr) {
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode OfflinePipeline::StopProcess()
{
    if (processThread_ == nullptr) {
        CAMERA_LOGE("cannot stop.");
        return RC_ERROR;
    }

    if (running_.load() == false) {
        return RC_OK;
    }

    running_ = false;
    cv_.notify_one();
    processThread_->join();
    delete processThread_;
    processThread_ = nullptr;
    return RC_OK;
}

void OfflinePipeline::BindOfflineStreamCallback(std::function<void(std::shared_ptr<IBuffer>&)>& callback)
{
    std::lock_guard<std::mutex> l(cbLock_);
    callback_ = callback;

    return;
}

void OfflinePipeline::SwitchToOfflineMode()
{
    offlineMode_ = true;
}

RetCode OfflinePipeline::CancelCapture(int32_t captureId)
{
    CAMERA_LOGE("cancel capture begin");
    if (bufferCache_.empty()) {
        CAMERA_LOGE("cancel capture failed, capture id = %{public}d doesn't exist", captureId);
        return RC_OK;
    }

    std::vector<std::shared_ptr<IBuffer>> cache = {};
    {
        std::unique_lock<std::mutex> l(queueLock_);
        auto it = std::find_if(bufferCache_.begin(), bufferCache_.end(),
            [&captureId](const std::vector<std::shared_ptr<IBuffer>>& c) {
                for (auto b : c) {
                    if (b->GetCaptureId() == captureId) {
                        return true;
                    }
                }
                return false;
            });
        if (it == bufferCache_.end()) {
            CAMERA_LOGE("cancel capture failed, capture id = %{public}d doesn't exist", captureId);
            return RC_OK;
        }
        cache = *it;
        bufferCache_.erase(it);
    }
    for (auto it : cache) {
        it->SetBufferStatus(CAMERA_BUFFER_STATUS_DROP);
    }
    DeliverCancelCache(cache);
    CAMERA_LOGE("cancel capture end");
    return RC_OK;
}

RetCode OfflinePipeline::FlushOfflineStream()
{
    if (!offlineMode_.load()) {
        CAMERA_LOGE("can't flush in online mode");
        return RC_ERROR;
    }

    if (!bufferCache_.empty()) {
        std::unique_lock<std::mutex> l(queueLock_);
        while (!bufferCache_.empty()) {
            auto cache = bufferCache_.front();
            bufferCache_.pop_front();

            for (auto it : cache) {
                it->SetBufferStatus(CAMERA_BUFFER_STATUS_DROP);
            }
            DeliverCancelCache(cache);
        }
    }

    return RC_OK;
}

void OfflinePipeline::ReceiveCache(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    if (!buffers.empty() && buffers[0]->GetBufferStatus() != CAMERA_BUFFER_STATUS_OK) {
        DeliverCancelCache(buffers);
        return;
    }

    std::unique_lock<std::mutex> l(queueLock_);
    bufferCache_.emplace_back(buffers);
    cv_.notify_one();

    return;
}

void OfflinePipeline::HandleBuffers()
{
    if (bufferCache_.empty()) {
        std::unique_lock<std::mutex> l(queueLock_);
        if (bufferCache_.empty()) {
            cv_.wait(l, [this] { return !(running_.load() && bufferCache_.empty()); });
        }
    }

    if (running_ == false) {
        return;
    }

    std::vector<std::shared_ptr<IBuffer>> cache = {};
    if (!bufferCache_.empty()) {
        std::unique_lock<std::mutex> l(queueLock_);
        if (!bufferCache_.empty()) {
            cache = bufferCache_.front();
            bufferCache_.pop_front();
        }
    }

    if (cache.empty()) {
        return;
    }
    ProcessCache(cache);

    return;
}

void OfflinePipeline::ProcessCache(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    DeliverCache(buffers);
    return;
}
void OfflinePipeline::DeliverCacheCheck(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    for (auto it : buffers) {
        if (it == nullptr) {
            continue;
        }
        auto bufferManager = BufferManager::GetInstance();
        if (bufferManager == nullptr) {
            CAMERA_LOGE("can't get buffer manager");
            continue;
        }
        auto bufferPool = bufferManager->GetBufferPool(it->GetPoolId());
        if (bufferPool == nullptr) {
            CAMERA_LOGE("can't get buffer pool");
            return;
        }
        bufferPool->ReturnBuffer(it);
    }
}
void OfflinePipeline::DeliverCache(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    DeliverCacheCheck(buffers);
    if (offlineMode_.load()) {
        std::shared_ptr<IBuffer> nullBuffer = nullptr;
        DeliverOfflineBuffer(nullBuffer);
    }
    return;
}

void OfflinePipeline::DeliverCancelCache(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    DeliverCache(buffers);
    return;
}

void OfflinePipeline::DeliverOfflineBuffer(std::shared_ptr<IBuffer>& buffer)
{
    if (!offlineMode_.load()) {
        CAMERA_LOGE("cannot deliver buffer in online mode");
        return;
    }

    if (callback_ == nullptr) {
        CAMERA_LOGE("cannot deliver offline buffer, callback_ is null");
        return;
    }

    callback_(buffer);

    return;
}

bool OfflinePipeline::CacheQueueDry()
{
    std::unique_lock<std::mutex> l(queueLock_);
    return bufferCache_.empty();
}

bool OfflinePipeline::CheckOwnerOfCaptureId(int32_t captureId)
{
    std::unique_lock<std::mutex> l(queueLock_);
    for (auto it : bufferCache_) {
        for (auto buffer : it) {
            if (captureId == buffer->GetCaptureId()) {
                return true;
            }
        }
    }
    return false;
}
} // namespace OHOS::Camera
