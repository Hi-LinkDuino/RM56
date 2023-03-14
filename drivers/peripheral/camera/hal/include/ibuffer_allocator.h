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

#ifndef HOS_IBUFFER_ALLOCATOR_H
#define HOS_IBUFFER_ALLOCATOR_H

#include "camera.h"
#include "memory"

namespace OHOS::Camera {
class IBuffer;
class IBufferAllocator {
public:
    IBufferAllocator() = default;
    virtual ~IBufferAllocator() = default;

    // init buffer allocator
    virtual RetCode Init() = 0;

    // allocate buffer
    virtual std::shared_ptr<IBuffer>
        AllocBuffer(const uint32_t width, const uint32_t height, const uint64_t cameraUsage, const uint32_t format) = 0;

    // free the buffer
    virtual RetCode FreeBuffer(std::shared_ptr<IBuffer>&) = 0;

    // map the buffer
    virtual RetCode MapBuffer(std::shared_ptr<IBuffer>&) = 0;

    // unmap the buffer
    virtual RetCode UnmapBuffer(std::shared_ptr<IBuffer>&) = 0;

    // flush data from cache to memory and invalidate the data in cache
    virtual RetCode FlushCache(std::shared_ptr<IBuffer>&) = 0;

    // invalidate the cache, update cache from memory
    virtual RetCode InvalidateCache(std::shared_ptr<IBuffer>& buffer) = 0;
};
} // namespace OHOS::Camera
#endif
