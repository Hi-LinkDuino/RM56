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

#include "core/components/chart/flutter_render_chart.h"

#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/lib/ui/ui_dart_state.h"
#include "flutter/third_party/txt/src/txt/paragraph_txt.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/effects/Sk1DPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"

#include "base/utils/string_utils.h"
#include "core/components/calendar/flutter_render_calendar.h"
#include "core/components/font/flutter_font_collection.h"

namespace OHOS::Ace {
namespace {

#ifndef WEARABLE_PRODUCT
constexpr double EDGE_PADDING = 20.0;
constexpr double TEXT_PADDING = 10.0;
#else
constexpr double EDGE_PADDING = 2.0;
constexpr double TEXT_PADDING = 1.0;
#endif
constexpr double TICK_LENGTH = 5.0;
constexpr double RATIO_CONSTANT = 1.732;
constexpr double BARS_INTERVAL_PROPORTION = 0.3;
constexpr double BAR_INTERVAL_PROPORTION = 0.1;
constexpr double DEFAULT_AXISTICK = 10.0;
constexpr double DEFAULT_AXIS_STROKE_WIDTH = 3.0;
constexpr double BEZIER_CONSTANT = 6.0;
constexpr double DOUBLE_TEXT_PADDING = TEXT_PADDING * 2;
constexpr int32_t MIN_SDK_VERSION = 6;

} // namespace

RenderLayer FlutterRenderChart::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
            0, GetLayoutSize().Width(), 0, GetLayoutSize().Height(), Flutter::Clip::NONE);
        lastLayoutSize_ = GetLayoutSize();
    } else if (lastLayoutSize_ != GetLayoutSize()) {
        layer_->SetClip(0, GetLayoutSize().Width(), 0, GetLayoutSize().Height(), Flutter::Clip::NONE);
        lastLayoutSize_ = GetLayoutSize();
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderChart::Update(const RefPtr<Component>& component)
{
    RenderChart::Update(component);
}

Offset FlutterRenderChart::ConvertDataToPosition(const Rect& paintRegion, const PointInfo& point)
{
    double xLength = horizontal_.max - horizontal_.min;
    double yLength = vertical_.max - vertical_.min;
    if (NearEqual(xLength, 0.0) || NearEqual(yLength, 0.0)) {
        return Offset(0, 0);
    }
    return Offset(paintRegion.GetOffset().GetX() + (point.GetX() - horizontal_.min) * paintRegion.Width() / xLength,
        paintRegion.GetOffset().GetY() + paintRegion.Height() -
            (point.GetY() - vertical_.min) * paintRegion.Height() / yLength);
}

void FlutterRenderChart::Paint(RenderContext& context, const Offset& offset)
{
    if (LessOrEqual(vertical_.max, vertical_.min)) {
        vertical_.min = 0.0;
        vertical_.max = 100.0;
    }
    if (LessOrEqual(horizontal_.max, horizontal_.min)) {
        horizontal_.min = 0.0;
        horizontal_.max = 100.0;
    }
    Rect verticalPaintRegion =
        Rect(offset.GetX(), offset.GetY(), GetLayoutSize().Width() * 0.1, GetLayoutSize().Height());
    Rect horizontalPaintRegion =
        Rect(offset.GetX(), offset.GetY() + GetLayoutSize().Height() - GetLayoutSize().Height() * 0.1,
            GetLayoutSize().Width(), GetLayoutSize().Height() * 0.1);
    Rect dataRegion;
    if (!vertical_.display && !horizontal_.display && type_ == ChartType::LINE) {
        dataRegion = Rect(offset.GetX() + EDGE_PADDING, offset.GetY() + DOUBLE_TEXT_PADDING,
            GetLayoutSize().Width() - 2 * EDGE_PADDING, GetLayoutSize().Height() - 2 * DOUBLE_TEXT_PADDING);
    } else {
        PaintVerticalAxis(context, offset, verticalPaintRegion);

        PaintHorizontalAxis(context, horizontalPaintRegion);
        dataRegion = Rect(offset.GetX() + tickHorizontalOffset_ + EDGE_PADDING, offset.GetY() + EDGE_PADDING,
            horizontal_.tickNumber * tickHorizontalOffset_, vertical_.tickNumber * tickOffset_);
    }
    if (!dataRegion.IsValid()) {
        LOGW("chart paint data region is not valid height:%{public}lf, width:%{public}lf. do not paint data",
            dataRegion.Height(), dataRegion.Width());
        return;
    }
    paintWidth_ = dataRegion.GetSize().Width();
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext && pipelineContext->UseLiteStyle() &&
        (pipelineContext->GetMinPlatformVersion() >= MIN_SDK_VERSION) && layer_) {
        layer_->SetClip(
            dataRegion.Left(), dataRegion.Right(), dataRegion.Top(), dataRegion.Bottom(), Flutter::Clip::HARD_EDGE);
    }
    PaintDatas(context, dataRegion);
}

void FlutterRenderChart::PaintStylePoints(
    const ScopedCanvas& canvas, const Rect& paintRegion, const MainChart& chartData)
{
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);

    for (const auto& points : chartData.GetData()) {
        const PointInfo& point = points.GetPointInfo();
        if (point.GetDisplay()) {
            PaintPoint(canvas, ConvertDataToPosition(paintRegion, point), paint, paintData, point);
        }
    }
    PointInfo headPoint = chartData.GetHeadPoint();
    if (headPoint.GetDisplay()) {
        PaintPoint(canvas, ConvertDataToPosition(paintRegion, headPoint), paint, paintData, headPoint);
    }
    PointInfo topPoint = chartData.GetTopPoint();
    if (topPoint.GetDisplay()) {
        PaintPoint(canvas, ConvertDataToPosition(paintRegion, topPoint), paint, paintData, topPoint);
    }
    PointInfo bottomPoint = chartData.GetBottomPoint();
    if (bottomPoint.GetDisplay()) {
        PaintPoint(canvas, ConvertDataToPosition(paintRegion, bottomPoint), paint, paintData, bottomPoint);
    }
}

void FlutterRenderChart::PaintText(const ScopedCanvas& canvas, const Rect& paintRegion, const MainChart& chartData)
{
    if (chartData.GetData().empty()) {
        return;
    }
    auto fontCollection = FlutterFontCollection::GetInstance().GetFontCollection();
    if (!fontCollection) {
        LOGW("PaintText: fontCollection is null");
        return;
    }
    txt::ParagraphStyle style;
    txt::TextStyle txtStyle;
    txtStyle.font_size = chartData.GetTextSize();
    txtStyle.font_families = chartData.GetFontFamily();
    txtStyle.font_weight = txt::FontWeight::w400;
    std::unique_ptr<txt::ParagraphBuilder> builder;
    double paragraphSize = paintRegion.Width() / chartData.GetData().size();
    style.max_lines = 1;
    for (const auto& point : chartData.GetData()) {
        const TextInfo& text = point.GetTextInfo();
        const PointInfo& pointInfo = point.GetPointInfo();
        Offset pointPosition = ConvertDataToPosition(paintRegion, pointInfo);
        txtStyle.color = text.GetColor().GetValue();
        builder = txt::ParagraphBuilder::CreateTxtBuilder(style, fontCollection);
        builder->PushStyle(txtStyle);
        builder->AddText(StringUtils::Str8ToStr16(text.GetTextValue()));
        auto paragraph = builder->Build();
        paragraph->Layout(paragraphSize);
        Size textSize = Size(paragraph->GetMinIntrinsicWidth(), paragraph->GetHeight());
        if (text.GetPlacement() == Placement::TOP) {
            paragraph->Paint(canvas->canvas(), pointPosition.GetX() - textSize.Width() / 2,
                pointPosition.GetY() - textSize.Height() - TEXT_PADDING);
        } else if (text.GetPlacement() == Placement::BOTTOM) {
            paragraph->Paint(
                canvas->canvas(), pointPosition.GetX() - textSize.Width() / 2, pointPosition.GetY() + TEXT_PADDING);
        }
    }
}

void FlutterRenderChart::SetEdgeStyle(const PointInfo& point, flutter::Paint& paint) const
{
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setStrokeWidth(NormalizeToPx(point.GetPointStrokeWidth()));
    paint.paint()->setColor(point.GetStrokeColor().GetValue());
}

void FlutterRenderChart::PaintPoint(const ScopedCanvas& canvas, const Offset& offset, flutter::Paint paint,
    flutter::PaintData paintData, const PointInfo& point)
{
    paint.paint()->setColor(point.GetFillColor().GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    double pointSize = NormalizeToPx(point.GetPointSize());
    double halfThickness = NormalizeToPx(point.GetPointStrokeWidth()) / 2;
    double innerRadius = pointSize / 2 - halfThickness;
    // first fill point color, then draw edge of point.
    switch (point.GetPointShape()) {
        case PointShape::CIRCLE: {
            canvas->drawCircle(offset.GetX(), offset.GetY(), innerRadius, paint, paintData);
            SetEdgeStyle(point, paint);
            canvas->drawCircle(offset.GetX(), offset.GetY(), pointSize / 2, paint, paintData);
            break;
        }
        case PointShape::SQUARE: {
            canvas->drawRect(offset.GetX() - innerRadius, offset.GetY() - innerRadius, offset.GetX() + innerRadius,
                offset.GetY() + innerRadius, paint, paintData);
            SetEdgeStyle(point, paint);
            canvas->drawRect(offset.GetX() - pointSize / 2, offset.GetY() - pointSize / 2,
                offset.GetX() + pointSize / 2, offset.GetY() + pointSize / 2, paint, paintData);
            break;
        }
        case PointShape::TRIANGLE: {
            auto path = flutter::CanvasPath::Create();
            path->moveTo(offset.GetX(), offset.GetY() - innerRadius);
            path->lineTo(offset.GetX() - RATIO_CONSTANT * innerRadius / 2, offset.GetY() + innerRadius / 2);
            path->lineTo(offset.GetX() + RATIO_CONSTANT * innerRadius / 2, offset.GetY() + innerRadius / 2);
            path->close();
            canvas->drawPath(path.get(), paint, paintData);
            path->reset();
            path->moveTo(offset.GetX(), offset.GetY() - pointSize / 2);
            path->lineTo(offset.GetX() - RATIO_CONSTANT * pointSize / 4, offset.GetY() + pointSize / 4);
            path->lineTo(offset.GetX() + RATIO_CONSTANT * pointSize / 4, offset.GetY() + pointSize / 4);
            path->close();
            SetEdgeStyle(point, paint);
            canvas->drawPath(path.get(), paint, paintData);
            break;
        }
        default: {
            canvas->drawCircle(offset.GetX(), offset.GetY(), innerRadius, paint, paintData);
            SetEdgeStyle(point, paint);
            canvas->drawCircle(offset.GetX(), offset.GetY(), pointSize / 2, paint, paintData);
            break;
        }
    }
}

void FlutterRenderChart::PaintDatas(RenderContext& context, const Rect& paintRect)
{
    ScopedCanvas canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    if (NearEqual(paintRect.Width(), 0.0) || NearEqual(paintRect.Height(), 0.0)) {
        LOGE("data paint region width:%{public}lf height:%{public}lf", paintRect.Width(), paintRect.Height());
        return;
    }
    if (type_ == ChartType::LINE) {
        PaintLinearGraph(canvas, paintRect);
    } else {
        flutter::Paint paint;
        paint.paint()->setAntiAlias(true);
        paint.paint()->setStyle(SkPaint::Style::kStrokeAndFill_Style);

        const int32_t barGroupNumber = static_cast<int32_t>(mainCharts_.size());
        auto barsAreaNumber = horizontal_.tickNumber;
        for (int32_t barGroupIndex = 0; barGroupIndex < barGroupNumber; ++barGroupIndex) {
            auto barGroup = mainCharts_[barGroupIndex];
            paint.paint()->setColor(barGroup.GetFillColor().GetValue());
            PaintBar(canvas, paint, barGroup.GetData(), paintRect, barGroupNumber, barsAreaNumber, barGroupIndex);
        }
    }
}

void FlutterRenderChart::UpdateLineGradientPoint(
    const std::vector<LineInfo>& pointInfo, const MainChart& line, const Rect& paintRect)
{
    if (line.GetHeadPointIndex() > 0 && line.GetErasePointNumber() > 0) {
        startGradientIndex_ = static_cast<size_t>(line.GetHeadPointIndex() + line.GetErasePointNumber() - 1);
        endGradientIndex_ = startGradientIndex_ + 1;
        if (static_cast<size_t>(startGradientIndex_) >= pointInfo.size()) {
            startGradientIndex_ = pointInfo.size() - 1;
        }
        if (static_cast<size_t>(endGradientIndex_) >= pointInfo.size()) {
            endGradientIndex_ = pointInfo.size() - 1;
        }
        startGradientPoint_ = ConvertDataToPosition(paintRect, pointInfo[startGradientIndex_].GetPointInfo());
        endGradientPoint_ = ConvertDataToPosition(paintRect, pointInfo[endGradientIndex_].GetPointInfo());
    } else {
        startGradientPoint_ = Offset(0.0, 0.0);
        endGradientPoint_ = Offset(0.0, 0.0);
        gradientOfLine_ = false;
    }
}

void FlutterRenderChart::PaintLinearGraph(const ScopedCanvas& canvas, const Rect& paintRect)
{
    for (const auto& line : mainCharts_) {
        const auto& pointInfo = line.GetData();
        if (pointInfo.empty()) {
            // there is no data in current data set, and skip this line
            continue;
        }
        auto previousSegment = pointInfo[0].GetSegmentInfo();
        auto previousPoint = pointInfo[0].GetPointInfo();
        Offset previousPosition = ConvertDataToPosition(paintRect, previousPoint);
        startOffset_ = previousPosition;
        auto edgePath = flutter::CanvasPath::Create();
        auto gradientPath = flutter::CanvasPath::Create();
        edgePath->moveTo(previousPosition.GetX(), previousPosition.GetY());
        gradientPath->moveTo(previousPosition.GetX(), paintRect.Bottom());
        gradientPath->lineTo(previousPosition.GetX(), previousPosition.GetY());

        UpdateLineGradientPoint(pointInfo, line, paintRect);

        for (size_t index = 1; index < pointInfo.size(); index++) {
            const auto& currentPoint = pointInfo[index].GetPointInfo();
            Offset currentPosition = ConvertDataToPosition(paintRect, currentPoint);
            if (line.GetSmoothFlag()) {
                bool isEnd = previousSegment != pointInfo[index].GetSegmentInfo();
                AddCubicPath(gradientPath, paintRect, line.GetData(), index, isEnd);
                AddCubicPath(edgePath, paintRect, line.GetData(), index, isEnd);
            } else {
                gradientPath->lineTo(currentPosition.GetX(), currentPosition.GetY());
                edgePath->lineTo(currentPosition.GetX(), currentPosition.GetY());
            }
            wholeLineGradient_ = line.GetWholeLineGradient();
            if (wholeLineGradient_) {
                targetColor_ = line.GetTargetColor();
            }
            int32_t i = static_cast<int32_t>(index);
            if ((line.GetHeadPointIndex() > 0) && (i > line.GetHeadPointIndex()) &&
                (i <= line.GetHeadPointIndex() + line.GetErasePointNumber())) {
                gradientPath->reset();
                if (i < line.GetHeadPointIndex() + line.GetErasePointNumber()) {
                    edgePath->reset();
                    edgePath->moveTo(currentPosition.GetX(), currentPosition.GetY());
                } else {
                    edgePath->lineTo(currentPosition.GetX(), currentPosition.GetY());
                    gradientPath->moveTo(currentPosition.GetX(), paintRect.Bottom());
                    gradientPath->lineTo(currentPosition.GetX(), currentPosition.GetY());
                    gradientOfLine_ = true;
                }
                previousSegment = pointInfo[index].GetSegmentInfo();
                previousPoint = currentPoint;
                previousPosition = currentPosition;
                continue;
            }
            if (previousSegment != pointInfo[index].GetSegmentInfo() ||
                (line.GetHeadPointIndex() > 0 && line.GetHeadPointIndex() == i)) {
                gradientPath->lineTo(currentPosition.GetX(), paintRect.Bottom());
                gradientPath->close();
                if (line.GetGradient()) {
                    PaintLineGradient(canvas, gradientPath, paintRect, line.GetFillColor(), line.GetTopPoint());
                }
                PaintLineEdge(canvas, edgePath, previousSegment, line.GetLineWidth(), false);
                // print gradient
                gradientPath->reset();
                edgePath->reset();
                previousSegment = pointInfo[index].GetSegmentInfo();
                previousPoint = currentPoint;
                previousPosition = currentPosition;
                edgePath->moveTo(previousPosition.GetX(), previousPosition.GetY());
                gradientPath->moveTo(previousPosition.GetX(), paintRect.Bottom());
                gradientPath->lineTo(previousPosition.GetX(), previousPosition.GetY());
            } else {
                previousSegment = pointInfo[index].GetSegmentInfo();
                previousPoint = currentPoint;
                previousPosition = currentPosition;
            }
        }
        gradientPath->lineTo(previousPosition.GetX(), paintRect.Bottom());
        gradientPath->close();
        if (line.GetGradient()) {
            PaintLineGradient(canvas, gradientPath, paintRect, line.GetFillColor(), line.GetTopPoint());
        }
        PaintLineEdge(canvas, edgePath, previousSegment, line.GetLineWidth(), true);
        PaintStylePoints(canvas, paintRect, line);
        PaintText(canvas, paintRect, line);
    }
}

void FlutterRenderChart::PaintLineEdge(const ScopedCanvas& canvas, fml::RefPtr<flutter::CanvasPath> path,
    const SegmentInfo segmentInfo, double thickness, bool drawGradient)
{
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(segmentInfo.GetSegmentColor().GetValue());
    if (segmentInfo.GetLineType() == LineType::DASHED) {
        SkPath subPath;
        subPath.addRRect(
            SkRRect::MakeRectXY(SkRect::MakeXYWH(0.0, -0.5 * thickness, segmentInfo.GetSolidWidth(), thickness),
                0.5 * thickness, 0.5 * thickness));
        paint.paint()->setPathEffect(SkPath1DPathEffect::Make(subPath,
            segmentInfo.GetSpaceWidth() + segmentInfo.GetSolidWidth(), 5.0f, SkPath1DPathEffect::kMorph_Style));
    } else {
        paint.paint()->setStrokeWidth(thickness);
    }
    if (wholeLineGradient_) {
        double end = startGradientPoint_.GetX();
        if (NearZero(end)) {
            end = startOffset_.GetX() + paintWidth_;
        }
        SkPoint points[2] = { SkPoint::Make(startOffset_.GetX(), 0.0f), SkPoint::Make(end, 0.0f) };
        SkColor colors[2] = { segmentInfo.GetSegmentColor().GetValue(), targetColor_.GetValue() };
#ifdef USE_SYSTEM_SKIA
        paint.paint()->setShader(
            SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkShader::kClamp_TileMode, 0, nullptr));
#else
        paint.paint()->setShader(
            SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp, 0, nullptr));
#endif
    }
    if (gradientOfLine_ && drawGradient) {
        SkPoint points[2] = { SkPoint::Make(startGradientPoint_.GetX(), 0.0f),
            SkPoint::Make(endGradientPoint_.GetX(), 0.0f) };
        SkColor colors[2] = { segmentInfo.GetSegmentColor().ChangeAlpha(0).GetValue(),
            segmentInfo.GetSegmentColor().GetValue() };
#ifdef USE_SYSTEM_SKIA
        paint.paint()->setShader(
            SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkShader::kClamp_TileMode, 0, nullptr));
#else
        paint.paint()->setShader(
            SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp, 0, nullptr));
#endif
    }
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawPath(path.get(), paint, paintData);
}

void FlutterRenderChart::PaintLineGradient(const ScopedCanvas& canvas, fml::RefPtr<flutter::CanvasPath> path,
    const Rect& paintRect, Color fillColor, const PointInfo& peekPoint)
{
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setShader(
        CreateFillGradientShader(paintRect, fillColor, ConvertDataToPosition(paintRect, peekPoint).GetY()));

    paint.paint()->setStyle(SkPaint::Style::kStrokeAndFill_Style);
    canvas->drawPath(path.get(), paint, paintData);
}

sk_sp<SkShader> FlutterRenderChart::CreateFillGradientShader(const Rect& paintRect, const Color& fillColor, double top)
{
    SkPoint points[2] = { SkPoint::Make(paintRect.GetOffset().GetX(), top),
        SkPoint::Make(paintRect.GetOffset().GetX(), paintRect.GetOffset().GetY() + paintRect.Height()) };
    SkColor colors[2] = { fillColor.GetValue(), fillColor.ChangeAlpha(0).GetValue() };
#ifdef USE_SYSTEM_SKIA
    return SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkShader::kClamp_TileMode, 0, nullptr);
#else
    return SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp, 0, nullptr);
#endif
}

Offset FlutterRenderChart::CalculateControlA(const Offset& prev, const Offset& cur, const Offset& next)
{
    return Offset(cur.GetX() + (next.GetX() - prev.GetX()) / BEZIER_CONSTANT,
        cur.GetY() + (next.GetY() - prev.GetY()) / BEZIER_CONSTANT);
}

Offset FlutterRenderChart::CalculateControlB(const Offset& cur, const Offset& next, const Offset& nextNext)
{
    return Offset(next.GetX() - (nextNext.GetX() - cur.GetX()) / BEZIER_CONSTANT,
        next.GetY() - (nextNext.GetY() - cur.GetY()) / BEZIER_CONSTANT);
}

int32_t FlutterRenderChart::PaintLine(uint32_t startIndex, const std::vector<LineInfo>& line,
    fml::RefPtr<flutter::CanvasPath> path, const MainChart& data, const Rect& paintRect)
{
    uint32_t index = startIndex;
    startIndex_ = startIndex;
    bool startPoint = true;
    for (; index < line.size(); index++) {
        const auto& point = line[index].GetPointInfo();
        if (point.GetX() > horizontal_.max || point.GetX() < horizontal_.min || point.GetY() > vertical_.max ||
            point.GetY() < vertical_.min) {
            continue;
        }
        if (startPoint) {
            Offset position = ConvertDataToPosition(paintRect, point);
            if (data.GetGradient() && !drawLine_) {
                path->moveTo(position.GetX(), paintRect.GetOffset().GetY() + paintRect.GetSize().Height());
                path->lineTo(position.GetX(), position.GetY());
            } else {
                path->moveTo(position.GetX(), position.GetY());
            }
            startPoint = false;
        } else if (index == line.size() - 1 ||
                   (data.GetHeadPointIndex() >= 0 && (index == static_cast<uint32_t>(data.GetHeadPointIndex())))) {
            Offset position = ConvertDataToPosition(paintRect, point);
            if (data.GetSmoothFlag()) {
                AddCubicPath(path, paintRect, line, index, true);
            } else {
                path->lineTo(position.GetX(), position.GetY());
            }
            if (data.GetGradient() && !drawLine_) {
                path->lineTo(position.GetX(), paintRect.GetOffset().GetY() + paintRect.GetSize().Height());
            }
            index += 1;
            break;
        } else {
            if (data.GetSmoothFlag()) {
                AddCubicPath(path, paintRect, line, index, false);
            } else {
                Offset position = ConvertDataToPosition(paintRect, point);
                path->lineTo(position.GetX(), position.GetY());
            }
        }
    }
    return index;
}

void FlutterRenderChart::AddCubicPath(fml::RefPtr<flutter::CanvasPath>& path, const Rect& paintRect,
    const std::vector<LineInfo>& line, uint32_t index, bool isEnd)
{
    // use control point A = [(Xi + (Xi+1 - Xi-1) / 4), (Yi + (Yi+1 - Yi-1) / 4)]
    // and control point B = [(Xi+1 - (Xi+2 - Xi) / 4), (Yi+1 - (Yi+2 - Yi) / 4)]
    if (index > 0 && index < line.size()) {
        Offset prev = ConvertDataToPosition(
            paintRect, line[(index < 2 || index - 2 < startIndex_) ? startIndex_ : index - 2].GetPointInfo());
        Offset cur = ConvertDataToPosition(paintRect, line[index - 1].GetPointInfo());
        Offset next = ConvertDataToPosition(paintRect, line[index].GetPointInfo());
        Offset nextNext = ConvertDataToPosition(paintRect,
            (isEnd || index + 1 >= line.size()) ? line[index].GetPointInfo() : line[index + 1].GetPointInfo());
        Offset controlA = CalculateControlA(prev, cur, next);
        Offset controlB = CalculateControlB(cur, next, nextNext);
        path->cubicTo(controlA.GetX(), controlA.GetY(), controlB.GetX(), controlB.GetY(), next.GetX(), next.GetY());
    } else {
        LOGW("index out of region");
    }
}

void FlutterRenderChart::PaintHorizontalAxis(RenderContext& context, const Rect& paintRect)
{
    ScopedCanvas canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    const Offset offset = paintRect.GetOffset();

    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(horizontal_.color.GetValue());
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setStrokeWidth(DEFAULT_AXIS_STROKE_WIDTH);

    if ((horizontal_.tickNumber <= 0) || (horizontal_.tickNumber + 1 == 0)) {
        horizontal_.tickNumber = DEFAULT_AXISTICK;
    }
    tickHorizontalOffset_ = (paintRect.Width() - 2 * EDGE_PADDING) / (horizontal_.tickNumber + 1);

    if (!horizontal_.display) {
        return;
    }

    double tickPosition = EDGE_PADDING + tickHorizontalOffset_;
    for (int32_t index = 0; index < horizontal_.tickNumber; index++) {
        canvas->drawLine(offset.GetX() + tickPosition, offset.GetY(), offset.GetX() + tickPosition,
            offset.GetY() + TICK_LENGTH, paint, paintData);
        tickPosition += tickHorizontalOffset_;
    }

    canvas->drawLine(offset.GetX(), offset.GetY() + 0.5 * TICK_LENGTH, offset.GetX() + paintRect.Width(),
        offset.GetY() + 0.5 * TICK_LENGTH, paint, paintData);
}

void FlutterRenderChart::PaintVerticalAxis(RenderContext& context, const Offset& offset, const Rect& paintRect)
{
    ScopedCanvas canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(vertical_.color.GetValue());
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setStrokeWidth(DEFAULT_AXIS_STROKE_WIDTH);

    if ((vertical_.tickNumber <= 0) || (vertical_.tickNumber + 1 == 0)) {
        vertical_.tickNumber = DEFAULT_AXISTICK;
    }
    tickOffset_ = (paintRect.Height() - 2 * EDGE_PADDING) / (vertical_.tickNumber + 1);

    if (!vertical_.display) {
        return;
    }

    double tickPosition = EDGE_PADDING + tickOffset_;
    for (int32_t index = 0; index < vertical_.tickNumber; index++) {
        canvas->drawLine(offset.GetX() + paintRect.Width() - TICK_LENGTH, offset.GetY() + tickPosition,
            offset.GetX() + paintRect.Width(), offset.GetY() + tickPosition, paint, paintData);
        tickPosition += tickOffset_;
    }

    canvas->drawLine(offset.GetX() + paintRect.Width() - 0.5 * TICK_LENGTH, offset.GetY(),
        offset.GetX() + paintRect.Width() - 0.5 * TICK_LENGTH, offset.GetY() + paintRect.Height(), paint, paintData);
}

void FlutterRenderChart::PaintBar(const ScopedCanvas& canvas, flutter::Paint& paint,
    const std::vector<LineInfo>& barGroupData, const Rect& paintRect, int32_t barGroupNum, int32_t barsAreaNum,
    int32_t barGroupIndex)
{
    if (NearEqual(paintRect.Width(), 0.0) || NearEqual(paintRect.Height(), 0.0)) {
        LOGE("data paint region width:%{public}lf height:%{public}lf", paintRect.Width(), paintRect.Height());
        return;
    }
    const int32_t barSize = static_cast<int32_t>(barGroupData.size());
    for (int32_t barIndex = 0; barIndex < barSize; ++barIndex) {
        const auto point = barGroupData[barIndex].GetPointInfo();
        // If the actual number of data exceeds Tick, the extra part will not be laid out
        if (barIndex >= barsAreaNum) {
            return;
        }
        auto barsAreaPaintRect = GetBarsAreaPaintRect(paintRect, barIndex);
        double barInterval = BARS_INTERVAL_PROPORTION;
        auto context = GetContext().Upgrade();
        if (context && context->GetMinPlatformVersion() >= MIN_SDK_VERSION) {
            barInterval = 0;
        }
        auto barAreaPaintRect = GetBarAreaPaintRect(barsAreaPaintRect, barGroupIndex, barGroupNum, barInterval);
        Offset position = ConvertDataToPosition(paintRect, point);
        flutter::RRect rrect;
        flutter::PaintData paintData;
        // barAreaPaintRect left add bar interval is originX
        auto originX = barAreaPaintRect.GetOffset().GetX() + (BAR_INTERVAL_PROPORTION / 2) * barAreaPaintRect.Width();
        auto originY = position.GetY();

        canvas->drawRect(originX, originY, originX + barAreaPaintRect.Width() * (1 - BAR_INTERVAL_PROPORTION),
            paintRect.GetOffset().GetY() + paintRect.Height(), paint, paintData);
    }
}

Rect FlutterRenderChart::GetBarsAreaPaintRect(const Rect& paintRect, int32_t barsAreaIndex)
{
    auto barsAreaWidth = paintRect.Width() / horizontal_.tickNumber;
    auto barsAreaHeight = paintRect.Height();
    Rect barsAreaRect =
        Rect(paintRect.Left() + barsAreaIndex * barsAreaWidth, paintRect.Top(), barsAreaWidth, barsAreaHeight);
    return barsAreaRect;
}

Rect FlutterRenderChart::GetBarAreaPaintRect(
    const Rect& barsAreaPaintRect, int32_t barGroupIndex, int32_t barGroupNumber, double barInterval)
{
    // Divide 30% of the horizontal space of barsArea into 2 parts as left and right intervals,
    // and divide the remaining part by barGroupNumber to get barAreaWidth
    auto barAreaWidth = (1 - barInterval) * barsAreaPaintRect.Width() / barGroupNumber;
    auto barAreaHeight = barsAreaPaintRect.Height();
    // After leaving the interval, the left border of the barArea area is obtained
    auto barAreaLeft = barsAreaPaintRect.Left() + barInterval / 2 * barsAreaPaintRect.Width() +
                       barGroupIndex * barAreaWidth;
    Rect barAreaRect = Rect(barAreaLeft, barsAreaPaintRect.Top(), barAreaWidth, barAreaHeight);
    return barAreaRect;
}

} // namespace OHOS::Ace
