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

#include "draw/draw_label.h"
#include <cstdio>
#include "common/typed_text.h"
#include "draw/draw_utils.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font.h"
#include "font/ui_font_header.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
uint16_t DrawLabel::DrawTextOneLine(BufferInfo& gfxDstBuffer, const LabelLineInfo& labelLine,
                                    uint16_t& letterIndex)
{
    if (labelLine.text == nullptr) {
        return 0;
    }
    UIFont* fontEngine = UIFont::GetInstance();
    if (labelLine.direct == TEXT_DIRECT_RTL) {
        labelLine.pos.x -= labelLine.offset.x;
    } else {
        labelLine.pos.x += labelLine.offset.x;
    }

    uint32_t i = 0;
    uint32_t letter;
    uint16_t retOffsetY = 0; // ret value elipse offsetY
    uint16_t offsetPosY = 0;
    uint8_t maxLetterSize = GetLineMaxLetterSize(labelLine.text, labelLine.lineLength, labelLine.fontId,
                                                 labelLine.fontSize, letterIndex, labelLine.sizeSpans);
    DrawLineBackgroundColor(gfxDstBuffer, letterIndex, labelLine);
    GlyphNode glyphNode;
    uint8_t* fontMap;
    uint8_t weight;
    while (i < labelLine.lineLength) {
        letter = TypedText::GetUTF8Next(labelLine.text, i, i);
        uint8_t fontId = labelLine.fontId;
        uint8_t fontSize = labelLine.fontSize;
        if (labelLine.sizeSpans != nullptr && labelLine.sizeSpans[letterIndex].isSizeSpan) {
            fontId = labelLine.sizeSpans[letterIndex].fontId;
            fontSize = labelLine.sizeSpans[letterIndex].size;
        }
        bool havebackgroundColor = false;
        ColorType backgroundColor;
        GetBackgroundColor(letterIndex, labelLine.backgroundColor, havebackgroundColor, backgroundColor);

        ColorType foregroundColor = labelLine.style.textColor_;
        GetForegroundColor(letterIndex, labelLine.foregroundColor, foregroundColor);
#if ENABLE_VECTOR_FONT
        TextStyle textStyle = TEXT_STYLE_NORMAL;
        if (labelLine.textStyles) {
            textStyle = labelLine.textStyles[letterIndex];
        }
#endif
        LabelLetterInfo letterInfo{labelLine.pos,
                                   labelLine.mask,
                                   foregroundColor,
                                   labelLine.opaScale,
                                   0,
                                   0,
                                   letter,
                                   labelLine.direct,
                                   fontId,
                                   0,
                                   fontSize,
#if ENABLE_VECTOR_FONT
                                   textStyle,
#endif
                                   labelLine.baseLine,
                                   labelLine.style.letterSpace_,
                                   labelLine.style.lineSpace_,
                                   havebackgroundColor,
                                   backgroundColor};
#if ENABLE_VECTOR_FONT
        glyphNode.textStyle = letterInfo.textStyle;
#endif
        glyphNode.advance = 0;
        fontMap = fontEngine->GetBitmap(letterInfo.letter, glyphNode, letterInfo.fontId, letterInfo.fontSize,
                                        letterInfo.shapingId);
        if (fontMap != nullptr) {
            weight = UIFont::GetInstance()->GetFontWeight(glyphNode.fontId);
            if (weight >= 16) { // 16: rgb565->16 rgba8888->32 font with rgba
                DrawUtils::GetInstance()->DrawColorLetter(gfxDstBuffer, letterInfo, fontMap, glyphNode);
            } else {
                letterInfo.offsetY = labelLine.ellipsisOssetY == 0 ? offsetPosY : labelLine.ellipsisOssetY;
                retOffsetY = offsetPosY;
                DrawUtils::GetInstance()->DrawNormalLetter(gfxDstBuffer, letterInfo, fontMap, glyphNode, maxLetterSize);
            }
        }
        if (labelLine.direct == TEXT_DIRECT_RTL) {
            labelLine.pos.x -= (glyphNode.advance + labelLine.style.letterSpace_);
        } else {
            labelLine.pos.x += (glyphNode.advance + labelLine.style.letterSpace_);
        }

        letterIndex++;
    }
    return retOffsetY;
}

uint8_t DrawLabel::GetLineMaxLetterSize(const char* text, uint16_t lineLength, uint8_t fontId, uint8_t fontSize,
                                        uint16_t letterIndex, SizeSpan* sizeSpans)
{
    if (sizeSpans == nullptr) {
        return fontSize;
    }
    uint32_t i = 0;
    uint32_t unicode;
    uint8_t maxLetterSize = fontSize;
    while (i < lineLength) {
        unicode = TypedText::GetUTF8Next(text, i, i);
        if (TypedText::IsColourWord(unicode, fontId, fontSize)) {
            letterIndex++;
            continue;
        }
        if (sizeSpans != nullptr && sizeSpans[letterIndex].isSizeSpan) {
            uint8_t tempSize = sizeSpans[letterIndex].size;
            if (tempSize > maxLetterSize) {
                maxLetterSize = tempSize;
            }
        }
        letterIndex++;
    }
    return maxLetterSize;
}

void DrawLabel::DrawArcText(BufferInfo& gfxDstBuffer,
                            const Rect& mask,
                            const char* text,
                            const Point& arcCenter,
                            uint8_t fontId,
                            uint8_t fontSize,
                            const UIArcLabel::ArcTextInfo arcTextInfo,
                            UIArcLabel::TextOrientation orientation,
                            const Style& style,
                            OpacityType opaScale)
{
    if ((text == nullptr) || (arcTextInfo.lineStart == arcTextInfo.lineEnd) || (arcTextInfo.radius == 0)) {
        GRAPHIC_LOGE("DrawLabel::DrawArcText invalid parameter\n");
        return;
    }
    OpacityType opa = DrawUtils::GetMixOpacity(opaScale, style.textOpa_);
    if (opa == OPA_TRANSPARENT) {
        return;
    }
    uint16_t letterWidth;
    uint16_t letterHeight = UIFont::GetInstance()->GetHeight(fontId, fontSize);
    uint32_t i = arcTextInfo.lineStart;
    float angle = arcTextInfo.startAngle;
    float posX;
    float posY;
    float rotateAngle;

    bool orientationFlag = (orientation == UIArcLabel::TextOrientation::INSIDE);
    bool directFlag = (arcTextInfo.direct == TEXT_DIRECT_LTR);
    bool xorFlag = !((orientationFlag && directFlag) || (!orientationFlag && !directFlag));

    while (i < arcTextInfo.lineEnd) {
        uint32_t tmp = i;
        uint32_t letter = TypedText::GetUTF8Next(text, tmp, i);
        if (letter == 0) {
            continue;
        }
        if ((letter == '\r') || (letter == '\n')) {
            break;
        }
        letterWidth = UIFont::GetInstance()->GetWidth(letter, fontId, fontSize, 0);
        if ((tmp == arcTextInfo.lineStart) && xorFlag) {
            angle += TypedText::GetAngleForArcLen(static_cast<float>(letterWidth), letterHeight, arcTextInfo.radius,
                                                  arcTextInfo.direct, orientation);
        }
        uint16_t arcLen = letterWidth + style.letterSpace_;
        if (arcLen == 0) {
            continue;
        }
        float incrementAngle = TypedText::GetAngleForArcLen(static_cast<float>(arcLen), letterHeight,
                                                            arcTextInfo.radius, arcTextInfo.direct, orientation);

        rotateAngle = (orientation == UIArcLabel::TextOrientation::INSIDE) ? angle : (angle - SEMICIRCLE_IN_DEGREE);

        // 2: half
        float fineTuningAngle = incrementAngle * (static_cast<float>(letterWidth) / (2 * arcLen));
        rotateAngle += (xorFlag ? -fineTuningAngle : fineTuningAngle);
        TypedText::GetArcLetterPos(arcCenter, arcTextInfo.radius, angle, posX, posY);
        angle += incrementAngle;

        DrawLetterWithRotate(gfxDstBuffer, mask, fontId, fontSize, letter, Point { MATH_ROUND(posX), MATH_ROUND(posY) },
                             static_cast<int16_t>(rotateAngle), style.textColor_, opaScale);
    }
}

void DrawLabel::DrawLetterWithRotate(BufferInfo& gfxDstBuffer,
                                     const Rect& mask,
                                     uint8_t fontId,
                                     uint8_t fontSize,
                                     uint32_t letter,
                                     const Point& pos,
                                     int16_t rotateAngle,
                                     const ColorType& color,
                                     OpacityType opaScale)
{
    UIFont* fontEngine = UIFont::GetInstance();
    FontHeader head;
    GlyphNode node;
#if ENABLE_VECTOR_FONT
    node.textStyle = TEXT_STYLE_NORMAL;
#endif
    if (fontEngine->GetFontHeader(head, fontId, fontSize) != 0) {
        return;
    }

    const uint8_t* fontMap = fontEngine->GetBitmap(letter, node, fontId, fontSize, 0);
    if (fontMap == nullptr) {
        return;
    }
    uint8_t fontWeight = fontEngine->GetFontWeight(fontId);
    ColorMode colorMode;
    switch (fontWeight) {
        case FONT_WEIGHT_1:
            colorMode = A1;
            break;
        case FONT_WEIGHT_2:
            colorMode = A2;
            break;
        case FONT_WEIGHT_4:
            colorMode = A4;
            break;
        case FONT_WEIGHT_8:
            colorMode = A8;
            break;
        default:
            return;
    }
    Rect rectLetter;
    rectLetter.SetPosition(pos.x + node.left, pos.y + head.ascender - node.top);
    rectLetter.Resize(node.cols, node.rows);
    TransformMap transMap(rectLetter);
    transMap.Rotate(rotateAngle, Vector2<float>(-node.left, node.top - head.ascender));
    TransformDataInfo letterTranDataInfo = {ImageHeader{colorMode, 0, 0, 0, node.cols, node.rows}, fontMap, fontWeight,
                                            BlurLevel::LEVEL0, TransformAlgorithm::BILINEAR};
    BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, mask, Point { 0, 0 }, color, opaScale, transMap,
                                                letterTranDataInfo);
}

void DrawLabel::GetLineBackgroundColor(uint16_t letterIndex, List<LineBackgroundColor>* linebackgroundColor,
                                       bool& havelinebackground, ColorType& linebgColor)
{
    if (linebackgroundColor->Size() > 0) {
        ListNode<LineBackgroundColor>* lbColor = linebackgroundColor->Begin();
        for (; lbColor != linebackgroundColor->End(); lbColor = lbColor->next_) {
            uint32_t start = lbColor->data_.start;
            uint32_t end = lbColor->data_.end;
            if (letterIndex >= start && letterIndex <= end) {
                havelinebackground = true;
                linebgColor = lbColor->data_.linebackgroundColor ;
            }
        }
    }
};

void DrawLabel::GetBackgroundColor(uint16_t letterIndex, List<BackgroundColor>* backgroundColor,
                                   bool& havebackground, ColorType& bgColor)
{
    if (backgroundColor->Size() > 0) {
        ListNode<BackgroundColor>* bColor = backgroundColor->Begin();
        for (; bColor != backgroundColor->End(); bColor = bColor->next_) {
            uint16_t start = bColor->data_.start;
            uint16_t end = bColor->data_.end;
            if (letterIndex >= start && letterIndex <= end) {
                havebackground = true;
                bgColor = bColor->data_.backgroundColor ;
            }
        }
    }
};

void DrawLabel::GetForegroundColor(uint16_t letterIndex, List<ForegroundColor>* foregroundColor, ColorType& fgColor)
{
    if (foregroundColor->Size() > 0) {
        ListNode<ForegroundColor>* fColor = foregroundColor->Begin();
        for (; fColor != foregroundColor->End(); fColor = fColor->next_) {
            uint32_t start = fColor->data_.start;
            uint32_t end = fColor->data_.end;
            if (letterIndex >= start && letterIndex <= end) {
                fgColor = fColor->data_.fontColor;
            }
        }
    }
};

void DrawLabel::DrawLineBackgroundColor(BufferInfo& gfxDstBuffer, uint16_t letterIndex, const LabelLineInfo& labelLine)
{
    uint32_t i = 0;
    uint32_t letter;
    while (i < labelLine.lineLength) {
        letter = TypedText::GetUTF8Next(labelLine.text, i, i);
        bool havelinebackground = false;
        ColorType linebackgroundColor;
        GetLineBackgroundColor(letterIndex, labelLine.linebackgroundColor, havelinebackground, linebackgroundColor);
        if (havelinebackground) {
                Style style;
                style.bgColor_ = linebackgroundColor;
                Rect linebackground(labelLine.mask.GetLeft(), labelLine.pos.y,
                                    labelLine.mask.GetRight(), labelLine.pos.y + labelLine.lineHeight);
                uint8_t linebackgroundColorAlpha = 0xff;
                #if COLOR_DEPTH==32
                linebackgroundColorAlpha = linebackgroundColor.alpha;
                #endif
                BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, labelLine.mask,
                                                       linebackground, style, linebackgroundColorAlpha);
        }
        letterIndex++;
    }
};
} // namespace OHOS
