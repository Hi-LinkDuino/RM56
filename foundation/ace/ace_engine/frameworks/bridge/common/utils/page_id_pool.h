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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_PAGE_ID_POOL_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_PAGE_ID_POOL_H

#include <atomic>
#include <cstdint>

#include "base/utils/noncopyable.h"

namespace OHOS::Ace::Framework {

inline constexpr int32_t INVALID_PAGE_ID = -1;
inline constexpr int32_t MAX_PAGE_ID_SIZE = sizeof(uint64_t) * 8;

class PageIdPool final : private NonCopyable {
public:
    int32_t GenerateNextPageId()
    {
        for (int32_t idx = 0; idx < MAX_PAGE_ID_SIZE; ++idx) {
            uint64_t bitMask = (1ULL << idx);
            if ((bitMask & pageIdPool_.fetch_or(bitMask, std::memory_order_relaxed)) == 0) {
                return idx;
            }
        }
        return INVALID_PAGE_ID;
    }

    void RecyclePageId(int32_t pageId)
    {
        if (pageId < 0 || pageId >= MAX_PAGE_ID_SIZE) {
            return;
        }
        uint64_t bitMask = (1ULL << pageId);
        pageIdPool_.fetch_and(~bitMask, std::memory_order_relaxed);
    }

private:
    std::atomic<uint64_t> pageIdPool_ = 0;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_PAGE_ID_POOL_H
