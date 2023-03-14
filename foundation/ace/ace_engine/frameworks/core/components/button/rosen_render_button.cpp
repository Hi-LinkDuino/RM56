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

#include "core/components/button/rosen_render_button.h"

#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkRRect.h"

#include "core/components/box/render_box.h"
#include "core/pipeline/base/rosen_render_context.h"
#ifdef OHOS_PLATFORM
#include "core/components/common/painter/rosen_svg_painter.h"
#endif
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {
namespace {

// Definition for arc button of watch which intersected by circle and ellipse.
constexpr Dimension CIRCLE_DIAMETER = 233.0_vp;
constexpr Dimension OVAL_WIDTH = 260.0_vp;
constexpr Dimension OVAL_HEIGHT = 98.0_vp;
constexpr Dimension OFFSET_X = (OVAL_WIDTH - ARC_BUTTON_WIDTH) / 2.0;
constexpr Dimension OFFSET_Y = CIRCLE_DIAMETER - ARC_BUTTON_HEIGHT;
constexpr double CIRCLE_START_ANGLE = 0.759;
constexpr double CIRCLE_SWEEP_ANGLE = M_PI - CIRCLE_START_ANGLE * 2;
constexpr double OVAL_START_ANGLE = 4.0;
constexpr double OVAL_SWEEP_ANGLE = M_PI * 3 - OVAL_START_ANGLE * 2;
constexpr double RADIAN_TO_DEGREE = 180.0 / M_PI;

// Definition for download button in watch
constexpr Dimension CIRCLE_PROGRESS_THICKNESS = 2.0_vp;
constexpr Dimension WATCH_DOWNLOAD_SIZE_DELTA = 8.0_vp;
constexpr double PROGRESS_START_ANGLE = 1.5 * M_PI;

// Definition for animation
constexpr uint8_t DEFAULT_OPACITY = 255;

} // namespace

void RosenRenderButton::PerformLayout()
{
    RenderButton::PerformLayout();

    UpdateLayer();
}

void RosenRenderButton::UpdateLayer()
{
    float translateX = GetLayoutSize().Width() / 2 * (INIT_SCALE - scale_);
    // The bottom of the component must be close to the bottom of the circle when the type is arc.
    // The center point deviates 2 times downward.
    float translateY = (buttonComponent_->GetType() == ButtonType::ARC)
                        ? GetLayoutSize().Height() * (INIT_SCALE - scale_) * 2
                        : GetLayoutSize().Height() / 2 * (1.0 - scale_);
    Matrix4 translateMatrix = Matrix4::CreateTranslate(translateX, translateY, 0.0);
    Matrix4 scaleMatrix = Matrix4::CreateScale(scale_, scale_, 1.0);
    Matrix4 transformMatrix = translateMatrix * scaleMatrix;

    transformLayer_ = transformMatrix;
    opacityLayer_ = DEFAULT_OPACITY * opacity_;
}

void RosenRenderButton::PaintLayer(SkCanvas* canvas)
{
#ifdef OHOS_PLATFORM
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    recordingCanvas->MultiplyAlpha(opacityLayer_ / 255.0f);
    recordingCanvas->concat(RosenSvgPainter::ToSkMatrix(transformLayer_));
#endif
}

void RosenRenderButton::Paint(RenderContext& context, const Offset& offset)
{
    if (!buttonComponent_) {
        return;
    }
    LOGD("Paint button type : %{public}d", buttonComponent_->GetType());
    if (isHover_) {
        UpdateLayer();
        isHover_ = false;
    }
    PaintButtonAnimation();
    auto canvas = static_cast<RosenRenderContext&>(context).GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }
    PaintLayer(canvas);
    if (buttonComponent_->GetType() == ButtonType::ICON) {
        RenderNode::Paint(context, offset);
        return;
    }
    DrawButton(canvas, offset);
    RenderNode::Paint(context, offset);
}

void RosenRenderButton::PaintButtonAnimation()
{
    if (!animationRunning_) {
        return;
    }
    UpdateLayer();
    if (isLastFrame_) {
        animationRunning_ = false;
        isOpacityAnimation_ = false;
        isLastFrame_ = false;
        isTouchAnimation_ = false;
    }
}

Size RosenRenderButton::Measure()
{
    // Layout size need includes border width, the border width is half outside of button,
    // total width and height needs to add border width defined by user.
    if (!buttonComponent_) {
        return Size();
    }
    widthDelta_ = NormalizeToPx(buttonComponent_->GetBorderEdge().GetWidth());
    double delta = widthDelta_ / 2;
    offsetDelta_ = Offset(delta, delta);
    if (buttonComponent_->GetType() == ButtonType::ARC) {
        return buttonSize_ + Size(widthDelta_, widthDelta_);
    }
    if (NeedLayoutExtendToParant()) {
        buttonSize_ = GetLayoutParam().GetMaxSize();
    }
    MeasureButtonSize();
    if (buttonComponent_->GetType() == ButtonType::NORMAL) {
        if (buttonComponent_->GetDeclarativeFlag()) {
            ResetBoxRadius();
        }
        if (!buttonComponent_->GetRadiusState() && buttonComponent_->IsInputButton()) {
            rrectRadius_ = buttonSize_.Height() / 2.0;
        }
    }
    return buttonSize_ + Size(widthDelta_, widthDelta_);
}

void RosenRenderButton::MeasureButtonSize()
{
    if (buttonComponent_->GetType() == ButtonType::ICON) {
        return;
    }
    if (NearEqual(GetLayoutParam().GetMaxSize().Width(), Size::INFINITE_SIZE) || (!widthDefined_)) {
        buttonSize_.SetWidth(0.0);
    }
    if (buttonComponent_->GetType() == ButtonType::CAPSULE) {
        MeasureCapsule();
        return;
    }
    if (buttonComponent_->GetType() == ButtonType::CIRCLE) {
        MeasureCircle();
        return;
    }
    if (isWatch_ && (buttonComponent_->GetType() == ButtonType::DOWNLOAD)) {
        if (buttonComponent_->GetRadiusState() || widthDefined_ || heightDefined_) {
            MeasureCircle();
            progressDiameter_ = rrectRadius_ * 2 - NormalizeToPx(WATCH_DOWNLOAD_SIZE_DELTA);
        } else {
            buttonSize_ = Size(progressDiameter_ + NormalizeToPx(WATCH_DOWNLOAD_SIZE_DELTA),
                progressDiameter_ + NormalizeToPx(WATCH_DOWNLOAD_SIZE_DELTA));
        }
    }
}

void RosenRenderButton::MeasureCapsule()
{
    if (GreatNotEqual(rrectRadius_, buttonSize_.Height() / 2.0)) {
        return;
    }
    rrectRadius_ = buttonSize_.Height() / 2.0;
    ResetBoxRadius();
}

void RosenRenderButton::MeasureCircle()
{
    if (!buttonComponent_->GetRadiusState()) {
        if (widthDefined_ || heightDefined_) {
            double minSize = std::min(GetLayoutParam().GetMaxSize().Width(), GetLayoutParam().GetMaxSize().Height());
            if (buttonComponent_->GetDeclarativeFlag()) {
                minSize = widthDefined_ ? std::min(buttonSize_.Width(), buttonSize_.Height()) : buttonSize_.Height();
            }
            rrectRadius_ = (minSize - widthDelta_) / 2.0;
        }
    } else {
        auto constrainedSize =
            GetLayoutParam().Constrain(Size(rrectRadius_ * 2.0 + widthDelta_, rrectRadius_ * 2.0 + widthDelta_));
        rrectRadius_ = (std::min(constrainedSize.Width(), constrainedSize.Height()) - widthDelta_) / 2.0;
    }
    buttonSize_.SetWidth(rrectRadius_ * 2.0);
    buttonSize_.SetHeight(rrectRadius_ * 2.0);
    ResetBoxRadius();
}

void RosenRenderButton::ResetBoxRadius()
{
    if (!buttonComponent_->GetRadiusState() && buttonComponent_->GetDeclarativeFlag()) {
        return;
    }
    auto parent = GetParent().Upgrade();
    if (!parent) {
        return;
    }
    auto box = AceType::DynamicCast<RenderBox>(parent);
    if (box) {
        auto backDecoration = box->GetBackDecoration();
        if (backDecoration) {
            auto border = backDecoration->GetBorder();
            backDecoration->SetBorderRadius(Radius(rrectRadius_ + NormalizeToPx(border.Top().GetWidth())));
        }
    }
}

void RosenRenderButton::DrawShape(SkCanvas* canvas, const Offset& offset, bool isStroke)
{
    SkPaint paint;
    if (isStroke) {
        uint32_t focusColorValue = buttonComponent_->GetFocusColor().GetValue();
        uint32_t borderColorValue = buttonComponent_->GetBorderEdge().GetColor().GetValue();
        paint.setColor(needFocusColor_ ? focusColorValue : borderColorValue);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        paint.setStrokeWidth(NormalizeToPx(borderEdge_.GetWidth()));
    } else {
        paint.setColor(GetStateColor());
        paint.setStyle(SkPaint::Style::kFill_Style);
    }
    paint.setAntiAlias(true);
    SkRRect rRect;

    if (buttonComponent_->GetType() == ButtonType::CUSTOM) {
        ConvertToSkVector(buttonComponent_->GetRectRadii(), radii_);
        rRect.setRectRadii(SkRect::MakeIWH(buttonSize_.Width(), buttonSize_.Height()), radii_);
    } else {
        rRect.setRectXY(SkRect::MakeIWH(buttonSize_.Width(), buttonSize_.Height()), rrectRadius_, rrectRadius_);
    }
    rRect.offset(offset.GetX(), offset.GetY());

#ifdef OHOS_PLATFORM
    auto recordingCanvas = static_cast<Rosen::RSRecordingCanvas*>(canvas);
    recordingCanvas->DrawAdaptiveRRect(rRect.getSimpleRadii().x(), paint);
#else
    canvas->drawRRect(rRect, paint);
#endif
}

void RosenRenderButton::DrawArc(SkCanvas* canvas, const Offset& offset)
{
    double offsetDelta = NormalizeToPx((OVAL_WIDTH - CIRCLE_DIAMETER)) / 2;
    SkPath arcPath;
    arcPath.addArc({ 0, NormalizeToPx(OFFSET_Y), NormalizeToPx(OVAL_WIDTH), NormalizeToPx(OVAL_HEIGHT + OFFSET_Y) },
        OVAL_START_ANGLE * RADIAN_TO_DEGREE, OVAL_SWEEP_ANGLE * RADIAN_TO_DEGREE);
    arcPath.addArc({ offsetDelta, 0, NormalizeToPx(CIRCLE_DIAMETER) + offsetDelta, NormalizeToPx(CIRCLE_DIAMETER) },
        CIRCLE_START_ANGLE * RADIAN_TO_DEGREE, CIRCLE_SWEEP_ANGLE * RADIAN_TO_DEGREE);
    arcPath.offset(offset.GetX() - NormalizeToPx(OFFSET_X), offset.GetY() - NormalizeToPx(OFFSET_Y));

    SkPaint paint;
    paint.setColor(GetStateColor());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    canvas->drawPath(arcPath, paint);
}

void RosenRenderButton::DrawLineProgress(SkCanvas* canvas, const Offset& offset)
{
    SkPaint paint;
    paint.setColor(needFocusColor_ ? progressFocusColor_.GetValue() : progressColor_.GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    SkRRect rRect;
    rRect.setRectXY(SkRect::MakeWH(buttonSize_.Width(), buttonSize_.Height()), rrectRadius_, rrectRadius_);
    rRect.offset(offset.GetX(), offset.GetY());
    canvas->save();
    canvas->clipRRect(rRect, true);
    canvas->drawRect({ offset.GetX(), offset.GetY(), progressWidth_, buttonSize_.Height() }, paint);
    canvas->restore();
}

void RosenRenderButton::DrawLineProgressAnimation(SkCanvas* canvas, const Offset& offset)
{
    double offsetX = offset.GetX();
    double offsetY = offset.GetY();
    double radius = buttonSize_.Height() / 2.0;
    SkPath path;
    path.addArc({ offsetX, offsetY, buttonSize_.Height() + offsetX, buttonSize_.Height() + offsetY }, 90, 180);
    if (LessNotEqual(progressWidth_, radius)) {
        path.addArc({ progressWidth_ + offsetX, offsetY, buttonSize_.Height() - progressWidth_ + offsetX,
                        buttonSize_.Height() + offsetY },
            270, -180);
    } else if (GreatNotEqual(progressWidth_, buttonSize_.Width() - radius)) {
        path.addRect(
            { radius + offsetX, offsetY, buttonSize_.Width() - radius + offsetX, buttonSize_.Height() + offsetY });
        path.addArc({ (buttonSize_.Width() - radius) * 2.0 - progressWidth_ + offsetX, offsetY,
                        progressWidth_ + offsetX, buttonSize_.Height() + offsetY },
            270, 180);
    } else {
        path.addRect({ radius + offsetX, offsetY, progressWidth_ + offsetX, buttonSize_.Height() + offsetY });
    }
    SkPaint paint;
    paint.setColor(progressColor_.GetValue());
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setAntiAlias(true);
    canvas->drawPath(path, paint);
}

void RosenRenderButton::DrawCircleProgress(SkCanvas* canvas, const Offset& offset)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(progressColor_.GetValue());
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setStrokeWidth(NormalizeToPx(CIRCLE_PROGRESS_THICKNESS));
    paint.setStrokeCap(SkPaint::kRound_Cap);
    canvas->drawArc(
        { offset.GetX(), offset.GetY(), progressDiameter_ + offset.GetX(), progressDiameter_ + offset.GetY() },
        PROGRESS_START_ANGLE * RADIAN_TO_DEGREE, 360 * progressPercent_, false, paint);
}

void RosenRenderButton::DrawDownloadButton(SkCanvas* canvas, const Offset& offset)
{
    if (isWatch_) {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::Style::kFill_Style);
        canvas->save();
        paint.setColor(GetStateColor());
        canvas->drawCircle(offset.GetX() + buttonSize_.Width() / 2, offset.GetY() + buttonSize_.Height() / 2,
            (progressDiameter_ + NormalizeToPx(WATCH_DOWNLOAD_SIZE_DELTA)) / 2, paint);
        canvas->restore();
        if (progressDisplay_) {
            DrawCircleProgress(canvas, offset + Offset((buttonSize_.Width() - progressDiameter_) / 2,
                (buttonSize_.Height() - progressDiameter_) / 2));
        }
        return;
    }

    DrawShape(canvas, offset);
    if (isPhone_) {
        DrawShape(canvas, offset, true);
        if (progressDisplay_) {
            if (GreatOrEqual(rrectRadius_, buttonSize_.Height() / 2.0)) {
                DrawLineProgressAnimation(canvas, offset);
            } else {
                DrawLineProgress(canvas, offset);
            }
        }
        return;
    }
    if (progressDisplay_) {
        DrawShape(canvas, offset, true);
        DrawLineProgress(canvas, offset);
    }
}

void RosenRenderButton::DrawButton(SkCanvas* canvas, const Offset& inOffset)
{
    Offset offset = inOffset + offsetDelta_;
    if (buttonComponent_->GetType() == ButtonType::ARC) {
        DrawArc(canvas, offset);
        return;
    }
    if (buttonComponent_->GetType() == ButtonType::DOWNLOAD) {
        DrawDownloadButton(canvas, offset);
        return;
    }

    // Paint button with border
    if (NormalizeToPx(buttonComponent_->GetBorderEdge().GetWidth()) > 0.0) {
        DrawShape(canvas, offset);
        DrawShape(canvas, offset, true);
        return;
    }
    DrawShape(canvas, offset);
}

uint32_t RosenRenderButton::GetStateColor()
{
    if (!buttonComponent_) {
        return Color().GetValue();
    }
    if (!buttonComponent_->GetStateEffect()) {
        return buttonComponent_->GetBackgroundColor().GetValue();
    }
    if (needHoverColor_) {
        return buttonComponent_->GetHoverColor().GetValue();
    }
    if (buttonComponent_->GetDisabledState()) {
        return buttonComponent_->GetDisabledColor().GetValue();
    }
    if (needFocusColor_) {
        return buttonComponent_->GetFocusColor().GetValue();
    }
    Color backgroundColor = buttonComponent_->GetBackgroundColor();
    if (NeedClickedColor(backgroundColor)) {
        return isClicked_ ? clickedColor_.GetValue() : backgroundColor.GetValue();
    }
    if (!isMoveEventValid_) {
        maskingOpacity_ = 0.0;
    }
    uint32_t animationColor;
    if (isWatch_) {
        animationColor = backgroundColor.BlendColor(Color::WHITE.ChangeOpacity(maskingOpacity_)).GetValue();
    } else {
        animationColor = backgroundColor.BlendColor(Color::BLACK.ChangeOpacity(maskingOpacity_)).GetValue();
    }
    return animationColor;
}

bool RosenRenderButton::NeedClickedColor(const Color& backgroundColor)
{
    if (setClickColor_) {
        return true;
    }
    if (clickedColor_ != defaultClickedColor_) {
        return true;
    } else {
        if (backgroundColor == Color::TRANSPARENT) {
            return true;
        }
    }
    return false;
}

bool RosenRenderButton::HasEffectiveTransform() const
{
    return scale_ != INIT_SCALE;
}

void RosenRenderButton::ConvertToSkVector(const std::array<Radius, 4>& radii, SkVector* skRadii)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    double dipScale = context->GetDipScale();
    for (int32_t i = 0; i < 4; ++i) {
        skRadii[i].set(SkDoubleToScalar(std::max(radii[i].GetX().ConvertToPx(dipScale), 0.0)),
            SkDoubleToScalar(std::max(radii[i].GetY().ConvertToPx(dipScale), 0.0)));
    }
}

} // namespace OHOS::Ace
