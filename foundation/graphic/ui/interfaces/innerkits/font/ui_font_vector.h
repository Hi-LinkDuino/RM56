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

#ifndef UI_FONT_VECTOR_H
#define UI_FONT_VECTOR_H
#include "font/base_font.h"
#include "graphic_config.h"
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/tttables.h"
#include "font/ui_font_cache.h"
#include <memory>

namespace OHOS {
class UIFontVector : public BaseFont {
public:
    UIFontVector();

    ~UIFontVector();
    UIFontVector(const UIFontVector&) = delete;
    UIFontVector& operator=(const UIFontVector&) noexcept = delete;
    bool IsVectorFont() const override;
    int8_t SetFontPath(const char* path, FontType type) override;
    uint16_t GetHeight(uint8_t fontId, uint8_t fontSize) override;
    uint8_t GetFontId(const char* ttfName, uint8_t fontSize = 0) const override;
    int16_t GetWidth(uint32_t unicode, uint8_t fontId, uint8_t fontSize) override;
    uint8_t* GetBitmap(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize) override;
    int8_t GetFontHeader(FontHeader& fontHeader, uint8_t fontId, uint8_t fontSize) override;
    int8_t GetGlyphNode(uint32_t unicode, GlyphNode& glyphNode, uint8_t fontId, uint8_t fontSize) override;
    uint8_t GetFontWeight(uint8_t fontId) override;
    uint8_t GetShapingFontId(char* text, uint8_t& ttfId, uint32_t& script,
        uint8_t fontId, uint8_t size)  const override;
    uint8_t RegisterFontInfo(const char* ttfName, uint8_t shaping = 0) override;
    uint8_t RegisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num) override;
    uint8_t UnregisterFontInfo(const char* ttfName) override;
    uint8_t UnregisterFontInfo(const UITextLanguageFontParam* fontsTable, uint8_t num) override;
    const UITextLanguageFontParam* GetFontInfo(uint8_t fontId) const override;
    int32_t OpenVectorFont(uint8_t ttfId) override;
    bool IsColorEmojiFont(FT_Face &face);
    uint16_t GetOffsetPosY(const char* text, uint16_t lineLength,
                           bool& isEmojiLarge, uint8_t fontId, uint8_t fontSize) override;
    uint16_t GetLineMaxHeight(const char* text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                              uint16_t& letterIndex, SizeSpan* sizeSpans)  override;

    bool IsEmojiFont(uint8_t fontId) override;
private:
    static constexpr uint8_t FONT_ID_MAX = 0xFF;
    static constexpr uint8_t FONT_INVALID_TTF_ID = 0xFF;
    static constexpr uint8_t TTF_NAME_LEN_MAX = 128;
    static constexpr uint8_t FONT_BPP_8 = 8;
    UITextLanguageFontParam fontInfo_[FONT_ID_MAX] = {{0}};
    std::string ttfDir_;
    FT_Library ftLibrary_;
    FT_Face ftFaces_[FONT_ID_MAX] = {0};
    uint8_t currentFontInfoNum_ = 0;
    bool freeTypeInited_;
    UIFontCache* bitmapCache_;
    struct FaceInfo {
        FT_Face face;
        uint32_t key;
    };
    struct Metric {
        int left;
        int top;
        int cols;
        int rows;
        int advance;
        uint8_t buf[0];
    };
    void SetFace(FaceInfo& faceInfo, uint32_t unicode) const;
#if ENABLE_VECTOR_FONT
    void SetFace(FaceInfo& faceInfo, uint32_t unicode, TextStyle textStyle) const;
#endif
    uint8_t GetFontId(uint32_t unicode) const;
    uint32_t GetKey(uint8_t fontId, uint32_t size);
    int8_t LoadGlyphIntoFace(uint8_t& fontId, uint32_t unicode, FT_Face face);
#if ENABLE_VECTOR_FONT
    int8_t LoadGlyphIntoFace(uint8_t& fontId, uint32_t unicode, FT_Face face, TextStyle textStyle);
#endif
    uint8_t IsGlyphFont(uint32_t unicode);
#if ENABLE_VECTOR_FONT
    void SetItaly(FT_GlyphSlot slot);
    void SetBold(uint8_t fontId);
#endif
    int8_t GetFaceInfo(uint8_t fontId, uint8_t fontSize, FaceInfo& faceInfo);
    uint16_t GetMaxSubLineHeight(uint16_t textNum, uint16_t loopNum, uint16_t maxHeight, uint16_t emojiNum);
};
} // namespace OHOS
#endif
