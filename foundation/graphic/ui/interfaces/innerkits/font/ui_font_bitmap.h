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

#ifndef UI_FONT_BITMAP_H
#define UI_FONT_BITMAP_H

#include "font/base_font.h"
#include "font/glyphs_manager.h"
#include "font/ui_font_cache.h"
#include "graphic_locker.h"

namespace OHOS {
class UIFontBitmap : public BaseFont {
public:
    UIFontBitmap();
    virtual ~UIFontBitmap();
    UIFontBitmap(const UIFontBitmap&) = delete;
    UIFontBitmap& operator=(const UIFontBitmap&) noexcept = delete;
    bool IsVectorFont() const override;
    uint8_t GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script, uint8_t fontId, uint8_t size) const override;
    int8_t SetFontPath(const char* path, FontType type) override;
    uint16_t GetHeight(uint8_t fontId, uint8_t fontSize = 0) override;
    uint8_t GetFontId(const char* ttfName, uint8_t fontSize = 0) const override;
    int16_t GetWidth(uint32_t unicode, uint8_t fontId, uint8_t fontSize = 0) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize = 0) override;
    int8_t GetFontHeader(FontHeader& fontHeader, uint8_t fontId, uint8_t fontSize = 0) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize = 0) override;
    uint8_t GetFontWeight(uint8_t fontId) override;
    int8_t GetFontVersion(FontType type, const char* path, char* version, uint8_t len) override;
    int8_t SetCurrentLangId(uint8_t langId) override;
    UITextLanguageFontParam* GetFontInfo(uint8_t fontId) const override;
    void SetFontFileOffset(uint32_t offset) override;
    uint16_t GetOffsetPosY(const char* text, uint16_t lineLength, bool& isEmoijLerge,
                           uint8_t fontId, uint8_t fontSize) override;
    uint16_t GetLineMaxHeight(const char* text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                              uint16_t& letterIndex, SizeSpan* sizeSpans) override;
    bool IsEmojiFont(uint8_t fontId) override;
    void SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen) override;
protected:
    int8_t GetDynamicFontBitmap(uint32_t unicode, uint8_t* bitmap, uint8_t fontId);
    uint8_t* GetCacheBitmap(uint8_t fontId, uint32_t unicode);
    uint8_t* GetCacheSpace(uint8_t fontId, uint32_t unicode, uint32_t size);
    void PutCacheSpace(uint8_t* addr);
    int16_t GetDynamicFontWidth(uint32_t unicode, uint8_t fontId);
    uint32_t offset_;
    GraphicMutex lock_;

private:
    uint8_t* SearchInFont(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId);
    int16_t GetWidthInFontId(uint32_t unicode, uint8_t fontId);
#if ENABLE_MULTI_FONT
    int8_t GetMultiGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId);
#endif
    void CloseFontFd();
    void BitmapCacheInit();
    static constexpr uint32_t FONT_BITMAP_CACHE_SIZE = 0x64000;
    static constexpr uint8_t FONT_ID_MAX = 0xFF;
    static constexpr uint8_t TTF_NAME_LEN_MAX = 128;
    GlyphsManager dynamicFont_;
    UIFontCache* bitmapCache_;
    Graphic::Vector<int32_t> dynamicFontFd_;
};
} // namespce OHOS
#endif // UI_FONT_BITMAP_H
