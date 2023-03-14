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

#ifndef HOS_CAMERA_BUFFER_POOL_H
#define HOS_CAMERA_BUFFER_POOL_H

#include "buffer_allocator_factory.h"
#include "ibuffer.h"
#include "ibuffer_pool.h"
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <utility>

namespace OHOS::Camera {
class BufferPool : public IBufferPool {
public:
    BufferPool();
    virtual ~BufferPool();

    virtual RetCode Init(const uint32_t width,
                         const uint32_t height,
                         const uint64_t usage,
                         const uint32_t bufferFormat,
                         const uint32_t count,
                         const int32_t bufferSourceType) override;
    virtual RetCode AddBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual std::shared_ptr<IBuffer> AcquireBuffer(int timeout) override;
    virtual RetCode ReturnBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual void EnableTracking(const int32_t id) override;
    virtual void SetId(const int64_t id) override;
    virtual void NotifyStop() override;
    virtual void NotifyStart() override;
    virtual void ClearBuffers() override;
    virtual uint32_t GetIdleBufferCount() override;

private:
    RetCode PrepareBuffer();
    RetCode DestroyBuffer();

private:
    std::mutex lock_;
    std::condition_variable cv_;
    std::atomic_bool stop_ = false;
    int64_t poolId_ = -1;
    int32_t trackingId_ = -1;
    uint32_t bufferCount_ = 0;
    uint32_t bufferWidth_ = 0;
    uint32_t bufferHeight_ = 0;
    uint64_t bufferUsage_ = 0;
    uint32_t bufferFormat_ = CAMERA_FORMAT_INVALID;
    int32_t bufferSourceType_ = CAMERA_BUFFER_SOURCE_TYPE_NONE;
    std::shared_ptr<IBufferAllocator> bufferAllocator_ = nullptr;
    std::list<std::shared_ptr<IBuffer>> idleList_ = {};
    std::list<std::shared_ptr<IBuffer>> busyList_ = {};
};
} // namespace OHOS::Camera
#endif
