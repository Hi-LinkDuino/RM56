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

#include "core/components/tool_bar/render_tool_bar_item.h"

#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

constexpr Dimension TOOL_BAR_HEIGHT = 56.0_vp;
constexpr int32_t HOVER_ANIMATION_DURATION = 250;

RenderToolBarItem::RenderToolBarItem() : RenderNode(true) {}

void RenderToolBarItem::Initialize()
{
    auto wp = AceType::WeakClaim(this);

    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp](const TouchEventInfo&) {
        auto toolBarItem = wp.Upgrade();
        if (toolBarItem) {
            toolBarItem->HandleTouchEvent(true);
        }
    });
    touchRecognizer_->SetOnTouchUp([wp](const TouchEventInfo&) {
        auto toolBarItem = wp.Upgrade();
        if (toolBarItem) {
            toolBarItem->HandleTouchEvent(false);
        }
    });
    touchRecognizer_->SetOnTouchMove([wp](const TouchEventInfo& info) {
        auto toolBarItem = wp.Upgrade();
        if (toolBarItem) {
            toolBarItem->HandleMoveEvent(info);
        }
    });

    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp](const ClickInfo&) {
        auto toolBarItem = wp.Upgrade();
        if (toolBarItem) {
            toolBarItem->HandleClickEvent();
        }
    });
    InitAccessibilityEventListener();
}

void RenderToolBarItem::HandleTouchEvent(bool isTouch)
{
    onTouch_ = isTouch;
    isTouch ? PlayEventEffectAnimation(pressColor_) : PlayEventEffectAnimation(Color::TRANSPARENT);
}

void RenderToolBarItem::HandleMoveEvent(const TouchEventInfo& info)
{
    if (info.GetTouches().empty()) {
        return;
    }
    const auto& locationInfo = info.GetTouches().front();
    Point touchPoint = Point(locationInfo.GetLocalLocation().GetX(), locationInfo.GetLocalLocation().GetY());
    if (!GetPaintRect().IsInRegion(touchPoint)) {
        onTouch_ = false;
        MarkNeedRender();
    }
}

void RenderToolBarItem::HandleClickEvent()
{
    if (isEndItem_) {
        ShowMenu();
        return;
    }

    if (onClick_) {
        onClick_();
    }
}

void RenderToolBarItem::ShowMenu()
{
    if (targetMenuCallBack_) {
        Offset leftTop = GetOffsetToStage();
        double menuWidth = std::max(NormalizeToPx(menuMinWidth_), GetLayoutSize().Width());
        Size layoutSize = Size(menuWidth, GetLayoutSize().Height());
        double rootWidth = GetContext().Upgrade()->GetRootWidth();
        if (leftTop.GetX() + menuWidth > rootWidth) {
            leftTop.SetX(rootWidth - menuWidth);
        }
        Offset rightBottom = leftTop + layoutSize;
        targetMenuCallBack_(leftTop, rightBottom);
    }
}

void RenderToolBarItem::Update(const RefPtr<Component>& component)
{
    RefPtr<ToolBarItemComponent> toolBarItemComponent = AceType::DynamicCast<ToolBarItemComponent>(component);
    if (!toolBarItemComponent) {
        return;
    }
    if (!toolBarItemComponent->GetClickedEventId().IsEmpty()) {
        auto clickId = toolBarItemComponent->GetClickedEventId();
        auto catchMode = clickId.IsEmpty() || clickId.GetCatchMode();
        static const int32_t bubbleModeVersion = 6;
        auto pipeline = context_.Upgrade();
        if (!catchMode) {
            if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
                catchMode = false;
            } else {
                catchMode = true;
            }
        }
        if (clickRecognizer_) {
            clickRecognizer_->SetUseCatchMode(catchMode);
        }
        onClick_ = AceAsyncEvent<void()>::Create(toolBarItemComponent->GetClickedEventId(), context_);
    }
    if (!eventEffectController_) {
        eventEffectController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    isEndItem_ = toolBarItemComponent->GetIsEndItem();
    rrectRadius_ = toolBarItemComponent->GetRadius();
    pressColor_ = toolBarItemComponent->GetPressColor();
    mouseHoverColor_ = toolBarItemComponent->GetHoverColor();
    focusColor_ = toolBarItemComponent->GetFocusColor();
    menuMinWidth_ = toolBarItemComponent->GetMenuMinWidth();
    position_ = toolBarItemComponent->GetPositionType();
}

void RenderToolBarItem::PerformLayout()
{
    if (GetChildren().empty()) {
        return;
    }
    Size innerSize = GetLayoutParam().GetMaxSize();
    innerSize.SetHeight(NormalizeToPx(TOOL_BAR_HEIGHT));
    LayoutParam innerLayout;
    innerLayout.SetMaxSize(innerSize);
    RefPtr<RenderNode> child = GetChildren().front();
    child->Layout(innerLayout);
    child->SetPosition(Offset((GetLayoutParam().GetMaxSize().Width() - child->GetPaintRect().Width()) / 2.0, 0.0));
    Size layoutSize = Size(child->GetLayoutParam().GetMaxSize().Width(), NormalizeToPx(TOOL_BAR_HEIGHT));
    SetLayoutSize(layoutSize);
}

void RenderToolBarItem::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if ((!touchRecognizer_) || (!clickRecognizer_)) {
        return;
    }
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchRecognizer_);
    result.emplace_back(clickRecognizer_);
}

void RenderToolBarItem::RequestFocusAnimation(
    const Offset& animationOffset, const Rect& animationRect, const Radius& radius)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Pipeline context upgrade fail!");
        return;
    }
    focusAnimation_ = context->GetRenderFocusAnimation();
    if (!focusAnimation_) {
        LOGE("focusAnimation is null!");
        return;
    }
    context->ShowFocusAnimation(RRect::MakeRRect(animationRect, radius), focusColor_, animationOffset);
}

void RenderToolBarItem::OnMouseHoverEnterTest()
{
    ResetController(controllerExit_);
    if (!controllerEnter_) {
        controllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateColorAnimation(colorAnimationEnter_, Color::TRANSPARENT, mouseHoverColor_, true);
    colorAnimationEnter_->SetCurve(Curves::FRICTION);
    StartHoverAnimation(controllerEnter_, colorAnimationEnter_);
}

void RenderToolBarItem::OnMouseHoverExitTest()
{
    ResetController(controllerEnter_);
    if (!controllerExit_) {
        controllerExit_ = AceType::MakeRefPtr<Animator>(context_);
    }
    colorAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateColorAnimation(colorAnimationExit_, hoverColor_, Color::TRANSPARENT, false);
    if (hoverColor_ == mouseHoverColor_) {
        colorAnimationExit_->SetCurve(Curves::FRICTION);
    } else {
        colorAnimationExit_->SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
    StartHoverAnimation(controllerExit_, colorAnimationExit_);
}

void RenderToolBarItem::StartHoverAnimation(RefPtr<Animator> controller,
    RefPtr<KeyframeAnimation<Color>>& colorAnimation)
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

void RenderToolBarItem::CreateColorAnimation(
    RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue, const Color& endValue, bool hover)
{
    if (!colorAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<Color>>(1.0, endValue);
    colorAnimation->AddKeyframe(keyframeBegin);
    colorAnimation->AddKeyframe(keyframeEnd);
    colorAnimation->AddListener([weakBarItem = AceType::WeakClaim(this), hover](const Color& value) {
        auto toolBarItem = weakBarItem.Upgrade();
        if (toolBarItem) {
            toolBarItem->onHover_ = hover;
            toolBarItem->hoverColor_ = value;
            toolBarItem->MarkNeedRender();
        }
    });
}

void RenderToolBarItem::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void RenderToolBarItem::OnStatusChanged(RenderStatus renderStatus)
{
    if (renderStatus == RenderStatus::FOCUS) {
        SetFocusEventFlag(true);
    } else if (renderStatus == RenderStatus::BLUR) {
        SetFocusEventFlag(false);
    }
}

void RenderToolBarItem::PlayEventEffectAnimation(const Color& endColor)
{
    if (!eventEffectController_->IsStopped()) {
        eventEffectController_->Stop();
    }
    auto colorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    CreateMouseAnimation(colorAnimation, GetEventEffectColor(), endColor);
    eventEffectController_->ClearInterpolators();
    eventEffectController_->ClearStopListeners();
    eventEffectController_->AddInterpolator(colorAnimation);
    eventEffectController_->SetDuration(PRESS_DURATION);
    eventEffectController_->SetFillMode(FillMode::FORWARDS);
    eventEffectController_->Forward();
}

void RenderToolBarItem::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = accessibilityNode_.Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& toolBarItem = weakPtr.Upgrade();
        if (toolBarItem) {
            toolBarItem->HandleClickEvent();
        }
    });
}

} // namespace OHOS::Ace