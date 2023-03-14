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

#ifndef HOS_CAMERA_BUFFER_MANAGER_H
#define HOS_CAMERA_BUFFER_MANAGER_H

#include "ibuffer_pool.h"
#include <map>
#include <mutex>

namespace OHOS::Camera {
class BufferManager {
public:

    // get a pointer of BufferManager
    static BufferManager* GetInstance();

    // generate a random id for buffer pool.
    uint64_t GenerateBufferPoolId();

    // get a buffer pool from id.
    std::shared_ptr<IBufferPool> GetBufferPool(uint64_t id);

private:
    BufferManager() = default;
    BufferManager(const BufferManager&);
    BufferManager& operator=(const BufferManager&);
    BufferManager(BufferManager&&);
    BufferManager& operator=(BufferManager&&);

    ~BufferManager() = default;

    std::mutex lock_;
    std::map<int64_t, std::weak_ptr<IBufferPool>> bufferPoolMap_;
};
} // namespace OHOS::Camera
#endif
