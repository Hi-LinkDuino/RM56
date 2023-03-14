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

#include "buffer_manager.h"
#include <sys/time.h>
#include "buffer_pool.h"

namespace OHOS::Camera {
BufferManager* BufferManager::GetInstance()
{
    static BufferManager manager;
    return &manager;
}

uint64_t BufferManager::GenerateBufferPoolId()
{
    std::lock_guard<std::mutex> l(lock_);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t id = static_cast<uint64_t>(tv.tv_sec) * 1000 * 1000 + tv.tv_usec; // 1000:usec

    std::shared_ptr<IBufferPool> bufferPool = nullptr;
    bufferPoolMap_[id] = bufferPool;

    return id;
}

std::shared_ptr<IBufferPool> BufferManager::GetBufferPool(uint64_t id)
{
    std::lock_guard<std::mutex> l(lock_);

    if (bufferPoolMap_.find(id) == bufferPoolMap_.end()) {
        return nullptr;
    }

    if (bufferPoolMap_[id].expired()) {
        std::shared_ptr<IBufferPool> bufferPool = std::make_shared<BufferPool>();
        bufferPoolMap_[id] = bufferPool;
        bufferPool->SetId(id);
        return bufferPool;
    }

    return bufferPoolMap_[id].lock();
}
} // namespace OHOS::Camera
