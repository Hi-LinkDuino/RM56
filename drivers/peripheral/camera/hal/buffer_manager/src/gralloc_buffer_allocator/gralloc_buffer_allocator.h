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

#ifndef HOS_GRALLOC_BUFFER_ALLOCATOR_H
#define HOS_GRALLOC_BUFFER_ALLOCATOR_H

#include <display_gralloc.h>
#include "buffer_allocator.h"

namespace OHOS::Camera {
class GrallocBufferAllocator : public BufferAllocator {
public:
    GrallocBufferAllocator();
    virtual ~GrallocBufferAllocator();

    virtual RetCode Init() override;
    virtual std::shared_ptr<IBuffer> AllocBuffer(const uint32_t width,
                                                 const uint32_t height,
                                                 const uint64_t cameraUsage,
                                                 const uint32_t format) override;
    virtual RetCode FreeBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode MapBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode UnmapBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode FlushCache(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode InvalidateCache(std::shared_ptr<IBuffer>& buffer) override;

private:
    GrallocFuncs* grallocFuncs_ = nullptr;
    const int32_t sourceType_ = CAMERA_BUFFER_SOURCE_TYPE_GRALLOC;

private:
    void CameraBufferToGrallocBuffer(const std::shared_ptr<IBuffer>& src, BufferHandle& dest) const;
    void GrallocBufferToCameraBuffer(const BufferHandle& src, std::shared_ptr<IBuffer>& dest) const;
};
} // namespace OHOS::Camera

#endif
