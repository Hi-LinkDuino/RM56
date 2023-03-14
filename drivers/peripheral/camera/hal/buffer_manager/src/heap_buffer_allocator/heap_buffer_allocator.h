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

#ifndef HOS_HEAP_BUFFER_ALLOCATOR_H
#define HOS_HEAP_BUFFER_ALLOCATOR_H

#include <memory>
#include "buffer_allocator.h"


namespace OHOS::Camera {
class HeapBufferAllocator : public BufferAllocator {
public:
    HeapBufferAllocator();
    virtual ~HeapBufferAllocator();

    virtual RetCode Init() override;

    virtual std::shared_ptr<IBuffer> AllocBuffer(const uint32_t width,
                                                 const uint32_t height,
                                                 const uint64_t cameraUsage,
                                                 const uint32_t format) override;
    virtual RetCode FreeBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode MapBuffer(std::shared_ptr<IBuffer>&) override;
    virtual RetCode UnmapBuffer(std::shared_ptr<IBuffer>&) override;
    virtual RetCode FlushCache(std::shared_ptr<IBuffer>&) override;
    virtual RetCode InvalidateCache(std::shared_ptr<IBuffer>&) override;

private:
    const int32_t sourceType_ = CAMERA_BUFFER_SOURCE_TYPE_HEAP;
private:
    uint32_t CalculateSize(const uint32_t width,
                           const uint32_t height,
                           const uint64_t usage,
                           const uint32_t format) const;
};
} // namespace OHOS::Camera

#endif

