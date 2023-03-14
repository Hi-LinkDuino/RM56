/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/shape/flutter_render_shape.h"

#include "include/core/SkPaint.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/utils/SkParsePath.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

Size FlutterRenderShape::CalcSize()
{
    switch (shapeType_) {
        case ShapeType::RECT:
            return CreateRect();
        case ShapeType::CIRCLE:
            return CreateCircle();
        case ShapeType::ELLIPSE:
            return CreateEllipse();
        case ShapeType::LINE:
            path_.reset();
            path_.moveTo(NormalizePercentToPx(start_.first, false), NormalizePercentToPx(start_.second, true));
            path_.lineTo(NormalizePercentToPx(end_.first, false), NormalizePercentToPx(end_.second, true));
            break;
        case ShapeType::POLYLINE:
            return CreatePolygon(false);
        case ShapeType::POLYGON:
            return CreatePolygon(true);
        case ShapeType::PATH:
            return CreatePath();
        default:
            LOGE("invalid shapeType");
            return Size();
    }
    auto skRect =  path_.getBounds();
    if (width_.IsValid() && height_.IsValid()) {
        return Size(NormalizePercentToPx(width_, false), NormalizePercentToPx(height_, true));
    }
    return Size(skRect.right(), skRect.bottom());
}

Size FlutterRenderShape::CreateRect()
{
    if (!GetLayoutParam().GetMaxSize().IsValid()) {
        return Size();
    }
    SkRect rect = SkRect::MakeLTRB(0.0f, 0.0f, NormalizePercentToPx(width_, false),
        NormalizePercentToPx(height_, true));
    float topLeftRadiusX = GetFloatRadiusValue(topLeftRadius_.GetX(), topLeftRadius_.GetY(), false);
    float topLeftRadiusY = GetFloatRadiusValue(topLeftRadius_.GetY(), topLeftRadius_.GetX(), true);
    float topRightRadiusX = GetFloatRadiusValue(topRightRadius_.GetX(), topRightRadius_.GetY(), false);
    float topRightRadiusY = GetFloatRadiusValue(topRightRadius_.GetY(), topRightRadius_.GetX(), true);
    float bottomRightRadiusX = GetFloatRadiusValue(bottomRightRadius_.GetX(), bottomRightRadius_.GetY(), false);
    float bottomRightRadiusY = GetFloatRadiusValue(bottomRightRadius_.GetY(), bottomRightRadius_.GetX(), true);
    float bottomLeftRadiusX = GetFloatRadiusValue(bottomLeftRadius_.GetX(), bottomLeftRadius_.GetY(), false);
    float bottomLeftRadiusY = GetFloatRadiusValue(bottomLeftRadius_.GetY(), bottomLeftRadius_.GetX(), true);
    const SkVector fRadii[4] = {
        { topLeftRadiusX, topLeftRadiusY },
        { topRightRadiusX, topRightRadiusY },
        { bottomRightRadiusX, bottomRightRadiusY },
        { bottomLeftRadiusX, bottomLeftRadiusY }
    };
    path_.reset();
    SkRRect roundRect;
    roundRect.setRectRadii(rect, fRadii);
    path_.addRRect(roundRect);
    auto skRect =  path_.getBounds();
    return Size(skRect.right(), skRect.bottom());
}

float FlutterRenderShape::GetFloatRadiusValue(const Dimension& src, const Dimension& dest, bool isVertical)
{
    if (src.Value() < 0.0 && dest.Value() > 0.0) {
        return static_cast<float>(NormalizePercentToPx(dest, isVertical));
    }
    return static_cast<float>(NormalizePercentToPx(src, isVertical));
}

Size FlutterRenderShape::CreateCircle()
{
    if (!GetLayoutParam().GetMaxSize().IsValid()) {
        return Size();
    }
    path_.reset();
    double width = NormalizePercentToPx(width_, false);
    double height = NormalizePercentToPx(height_, true);
    path_.addCircle(width * 0.5, height * 0.5, std::min(width, height) * 0.5);
    return Size(width, height);
}

Size FlutterRenderShape::CreateEllipse()
{
    if (!GetLayoutParam().GetMaxSize().IsValid()) {
        return Size();
    }
    path_.reset();
    auto width = NormalizePercentToPx(width_, false);
    auto height = NormalizePercentToPx(height_, true);
    SkRect rect = SkRect::MakeXYWH(0.0f, 0.0f, width, height);
    path_.addOval(rect);
    return Size(width, height);
}

Size FlutterRenderShape::CreatePolygon(bool needClose)
{
    path_.reset();
    std::vector<SkPoint> skPoints;
    for (auto point : points_) {
        skPoints.emplace_back(
            SkPoint::Make(NormalizePercentToPx(point.first, false), NormalizePercentToPx(point.second, true)));
    }
    if (skPoints.empty()) {
        return Size();
    }
    path_.addPoly(&skPoints[0], skPoints.size(), needClose);
    auto skRect =  path_.getBounds();
    if (width_.IsValid() && height_.IsValid()) {
        return Size(NormalizePercentToPx(width_, false), NormalizePercentToPx(height_, true));
    }
    return Size(skRect.right(), skRect.bottom());
}

Size FlutterRenderShape::CreatePath()
{
    path_.reset();
    if (pathCmd_.GetValue().empty()) {
        return Size();
    }
    bool ret = SkParsePath::FromSVGString(pathCmd_.GetValue().c_str(), &path_);
    if (width_.IsValid() && height_.IsValid()) {
        return Size(NormalizePercentToPx(width_, false), NormalizePercentToPx(height_, true));
    }
    if (!ret) {
        LOGW("path value is invalid");
        return Size();
    }
    auto skRect =  path_.getBounds();
    return Size(skRect.right(), skRect.bottom());
}

void FlutterRenderShape::Paint(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return;
    }
    PaintOnCanvas(skCanvas, offset);
}

void FlutterRenderShape::PaintOnCanvas(SkCanvas* skCanvas, const Offset& offset)
{
    SkPath path;
    path_.offset(offset.GetX(), offset.GetY(), &path);
    FlutterSvgPainter::SetFillStyle(skCanvas, path, fillState_, UINT8_MAX, antiAlias_.second);
    DrawStroke(skCanvas, path);
}

void FlutterRenderShape::DrawStroke(SkCanvas* skCanvas, const SkPath& path)
{
    if (strokeState_.GetColor() != Color::TRANSPARENT && GreatNotEqual(strokeState_.GetLineWidth().Value(), 0.0)) {
        SkPaint strokePaint;
        FlutterSvgPainter::SetStrokeStyle(strokePaint, strokeState_, UINT8_MAX, antiAlias_.second);
        strokePaint.setStrokeWidth(NormalizePercentToPx(strokeState_.GetLineWidth(), false));
        if (!strokeState_.GetStrokeDashArray().empty()) {
            auto lineDashState = strokeState_.GetStrokeDashArray();
            SkScalar intervals[lineDashState.size()];
            for (size_t i = 0; i < lineDashState.size(); ++i) {
                intervals[i] = SkDoubleToScalar(NormalizePercentToPx(lineDashState[i], false));
            }
            SkScalar phase = SkDoubleToScalar(NormalizePercentToPx(strokeState_.GetStrokeDashOffset(), false));
            strokePaint.setPathEffect(SkDashPathEffect::Make(intervals, lineDashState.size(), phase));
        }
        skCanvas->drawPath(path, strokePaint);
    }
}

} // namespace OHOS::Ace