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

#include "core/components/tab_bar/render_tab_bar_item.h"

#include "core/components/common/properties/decoration.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension TAB_BAR_ITEM_SMALL_PADDING(8, DimensionUnit::VP);
constexpr Dimension TAB_BAR_ITEM_BIG_PADDING(12, DimensionUnit::VP);
constexpr int32_t HOVER_ANIMATION_DURATION = 250;
constexpr double HOVER_OPACITY_RATIO = 0.05;
constexpr double CLICK_OPACITY_RATIO = 0.1;

} // namespace

RenderTabBarItem::RenderTabBarItem() : RenderBox()
{
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchDown();
        }
    });
    touchRecognizer_->SetOnTouchUp([wp = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->HandleTouchUp();
        }
    });
}

void RenderTabBarItem::HandleTouchDown()
{
    SetTouching(true);
    PlayPressAnimation(CLICK_OPACITY_RATIO);
}

void RenderTabBarItem::HandleTouchUp()
{
    if (IsTouching()) {
        SetTouching(false);
        auto ratio = onHover_ ? HOVER_OPACITY_RATIO : 0.0;
        PlayPressAnimation(ratio);
    }
}

void RenderTabBarItem::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!touchRecognizer_) {
        return;
    }
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchRecognizer_);
}

void RenderTabBarItem::OnMouseHoverEnterTest()
{
    ResetController(controllerExit_);
    ResetController(controllerPress_);
    if (!controllerEnter_) {
        controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
    }
    doubleAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(doubleAnimationEnter_, hoverOpacity_, HOVER_OPACITY_RATIO, true);
    doubleAnimationEnter_->SetCurve(Curves::FRICTION);
    StartHoverAnimation(controllerEnter_, doubleAnimationEnter_);
}

void RenderTabBarItem::OnMouseHoverExitTest()
{
    ResetController(controllerEnter_);
    ResetController(controllerPress_);
    if (!controllerExit_) {
        controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
    }
    doubleAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(doubleAnimationExit_, hoverOpacity_, 0.0, false);
    if (hoverOpacity_ == HOVER_OPACITY_RATIO) {
        doubleAnimationExit_->SetCurve(Curves::FRICTION);
    } else {
        doubleAnimationExit_->SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
    StartHoverAnimation(controllerExit_, doubleAnimationExit_);
}

void RenderTabBarItem::PlayPressAnimation(double endOpacityRatio)
{
    ResetController(controllerEnter_);
    ResetController(controllerExit_);
    if (!controllerPress_) {
        controllerPress_ = AceType::MakeRefPtr<Animator>(context_);
    }
    auto doubleAnimationPress = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(doubleAnimationPress, hoverOpacity_, endOpacityRatio, onHover_);
    doubleAnimationPress->SetCurve(Curves::SHARP);
    controllerPress_->ClearInterpolators();
    controllerPress_->AddInterpolator(doubleAnimationPress);
    controllerPress_->SetDuration(HOVER_ANIMATION_DURATION);
    controllerPress_->SetFillMode(FillMode::FORWARDS);
    controllerPress_->Play();
}

void RenderTabBarItem::StartHoverAnimation(RefPtr<Animator> controller,
    RefPtr<KeyframeAnimation<double>>& doubleAnimation)
{
    if (!controller || !doubleAnimation) {
        return;
    }
    controller->ClearInterpolators();
    controller->AddInterpolator(doubleAnimation);
    controller->SetDuration(HOVER_ANIMATION_DURATION);
    controller->SetFillMode(FillMode::FORWARDS);
    controller->Play();
}

void RenderTabBarItem::CreateDoubleAnimation(
    RefPtr<KeyframeAnimation<double>>& doubleAnimation, double beginValue, double endValue, bool hover)
{
    if (!doubleAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<double>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<double>>(1.0, endValue);
    doubleAnimation->AddKeyframe(keyframeBegin);
    doubleAnimation->AddKeyframe(keyframeEnd);
    doubleAnimation->AddListener([weakBarItem = AceType::WeakClaim(this), hover](double value) {
        auto barItem = weakBarItem.Upgrade();
        if (barItem) {
            barItem->onHover_ = hover;
            barItem->hoverOpacity_ = value;
            barItem->MarkNeedRender();
        }
    });
}

void RenderTabBarItem::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void RenderTabBarItem::PerformLayout()
{
    auto currentSize = GridSystemManager::GetInstance().GetCurrentSize();
    if (currentSize == GridSizeType::SM) {
        paddingOrigin_.SetLeft(TAB_BAR_ITEM_SMALL_PADDING);
        paddingOrigin_.SetRight(TAB_BAR_ITEM_SMALL_PADDING);
    } else if (currentSize >= GridSizeType::MD) {
        paddingOrigin_.SetLeft(TAB_BAR_ITEM_BIG_PADDING);
        paddingOrigin_.SetRight(TAB_BAR_ITEM_BIG_PADDING);
    }
    RenderBox::PerformLayout();
}

} // namespace OHOS::Ace
