/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "font/glyphs_manager.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
GlyphsManager::GlyphsManager() {}

GlyphsManager::~GlyphsManager()
{
    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        delete glyphsFiles_[i];
        glyphsFiles_[i] = nullptr;
    }
}

int8_t GlyphsManager::SetFile(const char* fontName, int32_t fp, uint32_t start)
{
    if (glyphsCache_.CacheInit() != RET_VALUE_OK) {
        GRAPHIC_LOGE("GlyphsManager::SetFile cache init failed");
        return INVALID_RET_VALUE;
    }

    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        if (glyphsFiles_[i]->IsSameFile(fontName)) {
            GRAPHIC_LOGE("GlyphsManager::SetFile font name repeat");
            return INVALID_RET_VALUE;
        }
    }

    GlyphsFile* instance = new GlyphsFile();
    if (instance == nullptr) {
        GRAPHIC_LOGE("GlyphsManager::SetFile new failed");
        return INVALID_RET_VALUE;
    }

    if (instance->SetFile(fontName, fp, start) != RET_VALUE_OK) {
        delete instance;
        instance = nullptr;
        return INVALID_RET_VALUE;
    }

    glyphsFiles_.PushBack(instance);
    return RET_VALUE_OK;
}

int8_t GlyphsManager::GetFontVersion(const char* fontName, char* version, uint8_t len)
{
    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        if (glyphsFiles_[i]->GetFontVersion(fontName, version, len) == RET_VALUE_OK) {
            return RET_VALUE_OK;
        }
    }
    return INVALID_RET_VALUE;
}

const FontHeader* GlyphsManager::GetFontHeader(uint8_t fontId)
{
    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        const FontHeader* tmp = glyphsFiles_[i]->GetFontHeader(fontId);
        if (tmp != nullptr) {
            return tmp;
        }
    }

    return nullptr;
}

const GlyphNode* GlyphsManager::GetGlyphNode(uint32_t unicode, uint8_t fontId)
{
    GlyphNode* node = glyphsCache_.GetNodeFromCache(unicode, fontId);
    if (node != nullptr) {
        return node;
    }

    int8_t ret = INVALID_RET_VALUE;
    GlyphNode nodeInfo;
    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        ret = glyphsFiles_[i]->GetNodeFromFile(unicode, fontId, nodeInfo);
        if (ret == RET_VALUE_OK) {
            nodeInfo.fontId = fontId;
            break;
        }
    }

    if (ret != RET_VALUE_OK) {
        return nullptr;
    }

    node = glyphsCache_.GetNodeCacheSpace(unicode, fontId);
    if (node != nullptr) {
        *node = nodeInfo;
    }

    return node;
}

int16_t GlyphsManager::GetFontHeight(uint8_t fontId)
{
    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        int16_t height = glyphsFiles_[i]->GetFontHeight(fontId);
        if (height != INVALID_RET_VALUE) {
            return height;
        }
    }

    return INVALID_RET_VALUE;
}

int16_t GlyphsManager::GetFontWidth(uint32_t unicode, uint8_t fontId)
{
    const GlyphNode* node = GetGlyphNode(unicode, fontId);
    if (node == nullptr) {
        return INVALID_RET_VALUE;
    }
    return node->advance;
}

int8_t GlyphsManager::GetBitmap(uint32_t unicode, uint8_t* bitmap, uint8_t fontId)
{
    GlyphNode* node = const_cast<GlyphNode *>(GetGlyphNode(unicode, fontId));
    if (node == nullptr) {
        GRAPHIC_LOGE("GlyphsManager::GetBitmap node not found");
        return INVALID_RET_VALUE;
    }

    for (uint16_t i = 0; i < glyphsFiles_.Size(); i++) {
        int8_t ret = glyphsFiles_[i]->GetBitmap(*node, bitmap);
        if (ret == RET_VALUE_OK) {
            return RET_VALUE_OK;
        }
    }
    return INVALID_RET_VALUE;
}
} // namespace OHOS
