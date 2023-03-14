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

#include "buffer_allocator_factory.h"

namespace OHOS::Camera {
BufferAllocatorFactory *BufferAllocatorFactory::GetInstance()
{
    static BufferAllocatorFactory factory;
    return &factory;
}

std::shared_ptr<IBufferAllocator>
    BufferAllocatorFactory::GetBufferAllocator(const int32_t type)
{
    std::lock_guard<std::mutex> l(lock_);

    if (bufferAllocatorMap_.find(type) == bufferAllocatorMap_.end()) {
        return CreateBufferAllocator(type);
    }

    if (bufferAllocatorMap_[type].expired()) {
        return CreateBufferAllocator(type);
    }

    return bufferAllocatorMap_[type].lock();
}

std::shared_ptr<IBufferAllocator> BufferAllocatorFactory::CreateBufferAllocator(const int32_t type)
{
    if (allocatorRegisterMap_.find(type) == allocatorRegisterMap_.end()) {
        return nullptr;
    }

    std::shared_ptr<IBufferAllocator> allocator(allocatorRegisterMap_[type]());
    bufferAllocatorMap_[type] = allocator;
    return allocator;
}
}
