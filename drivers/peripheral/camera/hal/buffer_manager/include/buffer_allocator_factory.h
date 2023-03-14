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

#ifndef HOS_BUFFER_ALLOCATOR_FACTORY_H
#define HOS_BUFFER_ALLOCATOR_FACTORY_H

#include "camera.h"
#include "ibuffer_allocator.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>

namespace OHOS::Camera {
#define REGISTER_BUFFER_ALLOCATOR(C, type, ...) \
    static BufferAllocatorFactory::BufferAllocatorRegister<C> g_reg##C(type, ##__VA_ARGS__);

class BufferAllocatorFactory {
public:
    static BufferAllocatorFactory* GetInstance();
    std::shared_ptr<IBufferAllocator> GetBufferAllocator(const int32_t type);

    template<typename C> class BufferAllocatorRegister {
    public:
        template<typename ...Args> BufferAllocatorRegister(int32_t type, Args... args)
        {
            BufferAllocatorFactory* factory = BufferAllocatorFactory::GetInstance();
            if (factory != nullptr) {
                factory->allocatorRegisterMap_.emplace(type, [&args...] { return new C(std::forward<Args>(args)...); });
            }
        }
    };

private:
    std::shared_ptr<IBufferAllocator> CreateBufferAllocator(const int32_t type);

private:
    BufferAllocatorFactory() = default;
    BufferAllocatorFactory(const BufferAllocatorFactory&);
    BufferAllocatorFactory& operator=(const BufferAllocatorFactory&);
    BufferAllocatorFactory(BufferAllocatorFactory&&);
    BufferAllocatorFactory& operator=(BufferAllocatorFactory&&);

    ~BufferAllocatorFactory() = default;

    std::mutex lock_;
    std::map<int32_t, std::weak_ptr<IBufferAllocator>> bufferAllocatorMap_ = {};
    std::map<int32_t, std::function<IBufferAllocator*()>> allocatorRegisterMap_ = {};
};
} // namespace OHOS::Camera
#endif
