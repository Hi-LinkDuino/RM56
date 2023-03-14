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

#ifndef HOS_CAMERA_OFFLINE_PIPELINE_H
#define HOS_CAMERA_OFFLINE_PIPELINE_H

#include "camera.h"
#include "ibuffer.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

namespace OHOS::Camera {
class OfflinePipeline {
public:
    OfflinePipeline();
    virtual ~OfflinePipeline();
    void DeliverCacheCheck(std::vector<std::shared_ptr<IBuffer>>& buffers);
    RetCode StartProcess();
    RetCode StopProcess();
    void BindOfflineStreamCallback(std::function<void(std::shared_ptr<IBuffer>&)>& callback);
    void SwitchToOfflineMode();
    void ReceiveCache(std::vector<std::shared_ptr<IBuffer>>& buffers);
    RetCode CancelCapture(int32_t captureId);
    RetCode FlushOfflineStream();
    bool CacheQueueDry();
    bool CheckOwnerOfCaptureId(int32_t captureId);

    virtual void DeliverOfflineBuffer(std::shared_ptr<IBuffer>& buffer);
    virtual void ProcessCache(std::vector<std::shared_ptr<IBuffer>>& buffers);
    virtual void DeliverCache(std::vector<std::shared_ptr<IBuffer>>& buffers);
    virtual void DeliverCancelCache(std::vector<std::shared_ptr<IBuffer>>& buffers);

public:
    std::atomic<bool> offlineMode_ = false;
    std::list<std::vector<std::shared_ptr<IBuffer>>> bufferCache_ = {};
    std::function<void(std::shared_ptr<IBuffer>&)> callback_ = nullptr;

private:
    void HandleBuffers();

private:
    std::mutex cbLock_;
    std::mutex queueLock_;
    std::condition_variable cv_;
    std::atomic<bool> running_ = false;
    std::thread* processThread_ = nullptr;
};
} // namespace OHOS::Camera
#endif
