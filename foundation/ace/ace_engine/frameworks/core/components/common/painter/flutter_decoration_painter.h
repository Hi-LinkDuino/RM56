/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_DECORATION_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_DECORATION_PAINTER_H

#include "flutter/lib/ui/ui_dart_state.h"

#include <math.h>

#include "base/memory/ace_type.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/border_image_edge.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/edge.h"
#include "core/components/image/render_image.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/layers/clip_layer.h"

namespace flutter {
class Canvas;
class Paint;
class PaintData;
} // namespace flutter

namespace OHOS::Ace {

class Border;
class Offset;
class Size;

class FlutterDecorationPainter : public virtual AceType {
    DECLARE_ACE_TYPE(FlutterDecorationPainter, AceType);

public:
    FlutterDecorationPainter(
        const RefPtr<Decoration>& decoration, const Rect& paintRect, const Size& paintSize, double dipScale);
    ~FlutterDecorationPainter() override = default;

    static void PaintShadow(const SkPath& path, const Shadow& shadow, SkCanvas* canvas);

    static float ConvertRadiusToSigma(float radius);

    void PaintDecoration(const Offset& offset, SkCanvas* canvas, RenderContext& context, const sk_sp<SkImage>& image);

    void PaintDecoration(const Offset& offset, SkCanvas* canvas, RenderContext& context);

    void PaintBoxShadows(const SkRRect& rrect, const std::vector<Shadow>& shadows, SkCanvas* canvas);

    void PaintGrayScale(const flutter::RRect& outerRRect, SkCanvas* canvas,
                        const Dimension& grayscale, const Color& color);

    void PaintBrightness(const flutter::RRect& outerRRect, SkCanvas* canvas,
                         const Dimension& brightness, const Color& color);

    void PaintContrast(const flutter::RRect& outerRRect, SkCanvas* canvas,
                       const Dimension& contrast, const Color& color);

    void PaintSaturate(const flutter::RRect& outerRRect, SkCanvas* canvas,
                       const Dimension& saturate, const Color& color);

    void PaintSepia(const flutter::RRect& outerRRect, SkCanvas* canvas,
                    const Dimension& sepia, const Color& color);

    void PaintInvert(const flutter::RRect& outerRRect, SkCanvas* canvas,
                     const Dimension& invert, const Color& color);

    void PaintHueRotate(const flutter::RRect& outerRRect, SkCanvas* canvas, const float& hueRotate, const Color& color);

    void PaintBlur(const flutter::RRect& outerRRect, SkCanvas* canvas, const Dimension& blurRadius, const Color& color);

    void PaintColorBlend(const flutter::RRect& outerRRect, SkCanvas* canvas, const Color& colorBlend,
                         const Color& color);

    void PaintGradient(const Offset& offset, SkCanvas* canvas, SkPaint& paint);

    bool GetGradientPaint(SkPaint& paint);

    flutter::RRect GetBoxOuterRRect(const Offset& offset);

    void SetAlpha(uint8_t opacity)
    {
        opacity_ = opacity;
    }

    void SetMargin(const EdgePx& margin)
    {
        margin_ = margin;
    }

    void SetRenderImage(const RefPtr<RenderImage>& renderImage)
    {
        renderImage_ = renderImage;
    }

    void SetClipLayer(const RefPtr<Flutter::ClipLayer>& clipLayer)
    {
        clipLayer_ = clipLayer;
    }

    void SetDecoration(const RefPtr<Decoration>& decoration)
    {
        decoration_ = decoration;
    }

    flutter::RRect GetBoxRRect(const Offset& offset, const Border& border, double shrinkFactor, bool isRound);

protected:
    void PaintColorAndImage(const Offset& offset, SkCanvas* canvas, SkPaint& paint, RenderContext& context);

    void PaintAllEqualBorder(const flutter::RRect& rrect, const Border& border, SkCanvas* canvas, SkPaint& paint);
    void SetBorderStyle(const BorderEdge& borderEdge, SkPaint& paint,
        bool useDefaultColor = false, double spaceBetweenDot = 0.0, double borderLength = 0.0);

    void PaintBorder(const Offset& offset, const Border& border, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderWithLine(const Offset& offset, const Border& border, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderImage(const Offset& offset, const Border& border, SkCanvas* canvas, SkPaint& paint,
        const sk_sp<SkImage>& image);
    void PaintImage(const Offset& offset, RenderContext& context);
    sk_sp<SkShader> CreateGradientShader(const Gradient& gradient, const SkSize& size);
    flutter::RRect GetOuterRRect(const Offset& offset, const Border& border);
    flutter::RRect GetInnerRRect(const Offset& offset, const Border& border);
    flutter::RRect GetClipRRect(const Offset& offset, const Border& border);
    bool CanUseFillStyle(const Border& border, SkPaint& paint);
    bool CanUsePathRRect(const Border& border, SkPaint& paint);
    bool CanUseInnerRRect(const Border& border);
    bool CanUseFourLine(const Border& border);
    void AdjustBorderStyle(Border& border);

    bool CheckBorderEdgeForRRect(const Border& border);

    double NormalizeToPx(const Dimension& dimension) const;
    double SliceNormalizePercentToPx(const Dimension& dimension, bool isVertical) const;
    double WidthNormalizePercentToPx(const Dimension& dimension, bool isVertical) const;
    double OutsetNormalizePercentToPx(const Dimension& dimension, bool isVertical) const;
    void PaintBorderImageFourCorner(const Offset& offset, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderImageFourStretch(const Offset& offset, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderImageFourRound(const Offset& offset, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderImageFourSpace(const Offset& offset, SkCanvas* canvas, SkPaint& paint);
    void PaintBorderImageFourRepeat(const Offset& offset, SkCanvas* canvas, SkPaint& paint);

    Size GetLayoutSize() const
    {
        return paintRect_.GetSize();
    }

    double dipScale_ = 1.0;
    Rect paintRect_;

    RefPtr<Decoration> decoration_;
    Size paintSize_; // exclude margin
    EdgePx margin_;
    double scale_ = 0.0;
    uint8_t opacity_ = UINT8_MAX;

    RefPtr<RenderImage> renderImage_;
    RefPtr<Flutter::ClipLayer> clipLayer_;
    sk_sp<SkImage> image_;

    double leftWidth_ = 0.0;
    double topWidth_ = 0.0;
    double rightWidth_ = 0.0;
    double bottomWidth_ = 0.0;

    double leftSlice_ = 0.0;
    double topSlice_ = 0.0;
    double rightSlice_ = 0.0;
    double bottomSlice_ = 0.0;

    double leftOutset_ = 0.0;
    double topOutset_ = 0.0;
    double rightOutset_ = 0.0;
    double bottomOutset_ = 0.0;

};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_DECORATION_PAINTER_H
