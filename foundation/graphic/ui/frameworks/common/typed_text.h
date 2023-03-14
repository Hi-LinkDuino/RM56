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

#ifndef GRAPHIC_LITE_TYPE_TEXT_H
#define GRAPHIC_LITE_TYPE_TEXT_H
#include "common/text.h"
#include "graphic_config.h"
#ifndef _FONT_TOOL
#include "components/ui_arc_label.h"
#include "font/ui_font_header.h"
#include "gfx_utils/geometry2d.h"
#endif

namespace OHOS {
#ifndef _FONT_TOOL
class TypedText : public HeapBase {
public:
    static constexpr uint32_t MAX_UINT16_LOW_SCOPE = 0xFFFF;
    static constexpr uint32_t MAX_UINT16_HIGH_SCOPE = 0xEFFFF;
    static constexpr uint32_t UTF16_LOW_MASK = 0x03FF;
    static constexpr uint32_t UTF16_LOW_PARAM = 56320;
    static constexpr uint32_t UTF16_HIGH_PARAM1 = 55296;
    static constexpr uint8_t UTF16_HIGH_PARAM2 = 64;
    static constexpr uint8_t UTF16_HIGH_SHIFT = 10;
    static constexpr uint8_t UTF8_TO_UNICODE_SHIFT1 = 6;
    static constexpr uint8_t UTF8_TO_UNICODE_SHIFT2 = 12;
    static constexpr uint8_t UTF8_TO_UNICODE_SHIFT3 = 18;

    static Point GetTextSize(const char* text,
                             uint8_t fontId,
                             uint8_t fontSize,
                             int16_t letterSpace,
                             int16_t lineHeight,
                             int16_t maxWidth,
                             int8_t lineSpace,
                             SizeSpan* sizeSpans = nullptr);

    static uint32_t GetNextLine(const char* text,
                                uint8_t fontId,
                                uint8_t fontSize,
                                int16_t letterSpace,
                                int16_t maxWidth);

    static int16_t GetTextWidth(const char* text,
                                uint8_t fontId,
                                uint8_t fontSize,
                                uint16_t length,
                                int16_t letterSpace);

    static Rect GetArcTextRect(const char* text,
                                uint8_t fontId,
                                uint8_t fontSize,
                               const Point& arcCenter,
                               int16_t letterSpace,
                               UIArcLabel::TextOrientation orientation,
                               const UIArcLabel::ArcTextInfo& arcTextInfo);

    static float GetAngleForArcLen(float len,
                                   uint16_t height,
                                   uint16_t radius,
                                   UITextLanguageDirect direct,
                                   UIArcLabel::TextOrientation orientation);

    static void GetArcLetterPos(const Point& arcCenter, uint16_t radius, float angle, float& posX, float& posY);

#else  // _FONT_TOOL
class TypedText {
public:
#endif // _FONT_TOOL
    static uint32_t GetUTF8Next(const char* text, uint32_t i, uint32_t& j);
    static uint8_t GetUTF8OneCharacterSize(const char* str);
    static uint32_t GetByteIndexFromUTF8Id(const char* text, uint32_t utf8Id);
    static uint32_t GetUTF8CharacterSize(const char* text, uint32_t byteIndex = UINT32_MAX);
    static void Utf8ToUtf16(const char* utf8Str, uint16_t* utf16Str, uint32_t len);
    static uint32_t GetUtf16Cnt(const char* utf8Str);

    static bool IsEmoji(uint32_t codePoint);
    static bool IsEmojiModifier(uint32_t codePoint);
    static bool IsEmojiBase(uint32_t codePoint);

    static bool IsColourWord(uint32_t codePoint, uint8_t fontId, uint8_t fontSize);

private:
    static bool GetWrapPoint(const char* text, uint32_t& breakPoint);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_TYPE_TEXT_H
