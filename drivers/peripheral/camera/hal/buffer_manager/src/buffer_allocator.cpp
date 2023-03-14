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

#include "buffer_allocator.h"

namespace OHOS::Camera {
RetCode BufferAllocator::Init()
{
    return RC_OK;
}

std::shared_ptr<IBuffer> BufferAllocator::AllocBuffer(const uint32_t, const uint32_t, const uint64_t, const uint32_t)
{
    return nullptr;
}

RetCode BufferAllocator::FreeBuffer(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode BufferAllocator::MapBuffer(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode BufferAllocator::UnmapBuffer(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode BufferAllocator::FlushCache(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

RetCode BufferAllocator::InvalidateCache(std::shared_ptr<IBuffer>&)
{
    return RC_OK;
}

REGISTER_BUFFER_ALLOCATOR(BufferAllocator, CAMERA_BUFFER_SOURCE_TYPE_NONE);
} // namespace OHOS::Camera

