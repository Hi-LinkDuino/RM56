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

#include "core/components/data_panel/rosen_render_data_panel.h"

#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkShader.h"
#include "third_party/skia/include/effects/Sk1DPathEffect.h"
#include "third_party/skia/include/effects/SkDashPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {
namespace {

constexpr double START_COLOR_TRANSITION_EDGE = 30.0;
constexpr double HALF_CIRCLE = 180.0;
constexpr double QUARTER_CIRCLE = 90.0;
constexpr double PRECISION_CORRECTION = 0.5;
constexpr double SCAN_DEGREE = 30;
constexpr int32_t CIRCLE_NUMBER = 50;
constexpr float SHADOW_BLUR_RADIUS = 5.0f;
constexpr double DIAMETER_TO_THICKNESS_RATIO = 0.12;
constexpr double FIXED_WIDTH = 1.0;

} // namespace

namespace {
void PaintTrackBackground(SkCanvas* canvas, const Offset& center, double thickness, const Color& color, double diameter)
{
    SkPaint backgroundTrackData;
    SkPath backgroundTrackPath;

    SkRect rect = SkRect::MakeXYWH(center.GetX() - diameter / 2 + thickness / 2,
        center.GetY() - diameter / 2 + thickness / 2, diameter - thickness, diameter - thickness);
    backgroundTrackPath.addArc(rect, 0.0, HALF_CIRCLE * 2);
    backgroundTrackData.setStyle(SkPaint::kStroke_Style);
    backgroundTrackData.setStrokeWidth(thickness);
    backgroundTrackData.setAntiAlias(true);
    backgroundTrackData.setColor(color.GetValue());

    canvas->drawPath(backgroundTrackPath, backgroundTrackData);
}

sk_sp<SkShader> BlendSkShader(SkScalar cx, SkScalar cy, const SkColor colors[], const SkScalar pos[], int colorCount,
    SkScalar startAngle, SkScalar drawAngle, const SkMatrix* localMatrix, bool useAnimator = false,
    bool useEffect = false)
{
    // the skia interface is more than 5 params.
    const Color HIGHLIGHT = Color::FromString("#55ffffff");
    const Color SHADOW = Color::FromString("#00ffffff");
    SkColor scanColors[3] = { SHADOW.GetValue(), HIGHLIGHT.GetValue(), SHADOW.GetValue() };
    SkScalar scanPos[3] = { 0, 0.80, 1 };
    sk_sp<SkShader> scanShader;
    sk_sp<SkShader> backgroundShader;
    sk_sp<SkShader> blendShader;

    double scanStartDegree = 0.0;
    double scanEndDegree = drawAngle;
    if (startAngle < SCAN_DEGREE) {
        scanStartDegree = 0.0;
        scanEndDegree = startAngle;
    } else {
        scanStartDegree = startAngle - SCAN_DEGREE;
        scanEndDegree = startAngle;
    }
    if (drawAngle < scanEndDegree + 0.1 * drawAngle) {
        scanColors[1] =
            HIGHLIGHT.ChangeAlpha(((drawAngle - scanEndDegree) / (0.1 * drawAngle)) * HIGHLIGHT.GetAlpha()).GetValue();
    }
#ifdef USE_SYSTEM_SKIA
    backgroundShader =
        SkGradientShader::MakeSweep(cx, cy, colors, pos, 2, SkShader::kDecal_TileMode, 0.0, drawAngle, 0, nullptr);
    scanShader = SkGradientShader::MakeSweep(
        cx, cy, scanColors, scanPos, 3, SkShader::kDecal_TileMode, scanStartDegree, scanEndDegree, 0, nullptr);
    if (useAnimator) {
        blendShader = SkShader::MakeCompose(backgroundShader, scanShader, SkBlendMode::kSrcOver);
    } else {
        blendShader = backgroundShader;
    }
#else
    backgroundShader =
        SkGradientShader::MakeSweep(cx, cy, colors, pos, 2, SkTileMode::kDecal, 0.0, drawAngle, 0, nullptr);
    scanShader = SkGradientShader::MakeSweep(
        cx, cy, scanColors, scanPos, 3, SkTileMode::kDecal, scanStartDegree, scanEndDegree, 0, nullptr);
    if (useAnimator) {
        blendShader = SkShaders::Blend(SkBlendMode::kSrcOver, backgroundShader, scanShader);
    } else {
        blendShader = backgroundShader;
    }
#endif
    return blendShader;
}

void PaintProgress(
    SkCanvas* canvas, ArcData arcData, bool useEffect = false, bool useAnimator = false, double percent = 0.0)
{
    double thickness = arcData.thickness;
    double radius = arcData.radius;
    double progress = arcData.progress;
    if (GreatNotEqual(progress, 100.0)) {
        progress = 100.0;
    }
    if (LessNotEqual(progress, 0.0)) {
        progress = 0.0;
    }
    Offset center = arcData.center;
    // for example whole circle is 100 which is divided into 100 piece 360 / 100 = 3.6
    double drawAngle = arcData.wholeAngle * 0.01 * progress;
    double startAngle = arcData.startAngle;

    if (useEffect) {
        radius = radius - SHADOW_BLUR_RADIUS;
    }
    SkPath botPath;
    SkRect rect = SkRect::MakeXYWH(center.GetX() - radius + thickness / 2, center.GetY() - radius + thickness / 2,
        radius * 2 - thickness, radius * 2 - thickness);
    botPath.addArc(rect, 0.0, drawAngle);
    SkColor colors[2] = { arcData.startColor.GetValue(), arcData.endColor.GetValue() };
    SkScalar pos[2] = { 0.0, 1.0 };
    SkPaint gradientPaint;
    gradientPaint.setStyle(SkPaint::kStroke_Style);
    gradientPaint.setStrokeWidth(thickness);
    gradientPaint.setAntiAlias(true);
    if (NearEqual(progress, 0.0)) {
        return;
    }
    SkPaint startCirclePaint;
    startCirclePaint.setStyle(SkPaint::kFill_Style);
    startCirclePaint.setAntiAlias(true);
    startCirclePaint.setColor(arcData.startColor.GetValue());

    SkPaint endCirclePaint;
    endCirclePaint.setStyle(SkPaint::kFill_Style);
    endCirclePaint.setAntiAlias(true);
    endCirclePaint.setColor(arcData.endColor.GetValue());
    // this part is used to draw the circle edge
    if (useEffect) {
        gradientPaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
        startCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
        endCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    }
    canvas->save();

    if (progress > 50) {
        // this component contains three parts one edge circle, one half circle and a path region.
        canvas->save();
        canvas->rotate(startAngle, center.GetX(), center.GetY());
        canvas->drawCircle(center.GetX(), center.GetY() - radius + thickness / 2, thickness / 2, startCirclePaint);
        canvas->restore();

        canvas->save();

        gradientPaint.setShader(BlendSkShader(center.GetX(), center.GetY(), colors, pos, 2, percent * drawAngle,
            drawAngle, nullptr, useAnimator, useEffect));
        canvas->rotate(startAngle - QUARTER_CIRCLE, center.GetX(), center.GetY());
        canvas->drawPath(botPath, gradientPaint);
        canvas->restore();

        canvas->save();
        canvas->rotate(drawAngle + startAngle, center.GetX(), center.GetY());
        canvas->drawCircle(center.GetX(), center.GetY() - radius + thickness / 2, thickness / 2, endCirclePaint);
        canvas->restore();
    } else {
        // this component contains three parts two edge circle and a path region.
        if (progress < START_COLOR_TRANSITION_EDGE) {
            startCirclePaint.setColor(
                Color::LineColorTransition(arcData.endColor, arcData.startColor, progress / START_COLOR_TRANSITION_EDGE)
                    .GetValue());
        } else {
            startCirclePaint.setColor(arcData.startColor.GetValue());
        }
        // draw start edge circle
        canvas->save();
        canvas->rotate(startAngle, center.GetX(), center.GetY());
        canvas->drawCircle(center.GetX(), center.GetY() - radius + thickness / 2, thickness / 2, startCirclePaint);
        canvas->restore();

        // draw end edge circle
        canvas->save();
        canvas->rotate(startAngle + drawAngle - PRECISION_CORRECTION, center.GetX(), center.GetY());
        SkRect endOval = SkRect::MakeXYWH(center.GetX() - thickness / 2, center.GetY() - radius, thickness, thickness);
        canvas->drawArc(endOval, 3 * QUARTER_CIRCLE, HALF_CIRCLE, true, endCirclePaint);
        canvas->restore();

        // draw the region between start and end.
        canvas->save();
        canvas->rotate(startAngle - QUARTER_CIRCLE, center.GetX(), center.GetY());
        if (progress < START_COLOR_TRANSITION_EDGE) {
            colors[0] =
                Color::LineColorTransition(arcData.endColor, arcData.startColor, progress / START_COLOR_TRANSITION_EDGE)
                    .GetValue();
            gradientPaint.setShader(BlendSkShader(center.GetX(), center.GetY(), colors, pos, 2, percent * drawAngle,
                drawAngle, nullptr, useAnimator, useEffect));
        } else {
            gradientPaint.setShader(BlendSkShader(center.GetX(), center.GetY(), colors, pos, 2, percent * drawAngle,
                drawAngle, nullptr, useAnimator, useEffect));
        }
        canvas->drawPath(botPath, gradientPaint);
        canvas->restore();
    }
    canvas->restore();
}

void PaintProgressFilterMask(SkCanvas* canvas, ArcData arcData)
{
    if (LessOrEqual(arcData.progress, 0.0)) {
        return;
    }
    if (GreatNotEqual(arcData.progress, 100.0)) {
        arcData.progress = 100.0;
    }
    double thickness = arcData.thickness;
    double radius = arcData.radius - SHADOW_BLUR_RADIUS;
    Offset center = arcData.center;
    SkPaint filterPaint;
    filterPaint.setAntiAlias(true);
    filterPaint.setStrokeCap(SkPaint::kButt_Cap);
    filterPaint.setStrokeWidth(thickness);
    filterPaint.setStyle(SkPaint::kStroke_Style);
    filterPaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkRect rect = SkRect::MakeXYWH(center.GetX() - radius + thickness / 2, center.GetY() - radius + thickness / 2,
        radius * 2 - thickness, radius * 2 - thickness);
    double startAngle = arcData.startAngle;

    // paint start circle
    SkPaint startCirclePaint;
    startCirclePaint.setStyle(SkPaint::kStrokeAndFill_Style);
    startCirclePaint.setAntiAlias(true);
    startCirclePaint.setColor(arcData.startColor.ChangeAlpha(101).GetValue());
    startCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkPath startCircleArch;
    SkRect startCircleArchRect =
        SkRect::MakeXYWH(center.GetX() - thickness / 2, center.GetY() - radius, thickness, thickness);
    canvas->drawCircle(center.GetX(), center.GetY() - radius + thickness / 2, thickness / 2, startCirclePaint);
    canvas->drawPath(startCircleArch, startCirclePaint);

    // paint path
    canvas->save();
    canvas->rotate(-QUARTER_CIRCLE + 1, center.GetX(), center.GetY());
    double sweepAngle = arcData.progress * 0.01 * arcData.wholeAngle;
    SkPath rainbowFilterPath;
    rainbowFilterPath.addArc(rect, startAngle, sweepAngle);
    // 101 is Opacity 40%
    SkColor colors[2] = { arcData.startColor.ChangeAlpha(101).GetValue(),
        arcData.endColor.ChangeAlpha(101).GetValue() };
    SkScalar pos[2] = { 0.0, 1.0 };
    // kClamp_TileMode
#ifdef USE_SYSTEM_SKIA
    filterPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2,
        SkShader::kDecal_TileMode, startAngle, startAngle + sweepAngle, 0, nullptr));
#else
    filterPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2, SkTileMode::kDecal,
        startAngle, startAngle + sweepAngle, 0, nullptr));
#endif
    canvas->drawPath(rainbowFilterPath, filterPaint);
    startAngle += sweepAngle;

    // paint end circle
    canvas->save();
    SkPaint endCirclePaint;
    endCirclePaint.setStyle(SkPaint::kStrokeAndFill_Style);
    endCirclePaint.setAntiAlias(true);
    endCirclePaint.setColor(arcData.endColor.ChangeAlpha(101).GetValue());
    endCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkPath endCircleArch;
    endCircleArch.addArc(startCircleArchRect, 90.0, -180.0);
    canvas->rotate(startAngle + QUARTER_CIRCLE, center.GetX(), center.GetY());
    canvas->drawPath(endCircleArch, endCirclePaint);
    canvas->restore();

    canvas->restore();
}

void PaintFilterMask(SkCanvas* canvas, ArcData arcData)
{
    PaintProgressFilterMask(canvas, arcData);
}

void PaintRainbowFilterMask(SkCanvas* canvas, double factor, const std::vector<Segment>& segments, ArcData arcData)
{
    if (segments.empty()) {
        return;
    }
    double thickness = arcData.thickness;
    double radius = arcData.radius - SHADOW_BLUR_RADIUS;
    Offset center = arcData.center;
    double maxValue = arcData.maxValue;
    SkPaint filterPaint;
    filterPaint.setAntiAlias(true);
    filterPaint.setStrokeCap(SkPaint::kButt_Cap);
    filterPaint.setStrokeWidth(thickness);
    filterPaint.setStyle(SkPaint::kStroke_Style);
    filterPaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkRect rect = SkRect::MakeXYWH(center.GetX() - radius + thickness / 2, center.GetY() - radius + thickness / 2,
        radius * 2 - thickness, radius * 2 - thickness);
    double startAngle = arcData.startAngle;

    canvas->save();
    canvas->rotate(startAngle, center.GetX(), center.GetY());
    SkPaint startCirclePaint;
    startCirclePaint.setStyle(SkPaint::kStrokeAndFill_Style);
    startCirclePaint.setAntiAlias(true);
    startCirclePaint.setColor(segments[0].GetStartColor().ChangeAlpha(101).GetValue());
    startCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkPath startCircleArch;
    SkRect startCircleArchRect =
        SkRect::MakeXYWH(center.GetX() - thickness / 2, center.GetY() - radius, thickness, thickness);
    startCircleArch.addArc(startCircleArchRect, 90.0, 180.0);
    canvas->drawPath(startCircleArch, startCirclePaint);
    canvas->restore();

    canvas->save();
    canvas->rotate(-QUARTER_CIRCLE + 1, center.GetX(), center.GetY());
    for (const auto& segment : segments) {
        double sweepAngle = segment.GetValue() / maxValue * arcData.wholeAngle * factor;
        if (GreatNotEqual(sweepAngle, arcData.wholeAngle)) {
            sweepAngle = arcData.wholeAngle;
        }
        SkPath rainbowFilterPath;
        rainbowFilterPath.addArc(rect, startAngle, sweepAngle);
        SkColor colors[2] = { segment.GetStartColor().ChangeAlpha(101).GetValue(),
            segment.GetEndColor().ChangeAlpha(101).GetValue() };
        SkScalar pos[2] = { 0.0, 1.0 };
#ifdef USE_SYSTEM_SKIA
        filterPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2,
            SkShader::kClamp_TileMode, startAngle, startAngle + sweepAngle, 0, nullptr));
#else
        filterPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2,
            SkTileMode::kClamp, startAngle, startAngle + sweepAngle, 0, nullptr));
#endif
        canvas->drawPath(rainbowFilterPath, filterPaint);
        startAngle += sweepAngle;
    }
    canvas->save();
    SkPaint endCirclePaint;
    endCirclePaint.setStyle(SkPaint::kStrokeAndFill_Style);
    endCirclePaint.setAntiAlias(true);
    endCirclePaint.setColor(segments[segments.size() - 1].GetEndColor().ChangeAlpha(101).GetValue());
    endCirclePaint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, SHADOW_BLUR_RADIUS));
    SkPath endCircleArch;
    endCircleArch.addArc(startCircleArchRect, 90.0, -180.0);
    canvas->rotate(startAngle + QUARTER_CIRCLE, center.GetX(), center.GetY());
    canvas->drawPath(endCircleArch, endCirclePaint);
    canvas->restore();

    canvas->restore();
}
} // namespace

void RosenRenderProgressDataPanel::Paint(RenderContext& context, const Offset& offset)
{
    if (type_ == ChartType::LOADING) {
        PaintEffectedLoadingProgress(context, offset);
    } else {
        PaintRingProgress(context, offset);
    }
}

void RosenRenderProgressDataPanel::PaintEffectedLoadingProgress(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    const Offset& center = offset + Offset(GetLayoutSize().Width() / 2, GetLayoutSize().Height() / 2);
    double diameter = std::min(GetLayoutSize().Width(), GetLayoutSize().Height());
    double thickness = NormalizeToPx(thickness_);
    if (autoScale_) {
        thickness = diameter * DIAMETER_TO_THICKNESS_RATIO;
    }
    if (LessOrEqual(diameter, thickness)) {
        LOGE("Loading diameter error: %{public}lf thickness: %{public}lf. thickness is reduce to 1/10 of the diameter",
            diameter, thickness);
        thickness = diameter * 0.1;
    }
    PaintTrackBackground(canvas, center, thickness, backgroundTrack_, diameter);

    SkPaint circlePaint;
    circlePaint.setStyle(SkPaint::kFill_Style);
    circlePaint.setAntiAlias(true);
    double angle = sweepDegree_;
    double dx = 0.0;
    double dy = 0.0;
    double radius = diameter / 2 - thickness / 2;
    double animateAngle = rotateAngle_;
    if (animateAngle - angle < 0.0) {
        angle = animateAngle;
    }
    canvas->save();
    canvas->rotate(animateAngle, center.GetX(), center.GetY()); // animate
    for (int i = 0; i < CIRCLE_NUMBER; i++) {
        dx = radius * std::sin((CIRCLE_NUMBER - i) * (angle / CIRCLE_NUMBER) * M_PI / 180.0);
        dy = radius * std::cos((CIRCLE_NUMBER - i) * (angle / CIRCLE_NUMBER) * M_PI / 180.0);
        circlePaint.setColor(
            Color::LineColorTransition(GetStartColor(), GetEndColor(), static_cast<double>(i) / CIRCLE_NUMBER)
                .GetValue());
        canvas->drawCircle(center.GetX() - dx, center.GetY() - dy, thickness / 2, circlePaint);
    }
    canvas->restore();
}

void RosenRenderProgressDataPanel::PaintLoadingProgress(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    const Offset& center = offset + Offset(GetLayoutSize().Width() / 2, GetLayoutSize().Height() / 2);
    double diameter = std::min(GetLayoutSize().Width(), GetLayoutSize().Height());
    double thickness = NormalizeToPx(thickness_);
    if (autoScale_) {
        thickness = diameter * DIAMETER_TO_THICKNESS_RATIO;
    }
    if (LessOrEqual(diameter, thickness)) {
        LOGE("Loading diameter error: %{public}lf thickness: %{public}lf. thickness is reduce to 1/10 of the diameter",
            diameter, thickness);
        thickness = diameter * 0.1;
    }
    double animateAngle = rotateAngle_ - sweepDegree_;

    SkPaint botPaint;
    SkPath botPath;
    SkRect rect = SkRect::MakeXYWH(center.GetX() - diameter / 2 + thickness / 2,
        center.GetY() - diameter / 2 + thickness / 2, diameter - thickness, diameter - thickness);
    botPath.addArc(rect, 0, sweepDegree_);

    SkColor colors[2] = { GetStartColor().GetValue(), GetEndColor().GetValue() };
    SkScalar pos[2] = { 0.0, 1.0 };
    canvas->rotate(-QUARTER_CIRCLE, center.GetX(), center.GetY());
    botPaint.setStrokeCap(SkPaint::kRound_Cap);
    botPaint.setStyle(SkPaint::kStroke_Style);
    botPaint.setStrokeWidth(thickness);
    botPaint.setAntiAlias(true);

#ifdef USE_SYSTEM_SKIA
    botPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2,
        SkShader::kDecal_TileMode, 0.0,
        sweepDegree_ + 2 * PRECISION_CORRECTION + 180.0 / M_PI * std::asin(thickness / ((diameter) / 2)), 0, nullptr));
#else
    botPaint.setShader(SkGradientShader::MakeSweep(center.GetX(), center.GetY(), colors, pos, 2, SkTileMode::kDecal,
        0.0, sweepDegree_ + 2 * PRECISION_CORRECTION + 180.0 / M_PI * std::asin(thickness / ((diameter) / 2)), 0,
        nullptr));
#endif
    canvas->save();
    PaintTrackBackground(canvas, center, thickness, backgroundTrack_, diameter);
    canvas->rotate(animateAngle, center.GetX(), center.GetY()); // animate
    canvas->drawPath(botPath, botPaint);
    canvas->restore();
}

void RosenRenderProgressDataPanel::PaintRingProgress(RenderContext& context, const Offset& offset)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    // whole circle is divided into 100 piece 360 / 100 = 3.6
    ArcData arcData;
    arcData.progress = GetProgress();
    arcData.center = offset + Offset(GetLayoutSize().Width() / 2, GetLayoutSize().Height() / 2);
    arcData.thickness = NormalizeToPx(thickness_);
    arcData.radius = std::min(GetLayoutSize().Width(), GetLayoutSize().Height()) / 2;
    if (autoScale_) {
        arcData.thickness = arcData.radius * 2 * DIAMETER_TO_THICKNESS_RATIO;
    }
    arcData.startColor = GetStartColor();
    arcData.endColor = GetEndColor();
    PaintTrackBackground(canvas, arcData.center, arcData.thickness, backgroundTrack_, arcData.radius * 2);
    if (useEffect_) {
        PaintFilterMask(canvas, arcData);
    }
    PaintProgress(canvas, arcData, false, useEffect_, percent_);
}

void RosenRenderPercentageDataPanel::PaintBackground(
    RenderContext& context, const Offset& leftTop, double totalWidth, double height)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    SkPaint backgroundPaint;
    SkRRect rRect;
    rRect.setRectXY(SkRect::MakeWH(totalWidth, height), height, height);
    rRect.offset(leftTop.GetX(), leftTop.GetY());
    backgroundPaint.setColor(backgroundTrack_.GetValue());
    backgroundPaint.setStyle(SkPaint::kFill_Style);
    backgroundPaint.setAntiAlias(true);
    canvas->clipRRect(rRect, true);
    canvas->drawRect(
        { leftTop.GetX(), leftTop.GetY(), totalWidth + leftTop.GetX(), height + leftTop.GetY() }, backgroundPaint);
}

void RosenRenderPercentageDataPanel::PaintSpace(
    RenderContext& context, const Offset& leftTop, double spaceWidth, double xSpace, double height)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    SkPaint segmentPaint;
    SkRect rect = SkRect::MakeXYWH(xSpace, leftTop.GetY(), spaceWidth, height);
    segmentPaint.setColor(Color::WHITE.GetValue());
    segmentPaint.setStyle(SkPaint::kFill_Style);
    segmentPaint.setAntiAlias(true);
    canvas->drawRect(rect, segmentPaint);
}

void RosenRenderPercentageDataPanel::PaintColorSegment(RenderContext& context, const Offset& leftTop,
    double segmentValue, double xSegment, double height, const Color segmentStartColor, const Color segmentEndColor)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    SkPaint segmentPaint;
    SkRect rect;
    rect = SkRect::MakeXYWH(xSegment, leftTop.GetY(), segmentValue, height);
    SkPoint segmentStartPoint;
    segmentStartPoint.set(rect.left(), rect.top());
    SkPoint segmentEndPoint;
    segmentEndPoint.set(rect.right(), rect.bottom());
    SkPoint segmentPoint[2] = { segmentStartPoint, segmentEndPoint };
    SkColor segmentColor[2] = { segmentStartColor.GetValue(), segmentEndColor.GetValue() };
    SkScalar pos[2] = { 0.0, 1.0 };
    segmentPaint.setShader(SkGradientShader::MakeLinear(segmentPoint, segmentColor, pos, 2, SkTileMode::kClamp));
    segmentPaint.setStyle(SkPaint::kFill_Style);
    segmentPaint.setAntiAlias(true);
    canvas->drawRect(rect, segmentPaint);
}

void RosenRenderPercentageDataPanel::PaintLinearProgress(RenderContext& context, const Offset& offset)
{
    auto totalWidth = GetLayoutSize().Width();
    auto segment = GetSegments();
    auto spaceWidth = SystemProperties::Vp2Px(FIXED_WIDTH);
    auto segmentWidthSum = 0.0;
    for (uint32_t i = 0; i < segment.size(); i++) {
        segmentWidthSum += segment[i].GetValue();
    }
    auto segmentSize = 0.0;
    if (segmentWidthSum == GetMaxValue()) {
        segmentSize = static_cast<double>(segment.size() - 1);
    } else {
        segmentSize = static_cast<double>(segment.size());
    }
    for (uint32_t i = 0; i < segment.size(); i++) {
        if (NearEqual(segment[i].GetValue(), 0.0)) {
            segmentSize -= 1;
        }
    }
    double scaleMaxValue = 0.0;
    if (GetMaxValue() > 0) {
        scaleMaxValue = (totalWidth - segmentSize * spaceWidth) / GetMaxValue();
    }
    auto height = GetLayoutSize().Height();
    auto widthSegment = offset.GetX();
    PaintBackground(context, offset, totalWidth, height);
    for (uint32_t i = 0; i < segment.size(); i++) {
        auto segmentWidth = segment[i].GetValue();
        if (NearEqual(segmentWidth, 0.0)) {
            continue;
        }
        auto segmentStartColor = segment[i].GetStartColor();
        auto segmentEndColor = segment[i].GetEndColor();
        PaintColorSegment(
            context, offset, segmentWidth * scaleMaxValue, widthSegment, height, segmentStartColor, segmentEndColor);
        widthSegment += segment[i].GetValue() * scaleMaxValue;
        PaintSpace(context, offset, spaceWidth, widthSegment, height);
        widthSegment += spaceWidth;
    }
}

void RosenRenderPercentageDataPanel::Paint(RenderContext& context, const Offset& offset)
{
    if (type_ == ChartType::LINE) {
        PaintLinearProgress(context, offset);
        return;
    }
    ArcData arcData;
    arcData.center = Offset(GetLayoutSize().Width() / 2, GetLayoutSize().Height() / 2);
    arcData.thickness = NormalizeToPx(thickness_);
    arcData.radius = std::min(GetLayoutSize().Width(), GetLayoutSize().Height()) / 2;
    if (autoScale_) {
        arcData.thickness = arcData.radius * 2 * DIAMETER_TO_THICKNESS_RATIO;
    }
    arcData.wholeAngle = GetSweepDegree();
    arcData.startAngle = GetStartDegree();
    double totalValue = GetTotalValue();
    auto segments = GetSegments();
    auto maxValue = GetMaxValue();
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        return;
    }
    PaintTrackBackground(canvas, arcData.center, arcData.thickness, backgroundTrack_, arcData.radius * 2);
    double factor = 1.0;
    double proportions = 1.0;
    if (LessOrEqual(maxValue, 0.0)) {
        maxValue = 100.0;
    }
    if (GreatNotEqual(totalValue, maxValue)) {
        factor = maxValue / totalValue;
        proportions = 100 / totalValue;
    } else {
        proportions = 100 / maxValue;
    }
    arcData.maxValue = maxValue;
    if (useEffect_ && GreatNotEqual(totalValue, 0.0)) {
        PaintRainbowFilterMask(canvas, factor * animationPercent_, segments, arcData);
    }
    totalValue = totalValue * proportions;
    for (int32_t i = static_cast<int32_t>(segments.size()) - 1; i >= 0; i--) {
        const auto& segment = segments[i];
        arcData.startColor = segment.GetStartColor();
        arcData.endColor = segment.GetEndColor();
        arcData.progress = totalValue * animationPercent_;
        PaintProgress(canvas, arcData);
        totalValue -= segment.GetValue() * proportions;
    }
}

} // namespace OHOS::Ace