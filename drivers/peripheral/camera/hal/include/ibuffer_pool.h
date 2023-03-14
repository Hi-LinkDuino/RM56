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

#ifndef HOS_CAMERA_IBUFFER_POOL_H
#define HOS_CAMERA_IBUFFER_POOL_H

#include "ibuffer.h"

namespace OHOS::Camera {
class IBufferPool {
public:
    virtual ~IBufferPool(){};

    // init a buffer pool, bufferSourceType indicates where buffers come from.
    virtual RetCode Init(const uint32_t width,
                         const uint32_t height,
                         const uint64_t usage,
                         const uint32_t bufferFormat,
                         const uint32_t count,
                         const int32_t bufferSourceType) = 0;

    // add buffer to buffer pool, only for external source.
    virtual RetCode AddBuffer(std::shared_ptr<IBuffer>& buffer) = 0;

    /* request a idle buffer from pool.
     * timeout < 0, wait until there is idle buffer in pool.
     * timeout = 0, no wait, could return null.
     * timeout > 0, wait for timeout seconds, if timeout return null.
     */
    virtual std::shared_ptr<IBuffer> AcquireBuffer(int timeout) = 0;
    std::shared_ptr<IBuffer> AcquireBuffer()
    {
        return AcquireBuffer(0);
    }

    // return a buffer to pool.
    virtual RetCode ReturnBuffer(std::shared_ptr<IBuffer>& buffer) = 0;

    // enable tracking buffers of pool
    virtual void EnableTracking(const int32_t id) = 0;
    virtual void SetId(const int64_t id) = 0;
    virtual void NotifyStop() = 0;
    virtual void NotifyStart() = 0;
    virtual void ClearBuffers() = 0;
    virtual uint32_t GetIdleBufferCount() = 0;
};
} // namespace OHOS::Camera

#endif
