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

#include "draw/draw_utils.h"

#include "draw/draw_triangle.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "font/ui_font.h"
#include "gfx_utils/color.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#include "graphic_performance.h"
#include "securec.h"
#include "common/typed_text.h"
#ifdef ARM_NEON_OPT
#include "graphic_neon_pipeline.h"
#include "graphic_neon_utils.h"
#endif

#if ENABLE_ARM_MATH
#include "arm_math.h"
#endif
#include "hal_tick.h"

namespace OHOS {
// Preprocess operation for draw
#define DRAW_UTILS_PREPROCESS(gfxBufferInfo, opa)                         \
    if ((opa) == OPA_TRANSPARENT) {                                       \
        return;                                                           \
    }                                                                     \
    uint8_t* screenBuffer = static_cast<uint8_t*>(gfxBufferInfo.virAddr); \
    if (screenBuffer == nullptr) {                                        \
        return;                                                           \
    }                                                                     \
    ColorMode bufferMode = gfxBufferInfo.mode;                            \
    uint8_t bufferPxSize = GetByteSizeByColorMode(bufferMode);            \
    uint16_t screenBufferWidth = gfxBufferInfo.width;

/* cover mode, src alpha is 255 */
#define COLOR_FILL_COVER(d, dm, r2, g2, b2, sm)               \
    if ((dm) == ARGB8888) {                                   \
        reinterpret_cast<Color32*>(d)->alpha = OPA_OPAQUE;    \
        if (sm == RGB565) {                                   \
            reinterpret_cast<Color32*>(d)->red = (r2) << 3;   \
            reinterpret_cast<Color32*>(d)->green = (g2) << 2; \
            reinterpret_cast<Color32*>(d)->blue = (b2) << 3;  \
        } else {                                              \
            reinterpret_cast<Color32*>(d)->red = (r2);        \
            reinterpret_cast<Color32*>(d)->green = (g2);      \
            reinterpret_cast<Color32*>(d)->blue = (b2);       \
        }                                                     \
    } else if ((dm) == RGB888) {                              \
        if (sm == RGB565) {                                   \
            reinterpret_cast<Color24*>(d)->red = (r2) << 3;   \
            reinterpret_cast<Color24*>(d)->green = (g2) << 2; \
            reinterpret_cast<Color24*>(d)->blue = (b2) << 3;  \
        } else {                                              \
            reinterpret_cast<Color24*>(d)->red = (r2);        \
            reinterpret_cast<Color24*>(d)->green = (g2);      \
            reinterpret_cast<Color24*>(d)->blue = (b2);       \
        }                                                     \
    } else if ((dm) == RGB565) {                              \
        if ((sm) == ARGB8888 || (sm) == RGB888) {             \
            reinterpret_cast<Color16*>(d)->red = (r2) >> 3;   \
            reinterpret_cast<Color16*>(d)->green = (g2) >> 2; \
            reinterpret_cast<Color16*>(d)->blue = (b2) >> 3;  \
        } else {                                              \
            reinterpret_cast<Color16*>(d)->red = (r2);        \
            reinterpret_cast<Color16*>(d)->green = (g2);      \
            reinterpret_cast<Color16*>(d)->blue = (b2);       \
        }                                                     \
    } else {                                                  \
        ASSERT(0);                                            \
    }

#define COLOR_BLEND_RGBA(r1, g1, b1, a1, r2, g2, b2, a2)                                  \
    const float Alpha1 = static_cast<float>(a1) / OPA_OPAQUE;                             \
    const float Alpha2 = static_cast<float>(a2) / OPA_OPAQUE;                             \
    const float Alpha3 = 1 - (1 - Alpha1) * (1 - Alpha2);                                 \
    (r1) = static_cast<uint8_t>((Alpha2 * (r2) + (1 - Alpha2) * Alpha1 * (r1)) / Alpha3); \
    (g1) = static_cast<uint8_t>((Alpha2 * (g2) + (1 - Alpha2) * Alpha1 * (g1)) / Alpha3); \
    (b1) = static_cast<uint8_t>((Alpha2 * (b2) + (1 - Alpha2) * Alpha1 * (b1)) / Alpha3); \
    (a1) = static_cast<uint8_t>(Alpha3 * OPA_OPAQUE);

#define COLOR_BLEND_RGB(r1, g1, b1, r2, g2, b2, a2)                                    \
    (r1) = (((r2) * (a2)) / OPA_OPAQUE) + (((r1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (g1) = (((g2) * (a2)) / OPA_OPAQUE) + (((g1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE); \
    (b1) = (((b2) * (a2)) / OPA_OPAQUE) + (((b1) * (OPA_OPAQUE - (a2))) / OPA_OPAQUE);

// 565
#define COLOR_FILL_BLEND(d, dm, s, sm, a)                                                                           \
    if ((dm) == ARGB8888) {                                                                                         \
        Color32* p = reinterpret_cast<Color32*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, sTmp->red, sTmp->green, sTmp->blue, alpha);       \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, sTmp->red, sTmp->green, sTmp->blue, a);           \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGBA(p->red, p->green, p->blue, p->alpha, (sTmp->red) << 3, (sTmp->green) << 2,             \
                             (sTmp->blue) << 3, a);                                                                 \
        }                                                                                                           \
    } else if ((dm) == RGB888) {                                                                                    \
        Color24* p = reinterpret_cast<Color24*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, alpha);                  \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, a);                      \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) << 3, (sTmp->green) << 2, (sTmp->blue) << 3, a); \
        }                                                                                                           \
    } else if ((dm) == RGB565) {                                                                                    \
        Color16* p = reinterpret_cast<Color16*>(d);                                                                 \
        if ((sm) == ARGB8888) {                                                                                     \
            Color32* sTmp = reinterpret_cast<Color32*>(s);                                                          \
            uint8_t alpha = (sTmp->alpha * (a)) / OPA_OPAQUE;                                                       \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) >> 3, (sTmp->green) >> 2, (sTmp->blue) >> 3,     \
                            alpha);                                                                                 \
        } else if ((sm) == RGB888) {                                                                                \
            Color24* sTmp = reinterpret_cast<Color24*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, (sTmp->red) >> 3, (sTmp->green) >> 2, (sTmp->blue) >> 3, a); \
        } else if ((sm) == RGB565) {                                                                                \
            Color16* sTmp = reinterpret_cast<Color16*>(s);                                                          \
            COLOR_BLEND_RGB(p->red, p->green, p->blue, sTmp->red, sTmp->green, sTmp->blue, a);                      \
        }                                                                                                           \
    } else {                                                                                                        \
        ASSERT(0);                                                                                                  \
    }

namespace {
static constexpr uint8_t OPACITY_STEP_A1 = 255;
static constexpr uint8_t OPACITY_STEP_A2 = 85;
static constexpr uint8_t OPACITY_STEP_A4 = 17;
} // namespace

TriangleEdge::TriangleEdge(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
#if ENABLE_FIXED_POINT
    curX = FO_TRANS_INTEGER_TO_FIXED(x1);
    curY = FO_TRANS_INTEGER_TO_FIXED(y1);
    du = FO_TRANS_INTEGER_TO_FIXED(x2 - x1);
    dv = FO_TRANS_INTEGER_TO_FIXED(y2 - y1);
#else
    curX = static_cast<float>(x1);
    curY = static_cast<float>(y1);
    du = static_cast<float>(x2 - x1);
    dv = static_cast<float>(y2 - y1);
#endif
}

TriangleEdge::~TriangleEdge() {}

DrawUtils* DrawUtils::GetInstance()
{
    static DrawUtils instance;
    return &instance;
}

void DrawUtils::DrawColorAreaBySides(BufferInfo& gfxDstBuffer,
                                     const Rect& mask,
                                     const ColorType& color,
                                     OpacityType opa,
                                     const EdgeSides& sides) const
{
    Rect area(sides.left, sides.top, sides.right, sides.bottom);
    DrawUtils::GetInstance()->DrawColorArea(gfxDstBuffer, area, mask, color, opa);
}

void DrawUtils::DrawColorArea(BufferInfo& gfxDstBuffer,
                              const Rect& area,
                              const Rect& mask,
                              const ColorType& color,
                              OpacityType opa) const
{
    if (opa == OPA_TRANSPARENT) {
        return;
    }

    Rect maskedArea;
    if (!maskedArea.Intersect(area, mask)) {
        return;
    }

    BaseGfxEngine::GetInstance()->Fill(gfxDstBuffer, maskedArea, color, opa);
}

uint8_t DrawUtils::GetPxSizeByColorMode(uint8_t colorMode)
{
    switch (colorMode) {
        case TSC6:
        case TSC6A:
        case ARGB8888:
            return 32; // 32: 32 bit
        case RGB888:
            return 24; // 24: 24 bit
        case RGB565:
        case ARGB1555:
        case ARGB4444:
            return 16; // 16: 16 bit
        case L1:
        case A1:
            return 1; // 1: 1 bit
        case L2:
        case A2:
            return 2; // 2: 2 bit
        case L4:
        case A4:
            return 4; // 4: 4 bit
        case L8:
        case A8:
            return 8; // 8: 8 bit
        default:
            return 0;
    }
}

uint8_t DrawUtils::GetByteSizeByColorMode(uint8_t colorMode)
{
    switch (colorMode) {
        case ARGB8888:
            return 4; // 4: 4 Byte
        case RGB888:
            return 3; // 3: 3 Byte
        case RGB565:
        case ARGB1555:
        case ARGB4444:
            return 2; // 2: 2 Byte
        case L8:
            return 1; // 1: 1 Byte
        case ETC2:
            return 1;
        default:
            return 0;
    }
}

void DrawUtils::DrawPixel(BufferInfo& gfxDstBuffer,
                          int16_t x,
                          int16_t y,
                          const Rect& mask,
                          const ColorType& color,
                          OpacityType opa) const
{
    if ((x < mask.GetLeft()) || (x > mask.GetRight()) || (y < mask.GetTop()) || (y > mask.GetBottom())) {
        return;
    }

    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);

    Color32 fillColor;
    fillColor.full = Color::ColorTo32(color);

    screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
    COLOR_FILL_BLEND(screenBuffer, bufferMode, &fillColor, ARGB8888, opa);
}

void DrawUtils::DrawColorLetter(BufferInfo& gfxDstBuffer,
                                const LabelLetterInfo& letterInfo,
                                uint8_t* fontMap,
                                GlyphNode node) const
{
    if (fontMap == nullptr) {
        return;
    }
    UIFont* fontEngine = UIFont::GetInstance();
    uint16_t letterW = node.cols;
    uint16_t letterH = node.rows;
    int16_t posX;
    int16_t posY;
    if (letterInfo.baseLine) {
        posY = letterInfo.pos.y + letterInfo.offsetY;
    } else {
        FontHeader head;
        if (fontEngine->GetFontHeader(head, letterInfo.fontId, letterInfo.fontSize) != 0) {
            return;
        }
        posY = letterInfo.pos.y + head.ascender - letterInfo.offsetY;
    }
    if (letterInfo.direct == TEXT_DIRECT_RTL) {
        /* RTL */
        posX = letterInfo.pos.x- node.advance + letterInfo.offsetX;
    } else {
        /* LTR */
        posX = letterInfo.pos.x;
    }

    uint16_t rowStart = (posY >= letterInfo.mask.GetTop()) ? 0 : (letterInfo.mask.GetTop() - posY);
    uint16_t rowEnd =
        (posY + letterH <= letterInfo.mask.GetBottom()) ? letterH : (letterInfo.mask.GetBottom() - posY + 1);
    uint16_t colStart = (posX >= letterInfo.mask.GetLeft()) ? 0 : (letterInfo.mask.GetLeft() - posX);
    uint16_t colEnd =
        (posX + letterW <= letterInfo.mask.GetRight()) ? letterW : (letterInfo.mask.GetRight() - posX + 1);

    Rect srcRect(posX, posY, posX + letterW - 1, posY + letterH - 1);
    Rect subRect(posX + colStart, posY + rowStart, colEnd - 1 + posX, rowEnd - 1 + posY);

    uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(gfxDstBuffer.mode);
    DrawImage(gfxDstBuffer, srcRect, letterInfo.mask, fontMap, letterInfo.opa, pxSize, ARGB8888);
}

void DrawUtils::DrawNormalLetter(BufferInfo& gfxDstBuffer,
                                 const LabelLetterInfo& letterInfo,
                                 uint8_t* fontMap,
                                 GlyphNode node,
                                 uint8_t maxLetterSize) const
{
    if (fontMap == nullptr) {
        return;
    }

    UIFont* fontEngine = UIFont::GetInstance();
    uint16_t letterW = node.cols;
    uint16_t letterH = node.rows;
    int16_t posX;
    int16_t posY;

    if (letterInfo.baseLine) {
        posY = letterInfo.pos.y + maxLetterSize - node.top + letterInfo.offsetY;
    } else {
        FontHeader head;
        if (fontEngine->GetFontHeader(head, letterInfo.fontId, letterInfo.fontSize) != 0) {
            return;
        }
        posY = letterInfo.pos.y + head.ascender - node.top - letterInfo.offsetY;
    }
    if (letterInfo.direct == TEXT_DIRECT_RTL) {
        /* RTL */
        posX = letterInfo.pos.x - node.advance + node.left + letterInfo.offsetX;
    } else {
        /* LTR */
        posX = letterInfo.pos.x + node.left + letterInfo.offsetX;
    }

    if (letterInfo.havebackgroundColor) {
        Rect backgroundRect(posX, letterInfo.mask.GetTop(),
                            posX + letterW + letterInfo.letterSpace_ - 1, letterInfo.mask.GetBottom() - letterInfo.lineSpace_);
        Style style;
        style.bgColor_ = letterInfo.backgroundColor;
        uint8_t bgColor_Alpha = 0xff;
        #if COLOR_DEPTH==32
        bgColor_Alpha = style.bgColor_.alpha;
        #endif
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, backgroundRect,
                                               backgroundRect, style, bgColor_Alpha);
    }

    if ((posX + letterW < letterInfo.mask.GetLeft()) || (posX > letterInfo.mask.GetRight()) ||
        (posY + letterH < letterInfo.mask.GetTop()) || (posY > letterInfo.mask.GetBottom())) {
        return;
    }

    uint16_t rowStart = (posY >= letterInfo.mask.GetTop()) ? 0 : (letterInfo.mask.GetTop() - posY);
    uint16_t rowEnd =
        (posY + letterH <= letterInfo.mask.GetBottom()) ? letterH : (letterInfo.mask.GetBottom() - posY + 1);
    uint16_t colStart = (posX >= letterInfo.mask.GetLeft()) ? 0 : (letterInfo.mask.GetLeft() - posX);
    uint16_t colEnd =
        (posX + letterW <= letterInfo.mask.GetRight()) ? letterW : (letterInfo.mask.GetRight() - posX + 1);

    Rect srcRect(posX, posY, posX + letterW - 1, posY + letterH - 1);
    Rect subRect(posX + colStart, posY + rowStart, colEnd - 1 + posX, rowEnd - 1 + posY);

    uint8_t fontWeight = fontEngine->GetFontWeight(letterInfo.fontId);
    BaseGfxEngine::GetInstance()->DrawLetter(gfxDstBuffer, fontMap, srcRect, subRect,
                                             fontWeight, letterInfo.color, letterInfo.opa);
}

void DrawUtils::DrawLetter(BufferInfo& gfxDstBuffer,
                           const uint8_t* fontMap,
                           const Rect& fontRect,
                           const Rect& subRect,
                           const uint8_t fontWeight,
                           const ColorType& color,
                           const OpacityType opa) const
{
    Color32 fillColor;
    fillColor.full = Color::ColorTo32(color);
    uint8_t opacityMask;
    uint8_t opacityStep = 1;
    switch (fontWeight) {
        case FONT_WEIGHT_1:
            opacityStep = OPACITY_STEP_A1;
            opacityMask = 0x01;
            break;
        case FONT_WEIGHT_2:
            opacityStep = OPACITY_STEP_A2;
            opacityMask = 0x03;
            break;
        case FONT_WEIGHT_4:
            opacityStep = OPACITY_STEP_A4;
            opacityMask = 0x0F;
            break;
        case FONT_WEIGHT_8:
            opacityMask = 0xFF;
            break;
        case FONT_WEIGHT_32:
            opacityMask = 0xFF;
            break;
        default:
            return;
    }

    uint8_t letterWidthInByte = (fontRect.GetWidth() * fontWeight) >> SHIFT_3;
    if ((fontRect.GetWidth() * fontWeight) & 0x7) { // 0x7 : less than 1 byte is counted as 1 byte
        letterWidthInByte++;
    }
    int16_t rowStart = subRect.GetY() - fontRect.GetY();
    int16_t rowEnd = rowStart + subRect.GetHeight();
    int16_t colStart = subRect.GetX() - fontRect.GetX();
    int16_t colEnd = colStart + subRect.GetWidth();

    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);
    screenBuffer += ((subRect.GetY() * screenBufferWidth) + subRect.GetX()) * bufferPxSize;
    fontMap += (rowStart * letterWidthInByte) + ((colStart * fontWeight) >> SHIFT_3);

    uint8_t offsetInFont = (colStart * fontWeight) % FONT_WEIGHT_8;
    int16_t temp = subRect.GetWidth() * fontWeight - FONT_WEIGHT_8 + offsetInFont;
    if (temp < 0) {
        temp = 0;
    }
    int16_t validWidthInByte = temp / FONT_WEIGHT_8 + 1;
    if (temp % FONT_WEIGHT_8 != 0) {
        validWidthInByte++;
    }
    for (int16_t i = rowStart; i < rowEnd; i++) {
        int16_t col = colStart;
        uint8_t tempOffset = offsetInFont;
        uint8_t tempFontByte = (*fontMap++) >> offsetInFont;
        while (col < colEnd) {
            while ((tempOffset < FONT_WEIGHT_8) && (col < colEnd)) {
                uint8_t validOpacity = tempFontByte & opacityMask;
                if (validOpacity != 0) {
                    validOpacity *= opacityStep;
                    if (opa != OPA_OPAQUE) {
                        validOpacity =
                            static_cast<OpacityType>((static_cast<uint16_t>(validOpacity) * opa) >> FONT_WEIGHT_8);
                    }
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &fillColor, ARGB8888, validOpacity);
                }
                screenBuffer += bufferPxSize;
                tempFontByte = tempFontByte >> fontWeight;
                tempOffset += fontWeight;
                col++;
            }
            tempOffset = 0;
            tempFontByte = *(fontMap++);
        }
        fontMap += letterWidthInByte - validWidthInByte - 1;
        screenBuffer += (screenBufferWidth - (colEnd - colStart)) * bufferPxSize;
    }
}

void DrawUtils::DrawImage(BufferInfo& gfxDstBuffer,
                          const Rect& area,
                          const Rect& mask,
                          const uint8_t* image,
                          OpacityType opa,
                          uint8_t pxBitSize,
                          const ImageHeader &imageHeader,
                          const uint8_t* userData) const
{
    if (image == nullptr) {
        return;
    }
    Rect maskedArea;
    if (!maskedArea.Intersect(area, mask)) {
        return;
    }
    int16_t mapWidth = area.GetWidth();
    int16_t imageX = maskedArea.GetLeft() - area.GetLeft();
    int16_t imageY = maskedArea.GetTop() - area.GetTop();
    uint32_t imageWidthInByte = (static_cast<uint32_t>(mapWidth) * pxBitSize) >> SHIFT_3;
    if ((mapWidth * pxBitSize) & 0x7) { // 0x7 : less than 1 byte is counted as 1 byte
        imageWidthInByte++;
    }

    BufferInfo src;
    src.rect = {imageX, imageY, static_cast<int16_t>(imageX + maskedArea.GetWidth() - 1),
                static_cast<int16_t>(imageY + maskedArea.GetHeight() - 1)};

    src.virAddr = static_cast<void*>(const_cast<uint8_t*>(image));
    src.phyAddr = static_cast<void*>(const_cast<uint8_t*>(userData));
    src.width = imageHeader.width;
    src.height = imageHeader.height;
    src.stride = imageWidthInByte;
    src.mode = static_cast<ColorMode>(imageHeader.colorMode);
    src.color = 0;

    Point dstPos = {maskedArea.GetLeft(), maskedArea.GetTop()};
    BlendOption blendOption;
    blendOption.opacity = opa;
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, dstPos, src, maskedArea, blendOption);
}

void DrawUtils::DrawImage(BufferInfo& gfxDstBuffer,
                          const Rect& area,
                          const Rect& mask,
                          const uint8_t* image,
                          OpacityType opa,
                          uint8_t pxBitSize,
                          ColorMode colorMode) const
{
    if (image == nullptr) {
        return;
    }
    Rect maskedArea;
    if (!maskedArea.Intersect(area, mask)) {
        return;
    }
    int16_t mapWidth = area.GetWidth();
    int16_t imageX = maskedArea.GetLeft() - area.GetLeft();
    int16_t imageY = maskedArea.GetTop() - area.GetTop();
    uint32_t imageWidthInByte = (static_cast<uint32_t>(mapWidth) * pxBitSize) >> SHIFT_3;
    if ((mapWidth * pxBitSize) & 0x7) { // 0x7 : less than 1 byte is counted as 1 byte
        imageWidthInByte++;
    }

    BufferInfo src;
    src.rect = {imageX, imageY, static_cast<int16_t>(imageX + maskedArea.GetWidth() - 1),
                static_cast<int16_t>(imageY + maskedArea.GetHeight() - 1)};

    src.virAddr = static_cast<void*>(const_cast<uint8_t*>(image));
    src.stride = imageWidthInByte;
    src.mode = colorMode;
    src.color = 0;

    Point dstPos = {maskedArea.GetLeft(), maskedArea.GetTop()};
    BlendOption blendOption;
    blendOption.opacity = opa;
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, dstPos, src, maskedArea, blendOption);
}

void DrawUtils::FillAreaWithSoftWare(BufferInfo& gfxDstBuffer,
                                     const Rect& fillArea,
                                     const ColorType& color,
                                     const OpacityType& opa) const
{
    if (gfxDstBuffer.virAddr == nullptr) {
        return;
    }
    ColorMode mode = gfxDstBuffer.mode;
    uint8_t destByteSize = GetByteSizeByColorMode(mode);
    int16_t destWidth = gfxDstBuffer.width;
    int32_t halBufferDeltaByteLen = static_cast<int32_t>(destWidth) * destByteSize;
    int16_t width = fillArea.GetWidth();
    int16_t height = fillArea.GetHeight();
    uint8_t* dest = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
    int32_t offset = static_cast<int32_t>(fillArea.GetTop()) * destWidth + fillArea.GetLeft();
    dest += offset * destByteSize;

    int32_t dstMaxSize = (gfxDstBuffer.width * gfxDstBuffer.height - offset) * destByteSize;
    Color32 fillColor;
    fillColor.full = Color::ColorTo32(color);
    uint8_t* dstTmp = nullptr;

    if ((fillColor.alpha == OPA_TRANSPARENT) || (opa == OPA_TRANSPARENT)) {
        return;
    }
    /* cover mode */
    if ((opa == OPA_OPAQUE) && (fillColor.alpha == OPA_OPAQUE)) {
        for (int16_t col = 0; col < width; ++col) {
            dstTmp = dest + (col * destByteSize);
            COLOR_FILL_COVER(dstTmp, mode, fillColor.red, fillColor.green, fillColor.blue, ARGB8888);
        }
        uint8_t* memStart = dest;
        int32_t memSize = static_cast<int32_t>(width) * destByteSize;
        dest += halBufferDeltaByteLen;
        dstMaxSize -= halBufferDeltaByteLen;
        for (int16_t row = 1; row < height; ++row) {
#ifdef ARM_NEON_OPT
            {
                DEBUG_PERFORMANCE_TRACE("memcpy_neon");
                NeonMemcpy(dest, dstMaxSize, memStart, memSize);
            }
#else
            {
                DEBUG_PERFORMANCE_TRACE("memcpy");
                if (memcpy_s(dest, dstMaxSize, memStart, memSize) != EOK) {
                    GRAPHIC_LOGE("DrawUtils::FillAreaWithSoftWare memcpy failed!\n");
                    return;
                }
            }
#endif
            dest += halBufferDeltaByteLen;
            dstMaxSize -= halBufferDeltaByteLen;
        }
    } else {
#ifdef ARM_NEON_OPT
        {
            DEBUG_PERFORMANCE_TRACE("FillAreaWithSoftWare_neon");
            NeonBlendPipeLine pipeLine;
            pipeLine.Construct(mode, ARGB8888, &fillColor, opa);
            int16_t step = NEON_STEP_8 * GetByteSizeByColorMode(mode);
            for (int16_t row = 0; row < height; ++row) {
                uint8_t* buf = dest;
                int16_t tmpWidth = width;
                while (tmpWidth >= NEON_STEP_8) {
                    pipeLine.Invoke(buf);
                    buf += step;
                    tmpWidth -= NEON_STEP_8;
                }
                for (int16_t i = 0; i < tmpWidth; ++i) {
                    COLOR_FILL_BLEND(buf, mode, &fillColor, ARGB8888, opa);
                    buf += destByteSize;
                }
                dest += halBufferDeltaByteLen;
            }
        }
#else
        {
            DEBUG_PERFORMANCE_TRACE("FillAreaWithSoftWare");
            for (int16_t row = 0; row < height; row++) {
                for (int16_t col = 0; col < width; col++) {
                    dstTmp = dest + (col * destByteSize);
                    COLOR_FILL_BLEND(dstTmp, mode, &fillColor, ARGB8888, opa);
                }
                dest += destWidth * destByteSize;
            }
        }
#endif
    }
}

#ifdef ARM_NEON_OPT
void DrawUtils::BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                             uint8_t alpha, uint8_t cover)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(color, red, green, blue, alpha, cover);
}

void DrawUtils::BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(color, red, green, blue, alpha);
}

void DrawUtils::BlendLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t srcCover)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(dstColors, srcColors, srcCover);
}

void DrawUtils::BlendLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t* srcCovers)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(dstColors, srcColors, srcCovers);
}

void DrawUtils::BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                             uint8_t alpha, uint8_t* covers)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(color, red, green, blue, alpha, covers);
}

void DrawUtils::BlendPreLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                                uint8_t alpha, uint8_t cover)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonPrelerpARGB8888(color, red, green, blue, alpha, cover);
}

void DrawUtils::BlendPreLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonPrelerpARGB8888(color, red, green, blue, alpha);
}
void DrawUtils::BlendPreLerpPix(uint8_t *dstColors, uint8_t *srcColors, uint8_t srcCover)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(dstColors, srcColors, srcCover);
}

void DrawUtils::BlendPreLerpPix(uint8_t *dstColors, uint8_t *srcColors, uint8_t *srcCovers)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonLerpARGB8888(dstColors, srcColors, srcCovers);
}

void DrawUtils::BlendPreLerpPix(uint8_t *color, uint8_t red, uint8_t green, uint8_t blue,
                                uint8_t alpha, uint8_t *covers)
{
    NeonBlendPipeLine mNeonBlendPipeLine;
    mNeonBlendPipeLine.NeonPreLerpARGB8888(color, red, green, blue, alpha, covers);
}
#endif

void DrawUtils::BlendWithSoftWare(const uint8_t* src1,
                                  const Rect& srcRect,
                                  uint32_t srcStride,
                                  uint32_t srcLineNumber,
                                  ColorMode srcMode,
                                  uint32_t color,
                                  OpacityType opa,
                                  uint8_t* dst,
                                  uint32_t destStride,
                                  ColorMode destMode,
                                  uint32_t x,
                                  uint32_t y) const
{
    if ((dst == nullptr) || (src1 == nullptr)) {
        return;
    }
    uint8_t destByteSize = GetByteSizeByColorMode(destMode);
    uint8_t srcByteSize = GetByteSizeByColorMode(srcMode);

    uint8_t* dest = dst + destStride * y;
    dest += destByteSize * x;

    uint8_t pxByteSize = GetPxSizeByColorMode(srcMode) >> 3; // 3 : right shift 3 bits
    uint8_t* src = const_cast<uint8_t*>(src1) + srcStride * srcRect.GetY() + pxByteSize * srcRect.GetX();
    uint32_t width = srcRect.GetWidth();
    uint32_t height = srcRect.GetHeight();
#ifdef ARM_NEON_OPT
    {
        DEBUG_PERFORMANCE_TRACE("BlendWithSoftWare_neon");
        NeonBlendPipeLine pipeLine;
        pipeLine.Construct(destMode, srcMode);
        int16_t dstStep = NEON_STEP_8 * GetByteSizeByColorMode(destMode);
        int16_t srcStep = NEON_STEP_8 * GetByteSizeByColorMode(srcMode);
        for (uint32_t row = 0; row < height; ++row) {
            uint8_t* dstBuf = dest;
            uint8_t* srcBuf = const_cast<uint8_t*>(src);
            int16_t tmpWidth = width;
            while (tmpWidth >= NEON_STEP_8) {
                pipeLine.Invoke(dstBuf, srcBuf, opa);
                dstBuf += dstStep;
                srcBuf += srcStep;
                tmpWidth -= NEON_STEP_8;
            }
            for (int16_t i = 0; i < tmpWidth; ++i) {
                COLOR_FILL_BLEND(dstBuf, destMode, srcBuf, srcMode, opa);
                dstBuf += destByteSize;
                srcBuf += srcByteSize;
            }
            dest += destStride;
            src += srcStride;
        }
    }
#else
    {
        DEBUG_PERFORMANCE_TRACE("BlendWithSoftWare");
    uint32_t temp = HALTick::GetInstance().GetTime();

    
    if(opa == OPA_OPAQUE && srcMode == destMode){
        for (uint32_t row = 0; row < height; ++row) {
            memcpy(dest, src, width*destByteSize);
            dest += destStride;
            src += srcStride;
        }
    }else{
        for (uint32_t row = 0; row < height; ++row) {
            uint8_t* destTmp = dest;
            uint8_t* srcTmp = const_cast<uint8_t*>(src);
            for (uint32_t col = 0; col < width; ++col) {
                COLOR_FILL_BLEND(destTmp, destMode, srcTmp, srcMode, opa);
                destTmp += destByteSize;
                srcTmp += srcByteSize;
            }
            dest += destStride;
            src += srcStride;
        }        
    }
    GRAPHIC_LOGD("duration: %d", HALTick::GetInstance().GetElapseTime(temp));
    }
#endif
}

void DrawUtils::GetXAxisErrForJunctionLine(bool ignoreJunctionPoint,
                                           bool isRightPart,
                                           int16_t& xMinErr,
                                           int16_t& xMaxErr)
{
    xMinErr = 0;
    xMaxErr = 0;
    if (ignoreJunctionPoint) {
        if (isRightPart) {
            xMinErr = 1;
        } else {
            xMaxErr = -1;
        }
    }
}

void DrawUtils::GetTransformInitState(const TransformMap& transMap,
                                      const Point& position,
                                      const Rect& trans,
                                      TransformInitState& init)
{
    int16_t x = trans.GetLeft();
    int16_t y = trans.GetTop();
#if ENABLE_FIXED_POINT
    init.duHorizon = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[0]);
    init.dvHorizon = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[1]);
    init.duVertical = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[3]); // 3:RSxy
    init.dvVertical = FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[4]); // 4:RSyy
    init.verticalU = (x - position.x) * init.duHorizon + (y - position.y) * init.duVertical +
                     FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[6]); // 6:TRSx
    init.verticalV = (x - position.x) * init.dvHorizon + (y - position.y) * init.dvVertical +
                     FO_TRANS_FLOAT_TO_FIXED(transMap.invMatrix_.GetData()[7]); // 7:TRSy
#else
    init.duHorizon = transMap.invMatrix_.GetData()[0];
    init.dvHorizon = transMap.invMatrix_.GetData()[1];
    init.duVertical = transMap.invMatrix_.GetData()[3]; // 3:RSxy
    init.dvVertical = transMap.invMatrix_.GetData()[4]; // 4:RSyy
    init.verticalU = (x - position.x) * init.duHorizon + (y - position.y) * init.duVertical +
                     transMap.invMatrix_.GetData()[6]; // 6:TRSx
    init.verticalV = (x - position.x) * init.dvHorizon + (y - position.y) * init.dvVertical +
                     transMap.invMatrix_.GetData()[7]; // 7:TRSy
#endif
}

inline void DrawUtils::StepToNextLine(TriangleEdge& edge1, TriangleEdge& edge2)
{
#if ENABLE_FIXED_POINT
    edge1.curY += FIXED_NUM_1;
    edge2.curY += FIXED_NUM_1;
    edge1.curX += FO_DIV(edge1.du, edge1.dv);
    edge2.curX += FO_DIV(edge2.du, edge2.dv);
#else
    edge1.curY++;
    edge2.curY++;
    edge1.curX += edge1.du / edge1.dv;
    edge2.curX += edge2.du / edge2.dv;
#endif
}

void DrawUtils::DrawTriangleAlphaBilinear(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX);
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX);
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
#else
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX), maskRight);
        int16_t diffX = (xMin - static_cast<int32_t>(in.edge1.curX));
#endif
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;

#if ENABLE_FIXED_POINT
        // parameters below are Q15 fixed-point number
        int64_t u = in.init.verticalU;
        int64_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
#else
        float u = in.init.verticalU;
        float v = in.init.verticalV;
#endif
        for (int16_t x = xMin; x <= xMax; x++) {
#if ENABLE_FIXED_POINT
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
                int16_t intUPlus1 = intU + 1;
                int16_t intVPlus1 = intV + 1;
#else
            const int16_t intU = static_cast<int16_t>(u);
            const int16_t intV = static_cast<int16_t>(v);
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
                const int16_t intUPlus1 = intU + 1;
                const int16_t intVPlus1 = intV + 1;
#endif
                OpacityType p1 = GetPxAlphaForAlphaImg(in.info, {intU, intV});
                OpacityType p2 = GetPxAlphaForAlphaImg(in.info, {intUPlus1, intV});
                OpacityType p3 = GetPxAlphaForAlphaImg(in.info, {intU, intVPlus1});
                OpacityType p4 = GetPxAlphaForAlphaImg(in.info, {intUPlus1, intVPlus1});
#if ENABLE_FIXED_POINT
                // parameters below are Q15 fixed-point number
                int64_t decU = FO_DECIMAL(u);
                int64_t decV = FO_DECIMAL(v);
                int64_t decUMinus1 = FIXED_NUM_1 - decU;
                int64_t decVMinus1 = FIXED_NUM_1 - decV;
                int64_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int64_t w2 = FO_MUL(decU, decVMinus1);
                int64_t w3 = FO_MUL(decUMinus1, decV);
                int64_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#else
                const float decU = u - intU;
                const float decV = v - intV;
                const float decUMinus1 = 1.0f - decU;
                const float decVMinus1 = 1.0f - decV;

                const int32_t w1 = static_cast<int32_t>(decUMinus1 * decVMinus1 * 256.0f); // 256:shift 8 bit left
                const int32_t w2 = static_cast<int32_t>(decU * decVMinus1 * 256.0f);       // 256:shift 8 bit left
                const int32_t w3 = static_cast<int32_t>(decUMinus1 * decV * 256.0f);       // 256:shift 8 bit left
                const int32_t w4 = static_cast<int32_t>(decU * decV * 256.0f);             // 256:shift 8 bit left
#endif
#if ENABLE_ARM_MATH
                const int64_t outA = __SMUAD(p1, w1) + __SMUAD(p2, w2) + __SMUAD(p3, w3) + __SMUAD(p4, w4);
#else
                const int64_t outA = p1 * w1 + p2 * w2 + p3 * w3 + p4 * w4;
#endif
                Color32 result;
                result.full = Color::ColorTo32(in.color);
#if ENABLE_FIXED_POINT
                result.alpha = FO_TO_INTEGER(outA);
#else
                result.alpha = static_cast<uint8_t>(outA >> 8); // 8:shift 8 bit right
#endif
                COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
#if ENABLE_FIXED_POINT
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
#else
        int16_t deltaX = static_cast<int16_t>(in.edge1.curX) - xMin;
#endif
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorBilinear565(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
#else
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX + xMinErr), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX + xMaxErr), maskRight);
        int16_t diffX = (xMin - static_cast<int32_t>(in.edge1.curX));
#endif
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#if ENABLE_FIXED_POINT
        // parameters below are Q15 fixed-point number
        int64_t u = in.init.verticalU;
        int64_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
#else
        float u = in.init.verticalU;
        float v = in.init.verticalV;
#endif
        for (int16_t x = xMin; x <= xMax; x++) {
#if ENABLE_FIXED_POINT
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
#else
            const int16_t intU = static_cast<int16_t>(u);
            const int16_t intV = static_cast<int16_t>(v);
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#endif
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                const Color16 p1 = *(reinterpret_cast<Color16*>(&imgHead[px1]));
                const Color16 p2 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.pixelSize]));
                const Color16 p3 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.srcLineWidth]));
                const Color16 p4 = *(reinterpret_cast<Color16*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));
#if ENABLE_FIXED_POINT
                // parameters below are Q15 fixed-point number
                int64_t decU = FO_DECIMAL(u);
                int64_t decV = FO_DECIMAL(v);
                int64_t decUMinus1 = FIXED_NUM_1 - decU;
                int64_t decVMinus1 = FIXED_NUM_1 - decV;
                int64_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int64_t w2 = FO_MUL(decU, decVMinus1);
                int64_t w3 = FO_MUL(decUMinus1, decV);
                int64_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#else
                const float decU = u - intU;
                const float decV = v - intV;
                const float decUMinus1 = 1 - decU;
                const float decVMinus1 = 1 - decV;
                const int32_t w1 = static_cast<int32_t>(decUMinus1 * decVMinus1 * 256.0f); // 256:shift 8 bit left
                const int32_t w2 = static_cast<int32_t>(decU * decVMinus1 * 256.0f);       // 256:shift 8 bit left
                const int32_t w3 = static_cast<int32_t>(decUMinus1 * decV * 256.0f);       // 256:shift 8 bit left
                const int32_t w4 = static_cast<int32_t>(decU * decV * 256.0f);             // 256:shift 8 bit left
#endif
#if ENABLE_ARM_MATH
                const int64_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int64_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int64_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
#else
                const int64_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int64_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int64_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
#endif

                Color16 result;
#if ENABLE_FIXED_POINT
                result.red = static_cast<uint8_t>(outR >> FIXED_Q_NUM);
                result.green = static_cast<uint8_t>(outG >> FIXED_Q_NUM);
                result.blue = static_cast<uint8_t>(outB >> FIXED_Q_NUM);
#else
                result.red = static_cast<uint8_t>(outR >> 5);   // 5:shift 5 bit right
                result.green = static_cast<uint8_t>(outG >> 6); // 6:shift 6 bit right
                result.blue = static_cast<uint8_t>(outB >> 5);  // 5:shift 5 bit right
#endif
                if (in.opaScale == OPA_OPAQUE) {
                    COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, RGB565);
                } else {
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, RGB565, in.opaScale);
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
#if ENABLE_FIXED_POINT
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
#else
        int16_t deltaX = static_cast<int16_t>(in.edge1.curX) - xMin;
#endif
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorBilinear888(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
#else
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX + xMinErr), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX + xMaxErr), maskRight);
        int16_t diffX = (xMin - static_cast<int32_t>(in.edge1.curX));
#endif
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#if ENABLE_FIXED_POINT
        // parameters below are Q15 fixed-point number
        int64_t u = in.init.verticalU;
        int64_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
#else
        float u = in.init.verticalU;
        float v = in.init.verticalV;
#endif
        for (int16_t x = xMin; x <= xMax; x++) {
#if ENABLE_FIXED_POINT
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
#else
            const int16_t intU = static_cast<int16_t>(u);
            const int16_t intV = static_cast<int16_t>(v);
#endif
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                const Color24 p1 = *(reinterpret_cast<Color24*>(&imgHead[px1]));
                const Color24 p2 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.pixelSize]));
                const Color24 p3 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.srcLineWidth]));
                const Color24 p4 = *(reinterpret_cast<Color24*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));
#if ENABLE_FIXED_POINT
                // parameters below are Q15 fixed-point number
                int64_t decU = FO_DECIMAL(u);
                int64_t decV = FO_DECIMAL(v);
                int64_t decUMinus1 = FIXED_NUM_1 - decU;
                int64_t decVMinus1 = FIXED_NUM_1 - decV;
                int64_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int64_t w2 = FO_MUL(decU, decVMinus1);
                int64_t w3 = FO_MUL(decUMinus1, decV);
                int64_t w4 = FO_MUL(decU, decV);
                // parameters above are Q15 fixed-point number
#else
                const float decU = u - intU;
                const float decV = v - intV;
                const float decUMinus1 = 1 - decU;
                const float decVMinus1 = 1 - decV;
                const int32_t w1 = static_cast<int32_t>(decUMinus1 * decVMinus1 * 256.0f); // 256:shift 8 bit left
                const int32_t w2 = static_cast<int32_t>(decU * decVMinus1 * 256.0f);       // 256:shift 8 bit left
                const int32_t w3 = static_cast<int32_t>(decUMinus1 * decV * 256.0f);       // 256:shift 8 bit left
                const int32_t w4 = static_cast<int32_t>(decU * decV * 256.0f);             // 256:shift 8 bit left
#endif

#if ENABLE_ARM_MATH
                const int64_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int64_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int64_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
#else
                const int64_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int64_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int64_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
#endif

                Color24 result;
#if ENABLE_FIXED_POINT
                result.red = static_cast<uint8_t>(outR >> FIXED_Q_NUM);
                result.green = static_cast<uint8_t>(outG >> FIXED_Q_NUM);
                result.blue = static_cast<uint8_t>(outB >> FIXED_Q_NUM);
#else
                result.red = static_cast<uint8_t>(outR >> 8);   // 8:shift 8 bit right
                result.green = static_cast<uint8_t>(outG >> 8); // 8:shift 8 bit right
                result.blue = static_cast<uint8_t>(outB >> 8);  // 8:shift 8 bit right
#endif
                if (in.opaScale == OPA_OPAQUE) {
                    COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, RGB888);
                } else {
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, RGB888, in.opaScale);
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
#if ENABLE_FIXED_POINT
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
#else
        int16_t deltaX = static_cast<int16_t>(in.edge1.curX) - xMin;
#endif
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

#if !ENABLE_FIXED_POINT
static void DrawTriangleTrueColorBilinear8888Inner(const TriangleScanInfo& in,
                                                   uint8_t* screenBuffer,
                                                   int16_t len,
                                                   const ColorMode bufferMode,
                                                   float u,
                                                   float v)
{
    for (int16_t x = 0; x < len; ++x) {
        const int16_t intU = static_cast<int16_t>(u);
        const int16_t intV = static_cast<int16_t>(v);
        if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
            uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
            uint32_t val2 = __SMUAD(intU, in.pixelSize);
            uint32_t px1 = val1 + val2;
#else
            uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
            uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
            const Color32 p1 = *(reinterpret_cast<Color32*>(&imgHead[px1]));
            const Color32 p2 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.pixelSize]));
            const Color32 p3 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth]));
            const Color32 p4 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));

            const float decU = u - intU;
            const float decV = v - intV;
            const float decUMinus1 = 1 - decU;
            const float decVMinus1 = 1 - decV;

            const int32_t w1 = static_cast<int32_t>(decUMinus1 * decVMinus1 * 256.0f); // 256:shift 8 bit left
            const int32_t w2 = static_cast<int32_t>(decU * decVMinus1 * 256.0f);       // 256:shift 8 bit left
            const int32_t w3 = static_cast<int32_t>(decUMinus1 * decV * 256.0f);       // 256:shift 8 bit left
            const int32_t w4 = static_cast<int32_t>(decU * decV * 256.0f);             // 256:shift 8 bit left

#if ENABLE_ARM_MATH
            const int32_t outR = __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
            const int32_t outG =
                __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
            const int32_t outB =
                __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
            const int32_t outA =
                __SMUAD(p1.alpha, w1) + __SMUAD(p2.alpha, w2) + __SMUAD(p3.alpha, w3) + __SMUAD(p4.alpha, w4);
#else
            const int32_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
            const int32_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
            const int32_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
            const int32_t outA = p1.alpha * w1 + p2.alpha * w2 + p3.alpha * w3 + p4.alpha * w4;
#endif

            Color32 result;
            result.red = static_cast<uint8_t>(outR >> 8);   // 8:shift 8 bit right
            result.green = static_cast<uint8_t>(outG >> 8); // 8:shift 8 bit right
            result.blue = static_cast<uint8_t>(outB >> 8);  // 8:shift 8 bit right
            result.alpha = static_cast<uint8_t>(outA >> 8); // 8:shift 8 bit right
            if ((in.opaScale == OPA_OPAQUE) && (result.alpha == OPA_OPAQUE)) {
                COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
            } else {
                COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
            }
        }
        u += in.init.duHorizon;
        v += in.init.dvHorizon;
        screenBuffer += in.bufferPxSize;
    }
}
#endif

#if ENABLE_FIXED_POINT
static void DrawFixedTriangleTrueColorBilinear8888Inner(const TriangleScanInfo& in,
                                                        uint8_t* screenBuffer,
                                                        int16_t len,
                                                        const ColorMode bufferMode,
                                                        int64_t u,
                                                        int64_t v)
{
    for (int16_t x = 0; x < len; ++x) {
        int16_t intU = FO_TO_INTEGER(u);
        int16_t intV = FO_TO_INTEGER(v);
        if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
            uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
            uint32_t val2 = __SMUAD(intU, in.pixelSize);
            uint32_t px1 = val1 + val2;
#else
            uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
            uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
            const Color32 p1 = *(reinterpret_cast<Color32*>(&imgHead[px1]));
            const Color32 p2 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.pixelSize]));
            const Color32 p3 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth]));
            const Color32 p4 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));

            // parameters below are Q15 fixed-point number
            int64_t decU = FO_DECIMAL(u);
            int64_t decV = FO_DECIMAL(v);
            int64_t decUMinus1 = FIXED_NUM_1 - decU;
            int64_t decVMinus1 = FIXED_NUM_1 - decV;
            int64_t w1 = FO_MUL(decUMinus1, decVMinus1);
            int64_t w2 = FO_MUL(decU, decVMinus1);
            int64_t w3 = FO_MUL(decUMinus1, decV);
            int64_t w4 = FO_MUL(decU, decV);
            // parameters above are Q15 fixed-point number

#if ENABLE_ARM_MATH
            const int64_t outR = __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
            const int64_t outG =
                __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
            const int64_t outB =
                __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
            const int64_t outA =
                __SMUAD(p1.alpha, w1) + __SMUAD(p2.alpha, w2) + __SMUAD(p3.alpha, w3) + __SMUAD(p4.alpha, w4);
#else
            const int64_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
            const int64_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
            const int64_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
            const int64_t outA = p1.alpha * w1 + p2.alpha * w2 + p3.alpha * w3 + p4.alpha * w4;
#endif

            Color32 result;
            result.red = static_cast<uint8_t>(outR >> FIXED_Q_NUM);
            result.green = static_cast<uint8_t>(outG >> FIXED_Q_NUM);
            result.blue = static_cast<uint8_t>(outB >> FIXED_Q_NUM);
            result.alpha = static_cast<uint8_t>(outA >> FIXED_Q_NUM);
            if ((in.opaScale == OPA_OPAQUE) && (result.alpha == OPA_OPAQUE)) {
                COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
            } else {
                COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
            }
        }
        u += in.init.duHorizon;
        v += in.init.dvHorizon;
        screenBuffer += in.bufferPxSize;
    }
}
#endif

#ifdef ARM_NEON_OPT
static void DrawTriangleTrueColorBilinear8888InnerNeon(const TriangleScanInfo& in,
                                                       uint8_t* screenBuffer,
                                                       int16_t len,
                                                       float u,
                                                       float v,
                                                       NeonBlendPipeLine& pipeLine,
                                                       const ColorMode bufferMode)
{
    Color32 arrayp1[NEON_STEP_8] = {};
    Color32 arrayp2[NEON_STEP_8] = {};
    Color32 arrayp3[NEON_STEP_8] = {};
    Color32 arrayp4[NEON_STEP_8] = {};
    float arrayU[NEON_STEP_8] = {0};
    float arrayV[NEON_STEP_8] = {0};
    int32_t arrayPx1[NEON_STEP_8] = {0};
    int16_t step = in.bufferPxSize * NEON_STEP_8;
#if ENABLE_FIXED_POINT
    float duHorizon = static_cast<float>(in.init.duHorizon) / FIXED_NUM_1;
    float dvHorizon = static_cast<float>(in.init.dvHorizon) / FIXED_NUM_1;
#endif
    while (len >= NEON_STEP_8) {
        for (uint32_t i = 0; i < NEON_STEP_8; ++i) {
            arrayU[i] = u;
            arrayV[i] = v;
#if ENABLE_FIXED_POINT
            u += duHorizon;
            v += dvHorizon;
#else
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
#endif
        }
        // Monotonically increasing or decreasing, so only judge the beginning and end.
        if ((arrayU[0] >= 0) && (arrayU[0] < in.info.header.width - 1) && (arrayV[0] >= 0) &&
            (arrayV[0] < in.info.header.height - 1) && (arrayU[NEON_STEP_8 - 1] >= 0) &&
            (arrayU[NEON_STEP_8 - 1] < in.info.header.width - 1) && (arrayV[NEON_STEP_8 - 1] >= 0) &&
            (arrayV[NEON_STEP_8 - 1] < in.info.header.height - 1)) {
            // Process the lower half of arrayU and arrayV
            float32x4_t vU = vld1q_f32(arrayU);
            float32x4_t vV = vld1q_f32(arrayV);
            int32x4_t vIntU = vcvtq_s32_f32(vU);
            int32x4_t vIntV = vcvtq_s32_f32(vV);
            int32x4_t vPx1 =
                vaddq_s32(vmulq_s32(vIntV, vdupq_n_s32(in.srcLineWidth)), vmulq_s32(vIntU, vdupq_n_s32(in.pixelSize)));
            vst1q_s32(arrayPx1, vPx1);
            float32x4_t vDecU = vsubq_f32(vU, vcvtq_f32_s32(vIntU));
            float32x4_t vDecV = vsubq_f32(vV, vcvtq_f32_s32(vIntV));
            float32x4_t vDecUMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecU);
            float32x4_t vDecVMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecV);
            // 256:shift 8 bit left
            uint32x4_t vLowW1 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vLowW2 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vLowW3 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecV), vdupq_n_f32(256.0)));
            uint32x4_t vLowW4 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecV), vdupq_n_f32(256.0)));
            // Process the higher half of arrayU and arrayV
            vU = vld1q_f32(arrayU + NEON_STEP_4);
            vV = vld1q_f32(arrayV + NEON_STEP_4);
            vIntU = vcvtq_s32_f32(vU);
            vIntV = vcvtq_s32_f32(vV);
            vPx1 =
                vaddq_s32(vmulq_s32(vIntV, vdupq_n_s32(in.srcLineWidth)), vmulq_s32(vIntU, vdupq_n_s32(in.pixelSize)));
            vst1q_s32(arrayPx1 + NEON_STEP_4, vPx1);
            vDecU = vsubq_f32(vU, vcvtq_f32_s32(vIntU));
            vDecV = vsubq_f32(vV, vcvtq_f32_s32(vIntV));
            vDecUMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecU);
            vDecVMinus1 = vsubq_f32(vdupq_n_f32(1.0), vDecV);
            // 256:shift 8 bit left
            uint32x4_t vHighW1 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vHighW2 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecVMinus1), vdupq_n_f32(256.0)));
            uint32x4_t vHighW3 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecUMinus1, vDecV), vdupq_n_f32(256.0)));
            uint32x4_t vHighW4 = vcvtq_u32_f32(vmulq_f32(vmulq_f32(vDecU, vDecV), vdupq_n_f32(256.0)));

            // joins two uint32x4_t vectors into a uint16x8_t vector
            uint16x8_t vW1 = vcombine_u16(vmovn_u32(vLowW1), vmovn_u32(vHighW1));
            uint16x8_t vW2 = vcombine_u16(vmovn_u32(vLowW2), vmovn_u32(vHighW2));
            uint16x8_t vW3 = vcombine_u16(vmovn_u32(vLowW3), vmovn_u32(vHighW3));
            uint16x8_t vW4 = vcombine_u16(vmovn_u32(vLowW4), vmovn_u32(vHighW4));

            uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
            for (uint32_t i = 0; i < NEON_STEP_8; ++i) {
                int32_t px1 = arrayPx1[i];
                arrayp1[i] = *(reinterpret_cast<Color32*>(&imgHead[px1]));
                arrayp2[i] = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.pixelSize]));
                arrayp3[i] = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth]));
                arrayp4[i] = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));
            }

            uint8x8x4_t v4p1 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp1));
            uint8x8x4_t v4p2 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp2));
            uint8x8x4_t v4p3 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp3));
            uint8x8x4_t v4p4 = vld4_u8(reinterpret_cast<uint8_t*>(arrayp4));
            uint8x8_t vOutB =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_B]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_B]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_B]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_B]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutG =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_G]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_G]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_G]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_G]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutR =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_R]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_R]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_R]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_R]), vW4),
                            8); // 8:shift 8 bit right
            uint8x8_t vOutA =
                vshrn_n_u16(vmulq_u16(vmovl_u8(v4p1.val[NEON_A]), vW1) + vmulq_u16(vmovl_u8(v4p2.val[NEON_A]), vW2) +
                                vmulq_u16(vmovl_u8(v4p3.val[NEON_A]), vW3) + vmulq_u16(vmovl_u8(v4p4.val[NEON_A]), vW4),
                            8); // 8:shift 8 bit right
            vOutA = NeonMulDiv255(vdup_n_u8(in.opaScale), vOutA);
            pipeLine.Invoke(screenBuffer, vOutR, vOutG, vOutB, vOutA);
        } else {
#if ENABLE_FIXED_POINT
            int64_t fixedU = FO_TRANS_FLOAT_TO_FIXED(arrayU[0]);
            int64_t fixedV = FO_TRANS_FLOAT_TO_FIXED(arrayV[0]);
            DrawFixedTriangleTrueColorBilinear8888Inner(in, screenBuffer, NEON_STEP_8, bufferMode, fixedU, fixedV);
#else
            DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, NEON_STEP_8, bufferMode, arrayU[0], arrayV[0]);
#endif
        }
        screenBuffer += step;
        len -= NEON_STEP_8;
    }
    if (len > 0) {
#if ENABLE_FIXED_POINT
        int64_t fixedU = FO_TRANS_FLOAT_TO_FIXED(u);
        int64_t fixedV = FO_TRANS_FLOAT_TO_FIXED(v);
        DrawFixedTriangleTrueColorBilinear8888Inner(in, screenBuffer, len, bufferMode, fixedU, fixedV);
#else
        DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, len, bufferMode, u, v);
#endif
    }
}
#endif

void DrawUtils::Draw3DTriangleTrueColorBilinear8888(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
#if ENABLE_FIXED_POINT
    int64_t invMatrix00 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[0]);
    int64_t invMatrix01 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[1]);
    int64_t invMatrix02 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[2]);
    int64_t invMatrix20 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[3]);
    int64_t invMatrix21 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[4]);
    int64_t invMatrix22 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[5]);
    int64_t invMatrix30 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[6]);
    int64_t invMatrix31 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[7]);
    int64_t invMatrix32 = FO_TRANS_FLOAT_TO_FIXED(in.matrix.GetData()[8]);
#else  // ENABLE_FIXED_POINT
    float invMatrix00 = in.matrix.GetData()[0];
    float invMatrix01 = in.matrix.GetData()[1];
    float invMatrix02 = in.matrix.GetData()[2];
    float invMatrix20 = in.matrix.GetData()[3];
    float invMatrix21 = in.matrix.GetData()[4];
    float invMatrix22 = in.matrix.GetData()[5];
    float invMatrix30 = in.matrix.GetData()[6];
    float invMatrix31 = in.matrix.GetData()[7];
    float invMatrix32 = in.matrix.GetData()[8];
#endif // ENABLE_FIXED_POINT
    for (int16_t y = in.yMin; y <= in.yMax; ++y) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
#else  // ENABLE_FIXED_POINT
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX + xMinErr), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX + xMaxErr), maskRight);
#endif // ENABLE_FIXED_POINT
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
        // move to current position
        for (int16_t x = xMin; x <= xMax; x++) {
#if ENABLE_FIXED_POINT
            int64_t w = invMatrix02 * x + invMatrix22 * y + invMatrix32;
            int64_t u = FO_DIV((invMatrix00 * x + invMatrix20 * y + invMatrix30), w);
            int64_t v = FO_DIV((invMatrix01 * x + invMatrix21 * y + invMatrix31), w);
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
#else  // ENABLE_FIXED_POINT
            float w = invMatrix02 * x + invMatrix22 * y + invMatrix32;
            float u = (invMatrix00 * x + invMatrix20 * y + invMatrix30) / w;
            float v = (invMatrix01 * x + invMatrix21 * y + invMatrix31) / w;
            int16_t intU = static_cast<int16_t>(u);
            int16_t intV = static_cast<int16_t>(v);
#endif // ENABLE_FIXED_POINT
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else  // ENABLE_ARM_MATH
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif // ENABLE_ARM_MATH
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                const Color32 p1 = *(reinterpret_cast<Color32*>(&imgHead[px1]));
                const Color32 p2 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.pixelSize]));
                const Color32 p3 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth]));
                const Color32 p4 = *(reinterpret_cast<Color32*>(&imgHead[px1 + in.srcLineWidth + in.pixelSize]));
#if ENABLE_FIXED_POINT
                int64_t decU = FO_DECIMAL(u);
                int64_t decV = FO_DECIMAL(v);
                int64_t decUMinus1 = FIXED_NUM_1 - decU;
                int64_t decVMinus1 = FIXED_NUM_1 - decV;
                int64_t w1 = FO_MUL(decUMinus1, decVMinus1);
                int64_t w2 = FO_MUL(decU, decVMinus1);
                int64_t w3 = FO_MUL(decUMinus1, decV);
                int64_t w4 = FO_MUL(decU, decV);
#if ENABLE_ARM_MATH
                const int64_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int64_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int64_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
                const int64_t outA =
                    __SMUAD(p1.alpha, w1) + __SMUAD(p2.alpha, w2) + __SMUAD(p3.alpha, w3) + __SMUAD(p4.alpha, w4);
#else
                const int64_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int64_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int64_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
                const int64_t outA = p1.alpha * w1 + p2.alpha * w2 + p3.alpha * w3 + p4.alpha * w4;
#endif
                Color32 result;
                result.red = static_cast<uint8_t>(outR >> FIXED_Q_NUM);
                result.green = static_cast<uint8_t>(outG >> FIXED_Q_NUM);
                result.blue = static_cast<uint8_t>(outB >> FIXED_Q_NUM);
                result.alpha = static_cast<uint8_t>(outA >> FIXED_Q_NUM);
#else // ENABLE_FIXED_POINT
                const float decU = u - intU;
                const float decV = v - intV;
                const float decUMinus1 = 1 - decU;
                const float decVMinus1 = 1 - decV;
                const int32_t w1 = static_cast<int32_t>(decUMinus1 * decVMinus1 * 256.0f); // 256:shift 8 bit left
                const int32_t w2 = static_cast<int32_t>(decU * decVMinus1 * 256.0f);       // 256:shift 8 bit left
                const int32_t w3 = static_cast<int32_t>(decUMinus1 * decV * 256.0f);       // 256:shift 8 bit left
                const int32_t w4 = static_cast<int32_t>(decU * decV * 256.0f);
#if ENABLE_ARM_MATH
                const int32_t outR =
                    __SMUAD(p1.red, w1) + __SMUAD(p2.red, w2) + __SMUAD(p3.red, w3) + __SMUAD(p4.red, w4);
                const int32_t outG =
                    __SMUAD(p1.green, w1) + __SMUAD(p2.green, w2) + __SMUAD(p3.green, w3) + __SMUAD(p4.green, w4);
                const int32_t outB =
                    __SMUAD(p1.blue, w1) + __SMUAD(p2.blue, w2) + __SMUAD(p3.blue, w3) + __SMUAD(p4.blue, w4);
                const int32_t outA =
                    __SMUAD(p1.alpha, w1) + __SMUAD(p2.alpha, w2) + __SMUAD(p3.alpha, w3) + __SMUAD(p4.alpha, w4);
#else  // ENABLE_ARM_MATH
                const int32_t outR = p1.red * w1 + p2.red * w2 + p3.red * w3 + p4.red * w4;
                const int32_t outG = p1.green * w1 + p2.green * w2 + p3.green * w3 + p4.green * w4;
                const int32_t outB = p1.blue * w1 + p2.blue * w2 + p3.blue * w3 + p4.blue * w4;
                const int32_t outA = p1.alpha * w1 + p2.alpha * w2 + p3.alpha * w3 + p4.alpha * w4;
#endif // ENABLE_ARM_MATH
                Color32 result;
                result.red = static_cast<uint8_t>(outR >> 8);   // 8:shift 8 bit right
                result.green = static_cast<uint8_t>(outG >> 8); // 8:shift 8 bit right
                result.blue = static_cast<uint8_t>(outB >> 8);  // 8:shift 8 bit right
                result.alpha = static_cast<uint8_t>(outA >> 8); // 8:shift 8 bit right
#endif // ENABLE_FIXED_POINT
                if ((in.opaScale == OPA_OPAQUE) && (result.alpha == OPA_OPAQUE)) {
                    COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
                } else {
                    COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, in.opaScale);
                }
            }
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
    }
}

void DrawUtils::DrawTriangleTrueColorBilinear8888(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
#ifdef ARM_NEON_OPT
    NeonBlendPipeLine pipeLine;
    pipeLine.Construct(bufferMode, ARGB8888);
#endif
    for (int16_t y = in.yMin; y <= in.yMax; ++y) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
#else
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX + xMinErr), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX + xMaxErr), maskRight);
        int16_t diffX = (xMin - static_cast<int32_t>(in.edge1.curX));
#endif
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#ifdef ARM_NEON_OPT
        {
#if ENABLE_FIXED_POINT
            float u = static_cast<float>(in.init.verticalU) / FIXED_NUM_1;
            float v = static_cast<float>(in.init.verticalV) / FIXED_NUM_1;
#else
            float u = in.init.verticalU;
            float v = in.init.verticalV;
#endif
            DEBUG_PERFORMANCE_TRACE("DrawTriangleTrueColorBilinear8888_neon");
            DrawTriangleTrueColorBilinear8888InnerNeon(in, screenBuffer, xMax - xMin + 1, u, v, pipeLine, bufferMode);
        }
#else
        {
            DEBUG_PERFORMANCE_TRACE("DrawTriangleTrueColorBilinear8888");
#if ENABLE_FIXED_POINT
            DrawFixedTriangleTrueColorBilinear8888Inner(in, screenBuffer, xMax - xMin + 1, bufferMode,
                                                        in.init.verticalU, in.init.verticalV);
#else
            DrawTriangleTrueColorBilinear8888Inner(in, screenBuffer, xMax - xMin + 1, bufferMode, in.init.verticalU,
                                                   in.init.verticalV);
#endif
        }
#endif
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
#if ENABLE_FIXED_POINT
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
#else
        int16_t deltaX = static_cast<int16_t>(in.edge1.curX) - xMin;
#endif
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTrueColorNearest(const TriangleScanInfo& in, const ColorMode bufferMode)
{
    int16_t maskLeft = in.mask.GetLeft();
    int16_t maskRight = in.mask.GetRight();
    int16_t xMinErr = 0;
    int16_t xMaxErr = 0;
    GetXAxisErrForJunctionLine(in.ignoreJunctionPoint, in.isRightPart, xMinErr, xMaxErr);
    for (int16_t y = in.yMin; y <= in.yMax; y++) {
#if ENABLE_FIXED_POINT
        int16_t tempV = FO_TO_INTEGER(in.edge1.curX) + xMinErr;
        int16_t xMin = MATH_MAX(tempV, maskLeft);
        tempV = FO_TO_INTEGER(in.edge2.curX) + xMaxErr;
        int16_t xMax = MATH_MIN(tempV, maskRight);
        int16_t diffX = xMin - FO_TO_INTEGER(in.edge1.curX);
#else
        int16_t xMin = MATH_MAX(static_cast<int16_t>(in.edge1.curX + xMinErr), maskLeft);
        int16_t xMax = MATH_MIN(static_cast<int16_t>(in.edge2.curX + xMaxErr), maskRight);
        int16_t diffX = (xMin - static_cast<int32_t>(in.edge1.curX));
#endif
        in.init.verticalU += in.init.duHorizon * diffX;
        in.init.verticalV += in.init.dvHorizon * diffX;
        uint8_t* screenBuffer = in.screenBuffer + (y * in.screenBufferWidth + xMin) * in.bufferPxSize;
#if ENABLE_FIXED_POINT
        // parameters below are Q15 fixed-point number
        int64_t u = in.init.verticalU;
        int64_t v = in.init.verticalV;
        // parameters above are Q15 fixed-point number
#else
        float u = in.init.verticalU;
        float v = in.init.verticalV;
#endif
        for (int16_t x = xMin; x <= xMax; x++) {
#if ENABLE_FIXED_POINT
            int16_t intU = FO_TO_INTEGER(u);
            int16_t intV = FO_TO_INTEGER(v);
            if ((u >= 0) && (intU < (in.info.header.width - 1)) && (v >= 0) && (intV < (in.info.header.height - 1))) {
#else
            const int16_t intU = static_cast<int16_t>(u);
            const int16_t intV = static_cast<int16_t>(v);
            if ((u >= 0) && (intU < in.info.header.width - 1) && (v >= 0) && (intV < in.info.header.height - 1)) {
#endif
#if ENABLE_ARM_MATH
                uint32_t val1 = __SMUAD(intV, in.srcLineWidth);
                uint32_t val2 = __SMUAD(intU, in.pixelSize);
                uint32_t px1 = val1 + val2;
#else
                uint32_t px1 = intV * in.srcLineWidth + intU * in.pixelSize;
#endif
                uint8_t* imgHead = const_cast<uint8_t*>(in.info.data);
                OpacityType opa = in.opaScale;

                switch (in.info.header.colorMode) {
                    case RGB888: {
                        Color24 p24 = *(reinterpret_cast<Color24*>(&imgHead[px1]));
                        if (opa == OPA_OPAQUE) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p24.red, p24.green, p24.blue, RGB888);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p24, RGB888, opa);
                        }
                        break;
                    }
                    case RGB565: {
                        Color16 p16 = *(reinterpret_cast<Color16*>(&imgHead[px1]));
                        if (opa == OPA_OPAQUE) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p16.red, p16.green, p16.blue, RGB565);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p16, RGB565, opa);
                        }
                        break;
                    }
                    case ARGB8888: {
                        Color32 p32 = *(reinterpret_cast<Color32*>(&imgHead[px1]));
                        if ((in.opaScale == OPA_OPAQUE) && (p32.alpha == OPA_OPAQUE)) {
                            COLOR_FILL_COVER(screenBuffer, bufferMode, p32.red, p32.green, p32.blue, ARGB8888);
                        } else {
                            COLOR_FILL_BLEND(screenBuffer, bufferMode, &p32, ARGB8888, in.opaScale);
                        }
                        break;
                    }
                    default:
                        return;
                }
            }
            u += in.init.duHorizon;
            v += in.init.dvHorizon;
            screenBuffer += in.bufferPxSize;
        }
        StepToNextLine(in.edge1, in.edge2);
        in.init.verticalU += in.init.duVertical;
        in.init.verticalV += in.init.dvVertical;
#if ENABLE_FIXED_POINT
        int16_t deltaX = FO_TO_INTEGER(in.edge1.curX) - xMin;
#else
        int16_t deltaX = static_cast<int16_t>(in.edge1.curX) - xMin;
#endif
        in.init.verticalU += in.init.duHorizon * deltaX;
        in.init.verticalV += in.init.dvHorizon * deltaX;
    }
}

void DrawUtils::DrawTriangleTransformPart(BufferInfo& gfxDstBuffer, const TrianglePartInfo& part)
{
#if ENABLE_FIXED_POINT
    // parameters below are Q15 fixed-point number
    int64_t yMin = FO_TRANS_INTEGER_TO_FIXED(part.yMin);
    part.edge1.curX += (static_cast<int64_t>(part.edge1.du) * (yMin - part.edge1.curY) / part.edge1.dv);
    part.edge1.curY = yMin;
    part.edge2.curX += (static_cast<int64_t>(part.edge2.du) * (yMin - part.edge2.curY) / part.edge2.dv);
    part.edge2.curY = yMin;
    Rect line;
    line.SetLeft(FO_TO_INTEGER(part.edge1.curX));
    line.SetRight(FO_TO_INTEGER(part.edge1.curX));
    line.SetTop(FO_TO_INTEGER(part.edge1.curY));
    line.SetBottom(FO_TO_INTEGER(part.edge1.curY));
    // parameters above are Q15 fixed-point number
#else
    part.edge1.curX += part.edge1.du * (part.yMin - part.edge1.curY) / part.edge1.dv;
    part.edge1.curY = part.yMin;
    part.edge2.curX += part.edge2.du * (part.yMin - part.edge2.curY) / part.edge2.dv;
    part.edge2.curY = part.yMin;
    Rect line;
    line.SetLeft(static_cast<int16_t>(part.edge1.curX));
    line.SetRight(static_cast<int16_t>(part.edge1.curX));
    line.SetTop(static_cast<int16_t>(part.edge1.curY));
    line.SetBottom(static_cast<int16_t>(part.edge1.curY));
#endif
    TransformInitState init;
    GetTransformInitState(part.transMap, part.position, line, init);

    uint8_t* screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
    if (screenBuffer == nullptr) {
        return;
    }
    ColorMode bufferMode = gfxDstBuffer.mode;
    uint8_t bufferPxSize = GetByteSizeByColorMode(bufferMode);

    uint8_t pixelSize;
    DrawTriangleTransformFuc fuc;
    bool isTrueColor = (part.info.header.colorMode == ARGB8888) || (part.info.header.colorMode == RGB888) ||
                       (part.info.header.colorMode == RGB565);
    if (isTrueColor) {
        pixelSize = part.info.pxSize >> SHIFT_3;
        if (part.info.algorithm == TransformAlgorithm::NEAREST_NEIGHBOR) {
            fuc = DrawTriangleTrueColorNearest;
        } else if (part.info.header.colorMode == ARGB8888) {
            if (part.transMap.Is3DTransform()) {
                fuc = Draw3DTriangleTrueColorBilinear8888;
            } else {
                fuc = DrawTriangleTrueColorBilinear8888;
            }
        } else if (part.info.header.colorMode == RGB888) {
            fuc = DrawTriangleTrueColorBilinear888;
        } else {
            fuc = DrawTriangleTrueColorBilinear565;
        }
    } else {
        pixelSize = part.info.pxSize;
        fuc = DrawTriangleAlphaBilinear;
    }
    const int32_t srcLineWidth = part.info.header.width * pixelSize;
    TriangleScanInfo input{part.yMin,
                           part.yMax,
                           part.edge1,
                           part.edge2,
                           screenBuffer,
                           bufferPxSize,
                           part.color,
                           part.opaScale,
                           init,
                           gfxDstBuffer.width,
                           pixelSize,
                           srcLineWidth,
                           part.info,
                           part.mask,
                           part.isRightPart,
                           part.ignoreJunctionPoint,
                           part.transMap.invMatrix_};
    fuc(input, gfxDstBuffer.mode);
}

void DrawUtils::DrawTriangleTransform(BufferInfo& gfxDstBuffer,
                                      const Rect& mask,
                                      const Point& position,
                                      const ColorType& color,
                                      OpacityType opaScale,
                                      const TransformMap& transMap,
                                      const TriangleTransformDataInfo& triangleInfo)
{
    bool p3IsInRight = ((triangleInfo.p1.y - triangleInfo.p2.y) * triangleInfo.p3.x +
                        (triangleInfo.p2.x - triangleInfo.p1.x) * triangleInfo.p3.y +
                        triangleInfo.p1.x * triangleInfo.p2.y - triangleInfo.p2.x * triangleInfo.p1.y) < 0;
    TriangleEdge edge1;
    TriangleEdge edge2;
    TrianglePartInfo part{
        mask,
        transMap,
        position,
        edge1,
        edge2,
        0,
        0,
        triangleInfo.info,
        color,
        opaScale,
        triangleInfo.isRightPart,
        triangleInfo.ignoreJunctionPoint,
    };

    uint8_t yErr = 1;
    if (triangleInfo.p2.y == triangleInfo.p1.y) {
        yErr = 0;
        goto BottomHalf;
    }
    if (p3IsInRight) {
        edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p2.x, triangleInfo.p2.y);
        edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
    } else {
        edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p2.x, triangleInfo.p2.y);
        edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
    }

    part.yMin = MATH_MAX(mask.GetTop(), triangleInfo.p1.y);
    part.yMax = MATH_MIN(mask.GetBottom(), triangleInfo.p2.y);
    part.edge1 = edge1;
    part.edge2 = edge2;
    DrawTriangleTransformPart(gfxDstBuffer, part);
BottomHalf:
    if (triangleInfo.p2.y == triangleInfo.p3.y) {
        return;
    }

    if (triangleInfo.p2.y == triangleInfo.p1.y) {
        if (triangleInfo.p1.x < triangleInfo.p2.x) {
            edge1 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
            edge2 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        } else {
            edge2 = TriangleEdge(triangleInfo.p1.x, triangleInfo.p1.y, triangleInfo.p3.x, triangleInfo.p3.y);
            edge1 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        }
    } else {
        if (p3IsInRight) {
            edge1 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        } else {
            edge2 = TriangleEdge(triangleInfo.p2.x, triangleInfo.p2.y, triangleInfo.p3.x, triangleInfo.p3.y);
        }
    }

    part.yMin = MATH_MAX(mask.GetTop(), triangleInfo.p2.y + yErr);
    part.yMax = MATH_MIN(mask.GetBottom(), triangleInfo.p3.y);
    part.edge1 = edge1;
    part.edge2 = edge2;
    DrawTriangleTransformPart(gfxDstBuffer, part);
}

void DrawUtils::AddBorderToImageData(TransformDataInfo& newDataInfo)
{
    int16_t border = 1;          // 1 : border width
    int16_t offset = border * 2; // 2 : offset
    uint16_t width = newDataInfo.header.width;
    uint16_t height = newDataInfo.header.height;
    int16_t diff = 0;
    if (newDataInfo.pxSize > FONT_WEIGHT_8) {
        width += offset;
        height += offset;
        diff = border * newDataInfo.pxSize / FONT_WEIGHT_8;
    } else {
        width += offset * FONT_WEIGHT_8 / newDataInfo.pxSize;
        height += offset;
        diff = border;
    }
    uint16_t widthInByte = width * newDataInfo.pxSize / FONT_WEIGHT_8;
    if ((width * newDataInfo.pxSize) % FONT_WEIGHT_8 != 0) {
        widthInByte++;
    }
    uint8_t* newData = static_cast<uint8_t*>(UIMalloc(widthInByte * height));
    if (newData == nullptr) {
        return;
    }
    if (memset_s(newData, widthInByte * height, 0, widthInByte * height) != EOK) {
        UIFree(static_cast<void*>(newData));
        newData = nullptr;
        return;
    }
    uint8_t* tmp = newData;
    uint8_t* data = const_cast<uint8_t*>(newDataInfo.data);
    tmp += widthInByte * border + diff;
    for (int i = 0; i < newDataInfo.header.height; ++i) {
        // 2 : double
        if (memcpy_s(tmp, widthInByte - diff * 2, data, widthInByte - diff * 2) != EOK) {
            UIFree(static_cast<void*>(newData));
            newData = nullptr;
            return;
        }
        tmp += widthInByte;
        data += widthInByte - diff * 2; // 2 : double
    }
    newDataInfo.header.width = width;
    newDataInfo.header.height = height;
    newDataInfo.data = newData;
}

void DrawUtils::UpdateTransMap(int16_t width, int16_t height, TransformMap& transMap)
{
    Rect rect = transMap.GetTransMapRect();
    Matrix4<float> matrix = transMap.GetTransformMatrix();
    matrix = matrix * (Matrix4<float>::Translate(Vector3<float>(-rect.GetX(), -rect.GetY(), 0)));
    int16_t offsetX = (width - rect.GetWidth()) / 2;   //  2 : half;
    int16_t offsetY = (height - rect.GetHeight()) / 2; //  2 : half;
    rect.SetPosition(rect.GetX() - offsetX, rect.GetY() - offsetY);
    rect.Resize(width, height);
    Polygon polygon = Polygon(rect);
    uint8_t vertexNum = transMap.GetPolygon().GetVertexNum();
    Vector4<float> imgPoint4;
    for (uint8_t i = 0; i < vertexNum; i++) {
        Vector4<float> point(polygon[i].x_, polygon[i].y_, 0, 1);
        imgPoint4 = matrix * point;
        if (imgPoint4.x_ < COORD_MIN) {
            polygon[i].x_ = COORD_MIN;
        } else if (imgPoint4.x_ > COORD_MAX) {
            polygon[i].x_ = COORD_MAX;
        } else {
            polygon[i].x_ = MATH_ROUND(imgPoint4.x_);
        }

        if (imgPoint4.y_ < COORD_MIN) {
            polygon[i].y_ = COORD_MIN;
        } else if (imgPoint4.y_ > COORD_MAX) {
            polygon[i].y_ = COORD_MAX;
        } else {
            polygon[i].y_ = MATH_ROUND(imgPoint4.y_);
        }
    }
    transMap.SetPolygon(polygon);
    Matrix3<float> matrix3(matrix[0][0], matrix[0][1], matrix[0][3], matrix[1][0], matrix[1][1], matrix[1][3],
                           matrix[3][0], matrix[3][1], matrix[3][3]);
    transMap.invMatrix_ = (matrix3 * (Matrix3<float>::Translate(Vector2<float>(rect.GetX(), rect.GetY())))).Inverse();
}

void DrawUtils::DrawTransform(BufferInfo& gfxDstBuffer,
                              const Rect& mask,
                              const Point& position,
                              const ColorType& color,
                              OpacityType opaScale,
                              const TransformMap& transMap,
                              const TransformDataInfo& dataInfo) const
{
    if (opaScale == OPA_TRANSPARENT) {
        return;
    }
    if ((gfxDstBuffer.virAddr == nullptr) || (dataInfo.data == nullptr)) {
        return;
    }
    TransformDataInfo newDataInfo = dataInfo;
    TransformMap newTransMap = transMap;
    // If the width and height of the rectangle of transMap are not equal to the width and height of the ImageHeader,
    // a border of transparency values to the data cannot be added.
    if ((transMap.GetTransMapRect().GetWidth() == dataInfo.header.width) &&
        (transMap.GetTransMapRect().GetHeight() == dataInfo.header.height)) {
        // Add a border of transparency values to the data
        AddBorderToImageData(newDataInfo);
        // Update the transMap according to new rect width and height
        UpdateTransMap(newDataInfo.header.width, newDataInfo.header.height, newTransMap);
    }

    Rect trans = newTransMap.GetBoxRect();
    trans.SetX(trans.GetX() + position.x);
    trans.SetY(trans.GetY() + position.y);
    if (!trans.Intersect(trans, mask)) {
        if (newDataInfo.data != dataInfo.data) {
            UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(newDataInfo.data)));
        }
        return;
    }

    TriangleTransformDataInfo triangleInfo{
        newDataInfo,
    };
    Polygon polygon = newTransMap.GetPolygon();
    Point p1;
    p1.x = polygon[0].x_ + position.x; // 0:first point
    p1.y = polygon[0].y_ + position.y; // 0:first point
    Point p2;
    p2.x = polygon[1].x_ + position.x; // 1:second point
    p2.y = polygon[1].y_ + position.y; // 1:second point
    Point p3;
    p3.x = polygon[2].x_ + position.x; // 2:third point
    p3.y = polygon[2].y_ + position.y; // 2:third point
    triangleInfo.isRightPart = ((p1.y - p3.y) * p2.x + (p3.x - p1.x) * p2.y + p1.x * p3.y - p3.x * p1.y) < 0;
    triangleInfo.isRightPart = (p1.y < p3.y) ? triangleInfo.isRightPart : !triangleInfo.isRightPart;
    DrawTriangle::SortVertexs(p1, p2, p3);
    triangleInfo.ignoreJunctionPoint = false;
    triangleInfo.p1 = p1;
    triangleInfo.p2 = p2;
    triangleInfo.p3 = p3;
    if ((triangleInfo.p1.y <= mask.GetBottom()) && (triangleInfo.p3.y >= mask.GetTop())) {
        DrawTriangleTransform(gfxDstBuffer, mask, position, color, opaScale, newTransMap, triangleInfo);
    }

    triangleInfo.ignoreJunctionPoint = true;
    triangleInfo.isRightPart = !triangleInfo.isRightPart;
    p1.x = polygon[0].x_ + position.x; // 0:first point
    p1.y = polygon[0].y_ + position.y; // 0:first point
    p3.x = polygon[2].x_ + position.x; // 2:third point
    p3.y = polygon[2].y_ + position.y; // 2:third point
    Point p4;
    p4.x = polygon[3].x_ + position.x; // 3:fourth point
    p4.y = polygon[3].y_ + position.y; // 3:fourth point
    DrawTriangle::SortVertexs(p1, p3, p4);
    triangleInfo.p1 = p1;
    triangleInfo.p2 = p3;
    triangleInfo.p3 = p4;
    if ((triangleInfo.p1.y <= mask.GetBottom()) && (triangleInfo.p3.y >= mask.GetTop())) {
        DrawTriangleTransform(gfxDstBuffer, mask, position, color, opaScale, newTransMap, triangleInfo);
    }
    if (newDataInfo.data != dataInfo.data) {
        UIFree(reinterpret_cast<void*>(const_cast<uint8_t*>(newDataInfo.data)));
    }
}

OpacityType DrawUtils::GetPxAlphaForAlphaImg(const TransformDataInfo& dataInfo, const Point& point)
{
    Point tmpPoint = point;
    const uint8_t* bufU8 = const_cast<uint8_t*>(dataInfo.data);
#if ENABLE_SPEC_FONT
    if (dataInfo.header.colorMode == A1) {
        uint8_t bit = tmpPoint.x & 0x7; // 0x7: 1 byte is 8 bit,
        tmpPoint.x = tmpPoint.x >> SHIFT_3;

        uint32_t px = (dataInfo.header.width >> SHIFT_3) * tmpPoint.y + tmpPoint.x;
        // 1: A1 means 1 bit, 7: maximum offset in bytes
        uint8_t pxOpa = (bufU8[px] & (1 << (7 - bit))) >> (7 - bit);
        return pxOpa ? OPA_TRANSPARENT : OPA_OPAQUE;
    } else if (dataInfo.header.colorMode == A2) {
        uint8_t bit = (tmpPoint.x & 0x3) * 2; // 0x3: 0b0011, 2: A2 color mode
        tmpPoint.x = tmpPoint.x >> SHIFT_2;

        uint32_t px = (dataInfo.header.width >> SHIFT_2) * tmpPoint.y + tmpPoint.x;
        // 3: the value of 0b0011
        uint8_t pxOpa = (bufU8[px] & (3 << (SHIFT_6 - bit))) >> (SHIFT_6 - bit);
        return pxOpa * OPACITY_STEP_A2;
    } else if (dataInfo.header.colorMode == A8) {
        uint32_t px = dataInfo.header.width * tmpPoint.y + tmpPoint.x;
        return bufU8[px];
    }
#else
    uint8_t letterWidthInByte = (dataInfo.header.width * dataInfo.pxSize) >> SHIFT_3;
    // 0x7: for rounding
    if ((dataInfo.header.width * dataInfo.pxSize) & 0x7) {
        letterWidthInByte++;
    }
    uint8_t bit = (tmpPoint.x & 0x1) << SHIFT_2;
    bufU8 += (tmpPoint.y * letterWidthInByte) + ((tmpPoint.x * dataInfo.pxSize) >> SHIFT_3);
    // 0xF: 0b1111, get the data of the A4 color mode
    uint8_t pxOpa = (*bufU8 & (0xF << bit)) >> (bit);
    return pxOpa * OPACITY_STEP_A4;
#endif // ENABLE_SPEC_FONT
}

void DrawUtils::DrawTranspantArea(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask)
{
    FillArea(gfxDstBuffer, rect, mask, true, nullptr);
}

void DrawUtils::DrawWithBuffer(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask, const ColorType* colorBuf)
{
    FillArea(gfxDstBuffer, rect, mask, false, colorBuf);
}

void DrawUtils::FillArea(BufferInfo& gfxDstBuffer,
                         const Rect& rect,
                         const Rect& mask,
                         bool isTransparent,
                         const ColorType* colorBuf)
{
    Rect maskedArea;
    if (!maskedArea.Intersect(rect, mask)) {
        return;
    }

    int16_t left = maskedArea.GetLeft();
    int16_t right = maskedArea.GetRight();
    int16_t top = maskedArea.GetTop();
    int16_t bottom = maskedArea.GetBottom();

    DRAW_UTILS_PREPROCESS(gfxDstBuffer, OPA_OPAQUE);
    uint8_t* mem = screenBuffer;
    mem += top * screenBufferWidth * bufferPxSize;
    if (isTransparent) {
        uint16_t sz = (right - left + 1) * bufferPxSize;
        for (int16_t row = top; row <= bottom; row++) {
            if (memset_s(mem + (left * bufferPxSize), sz, 0, sz) != EOK) {
                return;
            }
            mem += screenBufferWidth * bufferPxSize;
        }
    } else {
        if (colorBuf == nullptr) {
            return;
        }
        for (int16_t row = top; row <= bottom; row++) {
            for (int16_t col = left; col <= right; col++) {
#if COLOR_DEPTH == 32
                COLOR_FILL_COVER(mem[col * bufferPxSize], bufferMode, colorBuf[row * screenBufferWidth + col].red,
                                 colorBuf[row * screenBufferWidth + col].green,
                                 colorBuf[row * screenBufferWidth + col].blue, ARGB8888);
#elif COLOR_DEPTH == 24
                COLOR_FILL_COVER(mem[col * bufferPxSize], bufferMode, colorBuf[row * screenBufferWidth + col].red,
                                 colorBuf[row * screenBufferWidth + col].green,
                                 colorBuf[row * screenBufferWidth + col].blue, RGB888);
#else
                COLOR_FILL_COVER(mem[col * bufferPxSize], bufferMode, colorBuf[row * screenBufferWidth + col].red,
                                 colorBuf[row * screenBufferWidth + col].green,
                                 colorBuf[row * screenBufferWidth + col].blue, RGB565);
#endif
            }
            mem += screenBufferWidth * bufferPxSize;
        }
    }
}

void DrawUtils::DrawAdjPixelInLine(BufferInfo& gfxDstBuffer,
                                   int16_t x1,
                                   int16_t y1,
                                   int16_t x2,
                                   int16_t y2,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);
    Color32 result;
    result.full = Color::ColorTo32(color);
    if ((x1 >= mask.GetLeft()) && (x1 <= mask.GetRight()) && (y1 >= mask.GetTop()) && (y1 <= mask.GetBottom())) {
        screenBuffer += (y1 * screenBufferWidth + x1) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((x2 >= mask.GetLeft()) && (x2 <= mask.GetRight()) && (y2 >= mask.GetTop()) && (y2 <= mask.GetBottom())) {
        screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
        screenBuffer += (y2 * screenBufferWidth + x2) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawPixelInLine(BufferInfo& gfxDstBuffer,
                                int16_t x,
                                int16_t y,
                                const Rect& mask,
                                const ColorType& color,
                                OpacityType opa,
                                uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);
    Color32 result;
    result.full = Color::ColorTo32(color);
    if ((x >= mask.GetLeft()) && (x <= mask.GetRight()) && (y >= mask.GetTop()) && (y <= mask.GetBottom())) {
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawVerPixelInLine(BufferInfo& gfxDstBuffer,
                                   int16_t x,
                                   int16_t y,
                                   int8_t dir,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);
    if ((y < mask.GetTop()) || (y > mask.GetBottom())) {
        return;
    }
    Color32 result;
    result.full = Color::ColorTo32(color);
    int16_t x0 = x + dir;
    int16_t x1 = x - dir;
    if ((x0 >= mask.GetLeft()) && (x0 <= mask.GetRight())) {
        screenBuffer += (y * screenBufferWidth + x0) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((x >= mask.GetLeft()) && (x <= mask.GetRight())) {
        screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        if (opa == OPA_OPAQUE) {
            COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
        } else {
            COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, opa);
        }
    }
    if ((x1 >= mask.GetLeft()) && (x1 <= mask.GetRight())) {
        screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
        screenBuffer += (y * screenBufferWidth + x1) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}

void DrawUtils::DrawHorPixelInLine(BufferInfo& gfxDstBuffer,
                                   int16_t x,
                                   int16_t y,
                                   int8_t dir,
                                   const Rect& mask,
                                   const ColorType& color,
                                   OpacityType opa,
                                   uint16_t weight) const
{
    DRAW_UTILS_PREPROCESS(gfxDstBuffer, opa);
    if ((x < mask.GetLeft()) || (x > mask.GetRight())) {
        return;
    }
    Color32 result;
    result.full = Color::ColorTo32(color);
    int16_t y0 = y + dir;
    int16_t y1 = y - dir;
    if ((y0 >= mask.GetTop()) && (y0 <= mask.GetBottom())) {
        screenBuffer += (y0 * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = weight * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
    if ((y >= mask.GetTop()) && (y <= mask.GetBottom())) {
        screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
        screenBuffer += (y * screenBufferWidth + x) * bufferPxSize;
        if (opa == OPA_OPAQUE) {
            COLOR_FILL_COVER(screenBuffer, bufferMode, result.red, result.green, result.blue, ARGB8888);
        } else {
            COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, opa);
        }
    }
    if ((y1 >= mask.GetTop()) && (y1 <= mask.GetBottom())) {
        screenBuffer = static_cast<uint8_t*>(gfxDstBuffer.virAddr);
        screenBuffer += (y1 * screenBufferWidth + x) * bufferPxSize;
        OpacityType fillOpa = (weight ^ OPA_OPAQUE) * opa / OPA_OPAQUE;
        COLOR_FILL_BLEND(screenBuffer, bufferMode, &result, ARGB8888, fillOpa);
    }
}
} // namespace OHOS
