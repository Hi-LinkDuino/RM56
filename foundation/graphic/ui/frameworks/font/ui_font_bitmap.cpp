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

#include "font/ui_font_bitmap.h"

#include "font/ui_font_adaptor.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "font/font_ram_allocator.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
#if ENABLE_SHAPING
#include "font/ui_text_shaping.h"
#endif

namespace OHOS {
UIFontBitmap::UIFontBitmap() : offset_(0), dynamicFont_()
{
    bitmapCache_ = nullptr;
}

UIFontBitmap::~UIFontBitmap()
{
    CloseFontFd();
    if (bitmapCache_ != nullptr) {
        delete bitmapCache_;
        bitmapCache_ = nullptr;
    }
}

void UIFontBitmap::CloseFontFd()
{
    for (uint16_t i = 0; i < dynamicFontFd_.Size(); i++) {
        if (dynamicFontFd_[i] >= 0) {
            close(dynamicFontFd_[i]);
            dynamicFontFd_[i] = -1;
        }
    }
}

bool UIFontBitmap::IsVectorFont() const
{
    return false;
}

uint8_t UIFontBitmap::GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const
{
#if ENABLE_MULTI_FONT
    return UIMultiFontManager::GetInstance()->GetShapingFontId(text, fontId, ttfId, script);
#else
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        return 0;
    }
    ttfId = fontParam->ttfId;
    return fontParam->shaping;
#endif
}

uint8_t UIFontBitmap::GetFontWeight(uint8_t fontId)
{
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        GRAPHIC_LOGE("UIFontBitmap::GetFontWeigh invalid fontId");
        return 0;
    }
    return fontParam->fontWeight;
}

int8_t UIFontBitmap::SetFontPath(const char* path, FontType type)
{
    if ((path == nullptr) || (type != DYNAMIC_FONT)) {
        GRAPHIC_LOGE("UIFontBitmap::SetFontPath invalid parameter");
        return INVALID_RET_VALUE;
    }
#ifdef _WIN32
    int32_t fontFd = open(path, O_RDONLY | O_BINARY);
#else
    int32_t fontFd = open(path, O_RDONLY);
#endif
    if (fontFd < 0) {
        GRAPHIC_LOGE("UIFontBitmap::SetFontPath file Open failed");
        return INVALID_RET_VALUE;
    }

    int32_t ret = dynamicFont_.SetFile(path, fontFd, offset_);
    if (ret == INVALID_RET_VALUE) {
        GRAPHIC_LOGE("GlyphsManager::SetFile failed");
        close(fontFd);
        return ret;
    }
    dynamicFontFd_.PushBack(fontFd);
    return RET_VALUE_OK;
}

uint16_t UIFontBitmap::GetHeight(uint8_t fontId, uint8_t fontSize)
{
    return dynamicFont_.GetFontHeight(fontId);
}

uint8_t UIFontBitmap::GetFontId(const char* ttfName, uint8_t size) const
{
    if (ttfName == nullptr) {
        return UIFontBuilder::GetInstance()->GetBitmapFontIdMax();
    }
    uint8_t id;
    for (id = 0; id < UIFontBuilder::GetInstance()->GetBitmapFontIdMax(); ++id) {
        UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(id);
        if (fontParam != nullptr) {
            if ((fontParam->size == size) && (strncmp(fontParam->ttfName, ttfName, TTF_NAME_LEN_MAX) == 0)) {
                break;
            }
        }
    }
    return id;
}

int16_t UIFontBitmap::GetWidth(uint32_t unicode, uint8_t fontId, uint8_t fontSize)
{
    return GetWidthInFontId(unicode, fontId);
}

uint8_t* UIFontBitmap::GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize)
{
    return SearchInFont(unicode, glyphNode, fontId);
}

bool UIFontBitmap::IsEmojiFont(uint8_t fontId)
{
    return false;
}

int8_t UIFontBitmap::GetFontHeader(FontHeader& fontHeader, uint8_t fontId, uint8_t fontSize)
{
    const FontHeader* header = dynamicFont_.GetFontHeader(fontId);
    if (header != nullptr) {
        fontHeader = *header;
        return RET_VALUE_OK;
    }
    return INVALID_RET_VALUE;
}

#if ENABLE_MULTI_FONT
int8_t UIFontBitmap::GetMultiGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId)
{
    int8_t ret = GetGlyphNode(unicode, glyphNode, fontId);
    if (ret == RET_VALUE_OK) {
        return ret;
    }

    uint8_t* searchLists = nullptr;
    int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
    int8_t currentIndex = 0;
    if ((searchLists == nullptr) || (listSize == 0)) {
        return INVALID_RET_VALUE;
    }
    do {
        ret = GetGlyphNode(unicode, glyphNode, searchLists[currentIndex]);
        if (ret != INVALID_RET_VALUE) {
            return ret;
        }
        // switch to next search List
        currentIndex++;
    } while ((currentIndex < listSize) && (searchLists != nullptr));
    return INVALID_RET_VALUE;
}
#endif

int8_t UIFontBitmap::GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize)
{
    const GlyphNode* node = dynamicFont_.GetGlyphNode(unicode, fontId);
    if (node != nullptr) {
        glyphNode = *node;
        return RET_VALUE_OK;
    }
    GRAPHIC_LOGE("UIFontBitmap::GetGlyphNode get glyphNode failed");
    return INVALID_RET_VALUE;
}

int8_t UIFontBitmap::GetFontVersion(FontType type, const char* path, char* version, uint8_t len)
{
    if (type != DYNAMIC_FONT) {
        return INVALID_RET_VALUE;
    }
    return dynamicFont_.GetFontVersion(path, version, len);
}

void UIFontBitmap::BitmapCacheInit()
{
    if (bitmapCache_ != nullptr) {
        return;
    }

    uint8_t* bitmapCacheAddr =
        reinterpret_cast<uint8_t*>(FontRamAllocator::GetInstance().Allocate(FONT_BITMAP_CACHE_SIZE));
    if (bitmapCacheAddr == nullptr) {
        GRAPHIC_LOGE("UIFontBitmap::BitmapCacheInit allocate failed");
        return;
    }
    bitmapCache_ = new UIFontCache(bitmapCacheAddr, FONT_BITMAP_CACHE_SIZE);
}

int8_t UIFontBitmap::SetCurrentLangId(uint8_t langId)
{
    return RET_VALUE_OK;
}

UITextLanguageFontParam* UIFontBitmap::GetFontInfo(uint8_t fontId) const
{
    return UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
}

int8_t UIFontBitmap::GetDynamicFontBitmap(uint32_t unicode, uint8_t* bitmap, uint8_t fontId)
{
    return dynamicFont_.GetBitmap(unicode, bitmap, fontId);
}

uint8_t* UIFontBitmap::GetCacheBitmap(uint8_t fontId, uint32_t unicode)
{
    if (bitmapCache_ != nullptr) {
        return bitmapCache_->GetBitmap(fontId, unicode);
    }
    GRAPHIC_LOGE("UIFontBitmap::GetCacheBitmap invalid bitmapCache");
    return nullptr;
}

uint8_t* UIFontBitmap::GetCacheSpace(uint8_t fontId, uint32_t unicode, uint32_t size)
{
    if (bitmapCache_ != nullptr) {
        return bitmapCache_->GetSpace(fontId, unicode, size);
    }
    GRAPHIC_LOGE("UIFontBitmap::GetCacheSpace invalid bitmapCache");
    return nullptr;
}

void UIFontBitmap::PutCacheSpace(uint8_t* addr)
{
    if (bitmapCache_ != nullptr) {
        bitmapCache_->PutSpace(addr);
    }
    GRAPHIC_LOGE("UIFontBitmap::PutCacheSpace invalid bitmapCache");
}

int16_t UIFontBitmap::GetDynamicFontWidth(uint32_t unicode, uint8_t fontId)
{
    return dynamicFont_.GetFontWidth(unicode, fontId);
}

uint8_t* UIFontBitmap::SearchInFont(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId)
{
    GraphicLockGuard guard(lock_);
    if (bitmapCache_ == nullptr) {
        return nullptr;
    }
    if (!UIFontAdaptor::IsSameTTFId(fontId, unicode)) {
        GRAPHIC_LOGE("UIFontBitmap::SearchInFont fontId and unicode not match");
        return nullptr;
    }
    int8_t ret = GetGlyphNode(unicode, glyphNode, fontId);
    if (ret != RET_VALUE_OK) {
        return nullptr;
    }
    uint8_t* bitmap = bitmapCache_->GetBitmap(fontId, unicode);
    if (bitmap != nullptr) {
        if (glyphNode.dataFlag == glyphNode.fontId && fontId == glyphNode.fontId) {
            return bitmap;
        } else {
            GRAPHIC_LOGE("DataFlag of bitmap node not equal to fontId.");
        }
    }
    if (glyphNode.kernOff <= glyphNode.dataOff) {
        return nullptr;
    }
    uint32_t bitmapSize = glyphNode.kernOff - glyphNode.dataOff;
    if (bitmap == nullptr) {
        bitmap = bitmapCache_->GetSpace(fontId, unicode, bitmapSize);
    }
    ret = dynamicFont_.GetBitmap(unicode, bitmap, fontId);
    if (ret == RET_VALUE_OK) {
        return bitmap;
    }
    bitmapCache_->PutSpace(bitmap);
    return nullptr;
}

int16_t UIFontBitmap::GetWidthInFontId(uint32_t unicode, uint8_t fontId)
{
    if (!UIFontAdaptor::IsSameTTFId(fontId, unicode)) {
        GRAPHIC_LOGE("UIFontBitmap::GetWidthInFontId fontId and unicode not match");
        return INVALID_RET_VALUE;
    }
    return GetDynamicFontWidth(unicode, fontId);
}

void UIFontBitmap::SetFontFileOffset(uint32_t offset)
{
    offset_ = offset;
}

uint16_t UIFontBitmap::GetOffsetPosY(const char *text, uint16_t lineLength,
                                     bool& isEmoijLarge, uint8_t fontId, uint8_t fontSize)
{
    uint32_t i = 0;
    uint32_t unicode;
    uint16_t textNum = 0;
    uint16_t emojiNum = 0;

    uint16_t loopNum = 0;
    GlyphNode glyphNode;
    GlyphNode emoijMaxNode = {};
    uint8_t maxFontSie = fontSize;
    while (i < lineLength) {
        unicode = TypedText::GetUTF8Next(text, i, i);
#if ENABLE_MULTI_FONT
        uint8_t ret = GetMultiGlyphNode(unicode, glyphNode, fontId);
#else
        uint8_t ret = GetGlyphNode(unicode, glyphNode, fontId, fontSize);
#endif
        if (ret == RET_VALUE_OK) {
            uint8_t weight = GetFontWeight(glyphNode.fontId);
            if (weight >= 16) { // 16: bit rgb565 rgba8888
                emoijMaxNode = glyphNode.rows > emoijMaxNode.rows ? glyphNode : emoijMaxNode;
                emojiNum++;
            } else {
                textNum++;
            }
            loopNum++;
        }
    }
    // The number of emoji is the same as the number of cycles, indicating that this line is all emoji
    // The number of words is the same as the number of cycles, which means that this line is all words
    if ((emojiNum == loopNum) || (textNum == loopNum)) {
        isEmoijLarge = true;
        return 0;
    }
    isEmoijLarge = emoijMaxNode.rows > maxFontSie;
    uint16_t offset = 0;
    if (isEmoijLarge) {
        // If the emoji is higher than the text
        if (emoijMaxNode.top >= maxFontSie) {
            offset = emoijMaxNode.top - maxFontSie;
        }
    } else {
        // If text are higher than emoji
        if (maxFontSie >= emoijMaxNode.rows) {
            // should top - ros.
            offset = maxFontSie - emoijMaxNode.rows;
        }
    }
    return offset;
}

uint16_t UIFontBitmap::GetLineMaxHeight(const char *text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                                        uint16_t& letterIndex, SizeSpan* sizeSpans)
{
    uint16_t maxHeight = GetHeight(fontId, fontSize);
    if (sizeSpans == nullptr) {
        return maxHeight;
    }

    uint32_t i = 0;
    uint32_t unicode;
    GlyphNode glyphNode;
    while (i < lineLength) {
        unicode = TypedText::GetUTF8Next(text, i, i);
        if (sizeSpans != nullptr && sizeSpans[letterIndex].isSizeSpan) {
            uint16_t spannableHeight = 0;
            if (sizeSpans[letterIndex].height == 0) {
                spannableHeight = GetHeight(sizeSpans[letterIndex].fontId, sizeSpans[letterIndex].size);
                sizeSpans[letterIndex].height = spannableHeight;
            } else {
                spannableHeight = sizeSpans[letterIndex].height;
            }
            maxHeight = spannableHeight > maxHeight ? spannableHeight : maxHeight;
        }

        letterIndex++;
        if (i > 0 && ((text[i - 1] == '\r') || (text[i - 1] == '\n'))) {
            break;
        }
    }

    return maxHeight;
}

void UIFontBitmap::SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen)
{
    BaseFont::SetPsramMemory(psramAddr, psramLen);
    FontRamAllocator::GetInstance().SetRamAddr(psramAddr, psramLen);
    BitmapCacheInit();
}
} // namespace OHOS
