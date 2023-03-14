/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "font/ui_font.h"
#include "common/text.h"
#include "font/ui_font_cache.h"
#if ENABLE_VECTOR_FONT
#include "font/ui_font_vector.h"
#else
#include "font/ui_font_bitmap.h"
#endif
#include "graphic_config.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
bool UIFont::setFontAllocFlag_ = false;
UIFont::UIFont() : instance_(nullptr), defaultInstance_(nullptr){}

UIFont::~UIFont(){}

UIFont* UIFont::GetInstance()
{
    static UIFont instance;
#if ENABLE_VECTOR_FONT
    if (instance.instance_ == nullptr) {
        instance.defaultInstance_ = new UIFontVector();
        instance.instance_ = instance.defaultInstance_;
        setFontAllocFlag_ = true;
    }
#else
    if (instance.instance_ == nullptr) {
        instance.defaultInstance_ = new UIFontBitmap();
        instance.instance_ = instance.defaultInstance_;
        setFontAllocFlag_ = true;
    }
#endif
    return &instance;
}

void UIFont::SetFont(BaseFont* font)
{
    if (font != nullptr) {
        if (defaultInstance_ != nullptr && setFontAllocFlag_) {
            delete defaultInstance_;
            defaultInstance_ = nullptr;
            setFontAllocFlag_ = false;
        }
        defaultInstance_ = font;
        instance_ = font;
    }
}

uint8_t* UIFont::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize,
                           uint8_t shapingFont)
{
    uint8_t* bitmap = nullptr;
#if ENABLE_MULTI_FONT
    // shaping font is in search list, search shaping font first
    if (shapingFont > 1) {
        bitmap = instance_->GetBitmap(unicode, glyphNode, shapingFont, fontSize);
        if (bitmap != nullptr) {
            return bitmap;
        }
    }
#endif
    bitmap = instance_->GetBitmap(unicode, glyphNode, fontId, fontSize);
    if (bitmap != nullptr) {
        return bitmap;
    }
#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
    int8_t currentIndex = 0;
    if ((searchLists == nullptr) || (listSize == 0)) {
        return nullptr;
    }
    do {
        bitmap = instance_->GetBitmap(unicode, glyphNode, searchLists[currentIndex], fontSize);
        if (bitmap != nullptr) {
            return bitmap;
        }
        // switch to next search List
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
#endif
    return nullptr;
}

int8_t UIFont::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize)
{
    int8_t result = instance_->GetGlyphNode(unicode, glyphNode, fontId, fontSize);
    if (result == RET_VALUE_OK) {
        return result;
    }

#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
    if ((searchLists == nullptr) || (listSize == 0)) {
        return INVALID_RET_VALUE;
    }
    int8_t currentIndex = 0;
    do {
        result = instance_->GetGlyphNode(unicode, glyphNode, searchLists[currentIndex], fontSize);
        if (result == RET_VALUE_OK) {
            return result;
        }
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
#endif
    return INVALID_RET_VALUE;
}

uint16_t UIFont::GetWidth(uint32_t unicode, uint8_t fontId, uint8_t fontSize, uint8_t shapingId)
{
    int16_t result;
#if ENABLE_MULTI_FONT
    if (shapingId > 1) {
        result = instance_->GetWidth(unicode, shapingId, fontSize);
        if (result >= 0) {
            return result;
        }
    }
#endif
    result = instance_->GetWidth(unicode, fontId, fontSize);
    if (result >= 0) {
        return result;
    }

#if ENABLE_MULTI_FONT
    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
    if ((searchLists == nullptr) || (listSize == 0)) {
        return 0;
    }
    int8_t currentIndex = 0;
    do {
        result = instance_->GetWidth(unicode, searchLists[currentIndex], fontSize);
        if (result >= 0) {
            return result;
        }
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
#endif
    return 0;
}

uint16_t UIFont::GetLineMaxHeight(const char* text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                                  uint16_t letterIndex, SizeSpan* sizeSpans)
{
    return instance_->GetLineMaxHeight(text, lineLength, fontId, fontSize, letterIndex, sizeSpans);
}
} // namespace OHOS
