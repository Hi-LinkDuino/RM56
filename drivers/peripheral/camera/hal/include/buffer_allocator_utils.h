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

#ifndef HOS_BUFFER_ALLOCATOR_UTILS_H
#define HOS_BUFFER_ALLOCATOR_UTILS_H

#include "camera.h"
#include "ibuffer.h"
#include "ibuffer_allocator.h"

namespace OHOS::Camera {
class BufferAllocatorUtils {
public:
    // get buffer allocator
    static std::shared_ptr<IBufferAllocator> GetAllocator(std::shared_ptr<IBuffer>& buffer);

    // allocate buffer from source.
    static std::shared_ptr<IBuffer> AllocBuffer(const int32_t source,
                                                const uint32_t width,
                                                const uint32_t height,
                                                const uint64_t usage,
                                                const uint32_t format);

    // free the buffer
    static RetCode FreeBuffer(std::shared_ptr<IBuffer>& buffer);

    // Map the buffer
    static RetCode MapBuffer(std::shared_ptr<IBuffer>& buffer);

    // Unmap the buffer
    static RetCode UnmapBuffer(std::shared_ptr<IBuffer>& buffer);

    // flush the buffer
    static RetCode FlushCache(std::shared_ptr<IBuffer>& buffer);

    // invalidate the cache, update cache from memory
    static RetCode InvalidateCache(std::shared_ptr<IBuffer>& buffer);
private:
    static std::shared_ptr<IBufferAllocator> GetBufferAllocator(const int32_t source);
};
} // namespace OHOS::Camera
#endif
