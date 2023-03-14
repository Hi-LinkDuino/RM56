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

#include "core/components/checkable/flutter_render_checkbox.h"

#include "flutter/lib/ui/painting/canvas.h"
#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/common/painter/flutter_checkable_painter.h"
#include "core/components/common/painter/flutter_universal_painter.h"

namespace OHOS::Ace {
namespace {

constexpr float CHECK_MARK_START_X_POSITION = 0.25f;
constexpr float CHECK_MARK_START_Y_POSITION = 0.49f;
constexpr float CHECK_MARK_MIDDLE_X_POSITION = 0.44f;
constexpr float CHECK_MARK_MIDDLE_Y_POSITION = 0.68f;
constexpr float CHECK_MARK_END_X_POSITION = 0.76f;
constexpr float CHECK_MARK_END_Y_POSITION = 0.33f;
constexpr float CHECK_MARK_PART_X_POSITION = 0.25f;
constexpr float CHECK_MARK_PART_Y_POSITION = 0.25f;
constexpr double CHECK_MARK_LEFT_ANIMATION_PERCENT = 0.45;
constexpr double CHECK_MARK_RIGHT_ANIMATION_PERCENT = 0.55;
constexpr double DEFAULT_MAX_CHECKBOX_SHAPE_SCALE = 1.0;
constexpr double DEFAULT_MIN_CHECKBOX_SHAPE_SCALE = 0.0;
constexpr double HALF_THE_WIDTH = 0.5;

} // namespace

void FlutterRenderCheckbox::Paint(RenderContext& context, const Offset& offset)
{
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    auto paintOffset = offset + paintPosition_;
    if (IsPhone() && onFocus_) {
        RequestFocusBorder(paintOffset, drawSize_, NormalizeToPx(borderRadius_));
    }
    flutter::Paint strokePaint;
    flutter::Paint shadowPaint;
    shadowPaint.paint()->setAntiAlias(true);
    strokePaint.paint()->setAntiAlias(true);
    if (!strokePaint.paint() || !shadowPaint.paint()) {
        LOGE("paint is nullptr!");
        return;
    }
    shadowPaint.paint()->setColor(shadowColor_);
    shadowPaint.paint()->setStrokeCap(SkPaint::kRound_Cap);
    SetStrokeWidth(NormalizeToPx(checkStroke_ + shadowWidth_ * 2), shadowPaint);
    SetUIStatus(canvas, paintOffset, strokePaint, shadowPaint);
}

void FlutterRenderCheckbox::SetUIStatus(ScopedCanvas& canvas,
    Offset& paintOffset, flutter::Paint& strokePaint, flutter::Paint& shadowPaint)
{
    switch (uiStatus_) {
        case UIStatus::SELECTED: {
            DrawActiveBorder(canvas, paintOffset, strokePaint);
            DrawCheck(canvas, paintOffset, strokePaint, shadowPaint);
            break;
        }
        case UIStatus::UNSELECTED: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            break;
        }
        case UIStatus::FOCUS: { // focus of unselected
            DrawUnselected(canvas, paintOffset, focusColor_, strokePaint);
            break;
        }
        case UIStatus::OFF_TO_ON: {
            DrawActiveBorder(canvas, paintOffset, strokePaint);
            DrawAnimationOffToOn(canvas, paintOffset, strokePaint, shadowPaint);
            break;
        }
        case UIStatus::ON_TO_OFF: {
            DrawActiveBorder(canvas, paintOffset, strokePaint);
            DrawAnimationOnToOff(canvas, paintOffset, strokePaint, shadowPaint);
            break;
        }
        case UIStatus::PART: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            DrawActiveSquare(canvas, paintOffset, strokePaint);
            break;
        }
        case UIStatus::PART_TO_OFF: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            break;
        }
        case UIStatus::OFF_TO_PART: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            break;
        }
        case UIStatus::PART_TO_ON: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            break;
        }
        case UIStatus::ON_TO_PART: {
            DrawUnselected(canvas, paintOffset, inactiveColor_, strokePaint);
            break;
        }
        default:
            LOGE("unknown ui status");
            break;
    }
}

void FlutterRenderCheckbox::DrawActiveBorder(
    const ScopedCanvas& canvas, const Offset& paintOffset, flutter::Paint& strokePaint) const
{
    flutter::Paint paint;
    auto* skPaint = paint.paint();
    if (skPaint == nullptr) {
        LOGE("the sk paint is nullptr");
        return;
    }
    skPaint->setAntiAlias(true);
    skPaint->setColor(activeColor_);
    DrawBorder(canvas, paintOffset, paint, drawSize_);
    SetStrokeWidth(NormalizeToPx(checkStroke_), strokePaint);
    strokePaint.paint()->setColor(pointColor_);
    strokePaint.paint()->setStrokeCap(SkPaint::kRound_Cap);
}

void FlutterRenderCheckbox::DrawActiveSquare(
    const ScopedCanvas& canvas, const Offset& paintOffset, flutter::Paint& strokePaint) const
{
    flutter::Paint paint;
    auto* skPaint = paint.paint();
    if (skPaint == nullptr) {
        LOGE("the sk paint is nullptr");
        return;
    }
    skPaint->setAntiAlias(true);
    skPaint->setColor(activeColor_);
    DrawSquare(canvas, paintOffset, paint, drawSize_ * HALF_THE_WIDTH);
 
    SetStrokeWidth(NormalizeToPx(checkStroke_), strokePaint);
    strokePaint.paint()->setColor(pointColor_);
    strokePaint.paint()->setStrokeCap(SkPaint::kRound_Cap);
}

void FlutterRenderCheckbox::DrawSquare(
    const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const Size& paintSize) const
{
    flutter::PaintData paintData;
    flutter::RRect rrect;
    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    if (!path) {
        LOGE("path in null");
        return;
    }
    double originX = origin.GetX();
    double originY = origin.GetY();

    const Offset start =
        Offset(drawSize_.Width() * CHECK_MARK_PART_X_POSITION, drawSize_.Width() * CHECK_MARK_PART_Y_POSITION);

    double borderRadius = 0;
    rrect.sk_rrect = SkRRect::MakeRectXY(
        { originX + start.GetX(), originY + start.GetY(), paintSize.Width() + originX + start.GetX(),
            paintSize.Height() + originY + start.GetY() },
        borderRadius, borderRadius);
    canvas->drawRRect(rrect, paint, paintData);
}

void FlutterRenderCheckbox::DrawUnselected(
    const ScopedCanvas& canvas, const Offset& origin, uint32_t paintColor, flutter::Paint& paint) const
{
    Offset paintOffset = origin;
    auto borderWidth = NormalizeToPx(borderWidth_);
    SetStrokeWidth(borderWidth, paint);
    double strokeOffset = borderWidth / 2;
    paintOffset += Offset(strokeOffset, strokeOffset);
    Size paintSize = drawSize_;
    paintSize.SetWidth(paintSize.Width() - borderWidth);
    paintSize.SetHeight(paintSize.Height() - borderWidth);
    paint.paint()->setColor(paintColor);
    auto bgPaint = paint;
    bgPaint.paint()->setColor(inactivePointColor_);
    bgPaint.paint()->setStyle(SkPaint::Style::kFill_Style);
    DrawBorder(canvas, paintOffset, bgPaint, paintSize);
    DrawBorder(canvas, paintOffset, paint, paintSize);
}

void FlutterRenderCheckbox::DrawCheck(const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint,
    const flutter::Paint& shadowPaint) const
{
    flutter::PaintData paintData;
    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    if (!path) {
        LOGE("path in null");
        return;
    }
    double originX = origin.GetX();
    double originY = origin.GetY();
    const Offset start =
        Offset(drawSize_.Width() * CHECK_MARK_START_X_POSITION, drawSize_.Width() * CHECK_MARK_START_Y_POSITION);
    const Offset middle =
        Offset(drawSize_.Width() * CHECK_MARK_MIDDLE_X_POSITION, drawSize_.Width() * CHECK_MARK_MIDDLE_Y_POSITION);
    const Offset end =
        Offset(drawSize_.Width() * CHECK_MARK_END_X_POSITION, drawSize_.Width() * CHECK_MARK_END_Y_POSITION);

    path->moveTo(originX + start.GetX(), originY + start.GetY());
    path->lineTo(originX + middle.GetX(), originY + middle.GetY());

    canvas->drawPath(path.get(), shadowPaint, paintData);
    canvas->drawPath(path.get(), paint, paintData);
    path->moveTo(originX + middle.GetX(), originY + middle.GetY());
    path->lineTo(originX + end.GetX(), originY + end.GetY());

    canvas->drawPath(path.get(), shadowPaint, paintData);
    canvas->drawPath(path.get(), paint, paintData);
}

void FlutterRenderCheckbox::DrawBorder(
    const ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const Size& paintSize) const
{
    flutter::PaintData paintData;
    flutter::RRect rrect;
    double originX = origin.GetX();
    double originY = origin.GetY();
    double borderRadius = NormalizeToPx(borderRadius_);
    rrect.sk_rrect = SkRRect::MakeRectXY(
        { originX, originY, paintSize.Width() + originX, paintSize.Height() + originY }, borderRadius, borderRadius);
    canvas->drawRRect(rrect, paint, paintData);
}

void FlutterRenderCheckbox::DrawAnimationOffToOn(
    ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const flutter::Paint& shadowPaint) const
{
    flutter::PaintData paintData;
    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    if (!path) {
        LOGE("path in null");
        return;
    }
    double originX = origin.GetX();
    double originY = origin.GetY();
    const Offset start =
        Offset(drawSize_.Width() * CHECK_MARK_START_X_POSITION, drawSize_.Width() * CHECK_MARK_START_Y_POSITION);
    const Offset middle =
        Offset(drawSize_.Width() * CHECK_MARK_MIDDLE_X_POSITION, drawSize_.Width() * CHECK_MARK_MIDDLE_Y_POSITION);
    const Offset end =
        Offset(drawSize_.Width() * CHECK_MARK_END_X_POSITION, drawSize_.Width() * CHECK_MARK_END_Y_POSITION);

    double deltaX = middle.GetX() - start.GetX();
    double deltaY = middle.GetY() - start.GetY();
    path->moveTo(originX + start.GetX(), originY + start.GetY());
    double ratio = DEFAULT_MIN_CHECKBOX_SHAPE_SCALE;
    if (shapeScale_ < CHECK_MARK_LEFT_ANIMATION_PERCENT) {
        ratio = shapeScale_ / CHECK_MARK_LEFT_ANIMATION_PERCENT;
        path->lineTo(originX + start.GetX() + deltaX * ratio, originY + start.GetY() + deltaY * ratio);
    } else {
        path->lineTo(originX + middle.GetX(), originY + middle.GetY());
    }
    canvas->drawPath(path.get(), shadowPaint, paintData);
    canvas->drawPath(path.get(), paint, paintData);
    if (shapeScale_ > CHECK_MARK_LEFT_ANIMATION_PERCENT) {
        deltaX = end.GetX() - middle.GetX();
        deltaY = middle.GetY() - end.GetY();
        path->moveTo(originX + middle.GetX(), originY + middle.GetY());
        if (shapeScale_ == DEFAULT_MAX_CHECKBOX_SHAPE_SCALE) {
            path->lineTo(originX + end.GetX(), originY + end.GetY());
        } else {
            ratio = (shapeScale_ - CHECK_MARK_LEFT_ANIMATION_PERCENT) / CHECK_MARK_RIGHT_ANIMATION_PERCENT;
            path->lineTo(originX + middle.GetX() + deltaX * ratio, originY + middle.GetY() - deltaY * ratio);
        }
        canvas->drawPath(path.get(), shadowPaint, paintData);
        canvas->drawPath(path.get(), paint, paintData);
    }
}

void FlutterRenderCheckbox::DrawAnimationOnToOff(
    ScopedCanvas& canvas, const Offset& origin, const flutter::Paint& paint, const flutter::Paint& shadowPaint) const
{
    if (shapeScale_ == DEFAULT_MIN_CHECKBOX_SHAPE_SCALE) {
        return;
    }
    flutter::PaintData paintData;
    fml::RefPtr<flutter::CanvasPath> path = flutter::CanvasPath::Create();
    if (!path) {
        LOGE("path in null");
        return;
    }
    double originX = origin.GetX();
    double originY = origin.GetY();
    const Offset start =
        Offset(drawSize_.Width() * CHECK_MARK_START_X_POSITION, drawSize_.Width() * CHECK_MARK_START_Y_POSITION);
    const Offset middle =
        Offset(drawSize_.Width() * CHECK_MARK_MIDDLE_X_POSITION, drawSize_.Width() * CHECK_MARK_MIDDLE_Y_POSITION);
    const Offset end =
        Offset(drawSize_.Width() * CHECK_MARK_END_X_POSITION, drawSize_.Width() * CHECK_MARK_END_Y_POSITION);
    const Offset middlePoint = Offset(drawSize_.Width() / 2, drawSize_.Height() / 2);
    double deltaX = middlePoint.GetX() - start.GetX();
    double deltaY = middlePoint.GetY() - start.GetY();
    double ratio = DEFAULT_MAX_CHECKBOX_SHAPE_SCALE - shapeScale_;
    path->moveTo(originX + start.GetX() + deltaX * ratio, originY + start.GetY() + deltaY * ratio);
    deltaX = middlePoint.GetX() - middle.GetX();
    deltaY = middle.GetY() - middlePoint.GetY();
    path->lineTo(originX + middle.GetX() + deltaX * ratio, originY + middle.GetY() - deltaY * ratio);
    canvas->drawPath(path.get(), shadowPaint, paintData);
    canvas->drawPath(path.get(), paint, paintData);
    path->moveTo(originX + middle.GetX() + deltaX * ratio, originY + middle.GetY() - deltaY * ratio);
    deltaX = end.GetX() - middlePoint.GetX();
    deltaY = middlePoint.GetY() - end.GetY();
    path->lineTo(originX + end.GetX() - deltaX * ratio, originY + end.GetY() + deltaY * ratio);
    canvas->drawPath(path.get(), shadowPaint, paintData);
    canvas->drawPath(path.get(), paint, paintData);
}

} // namespace OHOS::Ace