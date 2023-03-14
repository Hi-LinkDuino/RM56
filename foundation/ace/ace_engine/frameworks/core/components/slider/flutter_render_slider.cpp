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

#include "core/components/slider/flutter_render_slider.h"

#include "flutter/lib/ui/ui_dart_state.h"

#include "core/components/box/render_box.h"
#include "core/components/common/properties/color.h"
#include "core/components/slider/render_block.h"
#include "core/components/tip/render_tip.h"
#include "core/components/tip/tip_component.h"
#include "core/components/track/render_track.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

namespace {

constexpr Dimension FOCUS_PADDING = 2.0_vp;
constexpr double DOUBLE_TO_PERCENT = 100.0;
constexpr int32_t HOVER_ANIMATION_DURATION = 250;

} // namespace

void FlutterRenderSlider::Update(const RefPtr<Component>& component)
{
    RenderSlider::Update(component);
    if (initialUpdate_) {
        track_ = RenderTrack::Create();
        AddChild(track_);
        track_->Attach(GetContext());

        block_ = RenderBlock::Create();
        AddChild(block_);
        block_->Attach(GetContext());

        initialUpdate_ = false;
    }

    AddTipChild();
    auto slider = AceType::DynamicCast<SliderComponent>(component);
    auto block = slider->GetBlock();
    auto blockSize = block->GetBlockSize();
    double scale = GreatNotEqual(scaleValue_, 0) ? scaleValue_ : 1;
    block->SetBlockSize(blockSize * scale);
    block_->Update(slider->GetBlock());
    hotWidth_ = slider->GetBlock()->GetHotRegionWidth();
    blockHotWidth_ = GreatNotEqual(scaleValue_, 0) ? slider->GetBlock()->GetHotRegionWidth() * scaleValue_ :
        slider->GetBlock()->GetHotRegionWidth();
    blockHotHeight_ = GreatNotEqual(scaleValue_, 0) ? slider->GetBlock()->GetHotRegionHeight() * scaleValue_ :
        slider->GetBlock()->GetHotRegionHeight();
    auto track = slider->GetTrack();
    auto thickness = track->GetTrackThickness();
    track->SetTrackThickness(thickness * scaleValue_);
    track_->Update(track);

    LOGD("Slider::Update end");
    MarkNeedLayout();
}

void FlutterRenderSlider::PerformLayout()
{
    RenderSlider::PerformLayout();

    LOGD("Slider::PerformLayout totalRatio_:%{public}lf, trackLength:%{public}lf", totalRatio_, trackLength_);
    if (direction_ == Axis::VERTICAL) {
        double dxOffset = GetLayoutSize().Width() * HALF;
        double dyOffset = isReverse_ ? GetLayoutSize().Height() - NormalizeToPx(SLIDER_PADDING_DP) -
            trackLength_ * totalRatio_ : NormalizeToPx(SLIDER_PADDING_DP) + trackLength_ * totalRatio_;
        ProcessBlock(Offset(dxOffset, dyOffset));
        ProcessTrack(Offset(dxOffset, dyOffset));
        SetTipPosition(dyOffset);
    } else {
        double dxOffset = 0.0;
        if ((GetTextDirection() == TextDirection::LTR &&
            !isReverse_) || (GetTextDirection() == TextDirection::RTL && isReverse_)) {
            dxOffset = NormalizeToPx(SLIDER_PADDING_DP) + trackLength_ * totalRatio_;
        } else if ((GetTextDirection() == TextDirection::RTL &&
            !isReverse_) || (GetTextDirection() == TextDirection::LTR && isReverse_)) {
            dxOffset = GetLayoutSize().Width() - NormalizeToPx(SLIDER_PADDING_DP) - trackLength_ * totalRatio_;
        }
        double dyOffset = GetLayoutSize().Height() * HALF;
        ProcessBlock(Offset(dxOffset, dyOffset));
        ProcessTrack(Offset(dxOffset, dyOffset));
        SetTipPosition(dxOffset);
    }
}

RenderLayer FlutterRenderSlider::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderSlider::AddTipChild()
{
    if (!showTips_) {
        return;
    }

    if (tip_) {
        return;
    }

    RefPtr<SliderTheme> theme = GetTheme<SliderTheme>();
    if (!theme) {
        return;
    }

    int32_t percent = std::round(totalRatio_ * DOUBLE_TO_PERCENT);
    tipText_ = AceType::MakeRefPtr<TextComponent>(std::to_string(percent).append("%"));
    TextStyle textStyle;
    textStyle.SetTextAlign(TextAlign::CENTER);
    textStyle.SetFontSize(theme->GetTipFontSize());
    textStyle.SetTextColor(theme->GetTipTextColor());
    tipText_->SetFocusColor(theme->GetTipTextColor());
    tipText_->SetTextStyle(textStyle);

    auto tipComponent = AceType::MakeRefPtr<TipComponent>(tipText_);
    tipComponent->SetBgColor(theme->GetTipColor());
    tipComponent->SetDirection(direction_);

    renderText_ = RenderText::Create();
    renderText_->Attach(GetContext());

    tip_ = RenderTip::Create();
    tip_->AddChild(renderText_);
    tip_->Attach(GetContext());
    tip_->SetVisible(false);
    auto padding = NormalizeToPx(theme->GetTipTextPadding());
    auto renderTip = AceType::DynamicCast<RenderTip>(tip_);
    if (renderTip) {
        renderTip->SetPadding(direction_ == Axis::VERTICAL ?
            Edge(0.0, padding, 0.0, padding, DimensionUnit::PX) : Edge(padding, 0.0, padding, 0.0, DimensionUnit::PX));
    }

    AddChild(tip_);
    renderText_->Update(tipText_);
    tip_->Update(tipComponent);
}

void FlutterRenderSlider::ProcessBlock(const Offset& currentPosition)
{
    Offset blockPosition = currentPosition;
    auto blockRenderNode = AceType::DynamicCast<RenderBlock>(block_);
    if (!blockRenderNode) {
        return;
    }

    double hotRegionWidth = NormalizeToPx(blockHotWidth_);
    double hotRegionHeight = NormalizeToPx(blockHotHeight_);
    blockRenderNode->SetLayoutSize(Size(hotRegionWidth, hotRegionHeight));
    blockRenderNode->SetPosition(blockPosition);
    blockRenderNode->SetFocus(GetFocus());
    blockRenderNode->SetRadiusScale(radiusScale_);
}

void FlutterRenderSlider::ProcessTrack(const Offset& currentPosition)
{
    const RefPtr<RenderTrack> track = AceType::DynamicCast<RenderTrack>(track_);
    if (!track) {
        return;
    }
    if (direction_  == Axis::VERTICAL) {
        double dxOffset = currentPosition.GetX();
        double trackPositionHorizontal = NormalizeToPx(SLIDER_PADDING_DP);
        double hotRegionWidth = NormalizeToPx(blockHotWidth_);
        Offset trackPosition = Offset(dxOffset - track->GetTrackThickness() * HALF, trackPositionHorizontal);
        track->SetSliderMode(mode_);
        if (showSteps_) {
            double stepLength = step_ * trackLength_ / (max_ - min_);
            track->SetSliderSteps(stepLength);
        } else {
            track->SetSliderSteps(0.0);
        }
        track->SetPosition(trackPosition);
        track->SetTotalRatio(totalRatio_);
        track->SetLayoutSize(Size(hotRegionWidth, trackLength_));
    } else {
        double trackPositionHorizontal = NormalizeToPx(SLIDER_PADDING_DP);
        if (GetTextDirection() == TextDirection::RTL) {
            trackPositionHorizontal = GetLayoutSize().Width() - NormalizeToPx(SLIDER_PADDING_DP) - trackLength_;
        }
        double dyOffset = currentPosition.GetY();
        double hotRegionHeight = NormalizeToPx(blockHotHeight_);
        Offset trackPosition = Offset(trackPositionHorizontal, dyOffset - track->GetTrackThickness() * HALF);
        track->SetSliderMode(mode_);
        if (showSteps_) {
            double stepLength = step_ * trackLength_ / (max_ - min_);
            track->SetSliderSteps(stepLength);
        } else {
            track->SetSliderSteps(0.0);
        }
        track->SetPosition(trackPosition);
        track->SetTotalRatio(totalRatio_);
        track->SetLayoutSize(Size(trackLength_, hotRegionHeight));
    }
}

void FlutterRenderSlider::SetTipPosition(double blockOffset)
{
    auto renderTip = AceType::DynamicCast<RenderTip>(tip_);
    if (renderTip) {
        if (direction_ == Axis::VERTICAL) {
            double tipLayoutWidth = renderTip->GetLayoutSize().Width();
            double childHalfHeight = renderTip->GetChildSize().Height() * HALF;
            renderTip->SetPosition(
                Offset(-tipLayoutWidth - HALF * (NormalizeToPx(blockHotWidth_) - NormalizeToPx(hotWidth_)),
                    blockOffset - childHalfHeight));
        } else {
            double childHalfWidth = renderTip->GetChildSize().Width() * HALF;
            double tipLayoutHeight = renderTip->GetLayoutSize().Height();
            renderTip->SetPosition(Offset(blockOffset - childHalfWidth, -tipLayoutHeight -
                HALF * (NormalizeToPx(blockHotWidth_) - NormalizeToPx(hotWidth_))));
        }
    }
}

void FlutterRenderSlider::HandleFocus()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    auto block = AceType::DynamicCast<RenderBlock>(block_);
    auto track = AceType::DynamicCast<RenderTrack>(track_);
    if (!block || !track) {
        return;
    }

    if (GetFocus()) {
        const double focusPadding = NormalizeToPx(FOCUS_PADDING);
        if (mode_ == SliderMode::INSET) {
            const Size focus = Size(trackLength_ + track->GetTrackThickness(), track->GetTrackThickness());
            context->ShowFocusAnimation(
                RRect::MakeRRect(Rect(Offset(), focus), focus.Height() * HALF, focus.Height() * HALF), Color::BLUE,
                track->GetGlobalOffset() - Offset(track->GetTrackThickness() * HALF, 0.0));
        } else if (mode_ == SliderMode::OUTSET) {
            const double blockSize = NormalizeToPx(block->GetBlockSize());
            const Size focus = Size(blockSize, blockSize) + Size(focusPadding, focusPadding);
            context->ShowFocusAnimation(
                RRect::MakeRRect(Rect(Offset(), focus), focus.Width() * HALF, focus.Width() * HALF), Color::BLUE,
                block->GetGlobalOffset() - Offset(focus.Width() * HALF, focus.Width() * HALF));
        } else {
            LOGW("invalid mode");
        }
    }
}

void FlutterRenderSlider::OnPaintFinish()
{
    HandleFocus();
}

void FlutterRenderSlider::OnMouseHoverEnterTest()
{
    ResetController(controllerExit_);
    if (!controllerEnter_) {
        controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateColorAnimation(colorAnimationEnter_, Color::TRANSPARENT, Color::FromRGBO(0, 0, 0, 0.05), true);
    colorAnimationEnter_->SetCurve(Curves::FRICTION);
    StartHoverAnimation(controllerEnter_, colorAnimationEnter_);
}

void FlutterRenderSlider::OnMouseHoverExitTest()
{
    ResetController(controllerEnter_);
    if (!controllerExit_) {
        controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    auto renderBlock = AceType::DynamicCast<RenderBlock>(block_);
    if (!renderBlock) {
        return;
    }
    auto currentColor = renderBlock->GetHoverColor();
    CreateColorAnimation(colorAnimationExit_, currentColor, Color::TRANSPARENT, false);
    if (currentColor == Color::FromRGBO(0, 0, 0, 0.05)) {
        colorAnimationExit_->SetCurve(Curves::FRICTION);
    } else {
        colorAnimationExit_->SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
    StartHoverAnimation(controllerExit_, colorAnimationExit_);
}

void FlutterRenderSlider::StartHoverAnimation(
    RefPtr<Animator> controller, RefPtr<KeyframeAnimation<Color>>& colorAnimation)
{
    if (!controller || !colorAnimation) {
        return;
    }
    controller->ClearInterpolators();
    controller->AddInterpolator(colorAnimation);
    controller->SetDuration(HOVER_ANIMATION_DURATION);
    controller->SetFillMode(FillMode::FORWARDS);
    controller->Play();
}

void FlutterRenderSlider::CreateColorAnimation(
    RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue, const Color& endValue, bool hover)
{
    if (!colorAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0, endValue);
    colorAnimation->AddKeyframe(keyframeBegin);
    colorAnimation->AddKeyframe(keyframeEnd);
    auto renderBlock = AceType::DynamicCast<RenderBlock>(block_);
    if (!renderBlock) {
        return;
    }
    auto weakBlock = WeakClaim(RawPtr(renderBlock));
    colorAnimation->AddListener([weakBlock, hover](const Color& value) {
        auto block = weakBlock.Upgrade();
        if (block) {
            block->SetHover(hover);
            block->SetHoverColor(value);
            block->MarkNeedRender();
        }
    });
}

void FlutterRenderSlider::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void FlutterRenderSlider::Paint(RenderContext& context, const Offset& offset)
{
    if (GetErrorBit()) {
        LOGE("slider size error. requested size [80dp, 40dp]");
    }
    auto canvas = ScopedCanvas::Create(context);
    if (!canvas) {
        LOGE("paint canvas is null");
        return;
    }
    if ((!showTips_ && tip_) || (tip_ && !isDraging_)) {
        tip_->SetVisible(false);
    }
    RenderNode::Paint(context, offset);
}

} // namespace OHOS::Ace