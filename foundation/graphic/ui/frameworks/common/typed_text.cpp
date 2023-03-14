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

#include "common/typed_text.h"
#include "font/ui_font.h"
#include "font/ui_font_adaptor.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "gfx_utils/transform.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
#ifndef _FONT_TOOL
Point TypedText::GetTextSize(const char* text, uint8_t fontId, uint8_t fontSize, int16_t letterSpace,
                             int16_t lineHeight, int16_t maxWidth, int8_t lineSpace, SizeSpan* sizeSpans)
{
    Point size{0, 0};

    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetTextSize invalid parameter");
        return size;
    }

    uint32_t lineBegin = 0;
    uint32_t newLineBegin = 0;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight(fontId, fontSize);
    uint16_t height = lineHeight;
    if (lineHeight == 0) {
        lineHeight = letterHeight + lineSpace;
    }

    uint16_t letterIndex = 0;
    while (text[lineBegin] != '\0') {
        int16_t lineWidth = maxWidth;
        newLineBegin += UIFontAdaptor::GetNextLineAndWidth(&text[lineBegin], fontId, fontSize, letterSpace,
                                                           lineWidth, lineHeight, letterIndex, sizeSpans);
        if (newLineBegin == lineBegin) {
            break;
        }
        size.y += lineHeight;
        size.x = MATH_MAX(lineWidth, size.x);
        lineBegin = newLineBegin;
    }

    if ((lineBegin != 0) && ((text[lineBegin - 1] == '\n') || (text[lineBegin - 1] == '\r'))) {
        size.y += lineHeight;
    }

    if (height == 0) {
        if (size.y == 0) {
            size.y = letterHeight;
        } else {
            size.y -= lineSpace;
        }
    } else {
        if (size.y == 0) {
            size.y = lineHeight;
        }
        if (lineHeight < letterHeight) {
            size.y += letterHeight - lineHeight;
        }
    }
    size.y += EXTRA_HEIGHT;
    return size;
}

Rect TypedText::GetArcTextRect(const char* text,
                               uint8_t fontId,
                               uint8_t fontSize,
                               const Point& arcCenter,
                               int16_t letterSpace,
                               UIArcLabel::TextOrientation orientation,
                               const UIArcLabel::ArcTextInfo& arcTextInfo)
{
    if ((text == nullptr) || (arcTextInfo.lineStart == arcTextInfo.lineEnd) || (arcTextInfo.radius == 0)) {
        GRAPHIC_LOGE("TypedText::GetArcTextRect invalid parameter\n");
        return Rect();
    }

    uint16_t letterHeight = UIFont::GetInstance()->GetHeight(fontId, fontSize);
    bool xorFlag = (orientation == UIArcLabel::TextOrientation::INSIDE) ^ (arcTextInfo.direct == TEXT_DIRECT_LTR);
    float posX = 0;
    float posY = 0;
    uint32_t i = arcTextInfo.lineStart;
    float angle = arcTextInfo.startAngle;
    Rect rect;
    Rect rectLetter;
    TransformMap transform;
    while (i < arcTextInfo.lineEnd) {
        uint32_t tmp = i;
        uint32_t letter = GetUTF8Next(text, tmp, i);
        if (letter == 0) {
            continue;
        }
        if ((letter == '\r') || (letter == '\n')) {
            break;
        }
        uint16_t letterWidth = UIFont::GetInstance()->GetWidth(letter, fontId, fontSize, 0);
        if (tmp == arcTextInfo.lineStart) {
            angle += xorFlag ? GetAngleForArcLen(static_cast<float>(letterWidth), letterHeight, arcTextInfo.radius,
                                                 arcTextInfo.direct, orientation)
                             : 0;
            GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
            rect.SetPosition(MATH_ROUND(posX), MATH_ROUND(posY));
        }
        rectLetter.SetPosition(MATH_ROUND(posX), MATH_ROUND(posY));
        rectLetter.Resize(letterWidth, letterHeight);
        transform.SetTransMapRect(rectLetter);

        uint16_t arcLen = letterWidth + letterSpace;
        if (arcLen == 0) {
            continue;
        }
        float incrementAngle = GetAngleForArcLen(static_cast<float>(arcLen), letterHeight, arcTextInfo.radius,
                                                 arcTextInfo.direct, orientation);
        float rotateAngle =
            (orientation == UIArcLabel::TextOrientation::INSIDE) ? angle : (angle - SEMICIRCLE_IN_DEGREE);
        // 2: letterWidth's half
        float fineTuningAngle = incrementAngle * (static_cast<float>(letterWidth) / (2 * arcLen));
        rotateAngle += (xorFlag ? -fineTuningAngle : fineTuningAngle);

        transform.Rotate(MATH_ROUND(rotateAngle), Vector2<float>(0, 0));
        rect.Join(rect, transform.GetBoxRect());

        angle += incrementAngle;
        GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
    }
    return rect;
}

float TypedText::GetAngleForArcLen(float len,
                                   uint16_t height,
                                   uint16_t radius,
                                   UITextLanguageDirect direct,
                                   UIArcLabel::TextOrientation orientation)
{
    if (radius == 0) {
        return 0;
    }
    float realRadius =
        static_cast<float>((orientation == UIArcLabel::TextOrientation::OUTSIDE) ? (radius + height) : radius);
    float angle = static_cast<float>(len * SEMICIRCLE_IN_DEGREE) / (UI_PI * realRadius);
    return (direct == TEXT_DIRECT_LTR) ? angle : -angle;
}

void TypedText::GetArcLetterPos(const Point& arcCenter, uint16_t radius, float angle, float& posX, float& posY)
{
    posX = arcCenter.x + (static_cast<float>(radius) * Sin(angle));
    posY = arcCenter.y - (static_cast<float>(radius) * Sin(angle + QUARTER_IN_DEGREE));
}

uint32_t TypedText::GetNextLine(const char* text, uint8_t fontId, uint8_t fontSize, int16_t letterSpace,
                                int16_t maxWidth)
{
    uint32_t index = 0;
    if ((text == nullptr) || (GetWrapPoint(text, index) &&
        (TypedText::GetTextWidth(text, fontId, fontSize, index, letterSpace) <= maxWidth))) {
        return index;
    }
    uint32_t lastBreakPos = 0;
    int16_t curW;
    uint32_t tmp = 0;
    while (true) {
        curW = TypedText::GetTextWidth(text, fontId, fontSize, index, letterSpace);
        if (curW > maxWidth) {
            index = lastBreakPos;
            if (lastBreakPos == 0) {
                curW = 0;
                uint32_t i = 0;
                uint32_t letter;
                uint16_t letterWidth;
                while (text[i] != '\0') {
                    tmp = i;
                    letter = TypedText::GetUTF8Next(text, tmp, i);
                    letterWidth = UIFont::GetInstance()->GetWidth(letter, fontId, fontSize, 0);
                    curW += letterWidth;
                    if (letterWidth > 0) {
                        curW += letterSpace;
                    }
                    if (curW > maxWidth) {
                        index = lastBreakPos;
                        return index;
                    }
                    lastBreakPos = i;
                }
            }
            break;
        }
        if ((index > 0) && (index < strlen(text)) && ((text[index - 1] == '\r') || (text[index - 1] == '\n'))) {
            break;
        }
        lastBreakPos = index;
        if (text[index] == '\0') {
            break;
        }
        if (GetWrapPoint(text + index, tmp) &&
            (TypedText::GetTextWidth(text, fontId, fontSize, index + tmp, letterSpace) <= maxWidth)) {
            return index + tmp;
        }
        index += tmp;
        if (lastBreakPos == index) {
            break;
        }
    }
    return index;
}

bool TypedText::GetWrapPoint(const char* text, uint32_t& breakPoint)
{
    breakPoint = 0;
    uint32_t j = 0;
    uint32_t letter = 0;
    if (text == nullptr) {
        return true;
    }

    while (text[breakPoint] != '\0') {
        letter = GetUTF8Next(text, breakPoint, j);
        breakPoint = j;
        if ((letter == ' ') || (letter == '.') || (letter == ',') || (letter == '!') || (letter == '=')
            || (letter == '?')) {
            return false;
        }
        if (letter == '\n') {
            return true;
        }
        if ((letter == '\r') && (GetUTF8Next(text, breakPoint, j) == '\n')) {
            breakPoint = j;
            return true;
        }
    }
    return false;
}

int16_t TypedText::GetTextWidth(const char* text, uint8_t fontId, uint8_t fontSize, uint16_t length,
                                int16_t letterSpace)
{
    if ((text == nullptr) || (length == 0) || (length > strlen(text))) {
        GRAPHIC_LOGE("TypedText::GetTextWidth invalid parameter\n");
        return 0;
    }

    uint32_t i = 0;
    uint16_t width = 0;
    uint32_t letter;

    while (i < length) {
        letter = GetUTF8Next(text, i, i);
        if ((letter == 0) || (letter == '\n') || (letter == '\r')) {
            continue;
        }
        uint16_t charWidth = UIFont::GetInstance()->GetWidth(letter, fontId, fontSize, 0);
        width += charWidth + letterSpace;
    }
    if (width > 0) {
        width -= letterSpace;
    }
    return width;
}
#endif // _FONT_TOOL

uint8_t TypedText::GetUTF8OneCharacterSize(const char* str)
{
    if (str == nullptr) {
        return 0;
    }
    if ((str[0] & 0x80) == 0) {
        return 1;
    } else if ((str[0] & 0xE0) == 0xC0) {
        return 2; // 2: 2 bytes
    } else if ((str[0] & 0xF0) == 0xE0) {
        return 3; // 3: 3 bytes
    } else if ((str[0] & 0xF8) == 0xF0) {
        return 4; // 4: 4 bytes
    }
    return 0;
}

uint32_t TypedText::GetUTF8Next(const char* text, uint32_t i, uint32_t& j)
{
    uint32_t unicode = 0;
    if (text == nullptr) {
        GRAPHIC_LOGE("text invalid parameter");
        return 0;
    }

    j = i;
    uint8_t lettetSize = GetUTF8OneCharacterSize(text + i);
    switch (lettetSize) {
        case 1:
            unicode = text[j];
            break;
        case 2: // 2: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x1F) << UTF8_TO_UNICODE_SHIFT1;
            j++;
            if ((text[j] & 0xC0) != 0x80) {
                return 0;
            }
            unicode += (text[j] & 0x3F);
            break;
        case 3: // 3: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x0F) << UTF8_TO_UNICODE_SHIFT2;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT1;
            unicode += (text[++j] & 0x3F);
            break;
        case 4: // 4: letter size
            unicode = static_cast<uint32_t>(text[j] & 0x07) << UTF8_TO_UNICODE_SHIFT3;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT2;
            unicode += static_cast<uint32_t>(text[++j] & 0x3F) << UTF8_TO_UNICODE_SHIFT1;
            unicode += text[++j] & 0x3F;
            break;
        default:
            break;
    }
    j++;
    return unicode;
}

uint32_t TypedText::GetByteIndexFromUTF8Id(const char* text, uint32_t utf8Id)
{
    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetByteIndexFromUTF8Id text invalid parameter\n");
        return 0;
    }
    uint32_t byteIndex = 0;
    for (uint32_t i = 0; i < utf8Id; i++) {
        byteIndex += GetUTF8OneCharacterSize(&text[byteIndex]);
    }

    return byteIndex;
}

uint32_t TypedText::GetUTF8CharacterSize(const char* text, uint32_t byteIndex)
{
    uint32_t i = 0;
    uint32_t size = 0;

    if (text == nullptr) {
        GRAPHIC_LOGE("TypedText::GetUTF8CharacterSize text invalid parameter\n");
        return 0;
    }
    while ((i < byteIndex) && (text[i] != '\0')) {
        GetUTF8Next(text, i, i);
        size++;
    }

    return size;
}

void TypedText::Utf8ToUtf16(const char* utf8Str, uint16_t* utf16Str, uint32_t len)
{
    if ((utf8Str == nullptr) || (utf16Str == nullptr)) {
        GRAPHIC_LOGE("utf8Str or u16Str is null");
        return;
    }

    uint32_t i = 0;
    uint32_t cnt = 0;
    while (utf8Str[i] != '\0') {
        uint32_t unicode = GetUTF8Next(utf8Str, i, i);
        if (cnt < len) {
            if (unicode <= MAX_UINT16_LOW_SCOPE) {
                utf16Str[cnt] = (unicode & MAX_UINT16_LOW_SCOPE);
            } else if (unicode <= MAX_UINT16_HIGH_SCOPE) {
                if (cnt + 1 < len) {
                    utf16Str[cnt] = static_cast<uint16_t>(UTF16_LOW_PARAM + (unicode & UTF16_LOW_MASK)); // low
                    cnt++;
                    utf16Str[cnt] = static_cast<uint16_t>(UTF16_HIGH_PARAM1 + (unicode >> UTF16_HIGH_SHIFT) -
                                                          UTF16_HIGH_PARAM2); // high
                }
            } else {
                GRAPHIC_LOGE("Invalid unicode");
                return;
            }
            cnt++;
        }
    }
}

uint32_t TypedText::GetUtf16Cnt(const char* utf8Str)
{
    if (utf8Str == nullptr) {
        GRAPHIC_LOGE("text invalid parameter");
        return 0;
    }
    uint32_t len = 0;
    uint32_t i = 0;

    while (utf8Str[i] != '\0') {
        uint32_t unicode = GetUTF8Next(utf8Str, i, i);
        if (unicode <= MAX_UINT16_LOW_SCOPE) {
            len++;
        } else if (unicode <= MAX_UINT16_HIGH_SCOPE) {
            len += 2; // 2: low and high, two uint16_t numbers
        } else {
            GRAPHIC_LOGE("Invalid unicode");
            return 0;
        }
    }
    return len;
}

bool TypedText::IsEmoji(uint32_t codePoint)
{
    // Miscellaneous symbols and symbol fonts
    return (codePoint >= 0x2600 && codePoint <= 0x27BF) || codePoint == 0x303D || codePoint == 0x2049 ||
            codePoint == 0x203C || (codePoint >= 0x2000 && codePoint <= 0x200F) ||
            (codePoint >= 0x2028 && codePoint <= 0x202F) || codePoint == 0x205F ||
            // Area occupied by punctuation, Alphabetic symbol
            (codePoint >= 0x2065 && codePoint <= 0x206F) || (codePoint >= 0x2100 && codePoint <= 0x214F) ||
            // Various technical symbols, Arrow A
            (codePoint >= 0x2300 && codePoint <= 0x23FF) || (codePoint >= 0x2B00 && codePoint <= 0x2BFF) ||
            // Arrow B,Chinese symbols
            (codePoint >= 0x2900 && codePoint <= 0x297F) || (codePoint >= 0x3200 && codePoint <= 0x32FF) ||
            // High and low substitution reserved area, Private reserved area
            (codePoint >= 0xD800 && codePoint <= 0xDFFF) || (codePoint >= 0xE000 && codePoint <= 0xF8FF) ||
            // Mutation selector, Plane above the second plane，char can't be saved, all can be transferred
            (codePoint >= 0xFE00 && codePoint <= 0xFE0F) || codePoint >= 0x10000;
}

bool TypedText::IsEmojiModifier(uint32_t codePoint)
{
    return (codePoint >= 0x1F3FB && codePoint <= 0x1F3FF);
}

// Based on Emoji_Modifier_Base from
bool TypedText::IsEmojiBase(uint32_t codePoint)
{
    if (codePoint >= 0x261D && codePoint <= 0x270D) {
        return (codePoint == 0x261D || codePoint == 0x26F9 || (codePoint >= 0x270A && codePoint <= 0x270D));
    } else if (codePoint >= 0x1F385 && codePoint <= 0x1F93E) {
        return (codePoint == 0x1F385 || (codePoint >= 0x1F3C3 && codePoint <= 0x1F3C4) ||
                (codePoint >= 0x1F3CA && codePoint <= 0x1F3CB) || (codePoint >= 0x1F442 && codePoint <= 0x1F443) ||
                (codePoint >= 0x1F446 && codePoint <= 0x1F450) || (codePoint >= 0x1F466 && codePoint <= 0x1F469) ||
                codePoint == 0x1F46E || (codePoint >= 0x1F470 && codePoint <= 0x1F478) || codePoint == 0x1F47C ||
                (codePoint >= 0x1F481 && codePoint <= 0x1F483) || (codePoint >= 0x1F485 && codePoint <= 0x1F487) ||
                codePoint == 0x1F4AA || codePoint == 0x1F575 || codePoint == 0x1F57A || codePoint == 0x1F590 ||
                (codePoint >= 0x1F595 && codePoint <= 0x1F596) || (codePoint >= 0x1F645 && codePoint <= 0x1F647) ||
                (codePoint >= 0x1F64B && codePoint <= 0x1F64F) || codePoint == 0x1F6A3 ||
                (codePoint >= 0x1F6B4 && codePoint <= 0x1F6B6) || codePoint == 0x1F6C0 ||
                (codePoint >= 0x1F918 && codePoint <= 0x1F91E) || codePoint == 0x1F926 || codePoint == 0x1F930 ||
                (codePoint >= 0x1F933 && codePoint <= 0x1F939) || (codePoint >= 0x1F93B && codePoint <= 0x1F93E));
    } else {
        return false;
    }
}

bool TypedText::IsColourWord(uint32_t codePoint, uint8_t fontId, uint8_t fontSize)
{
    bool hasColor = false;
    uint8_t weight = UIFont::GetInstance()->GetFontWeight(fontId);
    if (weight >= 16) { // 16: rgb565->16 rgba8888->32 font with rgba
        hasColor = true;
    } else {
#if ENABLE_MULTI_FONT
        uint8_t* searchLists = nullptr;
        int8_t listSize = UIMultiFontManager::GetInstance()->GetSearchFontList(fontId, &searchLists);
        int8_t currentIndex = 0;
        if ((listSize > 0) && (searchLists != nullptr)) {
            do {
                weight = UIFont::GetInstance()->GetFontWeight(searchLists[currentIndex]);
                if (weight >= 16) { // 16: rgb565->16 rgba8888->32 font with rgba
                    hasColor = true;
                }
                currentIndex++;
            } while ((currentIndex < listSize) && (searchLists != nullptr));
        }
#endif
    }
    if (!hasColor) {
        return false;
    }
    GlyphNode glyphNode;
    int8_t ret = UIFont::GetInstance()->GetGlyphNode(codePoint, glyphNode, fontId, fontSize);
    if (ret != RET_VALUE_OK) {
        GRAPHIC_LOGE("Failed to get glyphNode for color word");
        return false;
    }

    weight = UIFont::GetInstance()->GetFontWeight(glyphNode.fontId);
    return (weight >= 16); // 16: rgb565->16 rgba8888->32 font with rgba
}
} // namespace OHOS
