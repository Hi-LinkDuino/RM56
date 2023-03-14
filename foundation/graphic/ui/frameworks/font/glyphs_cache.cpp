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

#include "font/glyphs_cache.h"
#include "font/font_ram_allocator.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
GlyphsCache::GlyphsCache()
    : nodeCache_(nullptr),
      cacheStatus_(nullptr),
      hasInit_(false)
{
}
GlyphsCache::~GlyphsCache() {}

int8_t GlyphsCache::CacheInit()
{
    if (hasInit_) {
        return RET_VALUE_OK;
    }

    cacheStatus_ = reinterpret_cast<CacheState *>(FontRamAllocator::GetInstance().Allocate(sizeof(CacheState)));
    if (cacheStatus_ == nullptr) {
        GRAPHIC_LOGE("GlyphsCache::CacheInit allocate cache status failed");
        return INVALID_RET_VALUE;
    }

    for (int32_t font = 0; font < FONT_HASH_NR; font++) {
        for (int32_t uc = 0; uc < UNICODE_HASH_NR; uc++) {
            (*cacheStatus_)[font][uc] = 0;
        }
    }

    nodeCache_ = reinterpret_cast<CacheType *>(FontRamAllocator::GetInstance().Allocate(sizeof(CacheType)));
    if (nodeCache_ == nullptr) {
        GRAPHIC_LOGE("GlyphsCache::CacheInit allocate node cache failed");
        return INVALID_RET_VALUE;
    }

    for (int32_t font = 0; font < FONT_HASH_NR; font++) {
        for (int32_t uc = 0; uc < UNICODE_HASH_NR; uc++) {
            for (int32_t node = 0; node < NODE_HASH_NR; node++) {
                (*nodeCache_)[font][uc][node].unicode = 0;
            }
        }
    }

    hasInit_ = true;
    return RET_VALUE_OK;
}

GlyphNode* GlyphsCache::GetNodeFromCache(uint32_t unicode, uint8_t fontId)
{
    GlyphNode* node = nullptr;

    uint8_t font = fontId & FONT_HASH_MASK;
    uint8_t uc = unicode & UNICODE_HASH_MASK;
    for (uint8_t i = 0; i < NODE_HASH_NR; i++) {
        GlyphNode* p = &((*nodeCache_)[font][uc][i]);
        if ((p->unicode == unicode) && (p->fontId == fontId)) {
            node = p;
            break;
        }
    }
    return node;
}

GlyphNode* GlyphsCache::GetNodeCacheSpace(uint32_t unicode, uint8_t fontId)
{
    uint8_t font = fontId & FONT_HASH_MASK;
    uint8_t uc = unicode & UNICODE_HASH_MASK;
    uint8_t i = (*cacheStatus_)[font][uc];
    GlyphNode* node = &((*nodeCache_)[font][uc][i]);

    i++;
    if (i >= NODE_HASH_NR) {
        i = 0;
    }
    (*cacheStatus_)[font][uc] = i;

    return node;
}
} // namespace OHOS
