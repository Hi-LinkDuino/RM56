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

#include "buffer_allocator_utils.h"
#include "buffer_allocator_factory.h"

namespace OHOS::Camera {
std::shared_ptr<IBufferAllocator> BufferAllocatorUtils::GetBufferAllocator(const int32_t source)
{
    if (source == CAMERA_BUFFER_SOURCE_TYPE_NONE) {
        return nullptr;
    }

    if (source == CAMERA_BUFFER_SOURCE_TYPE_EXTERNAL) {
        return nullptr;
    }

    auto factory = BufferAllocatorFactory::GetInstance();
    if (factory == nullptr) {
        CAMERA_LOGE("can't get factory, alloc failed.");
        return nullptr;
    }

    auto allocator = factory->GetBufferAllocator(source);
    if (allocator == nullptr) {
        CAMERA_LOGE("fatal error, can't get allocator, alloc failed.");
        return nullptr;
    }

    if (allocator->Init() != RC_OK) {
        return nullptr;
    }

    return allocator;
}

std::shared_ptr<IBufferAllocator> BufferAllocatorUtils::GetAllocator(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer == nullptr) {
        CAMERA_LOGE("buffer is nullptr");
        return nullptr;
    }

    int32_t sourceType = buffer->GetSourceType();
    return GetBufferAllocator(sourceType);
}

std::shared_ptr<IBuffer> BufferAllocatorUtils::AllocBuffer(const int32_t source,
                                                           const uint32_t width,
                                                           const uint32_t height,
                                                           const uint64_t usage,
                                                           const uint32_t format)
{
    auto allocator = GetBufferAllocator(source);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, nullptr);

    return allocator->AllocBuffer(width, height, usage, format);
}

RetCode BufferAllocatorUtils::FreeBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto allocator = GetAllocator(buffer);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, RC_ERROR);
    return allocator->FreeBuffer(buffer);
}

RetCode BufferAllocatorUtils::MapBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto allocator = GetAllocator(buffer);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, RC_ERROR);
    return allocator->MapBuffer(buffer);
}

RetCode BufferAllocatorUtils::UnmapBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto allocator = GetAllocator(buffer);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, RC_ERROR);
    return allocator->UnmapBuffer(buffer);
}

RetCode BufferAllocatorUtils::FlushCache(std::shared_ptr<IBuffer>& buffer)
{
    auto allocator = GetAllocator(buffer);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, RC_ERROR);
    return allocator->FlushCache(buffer);
}

RetCode BufferAllocatorUtils::InvalidateCache(std::shared_ptr<IBuffer>& buffer)
{
    auto allocator = GetAllocator(buffer);
    CHECK_IF_PTR_NULL_RETURN_VALUE(allocator, RC_ERROR);
    return allocator->InvalidateCache(buffer);
}
} // namespace OHOS::Camera

