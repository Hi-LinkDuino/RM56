/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FONT_RAM_ALLOCATOR_H
#define FONT_RAM_ALLOCATOR_H

#include <cstdint>
#include "gfx_utils/heap_base.h"

namespace OHOS {
class FontRamAllocator : public HeapBase {
    static constexpr uint8_t RAM_ALIGN = 2; // align up to 4 byte, power of 2
public:
    FontRamAllocator();

    FontRamAllocator(const FontRamAllocator&) = delete;

    FontRamAllocator& operator=(const FontRamAllocator&) = delete;

    FontRamAllocator(const FontRamAllocator&&) = delete;

    FontRamAllocator& operator=(const FontRamAllocator&&) = delete;

    ~FontRamAllocator();

    static FontRamAllocator& GetInstance(void)
    {
        static FontRamAllocator instance_;
        return instance_;
    }

    void SetRamAddr(uintptr_t ramAddr, uint32_t ramLen);
    void* Allocate(uint32_t size);
    uint32_t GetRamUsedLen();
private:
    uint32_t AlignUp(uint32_t addr);
    bool IsAligned(uint32_t addr);

    uintptr_t ramAddr_;
    uint32_t ramLen_;
    uintptr_t currentRamAddr_;
};
} // namespace OHOS
#endif /* FONT_RAM_ALLOCATOR_H */