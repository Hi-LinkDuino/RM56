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

#ifndef GRAPHIC_LITE_DRAW_UTILS_H
#define GRAPHIC_LITE_DRAW_UTILS_H

#include "common/text.h"
#include "font/ui_font_header.h"
#include "gfx_utils/color.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"
#include "gfx_utils/graphic_types.h"
#include "gfx_utils/list.h"
#include "gfx_utils/style.h"
#include "gfx_utils/transform.h"

namespace OHOS {
#define SWAP_INT16(x, y)    \
    do {                    \
        int16_t temp = (x); \
        (x) = (y);          \
        (y) = temp;         \
    } while (0)

#define SWAP_POINTS(x1, x2, y1, y2) \
    SWAP_INT16(x1, x2);             \
    SWAP_INT16(y1, y2);

// FixedPointed Related definition.
#define FIXED_NUM_1 1048576
#define FIXED_Q_NUM 20
#define FO_TRANS_FLOAT_TO_FIXED(f) (static_cast<int64_t>((f) * FIXED_NUM_1))
#define FO_TRANS_INTEGER_TO_FIXED(f) ((static_cast<int64_t>(f)) << FIXED_Q_NUM)
#define FO_DIV(n1, n2) ((static_cast<int64_t>(n1) << FIXED_Q_NUM) / (n2))
#define FO_TO_INTEGER(n) ((n) >= 0 ? ((n) >> FIXED_Q_NUM) : (((n) >> FIXED_Q_NUM) + 1))
#define FO_DECIMAL(n) ((n) >= 0 ? ((n) & (FIXED_NUM_1 - 1)) : ((n) | (-FIXED_NUM_1)))
#define FO_MUL(n1, n2) ((static_cast<int64_t>(n1) * (n2)) >> FIXED_Q_NUM)

struct EdgeSides {
    int16_t left;
    int16_t right;
    int16_t top;
    int16_t bottom;
};

struct LabelLineInfo {
    Point& pos;
    Point& offset;
    const Rect& mask;
    int16_t lineHeight;
    uint16_t lineLength;
    uint8_t shapingId;
    uint8_t opaScale;
    const Style& style;

    const char* text;
    uint16_t length;
    uint16_t start;
    uint8_t fontId;
    uint8_t fontSize;
    uint8_t txtFlag;
    UITextLanguageDirect direct;
    uint32_t* codePoints;
    bool baseLine;
#if ENABLE_VECTOR_FONT
    TextStyle* textStyles;
#endif
    List<BackgroundColor>*  backgroundColor;
    List<ForegroundColor>*  foregroundColor;
    List<LineBackgroundColor>*  linebackgroundColor;
    SizeSpan* sizeSpans;
    uint16_t ellipsisOssetY;
};

struct LabelLetterInfo {
    const Point& pos;
    Rect mask;
    const ColorType& color;
    OpacityType opa;
    int8_t offsetX;
    int8_t offsetY;

    const uint32_t& letter;
    UITextLanguageDirect direct;
    uint8_t fontId;
    uint8_t shapingId;
    uint8_t fontSize;
#if ENABLE_VECTOR_FONT
    TextStyle textStyle;
#endif
    bool baseLine;
    int16_t letterSpace_;
    int16_t lineSpace_;
    bool havebackgroundColor;
    ColorType backgroundColor;
};

struct TransformInitState {
#if ENABLE_FIXED_POINT
    // parameters below are Q15 fixed-point number
    int64_t verticalU;
    int64_t verticalV;
    int64_t duHorizon;
    int64_t dvHorizon;
    int64_t duVertical;
    int64_t dvVertical;
    // parameters above are Q15 fixed-point number
#else
    float verticalU;
    float verticalV;
    float duHorizon;
    float dvHorizon;
    float duVertical;
    float dvVertical;
#endif
};

struct TriangleEdge {
    TriangleEdge() {}
    TriangleEdge(int16_t x1, int16_t y1, int16_t duInt, int16_t dvInt);
    ~TriangleEdge();
#if ENABLE_FIXED_POINT
    // parameters below are Q15 fixed-point number
    int64_t curX = 0;
    int64_t curY = 0;
    int64_t du = 0;
    int64_t dv = 0;
    // parameters above are Q15 fixed-point number
#else
    float curX = 0.0f;
    float curY = 0.0f;
    float du = 0.0f;
    float dv = 0.0f;
#endif
};

struct TriangleTransformDataInfo {
    const TransformDataInfo& info;
    Point p1;
    Point p2;
    Point p3;
    bool isRightPart;
    bool ignoreJunctionPoint;
};

struct TriangleScanInfo {
    int16_t yMin;
    int16_t yMax;
    TriangleEdge& edge1;
    TriangleEdge& edge2;
    uint8_t* screenBuffer;
    uint8_t bufferPxSize;
    const ColorType& color;
    const OpacityType opaScale;
    TransformInitState& init;
    uint16_t screenBufferWidth;
    uint8_t pixelSize;
    const int32_t srcLineWidth;
    const TransformDataInfo& info;
    const Rect& mask;
    bool isRightPart;
    bool ignoreJunctionPoint;
    Matrix3<float> matrix;
};

struct TrianglePartInfo {
    const Rect& mask;
    const TransformMap& transMap;
    const Point& position;
    TriangleEdge& edge1;
    TriangleEdge& edge2;
    int16_t yMin;
    int16_t yMax;
    const TransformDataInfo& info;
    const ColorType& color;
    const OpacityType opaScale;
    bool isRightPart;
    bool ignoreJunctionPoint;
};

enum {
    IMG_SRC_VARIABLE,
    IMG_SRC_FILE,
    IMG_SRC_UNKNOWN,
};

class DrawUtils : public HeapBase {
public:
    static DrawUtils* GetInstance();

    void DrawColorArea(BufferInfo& gfxDstBuffer, const Rect& area, const Rect& mask,
                       const ColorType& color, OpacityType opa) const;

    void DrawColorAreaBySides(BufferInfo& gfxDstBuffer, const Rect& mask, const ColorType& color,
                              OpacityType opa, const EdgeSides& sides) const;

    void DrawPixel(BufferInfo& gfxDstBuffer, int16_t x, int16_t y, const Rect& mask,
                   const ColorType& color, OpacityType opa) const;

    void DrawColorLetter(BufferInfo& gfxDstBuffer,
                         const LabelLetterInfo& letterInfo,
                         uint8_t* fontMap,
                         GlyphNode node) const;
    void DrawNormalLetter(BufferInfo& gfxDstBuffer,
                          const LabelLetterInfo& letterInfo,
                          uint8_t* fontMap,
                          GlyphNode node,
                          uint8_t maxLetterSize) const;

    void DrawLetter(BufferInfo& gfxDstBuffer,
                    const uint8_t* fontMap,
                    const Rect& fontRect,
                    const Rect& subRect,
                    const uint8_t fontWeight,
                    const ColorType& color,
                    const OpacityType opa) const;

    void DrawImage(BufferInfo& gfxDstBuffer, const Rect& area, const Rect& mask,
                   const uint8_t* image, OpacityType opa, uint8_t pxBitSize, const ImageHeader &imageHeader, const uint8_t* userData) const;

    void DrawImage(BufferInfo& gfxDstBuffer, const Rect& area, const Rect& mask,
                   const uint8_t* image, OpacityType opa, uint8_t pxBitSize, ColorMode colorMode) const;

    static void
        GetXAxisErrForJunctionLine(bool ignoreJunctionPoint, bool isRightPart, int16_t& xMinErr, int16_t& xMaxErr);

    static void GetTransformInitState(const TransformMap& transMap,
                                      const Point& position,
                                      const Rect& trans,
                                      TransformInitState& init);

    static void DrawTriangleTransform(BufferInfo& gfxDstBuffer,
                                      const Rect& mask,
                                      const Point& position,
                                      const ColorType& color,
                                      OpacityType opaScale,
                                      const TransformMap& transMap,
                                      const TriangleTransformDataInfo& dataInfo);

    void DrawTransform(BufferInfo& gfxDstBuffer,
                       const Rect& mask,
                       const Point& position,
                       const ColorType& color,
                       OpacityType opaScale,
                       const TransformMap& transMap,
                       const TransformDataInfo& dataInfo) const;

    void DrawTranspantArea(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask);

    void DrawWithBuffer(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask, const ColorType* colorBuf);

    static uint8_t GetPxSizeByColorMode(uint8_t colorMode);

    static uint8_t GetByteSizeByColorMode(uint8_t colorMode);

    static OpacityType GetMixOpacity(OpacityType opa1, OpacityType opa2)
    {
        // 8: Shift right 8 bits
        OpacityType opaMix = (opa1 == OPA_OPAQUE) ? opa2 : ((static_cast<uint16_t>(opa1) * opa2) >> 8);
        return opaMix;
    }

    void DrawAdjPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x1,
                            int16_t y1,
                            int16_t x2,
                            int16_t y2,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t w) const;

    void DrawPixelInLine(BufferInfo& gfxDstBuffer, int16_t x, int16_t y, const Rect& mask,
                         const ColorType& color, OpacityType opa, uint16_t w) const;

    void DrawVerPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x,
                            int16_t y,
                            int8_t dir,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t weight) const;

    void DrawHorPixelInLine(BufferInfo& gfxDstBuffer,
                            int16_t x,
                            int16_t y,
                            int8_t dir,
                            const Rect& mask,
                            const ColorType& color,
                            OpacityType opa,
                            uint16_t weight) const;

    void BlendWithSoftWare(const uint8_t* src1,
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
                           uint32_t y) const;

    void FillAreaWithSoftWare(BufferInfo& gfxDstBuffer,
                              const Rect& fillArea,
                              const ColorType& color,
                              const OpacityType& opa) const;
#ifdef ARM_NEON_OPT

    void BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                      uint8_t alpha, uint8_t cover);
    void BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                      uint8_t alpha);
    void BlendLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t srcCover);
    void BlendLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t* srcCovers);
    void BlendLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t* covers);
    void BlendPreLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                         uint8_t alpha, uint8_t cover);
    void BlendPreLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue,
                         uint8_t alpha);
    void BlendPreLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t srcCover);
    void BlendPreLerpPix(uint8_t* dstColors, uint8_t* srcColors, uint8_t* srcCovers);
    void BlendPreLerpPix(uint8_t* color, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t* covers);
#endif
private:
    using DrawTriangleTransformFuc = void (*)(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorNearest(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleAlphaBilinear(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear565(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear888(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void Draw3DTriangleTrueColorBilinear8888(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    static void DrawTriangleTrueColorBilinear8888(const TriangleScanInfo& triangle, const ColorMode bufferMode);

    inline static void StepToNextLine(TriangleEdge& edg1, TriangleEdge& edg2);

    static void DrawTriangleTransformPart(BufferInfo& gfxDstBuffer, const TrianglePartInfo& part);

    static OpacityType GetPxAlphaForAlphaImg(const TransformDataInfo& dataInfo, const Point& point);

    static void AddBorderToImageData(TransformDataInfo& newDataInfo);

    static void UpdateTransMap(int16_t width, int16_t height, TransformMap& transMap);

    void FillArea(BufferInfo& gfxDstBuffer, const Rect& rect, const Rect& mask,
                  bool isTransparent, const ColorType* colorBuf);
};
} // namespace OHOS
#endif // GRAPHIC_LITE_DRAW_UTILS_H
