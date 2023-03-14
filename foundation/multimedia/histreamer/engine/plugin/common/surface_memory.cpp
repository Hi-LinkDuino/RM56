/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#if !defined(OHOS_LITE) && defined(VIDEO_SUPPORT)
#include "surface_memory.h"
#include <utility>
#include "foundation/log.h"
#include "surface_allocator.h"

namespace OHOS {
namespace Media {
namespace Plugin {
SurfaceMemory::SurfaceMemory(size_t capacity, std::shared_ptr<Allocator> allocator, size_t align)
    : Memory(capacity, std::move(allocator), align, MemoryType::SURFACE_BUFFER, false),
      fence_(-1),
      stride_(0)
{
    bufferSize_ = align ? (capacity + align - 1) : capacity;
    if (this->allocator != nullptr && this->allocator->GetMemoryType() == MemoryType::SURFACE_BUFFER) {
        surfaceAllocator_ = ReinterpretPointerCast<SurfaceAllocator>(this->allocator);
        AllocSurfaceBuffer();
    }
}

SurfaceMemory::~SurfaceMemory()
{
    ReleaseSurfaceBuffer();
}

void SurfaceMemory::AllocSurfaceBuffer()
{
    if (surfaceAllocator_ == nullptr || bufferSize_ == 0 || surfaceBuffer_ != nullptr) {
        return;
    }
    surfaceBuffer_ = surfaceAllocator_->AllocSurfaceBuffer(bufferSize_);
    if (surfaceBuffer_ != nullptr) {
        auto bufferHandle = surfaceBuffer_->GetBufferHandle();
        if (bufferHandle != nullptr) {
            stride_ = bufferHandle->stride;
        }
        fence_ = -1;
    }
}

sptr<SurfaceBuffer> SurfaceMemory::GetSurfaceBuffer()
{
    OSAL::ScopedLock l(memMutex_);
    if (surfaceBuffer_ != nullptr) {
        return surfaceBuffer_;
    }
    // request surface buffer again when old buffer flush to nullptr
    AllocSurfaceBuffer();
    return surfaceBuffer_;
}

void SurfaceMemory::ReleaseSurfaceBuffer()
{
    OSAL::ScopedLock l(memMutex_);
    surfaceBuffer_ = nullptr;
}

int32_t SurfaceMemory::GetFlushFence()
{
    OSAL::ScopedLock l(memMutex_);
    return fence_;
}

BufferHandle *SurfaceMemory::GetBufferHandle()
{
    OSAL::ScopedLock l(memMutex_);
    if (surfaceBuffer_) {
        return surfaceBuffer_->GetBufferHandle();
    }
    return nullptr;
}

uint32_t SurfaceMemory::GetSurfaceBufferStride()
{
    OSAL::ScopedLock l(memMutex_);
    return stride_;
}

uint8_t* SurfaceMemory::GetRealAddr() const
{
    OSAL::ScopedLock l(memMutex_);
    if (surfaceBuffer_) {
        return static_cast<uint8_t *>(surfaceBuffer_->GetVirAddr());
    }
    return nullptr;
}

size_t SurfaceMemory::GetCapacity()
{
    OSAL::ScopedLock l(memMutex_);
    if (surfaceBuffer_) {
        return static_cast<size_t>(surfaceBuffer_->GetSize());
    }
    return 0;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif