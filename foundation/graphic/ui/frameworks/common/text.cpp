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

#include "common/text.h"
#include "common/typed_text.h"
#include "draw/draw_label.h"
#include "font/ui_font.h"
#include "font/ui_font_adaptor.h"
#include "font/ui_font_builder.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
Text::TextLine Text::textLine_[MAX_LINE_COUNT] = {{0}};

Text::Text()
    : text_(nullptr),
      fontId_(0),
      fontSize_(0),
      textSize_({0, 0}),
      needRefresh_(false),
      expandWidth_(false),
      expandHeight_(false),
      baseLine_(true),
      direct_(TEXT_DIRECT_LTR),
      sizeSpans_(nullptr),
      characterSize_(0),
      horizontalAlign_(TEXT_ALIGNMENT_LEFT),
      verticalAlign_(TEXT_ALIGNMENT_TOP)
{
#if ENABLE_VECTOR_FONT
    textStyles_ = nullptr;
#endif
    SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_VECTOR_FONT_SIZE);
}

Text::~Text()
{
    if (text_ != nullptr) {
        UIFree(text_);
        text_ = nullptr;
    }
#if ENABLE_VECTOR_FONT
    if (textStyles_ != nullptr) {
        UIFree(textStyles_);
        textStyles_ = nullptr;
    }
#endif
    if (backgroundColor_.Size() > 0) {
        backgroundColor_.Clear();
    }
    if (linebackgroundColor_.Size() > 0) {
        linebackgroundColor_.Clear();
    }
    if (foregroundColor_.Size() > 0) {
        foregroundColor_.Clear();
    }
    if (sizeSpans_ != nullptr) {
        UIFree(sizeSpans_);
        sizeSpans_ = nullptr;
    }
}

#if ENABLE_VECTOR_FONT
void Text::SetSpannableString(const SpannableString* spannableString)
{
    SetText(spannableString->text_);
    if (textStyles_ != nullptr) {
        UIFree(textStyles_);
        textStyles_ = nullptr;
    }
    if (spannableString->spanList_.IsEmpty()) {
        return;
    }
    uint32_t textLen = TypedText::GetUTF8CharacterSize(text_, GetTextStrLen());
    if (textLen == 0 || textLen > MAX_TEXT_LENGTH) {
        return;
    }
    textStyles_ = static_cast<TextStyle*>(UIMalloc(textLen));
    ListNode<StyleSpan*>* node = spannableString->spanList_.Begin();
    while (node != spannableString->spanList_.End()) {
        for (uint32_t i = node->data_->start_; i < node->data_->end_; i++) {
            if (textLen > i) {
                textStyles_[i] = node->data_->textStyle_;
            }
        }
        node = node->next_;
    }
    needRefresh_ = true;
}
#endif
void Text::SetText(const char* text)
{
    if (text == nullptr) {
        return;
    }
    uint32_t textLen = static_cast<uint32_t>(strlen(text));
    if (textLen > MAX_TEXT_LENGTH) {
        return;
    }
    if (text_ != nullptr) {
        if (strcmp(text, text_) == 0) {
            return;
        }
        UIFree(text_);
        text_ = nullptr;
    }
    text_ = static_cast<char*>(UIMalloc(++textLen));
    if (text_ == nullptr) {
        return;
    }
    if (memcpy_s(text_, textLen, text, textLen) != EOK) {
        UIFree(text_);
        text_ = nullptr;
        return;
    }
#if ENABLE_VECTOR_FONT
    if (textStyles_ != nullptr) {
        UIFree(textStyles_);
        textStyles_ = nullptr;
    }
#endif
    needRefresh_ = true;
    if (sizeSpans_ != nullptr) {
        UIFree(sizeSpans_);
        sizeSpans_ = nullptr;
    }
}

void Text::SetFont(const char* name, uint8_t size)
{
    if (name == nullptr) {
        return;
    }
    if (UIFont::GetInstance()->IsVectorFont()) {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(name);
        if ((fontId != UIFontBuilder::GetInstance()->GetTotalFontId()) &&
            ((fontId_ != fontId) || (fontSize_ != size))) {
            fontId_ = fontId;
            fontSize_ = size;
            needRefresh_ = true;
        }
    } else {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(name, size);
        SetFontId(fontId);
    }
}

void Text::SetFont(const char* name, uint8_t size, char*& destName, uint8_t& destSize)
{
    if (name == nullptr) {
        return;
    }
    uint32_t nameLen = static_cast<uint32_t>(strlen(name));
    if (nameLen > MAX_TEXT_LENGTH) {
        return;
    }
    if (destName != nullptr) {
        if (strcmp(destName, name) == 0) {
            destSize = size;
            return;
        }
        UIFree(destName);
        destName = nullptr;
    }
    if (nameLen != 0) {
        /* one more to store '\0' */
        destName = static_cast<char*>(UIMalloc(++nameLen));
        if (destName == nullptr) {
            return;
        }
        if (memcpy_s(destName, nameLen, name, nameLen) != EOK) {
            UIFree(destName);
            destName = nullptr;
            return;
        }
        destSize = size;
    }
}

void Text::SetFontId(uint8_t fontId)
{
    if ((fontId >= UIFontBuilder::GetInstance()->GetTotalFontId()) || ((fontId_ == fontId) && (fontSize_ != 0))) {
        GRAPHIC_LOGE("Text::SetFontId invalid fontId(%hhd)", fontId);
        return;
    }
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
    if (fontParam == nullptr) {
        return;
    }
    if (UIFont::GetInstance()->IsVectorFont()) {
        uint8_t fontId = UIFont::GetInstance()->GetFontId(fontParam->ttfName);
        if ((fontId != UIFontBuilder::GetInstance()->GetTotalFontId()) && ((fontId_ != fontId) ||
            (fontSize_ != fontParam->size))) {
            fontId_ = fontId;
            fontSize_ = fontParam->size;
            needRefresh_ = true;
        }
    } else {
        fontId_ = fontId;
        fontSize_ = fontParam->size;
        needRefresh_ = true;
    }
}

void Text::ReMeasureTextSize(const Rect& textRect, const Style& style)
{
    if (fontSize_ == 0) {
        return;
    }
    int16_t maxWidth = (expandWidth_ ? COORD_MAX : textRect.GetWidth());
    if (maxWidth > 0) {
        textSize_ = TypedText::GetTextSize(text_, fontId_, fontSize_, style.letterSpace_, style.lineHeight_, maxWidth,
                                           style.lineSpace_, sizeSpans_);
        if (baseLine_) {
            FontHeader head;
            if (UIFont::GetInstance()->GetFontHeader(head, fontId_, fontSize_) != 0) {
                return;
            }
            textSize_.y += fontSize_ - head.ascender;
        }
    }
}

void Text::ReMeasureTextWidthInEllipsisMode(const Rect& textRect, const Style& style, uint16_t ellipsisIndex)
{
    if (ellipsisIndex != TEXT_ELLIPSIS_END_INV) {
        int16_t lineMaxWidth  = expandWidth_ ? textSize_.x : textRect.GetWidth();
        uint32_t maxLineBytes = 0;
        uint16_t lineCount = GetLine(lineMaxWidth, style.letterSpace_, ellipsisIndex, maxLineBytes);
        if ((lineCount > 0) && (textSize_.x < textLine_[lineCount - 1].linePixelWidth)) {
            textSize_.x = textLine_[lineCount - 1].linePixelWidth;
        }
    }
}

void Text::DrawEllipsis(BufferInfo& gfxDstBuffer, LabelLineInfo& labelLine, uint16_t& letterIndex)
{
    labelLine.offset.x = 0;
    labelLine.text = TEXT_ELLIPSIS;
    labelLine.lineLength = TEXT_ELLIPSIS_DOT_NUM;
    labelLine.length = TEXT_ELLIPSIS_DOT_NUM;
    DrawLabel::DrawTextOneLine(gfxDstBuffer, labelLine, letterIndex);
}

void Text::OnDraw(BufferInfo& gfxDstBuffer,
                  const Rect& invalidatedArea,
                  const Rect& viewOrigRect,
                  const Rect& textRect,
                  int16_t offsetX,
                  const Style& style,
                  uint16_t ellipsisIndex,
                  OpacityType opaScale)
{
    if ((text_ == nullptr) || (strlen(text_) == 0) || (fontSize_ == 0)) {
        return;
    }
    Rect mask = invalidatedArea;

    if (mask.Intersect(mask, textRect)) {
        Draw(gfxDstBuffer, mask, textRect, style, offsetX, ellipsisIndex, opaScale);
    }
}

void Text::Draw(BufferInfo& gfxDstBuffer,
                const Rect& mask,
                const Rect& coords,
                const Style& style,
                int16_t offsetX,
                uint16_t ellipsisIndex,
                OpacityType opaScale)
{
    Point offset = {offsetX, 0};
    int16_t lineMaxWidth = expandWidth_ ? textSize_.x : coords.GetWidth();
    uint16_t lineBegin = 0;
    uint32_t maxLineBytes = 0;
    uint16_t lineCount = GetLine(lineMaxWidth, style.letterSpace_, ellipsisIndex, maxLineBytes);
    int16_t lineHeight = style.lineHeight_;
    int16_t curLineHeight;
    if (lineHeight == 0) {
        lineHeight = UIFont::GetInstance()->GetHeight(fontId_, fontSize_);
        lineHeight += style.lineSpace_;
    }
    if ((style.lineSpace_ == 0) && (sizeSpans_ != nullptr)) {
        uint16_t letterIndex = 0;
        curLineHeight = UIFont::GetInstance()->GetLineMaxHeight(text_, textLine_[0].lineBytes, fontId_, fontSize_,
                                                                letterIndex, sizeSpans_);
        curLineHeight += style.lineSpace_;
    } else {
        curLineHeight = lineHeight;
    }
    Point pos;
    if (lineHeight == style.lineHeight_) {
        pos.y = TextPositionY(coords, (lineCount * lineHeight));
    } else {
        pos.y = TextPositionY(coords, (lineCount * lineHeight - style.lineSpace_));
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.textOpa_);
    uint16_t letterIndex = 0;
    for (uint16_t i = 0; i < lineCount; i++) {
        if (pos.y > mask.GetBottom()) {
            return;
        }
        int16_t nextLine = pos.y + curLineHeight;
        if (lineHeight != style.lineHeight_) {
            nextLine -= style.lineSpace_;
        }
        int16_t tempLetterIndex = letterIndex;
        if (nextLine >= mask.GetTop()) {
            pos.x = LineStartPos(coords, textLine_[i].linePixelWidth);
            LabelLineInfo labelLine{pos, offset, mask, curLineHeight, textLine_[i].lineBytes,
                                    0, opa, style, &text_[lineBegin], textLine_[i].lineBytes,
                                    lineBegin, fontId_, fontSize_, 0, static_cast<UITextLanguageDirect>(direct_),
                                    nullptr, baseLine_,
#if ENABLE_VECTOR_FONT
                                    textStyles_,
#endif
                                    &backgroundColor_, &foregroundColor_, &linebackgroundColor_, sizeSpans_, 0};

            uint16_t ellipsisOssetY = DrawLabel::DrawTextOneLine(gfxDstBuffer, labelLine, letterIndex);
            if ((i == (lineCount - 1)) && (ellipsisIndex != TEXT_ELLIPSIS_END_INV)) {
                labelLine.ellipsisOssetY = ellipsisOssetY;
                DrawEllipsis(gfxDstBuffer, labelLine, letterIndex);
            }
        } else {
            letterIndex = TypedText::GetUTF8CharacterSize(text_, lineBegin + textLine_[i].lineBytes);
        }
        if ((style.lineSpace_ == 0) && (sizeSpans_ != nullptr)) {
            curLineHeight = UIFont::GetInstance()->GetLineMaxHeight(&text_[lineBegin], textLine_[i].lineBytes, fontId_,
                                                                    fontSize_, tempLetterIndex, sizeSpans_);
            curLineHeight += style.lineSpace_;
        } else {
            curLineHeight = lineHeight;
        }
        lineBegin += textLine_[i].lineBytes;
        pos.y += curLineHeight;
    }
}

int16_t Text::TextPositionY(const Rect& textRect, int16_t textHeight)
{
    int16_t yOffset = 0;
    if (!expandHeight_ && (verticalAlign_ != TEXT_ALIGNMENT_TOP) && (textRect.GetHeight() > textHeight)) {
        if (verticalAlign_ == TEXT_ALIGNMENT_CENTER) {
            yOffset = (textRect.GetHeight() - textHeight) >> 1;
        } else if (verticalAlign_ == TEXT_ALIGNMENT_BOTTOM) {
            yOffset = textRect.GetHeight() - textHeight;
        }
    }
    return textRect.GetY() + yOffset + EXTRA_HEIGHT / 2;   // 2:half extra height for first line
}

int16_t Text::LineStartPos(const Rect& textRect, uint16_t lineWidth)
{
    int16_t xOffset = 0;
    int16_t rectWidth = textRect.GetWidth();
    if (horizontalAlign_ == TEXT_ALIGNMENT_CENTER) {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? ((rectWidth + lineWidth + 1) >> 1) : ((rectWidth - lineWidth) >> 1);
    } else if (horizontalAlign_ == TEXT_ALIGNMENT_RIGHT) {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? rectWidth : (rectWidth - lineWidth);
    } else {
        xOffset = (direct_ == TEXT_DIRECT_RTL) ? lineWidth : 0;
    }
    return textRect.GetX() + xOffset;
}

uint16_t Text::GetLine(int16_t width, uint8_t letterSpace, uint16_t ellipsisIndex, uint32_t& maxLineBytes)
{
    if (text_ == nullptr) {
        return 0;
    }
    uint16_t lineNum = 0;
    uint32_t textLen = GetTextStrLen();
    if ((ellipsisIndex != TEXT_ELLIPSIS_END_INV) && (ellipsisIndex < textLen)) {
        textLen = ellipsisIndex;
    }
    uint32_t begin = 0;
    uint16_t letterIndex = 0;
    while ((begin < textLen) && (text_[begin] != '\0') && (lineNum < MAX_LINE_COUNT)) {
        begin += GetTextLine(begin, textLen, width, lineNum, letterSpace, letterIndex, sizeSpans_);
        if (maxLineBytes < textLine_[lineNum].lineBytes) {
            maxLineBytes = textLine_[lineNum].lineBytes;
        }
        lineNum++;
    }
    if ((lineNum != 0) && (ellipsisIndex != TEXT_ELLIPSIS_END_INV)) {
        uint16_t ellipsisWidth = UIFont::GetInstance()->GetWidth('.', fontId_, fontSize_, 0) + letterSpace;
        textLine_[lineNum - 1].linePixelWidth += ellipsisWidth * TEXT_ELLIPSIS_DOT_NUM;
        if (textLine_[lineNum - 1].linePixelWidth > width) {
            int16_t newWidth = width - ellipsisWidth * TEXT_ELLIPSIS_DOT_NUM;
            maxLineBytes = CalculateLineWithEllipsis(begin, textLen, newWidth, letterSpace, lineNum, letterIndex,
                                                     sizeSpans_);
            textLine_[lineNum - 1].linePixelWidth += ellipsisWidth * TEXT_ELLIPSIS_DOT_NUM;
        }
    }
    return lineNum;
}

uint32_t Text::CalculateLineWithEllipsis(uint32_t begin, uint32_t textLen, int16_t width,
                                         uint8_t letterSpace, uint16_t& lineNum,
                                         uint16_t& letterIndex,
                                         SizeSpan* sizeSpans)
{
    begin -= textLine_[lineNum - 1].lineBytes;
    lineNum--;
    while ((begin < textLen) && (text_[begin] != '\0') && (lineNum < MAX_LINE_COUNT)) {
        begin += GetTextLine(begin, textLen, width, lineNum, letterSpace, letterIndex, sizeSpans);
        lineNum++;
    }
    uint32_t maxLineBytes = 0;
    for (uint16_t i = 0; i < lineNum; i++) {
        if (maxLineBytes < textLine_[i].lineBytes) {
            maxLineBytes = textLine_[i].lineBytes;
        }
    }
    return maxLineBytes;
}

uint32_t Text::GetTextStrLen()
{
    return strlen(text_);
}

uint32_t Text::GetTextLine(uint32_t begin, uint32_t textLen, int16_t width, uint16_t lineNum, uint8_t letterSpace,
                           uint16_t& letterIndex, SizeSpan* sizeSpans)
{
    int16_t lineWidth = width;
    int16_t lineHeight = 0;
    uint16_t nextLineBytes = UIFontAdaptor::GetNextLineAndWidth(&text_[begin], fontId_, fontSize_, letterSpace,
                                                                lineWidth, lineHeight, letterIndex, sizeSpans, false,
                                                                textLen - begin);
    if (nextLineBytes + begin > textLen) {
        nextLineBytes = textLen - begin;
    }
    textLine_[lineNum].lineBytes = nextLineBytes;
    textLine_[lineNum].linePixelWidth = lineWidth;
    return nextLineBytes;
}

uint16_t Text::GetEllipsisIndex(const Rect& textRect, const Style& style)
{
    if ((textSize_.y <= textRect.GetHeight()) || (TypedText::GetUTF8CharacterSize(text_) <= TEXT_ELLIPSIS_DOT_NUM)) {
        return TEXT_ELLIPSIS_END_INV;
    }
    UIFont* fontEngine = UIFont::GetInstance();
    int16_t letterWidth = fontEngine->GetWidth('.', fontId_, fontSize_, 0) + style.letterSpace_;
    Point p;
    p.x = textRect.GetWidth() - letterWidth * TEXT_ELLIPSIS_DOT_NUM;
    p.y = textRect.GetHeight();
    int16_t height = style.lineHeight_;
    if (height == 0) {
        height = fontEngine->GetHeight(fontId_, fontSize_) + style.lineSpace_;
    }
    if (height) {
        p.y -= p.y % height;
    }
    if (height != style.lineHeight_) {
        p.y -= style.lineSpace_;
    }
    return GetLetterIndexByPosition(textRect, style, p);
}

uint16_t Text::GetLetterIndexByPosition(const Rect& textRect, const Style& style, const Point& pos)
{
    if (text_ == nullptr) {
        return 0;
    }
    uint32_t lineStart = 0;
    uint32_t nextLineStart = 0;
    int16_t lineHeight = style.lineHeight_;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight(fontId_, fontSize_);
    if (lineHeight == 0) {
        lineHeight = letterHeight + style.lineSpace_;
    }
    uint16_t height = 0;
    if (lineHeight != style.lineHeight_) {
        height = letterHeight;
    } else {
        height = lineHeight;
    }
    int16_t y = 0;
    uint32_t textLen = static_cast<uint32_t>(strlen(text_));
    int16_t width = 0;
    uint16_t letterIndex = 0;
    while ((lineStart < textLen) && (text_[lineStart] != '\0')) {
        width = textRect.GetWidth();
        nextLineStart += UIFontAdaptor::GetNextLineAndWidth(&text_[lineStart], fontId_, fontSize_, style.letterSpace_,
                                                            width, lineHeight, letterIndex, sizeSpans_);
        if (nextLineStart == 0) {
            break;
        }
        if (pos.y <= y + height) {
            break;
        }
        y += lineHeight;
        lineStart = nextLineStart;
    }
    if (nextLineStart == textLen) {
        return TEXT_ELLIPSIS_END_INV;
    }
    /* Calculate the x coordinate */
    width = pos.x;
    lineStart +=
        UIFontAdaptor::GetNextLineAndWidth(&text_[lineStart], fontId_, fontSize_, style.letterSpace_, width, lineHeight,
                                           letterIndex, sizeSpans_, true);
    return (lineStart < textLen) ? lineStart : TEXT_ELLIPSIS_END_INV;
}

void Text::SetAbsoluteSizeSpan(int16_t start, int16_t end, uint8_t size)
{
#if ENABLE_VECTOR_FONT
    if (fontId_ == FONT_ID_MAX) {
        return;
    }
#else
    if (fontId_ == UIFontBuilder::GetInstance()->GetBitmapFontIdMax()) {
        return;
    }
#endif
    uint8_t fontId = GetSpanFontIdBySize(size);
#if !ENABLE_VECTOR_FONT
    if (fontId == fontId_) {
        return;
    }
#endif
    if (text_ != nullptr && sizeSpans_ == nullptr) {
        characterSize_ = TypedText::GetUTF8CharacterSize(text_, GetTextStrLen());
        sizeSpans_ = static_cast<SizeSpan*>(UIMalloc(characterSize_ * sizeof(SizeSpan)));
        if (sizeSpans_ == nullptr) {
            GRAPHIC_LOGE("Text::SetAbsoluteSizeSpan invalid parameter");
            return;
        }
        InitSizeSpans();
    }

    if (sizeSpans_ != nullptr && start <= characterSize_) {
        for (int32_t i = start; i < end && i < characterSize_; i++) {
            sizeSpans_[i].fontId = fontId;
            sizeSpans_[i].size = size;
            sizeSpans_[i].isSizeSpan = true;
        }
    }
}

void Text::SetRelativeSizeSpan(int16_t start, int16_t end, float size)
{
    uint8_t absoluteSize = 0;
#if ENABLE_VECTOR_FONT
    absoluteSize = static_cast<uint8_t>(size * fontSize_);
#else
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId_);
    absoluteSize = static_cast<uint8_t>(size * fontParam->size);
#endif
    SetAbsoluteSizeSpan(start, end, absoluteSize);
}

uint8_t Text::GetSpanFontIdBySize(uint8_t size)
{
#if ENABLE_VECTOR_FONT
    return fontId_;
#else
    UITextLanguageFontParam* fontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId_);
    if (fontParam == nullptr) {
        return fontId_;
    }

    uint8_t ttfId = fontParam->ttfId;
    for (uint8_t fontId = 0; fontId < UIFontBuilder::GetInstance()->GetTotalFontId(); fontId++) {
        UITextLanguageFontParam* tempFontParam = UIFontBuilder::GetInstance()->GetTextLangFontsTable(fontId);
        if (tempFontParam == nullptr) {
            continue;
        }
        if (ttfId == tempFontParam->ttfId && size == tempFontParam->size) {
            return fontId;
        }
    }
    return fontId_;
#endif
}

void Text::InitSizeSpans()
{
    if (sizeSpans_ != nullptr) {
        for (uint32_t i = 0 ; i < TypedText::GetUTF8CharacterSize(text_, GetTextStrLen()); i++) {
            sizeSpans_[i].isSizeSpan = false;
            sizeSpans_[i].height = 0;
        }
    }
}
} // namespace OHOS
