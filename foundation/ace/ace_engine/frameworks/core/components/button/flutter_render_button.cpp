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

#include "core/components/button/flutter_render_button.h"

#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/components/box/render_box.h"
#include "core/components/transform/flutter_render_transform.h"
#include "core/pipeline/base/flutter_render_context.h"

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

// Definition for download button in watch
constexpr Dimension CIRCLE_PROGRESS_THICKNESS = 2.0_vp;
constexpr Dimension WATCH_DOWNLOAD_SIZE_DELTA = 8.0_vp;
constexpr double PROGRESS_START_ANGLE = 1.5 * M_PI;

// Definition for animation
constexpr uint8_t DEFAULT_OPACITY = 255;

} // namespace

RenderLayer FlutterRenderButton::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderButton::OnGlobalPositionChanged()
{
    UpdateLayer();
    RenderNode::OnGlobalPositionChanged();
}

void FlutterRenderButton::UpdateLayer()
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
    auto offset = GetGlobalOffset();
    transformMatrix = FlutterRenderTransform::GetTransformByOffset(transformMatrix, offset);
    if (transformLayer_) {
        transformLayer_->Update(transformMatrix);
    }
    if (opacityLayer_) {
        opacityLayer_->SetOpacity(DEFAULT_OPACITY * opacity_, 0.0, 0.0);
    }
}

void FlutterRenderButton::Paint(RenderContext& context, const Offset& offset)
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
    flutter::Canvas* canvas = static_cast<FlutterRenderContext&>(context).GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }
    if (buttonComponent_->GetType() == ButtonType::ICON) {
        RenderNode::Paint(context, offset);
        return;
    }
    DrawButton(*canvas, offset);
    RenderNode::Paint(context, offset);
}

void FlutterRenderButton::PaintButtonAnimation()
{
    if (!animationRunning_) {
        return;
    }
    if (!opacityLayer_ && transformLayer_) {
        opacityLayer_ = AceType::MakeRefPtr<Flutter::OpacityLayer>(DEFAULT_OPACITY, 0.0, 0.0);
        transformLayer_->AddChildren(opacityLayer_);
    }
    UpdateLayer();
    if (isLastFrame_) {
        animationRunning_ = false;
        isOpacityAnimation_ = false;
        isLastFrame_ = false;
        isTouchAnimation_ = false;
    }
}

Size FlutterRenderButton::Measure()
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

void FlutterRenderButton::MeasureButtonSize()
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

void FlutterRenderButton::MeasureCapsule()
{
    if (GreatNotEqual(rrectRadius_, buttonSize_.Height() / 2.0)) {
        return;
    }
    rrectRadius_ = buttonSize_.Height() / 2.0;
    ResetBoxRadius();
}

void FlutterRenderButton::MeasureCircle()
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

void FlutterRenderButton::ResetBoxRadius()
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

void FlutterRenderButton::DrawShape(flutter::Canvas& canvas, const Offset& offset, bool isStroke)
{
    flutter::Paint paint;
    if (isStroke) {
        uint32_t focusColorValue = buttonComponent_->GetFocusColor().GetValue();
        uint32_t borderColorValue = buttonComponent_->GetBorderEdge().GetColor().GetValue();
        paint.paint()->setColor(needFocusColor_ ? focusColorValue : borderColorValue);
        paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
        paint.paint()->setStrokeWidth(NormalizeToPx(borderEdge_.GetWidth()));
    } else {
        paint.paint()->setColor(GetStateColor());
        paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    }
    paint.paint()->setAntiAlias(true);
    flutter::RRect rRect;
    flutter::PaintData paintData;

    if (buttonComponent_->GetType() == ButtonType::CUSTOM) {
        ConvertToSkVector(buttonComponent_->GetRectRadii(), radii_);
        rRect.sk_rrect.setRectRadii(SkRect::MakeIWH(buttonSize_.Width(), buttonSize_.Height()), radii_);
    } else {
        rRect.sk_rrect.setRectXY(
            SkRect::MakeIWH(buttonSize_.Width(), buttonSize_.Height()), rrectRadius_, rrectRadius_);
    }
    rRect.sk_rrect.offset(offset.GetX(), offset.GetY());
    canvas.drawRRect(rRect, paint, paintData);
}

void FlutterRenderButton::DrawArc(flutter::Canvas& canvas, const Offset& offset)
{
    canvas.save();
    canvas.translate(offset.GetX() - NormalizeToPx(OFFSET_X), offset.GetY() - NormalizeToPx(OFFSET_Y));
    double offsetDelta = NormalizeToPx((OVAL_WIDTH - CIRCLE_DIAMETER)) / 2;
    auto arcPath = flutter::CanvasPath::Create();
    arcPath->addArc(0, NormalizeToPx(OFFSET_Y), NormalizeToPx(OVAL_WIDTH), NormalizeToPx(OVAL_HEIGHT + OFFSET_Y),
        OVAL_START_ANGLE, OVAL_SWEEP_ANGLE);
    arcPath->addArc(offsetDelta, 0, NormalizeToPx(CIRCLE_DIAMETER) + offsetDelta, NormalizeToPx(CIRCLE_DIAMETER),
        CIRCLE_START_ANGLE, CIRCLE_SWEEP_ANGLE);
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setColor(GetStateColor());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    canvas.drawPath(arcPath.get(), paint, paintData);
    canvas.restore();
}

void FlutterRenderButton::DrawLineProgress(flutter::Canvas& canvas, const Offset& offset)
{
    flutter::Paint paint;
    paint.paint()->setColor(needFocusColor_ ? progressFocusColor_.GetValue() : progressColor_.GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    flutter::RRect rRect;
    rRect.sk_rrect.setRectXY(SkRect::MakeIWH(buttonSize_.Width(), buttonSize_.Height()), rrectRadius_, rrectRadius_);
    rRect.sk_rrect.offset(offset.GetX(), offset.GetY());
    flutter::PaintData paintData;
    canvas.save();
    canvas.clipRRect(rRect, true);
    canvas.drawRect(offset.GetX(), offset.GetY(), progressWidth_ + offset.GetX(), buttonSize_.Height() + offset.GetY(),
        paint, paintData);
    canvas.restore();
}

void FlutterRenderButton::DrawLineProgressAnimation(flutter::Canvas& canvas, const Offset& offset)
{
    double offsetX = offset.GetX();
    double offsetY = offset.GetY();
    double radius = buttonSize_.Height() / 2.0;
    auto path = flutter::CanvasPath::Create();
    path->addArc(offsetX, offsetY, buttonSize_.Height() + offsetX, buttonSize_.Height() + offsetY, M_PI * 0.5, M_PI);
    if (LessNotEqual(progressWidth_, radius)) {
        path->addArc(progressWidth_ + offsetX, offsetY, buttonSize_.Height() - progressWidth_ + offsetX,
            buttonSize_.Height() + offsetY, M_PI * 1.5, -M_PI);
    } else if (GreatNotEqual(progressWidth_, buttonSize_.Width() - radius)) {
        path->addRect(radius + offsetX, offsetY, buttonSize_.Width() - radius + offsetX,
            buttonSize_.Height() + offsetY);
        path->addArc((buttonSize_.Width() - radius) * 2.0 - progressWidth_ + offsetX, offsetY,
            progressWidth_ + offsetX, buttonSize_.Height() + offsetY, M_PI * 1.5, M_PI);
    } else {
        path->addRect(radius + offsetX, offsetY, progressWidth_ + offsetX, buttonSize_.Height() + offsetY);
    }
    flutter::Paint paint;
    paint.paint()->setColor(progressColor_.GetValue());
    paint.paint()->setStyle(SkPaint::Style::kFill_Style);
    paint.paint()->setAntiAlias(true);
    flutter::PaintData paintData;
    canvas.drawPath(path.get(), paint, paintData);
}

void FlutterRenderButton::DrawCircleProgress(flutter::Canvas& canvas, const Offset& offset)
{
    flutter::Paint paint;
    flutter::PaintData paintData;
    paint.paint()->setAntiAlias(true);
    paint.paint()->setColor(progressColor_.GetValue());
    paint.paint()->setStyle(SkPaint::Style::kStroke_Style);
    paint.paint()->setStrokeWidth(NormalizeToPx(CIRCLE_PROGRESS_THICKNESS));
    paint.paint()->setStrokeCap(SkPaint::kRound_Cap);
    canvas.drawArc(offset.GetX(), offset.GetY(), progressDiameter_ + offset.GetX(), progressDiameter_ + offset.GetY(),
        PROGRESS_START_ANGLE, 2 * M_PI * progressPercent_, false, paint, paintData);
}

void FlutterRenderButton::DrawDownloadButton(flutter::Canvas& canvas, const Offset& offset)
{
    if (isWatch_) {
        flutter::Paint paint;
        flutter::PaintData paintData;
        paint.paint()->setAntiAlias(true);
        paint.paint()->setStyle(SkPaint::Style::kFill_Style);
        canvas.save();
        paint.paint()->setColor(GetStateColor());
        canvas.drawCircle(offset.GetX() + buttonSize_.Width() / 2, offset.GetY() + buttonSize_.Height() / 2,
            (progressDiameter_ + NormalizeToPx(WATCH_DOWNLOAD_SIZE_DELTA)) / 2, paint, paintData);
        canvas.restore();
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

void FlutterRenderButton::DrawButton(flutter::Canvas& canvas, const Offset& inOffset)
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

uint32_t FlutterRenderButton::GetStateColor()
{
    if (!buttonComponent_) {
        return Color().GetValue();
    }
    if (!buttonComponent_->GetStateEffect()) {
        return backgroundColor_.GetValue();
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
    Color backgroundColor = backgroundColor_;
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

bool FlutterRenderButton::NeedClickedColor(const Color& backgroundColor)
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

bool FlutterRenderButton::HasEffectiveTransform() const
{
    if (!transformLayer_) {
        return false;
    }
    return !transformLayer_->GetMatrix4().IsIdentityMatrix();
}

void FlutterRenderButton::ConvertToSkVector(const std::array<Radius, 4>& radii, SkVector* skRadii)
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
