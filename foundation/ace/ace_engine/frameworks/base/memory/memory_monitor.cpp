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

#include "base/memory/memory_monitor.h"

#include <map>
#include <mutex>

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include <malloc.h>
#endif

#include "base/log/dump_log.h"
#include "base/log/log.h"

namespace OHOS::Ace {

void PurgeMallocCache()
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#if defined(__BIONIC__)
    mallopt(M_PURGE, 0);
#endif
#endif
}

#ifdef ACE_MEMORY_MONITOR
class MemoryMonitorImpl : public MemoryMonitor {
public:
    void Add(void* ptr) final
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto result = memoryMap_.emplace(ptr, MemInfo());
        if (!result.second) {
            LOGE("Address is already in memory map");
            return;
        }

        count_++;
    }

    void Remove(void* ptr) final
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = memoryMap_.find(ptr);
        if (it == memoryMap_.end()) {
            LOGE("Address MUST be in memory map");
            return;
        }
        count_--;

        if (it->second.size > 0) {
            total_ -= it->second.size;
            auto& info = typeMap_[it->second.typeName];
            info.count--;
            info.total -= it->second.size;
        }
        memoryMap_.erase(it);
    }

    void Update(void* ptr, size_t size, const std::string& typeName) final
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = memoryMap_.find(ptr);
        if (it == memoryMap_.end()) {
            LOGE("Address MUST be in memory map");
            return;
        }

        it->second.size = size;
        it->second.typeName = typeName;

        total_ += size;
        auto& info = typeMap_[typeName];
        info.count++;
        info.total += size;
    }

    void Dump() const final
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string out = "total = " + std::to_string(total_) + ", count = " + std::to_string(count_);
        DumpLog::GetInstance().Print(0, out);
        for (auto&& [typeName, info] : typeMap_) {
            if (info.total == 0) {
                continue;
            }
            out = typeName + ": total = " + std::to_string(info.total) + ", count = " + std::to_string(info.count);
            DumpLog::GetInstance().Print(1, out);
        }
    }

private:
    struct MemInfo {
        size_t size = 0;
        std::string typeName = "Unknown";
    };

    struct TypeInfo {
        size_t count = 0;
        size_t total = 0;
    };

    std::map<void*, MemInfo> memoryMap_;
    std::map<std::string, TypeInfo> typeMap_;
    size_t total_ = 0;
    size_t count_ = 0;

    mutable std::mutex mutex_;
};

MemoryMonitor& MemoryMonitor::GetInstance()
{
    static MemoryMonitorImpl instance;
    return instance;
}
#endif // ACE_MEMORY_MONITOR

} // namespace OHOS::Ace
