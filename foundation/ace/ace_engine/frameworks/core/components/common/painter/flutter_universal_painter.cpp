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

#include "core/components/common/painter/flutter_universal_painter.h"

#include "third_party/skia/include/core/SkPaint.h"

namespace OHOS::Ace {

void FlutterUniversalPainter::DrawHoverBackground(
    SkCanvas* canvas, const Rect& paintRect, uint32_t hoverBackgroundColor, double borderRadius)
{
    RRect paintRRect = RRect::MakeRRect(paintRect, borderRadius, borderRadius);
    FlutterUniversalPainter::DrawRRectBackground(canvas, paintRRect, hoverBackgroundColor, 1.0);
}

void FlutterUniversalPainter::DrawRRectBackground(
    SkCanvas* canvas, const RRect& paintRRect, uint32_t backgroundColor, double dipScale)
{
    if (!canvas) {
        LOGE("canvas is null.");
        return;
    }
    SkPaint skPaint;

    skPaint.setAntiAlias(true);
    skPaint.setStyle(SkPaint::Style::kFill_Style);
    skPaint.setColor(backgroundColor);

    auto paintRect = paintRRect.GetRect();
    auto corner = paintRRect.GetCorner();
    SkRect skRect = SkRect::MakeXYWH(paintRect.Left(), paintRect.Top(), paintRect.Width(), paintRect.Height());
    SkVector fRadii[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    fRadii[SkRRect::kUpperLeft_Corner].set(
        SkDoubleToScalar(std::max(FlutterUniversalPainter::NormalizeToPx(corner.topLeftRadius.GetX(), dipScale), 0.0)),
        SkDoubleToScalar(std::max(FlutterUniversalPainter::NormalizeToPx(corner.topLeftRadius.GetY(), dipScale), 0.0)));
    fRadii[SkRRect::kUpperRight_Corner].set(
        SkDoubleToScalar(std::max(FlutterUniversalPainter::NormalizeToPx(corner.topRightRadius.GetX(), dipScale), 0.0)),
        SkDoubleToScalar(
            std::max(FlutterUniversalPainter::NormalizeToPx(corner.topRightRadius.GetY(), dipScale), 0.0)));
    fRadii[SkRRect::kLowerRight_Corner].set(
        SkDoubleToScalar(
            std::max(FlutterUniversalPainter::NormalizeToPx(corner.bottomRightRadius.GetX(), dipScale), 0.0)),
        SkDoubleToScalar(
            std::max(FlutterUniversalPainter::NormalizeToPx(corner.bottomRightRadius.GetY(), dipScale), 0.0)));
    fRadii[SkRRect::kLowerLeft_Corner].set(
        SkDoubleToScalar(
            std::max(FlutterUniversalPainter::NormalizeToPx(corner.bottomLeftRadius.GetX(), dipScale), 0.0)),
        SkDoubleToScalar(
            std::max(FlutterUniversalPainter::NormalizeToPx(corner.bottomLeftRadius.GetY(), dipScale), 0.0)));
    SkRRect skRRect;
    skRRect.setRectRadii(skRect, fRadii);
    canvas->drawRRect(skRRect, skPaint);
}

double FlutterUniversalPainter::NormalizeToPx(const Dimension& dimension, double scale)
{
    if ((dimension.Unit() == DimensionUnit::VP) || (dimension.Unit() == DimensionUnit::FP)) {
        return (dimension.Value() * scale);
    }
    return dimension.Value();
}

} // namespace OHOS::Ace
