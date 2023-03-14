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

#ifndef HOS_BUFFER_ALLOCATOR_H
#define HOS_BUFFER_ALLOCATOR_H

#include "buffer_allocator_factory.h"
#include "ibuffer.h"
#include "ibuffer_allocator.h"
#include <memory>

namespace OHOS::Camera {
class BufferAllocator : public IBufferAllocator {
public:
    BufferAllocator() = default;
    virtual ~BufferAllocator() = default;

    virtual RetCode Init() override;

    virtual std::shared_ptr<IBuffer>
        AllocBuffer(const uint32_t, const uint32_t, const uint64_t, const uint32_t) override;
    virtual RetCode FreeBuffer(std::shared_ptr<IBuffer>&) override;
    virtual RetCode MapBuffer(std::shared_ptr<IBuffer>&) override;
    virtual RetCode UnmapBuffer(std::shared_ptr<IBuffer>&) override;
    virtual RetCode FlushCache(std::shared_ptr<IBuffer>&) override;
    virtual RetCode InvalidateCache(std::shared_ptr<IBuffer>&) override;
};
} // namespace OHOS::Camera
#endif

