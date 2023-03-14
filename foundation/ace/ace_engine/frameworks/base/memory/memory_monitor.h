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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_H

#include <string>

#include "base/memory/memory_monitor_def.h"
#include "base/memory/type_info_base.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

void PurgeMallocCache();

#ifdef ACE_MEMORY_MONITOR
class ACE_EXPORT MemoryMonitor {
public:
    static MemoryMonitor& GetInstance();

    virtual ~MemoryMonitor() = default;

    virtual void Add(void* ptr) = 0;
    virtual void Remove(void* ptr) = 0;
    virtual void Update(void* ptr, size_t size, const std::string& typeName) = 0;
    virtual void Dump() const = 0;

    template<class T>
    void Update(T* ptr, void* refPtr)
    {
        if (ptr != nullptr && ptr->RefCount() == 0) {
            Update(refPtr, TypeInfo<T>::Size(ptr), TypeInfo<T>::Name(ptr));
        }
    }

private:
    template<class T, bool hasTypeInfo = std::is_base_of_v<TypeInfoBase, T>>
    struct TypeInfo;

    template<class T>
    struct TypeInfo<T, false> {
        static const char* Name(T*)
        {
            return "Unknown";
        }

        static size_t Size(T*)
        {
            return sizeof(T);
        }
    };

    template<class T>
    struct TypeInfo<T, true> {
        static const char* Name(T* rawPtr)
        {
            return TypeInfoHelper::TypeName(rawPtr);
        }

        static size_t Size(T* rawPtr)
        {
            return TypeInfoHelper::TypeSize(rawPtr);
        }
    };
};
#endif // ACE_MEMORY_MONITOR

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_H
