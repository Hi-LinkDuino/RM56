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

#include "core/animation/shared_transition_effect.h"

#include "core/animation/animation_pub.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/animation/keyframe.h"
#include "core/components/common/properties/motion_path_evaluator.h"
#include "core/components/common/properties/page_transition_option.h"
#include "core/components/overlay/overlay_element.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/shared_transition/shared_transition_element.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace {

SharedTransitionEffect::SharedTransitionEffect(const ShareId& shareId, SharedTransitionEffectType type)
    : shareId_(shareId), type_(type)
{
    controller_ = AceType::MakeRefPtr<Animator>();
}

bool SharedTransitionEffect::CheckIn(
    TransitionEvent event, WeakPtr<SharedTransitionElement>& sharedWeak, Offset& ticket)
{
    if (!controller_) {
        LOGE("Check In failed. controller is null. share id: %{public}s", shareId_.c_str());
        return false;
    }
    auto shared = sharedWeak.Upgrade();
    if (!shared) {
        LOGE("Check In failed. passenger element is null. share id: %{public}s", shareId_.c_str());
        return false;
    }
    // Check-in
    if (!shared->AboardShuttle(ticket)) {
        LOGE("Check In failed. aboard shuttle failed. share id: %{public}s.", shareId_.c_str());
        return false;
    }
    // Arrange Return Shuttle
    controller_->AddStopListener([sharedWeak, shareId = shareId_]() {
        auto shared = sharedWeak.Upgrade();
        if (!shared) {
            // In Pop Scene, target page may be removed, and will reach this branch.
            LOGD("Passenger missing return shuttle. shared element is null. shareId: %{public}s", shareId.c_str());
            return;
        }
        shared->GetOffShuttle();
    });
    controller_->AddStopListener([destWeak = dest_, srcWeak = src_]() {
        auto dest = destWeak.Upgrade();
        if (dest) {
            dest->SetSizeModified(nullptr);
        }
        auto src = srcWeak.Upgrade();
        if (src) {
            src->SetSizeModified(nullptr);
        }
    });
    return true;
}

bool SharedTransitionEffect::TakeOff(TransitionEvent event, RefPtr<OverlayElement>& overlay,
    WeakPtr<SharedTransitionElement>& sharedWeak, const Offset& ticket, TweenOption& option)
{
    if (!controller_) {
        LOGE("TakeOff failed. controller is null. share id: %{public}s", shareId_.c_str());
        return false;
    }
    if (!overlay) {
        LOGE("TakeOff failed. overlay is null. event: %{public}d, share id: %{public}s", event, shareId_.c_str());
        return false;
    }
    auto shared = sharedWeak.Upgrade();
    if (!shared) {
        LOGE("TakeOff failed. shared is null. event: %{public}d, share id: %{public}s", event, shareId_.c_str());
        return false;
    }
    auto passengerComponent = shared->GetPassengerComponent();
    auto passengerElement = shared->GetPassengerElement();
    if (!passengerComponent || !passengerElement) {
        LOGE("TakeOff failed. passenger not found. event: %{public}d, share id: %{public}s.", event, shareId_.c_str());
        return false;
    }
    auto tweenSeat = AceType::MakeRefPtr<TweenComponent>("TweenSeat");
    // Find Seat.
    auto seat = AceType::MakeRefPtr<PositionedComponent>(tweenSeat);
    Component::MergeRSNode(seat);
    LOGD("TakeOff. event: %{public}d, share id: %{public}s. x: %{public}lf, y: %{public}lf", event, shareId_.c_str(),
        ticket.GetX(), ticket.GetY());
    seat->SetLeft(Dimension(ticket.GetX(), DimensionUnit::PX));
    seat->SetTop(Dimension(ticket.GetY(), DimensionUnit::PX));
    // set zIndex
    auto zIndex = shared->GetZIndex();
    if (zIndex != 0) {
        seat->SetZIndex(zIndex);
    }
    // Take Off,
    overlay->PushInstant(seat);
    auto seatElement = AceType::DynamicCast<PositionedElement>(overlay->GetLastChild());
    if (!seatElement) {
        LOGE("TakeOff failed. seat not found. event: %{public}d, share id: %{public}s.", event, shareId_.c_str());
        return false;
    }
    // make overlay un-focusable.
    seatElement->SetFocusable(false);
    auto seatRender = seatElement->GetRenderNode();
    if (!seatRender) {
        LOGE("TakeOff failed, render is nullptr");
        return false;
    }
    seatRender->SetDisableTouchEvent(true);
    return TakeOffTween(seatElement->GetFirstChild(), passengerComponent, passengerElement, option);
}

bool SharedTransitionEffect::TakeOffTween(const RefPtr<Element>& tweenElement,
    const RefPtr<Component>& passengerComponent, const RefPtr<Element>& passengerElement, TweenOption& option)
{
    auto tweenSeatElement = AceType::DynamicCast<TweenElement>(tweenElement);
    if (!tweenSeatElement) {
        LOGE("TakeOff failed. tween not found. share id: %{public}s.", shareId_.c_str());
        return false;
    }
    auto contentParent = tweenSeatElement->GetContentParent();
    if (!contentParent) {
        LOGE("TakeOff failed. content parent not found. share id: %{public}s.", shareId_.c_str());
        return false;
    }
    passengerElement->SetNewComponent(passengerComponent);
    passengerElement->Mount(contentParent);
    auto passengerRender = passengerElement->GetRenderNode();
    auto parentRender = contentParent->GetRenderNode();
    if (passengerRender && parentRender) {
        // Follow parent's hidden status.
        passengerRender->SetHidden(parentRender->GetHidden());
    }
    tweenSeatElement->SetController(controller_);
    tweenSeatElement->SetOption(option);
    tweenSeatElement->ApplyKeyframes();
    tweenSeatElement->ApplyOptions();
    tweenSeatElement_ = tweenSeatElement;
    return true;
}

RefPtr<SharedTransitionEffect> SharedTransitionEffect::GetSharedTransitionEffect(
    SharedTransitionEffectType effect, const ShareId& shareId)
{
    LOGD("Get shared transition effect. effect: %{public}d, share id: %{public}s", effect, shareId.c_str());
    switch (effect) {
        case SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE: {
            return AceType::MakeRefPtr<SharedTransitionExchange>(shareId);
        }
        case SharedTransitionEffectType::SHARED_EFFECT_STATIC: {
            return AceType::MakeRefPtr<SharedTransitionStatic>(shareId);
        }
        default: {
            LOGE("Unknown effect. effect: %{public}d, share id: %{public}s", effect, shareId.c_str());
            return nullptr;
        }
    }
}

bool SharedTransitionEffect::ApplyAnimation(
    RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller, TweenOption& option, TransitionEvent event)
{
    controller_->ClearAllListeners();
    controller_->ClearInterpolators();
    return true;
}

bool SharedTransitionExchange::Allow(TransitionEvent event)
{
    auto dest = dest_.Upgrade();
    auto src = src_.Upgrade();
    if (!dest || !src) {
        LOGD("Check allow exchange failed. dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    bool allow = false;
    if (event == TransitionEvent::PUSH_START) {
        // In Push Scene, dest means Enter and Source means Exit
        allow = dest->IsEnablePushEnter() && src->IsEnablePushExit();
    } else if (event == TransitionEvent::POP_START) {
        // In Pop Scene, dest means Enter and Source means Exit
        allow = dest->IsEnablePopEnter() && src->IsEnablePopExit();
    }
    LOGD("Exchange Allow status: %{public}d, event: %{public}d, share id: %{public}s", allow, event, shareId_.c_str());
    return allow;
}

void SharedTransitionExchange::AddLazyLoadCallback(TransitionEvent event)
{
    auto src = src_.Upgrade();
    auto dest = dest_.Upgrade();
    if (!dest || !src) {
        LOGE("Add Lazy callback failed. Dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return;
    }
    // Lazy load child size. make width and height animation later.
    auto effectWeak = AceType::WeakClaim(this);
    dest->SetSizeModified([effectWeak, event, shareId = shareId_]() {
        LOGD("Lazy load size callback. event: %{public}d, share id: %{public}s", event, shareId.c_str());
        auto effect = effectWeak.Upgrade();
        if (!effect) {
            LOGE("Create Lazy load animation failed. effect is null.");
            return;
        }
        auto tweenSeatElement = effect->tweenSeatElement_.Upgrade();
        if (!tweenSeatElement) {
            LOGE("Create Lazy load animation failed. tween Seat Element is null.");
            return;
        }
        TweenOption option = tweenSeatElement->GetOption();
        option.ClearListeners();
        if (!effect->CreateAnimation(option, event, true)) {
            LOGE("Create animation failed. event: %{public}d, share id: %{public}s", event, shareId.c_str());
            return;
        }

        tweenSeatElement->SetOption(option);
        effect->controller_->ClearInterpolators();
        tweenSeatElement->ApplyKeyframes();
    });
}

bool SharedTransitionExchange::CreateTranslateAnimation(
    TweenOption& option, TransitionEvent event, bool calledByLazyLoad)
{
    auto src = src_.Upgrade();
    auto dest = dest_.Upgrade();
    if (!dest || !src) {
        LOGE("Create exchange animation failed. dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    auto& translateMap = option.GetTranslateAnimations();
    auto translateIter = translateMap.find(AnimationType::TRANSLATE);
    if ((calledByLazyLoad && autoTranslate_) || (translateIter == translateMap.end())) {
        // if no custom translate animation, add exchange translate animation.
        auto destOffset = dest->GetGlobalOffset();
        auto srcOffset = src->GetGlobalOffset();
        LOGD("Get Offset for event: %{public}d, share id: %{public}s. dest: x: %{public}lf, y: %{public}lf; "
             "src: x: %{public}lf, y: %{public}lf",
            event, shareId_.c_str(), destOffset.GetX(), destOffset.GetY(), srcOffset.GetX(), srcOffset.GetY());
        if (destOffset != srcOffset) {
            auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
                Offset(0, 0), destOffset - srcOffset, Curves::FRICTION);
            const auto& motionPathOption = option.GetMotionPathOption();
            if (motionPathOption.IsValid()) {
                auto motionPathEvaluator =
                    AceType::MakeRefPtr<MotionPathEvaluator>(motionPathOption, Offset(0, 0), destOffset - srcOffset);
                translateAnimation->SetEvaluator(motionPathEvaluator->CreateDimensionOffstEvaluator());
                if (motionPathOption.GetRotate()) {
                    auto rotateAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, 1.0f, option.GetCurve());
                    rotateAnimation->SetEvaluator(motionPathEvaluator->CreateRotateEvaluator());
                    option.SetTransformFloatAnimation(AnimationType::ROTATE_Z, rotateAnimation);
                }
            }
            option.SetTranslateAnimations(AnimationType::TRANSLATE, translateAnimation);
            autoTranslate_ = true;
            LOGD("Create shared exchange animation for translate. event: %{public}d, share id: %{public}s", event,
                shareId_.c_str());
        }
    }
    return true;
}

bool SharedTransitionExchange::CreateSizeAnimation(TweenOption& option, TransitionEvent event, bool isLazy)
{
    auto src = src_.Upgrade();
    auto dest = dest_.Upgrade();
    if (!dest || !src) {
        LOGE("Create exchange animation failed. dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    auto destSize = dest->GetSuitSize();
    auto srcSize = src->GetSuitSize();
    LOGD("Get Size for event: %{public}d, share id: %{public}s. dest: w: %{public}lf, h: %{public}lf; "
         "srcSize: w: %{public}lf, h: %{public}lf",
        event, shareId_.c_str(), destSize.Width(), destSize.Height(), srcSize.Width(), srcSize.Height());

    // add width shared transition
    auto& propertyMap = option.GetFloatPropertyAnimation();
    auto widthIter = propertyMap.find(PropertyAnimatableType::PROPERTY_WIDTH);
    if (((isLazy && autoWidth_) || (widthIter == propertyMap.end())) && !NearEqual(destSize.Width(), srcSize.Width())) {
        auto widthAnimation =
            AceType::MakeRefPtr<CurveAnimation<float>>(srcSize.Width(), destSize.Width(), Curves::FRICTION);
        option.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);
        autoWidth_ = true;
        LOGD("Create shared exchange animation for width. share id: %{public}s", shareId_.c_str());
    }

    // add scaleY shared transition
    auto heightIter = propertyMap.find(PropertyAnimatableType::PROPERTY_HEIGHT);
    if (((isLazy && autoHeight_) || (heightIter == propertyMap.end())) &&
        !NearEqual(destSize.Height(), srcSize.Height())) {
        auto heightAnimation =
            AceType::MakeRefPtr<CurveAnimation<float>>(srcSize.Height(), destSize.Height(), Curves::FRICTION);
        option.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimation);
        LOGD("Create shared exchange animation for height. share id: %{public}s", shareId_.c_str());
        autoHeight_ = true;
    }
    return true;
}

bool SharedTransitionExchange::CreateOpacityAnimation(TweenOption& option, TransitionEvent event, bool isLazy)
{
    auto src = src_.Upgrade();
    auto dest = dest_.Upgrade();
    if (!dest || !src) {
        LOGE("Create exchange animation failed. dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    auto destOpacity = dest->GetOpacity();
    auto srcOpacity = src->GetOpacity();

    LOGD("Get Opacity for event: %{public}d, share id: %{public}s. dest: %{public}f; src: %{public}f", event,
        shareId_.c_str(), destOpacity, srcOpacity);

    if (!NearEqual(destOpacity, srcOpacity) && !option.GetOpacityAnimation()) {
        auto opacityAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(srcOpacity, destOpacity, Curves::FRICTION);
        option.SetOpacityAnimation(opacityAnimation);
    }
    return true;
}

bool SharedTransitionExchange::CreateAnimation(TweenOption& option, TransitionEvent event, bool isLazy)
{
    auto src = src_.Upgrade();
    auto dest = dest_.Upgrade();
    if (!dest || !src) {
        LOGE("Create exchange animation failed. dest or src is null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    if (!isLazy) {
        autoTranslate_ = false;
        autoWidth_ = false;
        autoHeight_ = false;
    }

    // add translate shared transition
    if (!CreateTranslateAnimation(option, event, isLazy)) {
        return false;
    }
    if (!CreateSizeAnimation(option, event, isLazy)) {
        return false;
    }
    if (!CreateOpacityAnimation(option, event, isLazy)) {
        return false;
    }
    AddLazyLoadCallback(event);
    return true;
}

bool SharedTransitionExchange::ApplyAnimation(RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller,
    TweenOption& option, TransitionEvent event)
{
    if (!SharedTransitionEffect::ApplyAnimation(overlay, controller, option, event)) {
        return false;
    }
    Offset ticket;
    if (!CheckIn(event, src_, ticket)) {
        LOGE(
            "Apply exchange failed. check in failed. event: %{public}d, share id: %{public}s", event, shareId_.c_str());
        return false;
    }
    return TakeOff(event, overlay, src_, ticket, option);
}

bool SharedTransitionStatic::Allow(TransitionEvent event)
{
    auto current = GetCurrentSharedElement().Upgrade();
    if (!current) {
        LOGD("Check allow static failed. current null. event: %{public}d, share id: %{public}s", event,
            shareId_.c_str());
        return false;
    }
    bool allow = false;
    if (event == TransitionEvent::PUSH_START) {
        // In Push Scene, dest means Enter and Source means Exit
        allow = current->IsEnablePushEnter();
    } else if (event == TransitionEvent::POP_START) {
        // In Pop Scene, dest means Enter and Source means Exit
        allow = current->IsEnablePopEnter();
    }
    LOGD("Static Allow status: %{public}d, event: %{public}d, share id: %{public}s", allow, event, shareId_.c_str());
    return allow;
}

bool SharedTransitionStatic::CreateAnimation(TweenOption& option, TransitionEvent event, bool isLazy)
{
    if (src_.Invalid()) {
        // anchor appearing, opacity 0 to 1
        auto opacityAnimation = option.GetOpacityAnimation();
        if (!opacityAnimation) {
            TransitionTweenOptionFactory::CreateSharedTweenOption(
                SharedTransitionEffectType::SHARED_EFFECT_STATIC, option);
        }
    } else {
        // anchor disappearing, opacity 1 to 0
        if (!option.GetOpacityAnimation()) {
            auto animation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 0.0f, option.GetCurve());
            option.SetOpacityAnimation(animation);
        }
    }
    return true;
}

bool SharedTransitionStatic::ApplyAnimation(
    RefPtr<OverlayElement>& overlay, RefPtr<Animator>& controller, TweenOption& option, TransitionEvent event)
{
    if (!SharedTransitionEffect::ApplyAnimation(overlay, controller, option, event)) {
        return false;
    }
    Offset ticket;
    // the dest page and source page elements are in effect
    auto current = GetCurrentSharedElement();
    if (!CheckIn(event, current, ticket)) {
        LOGE("Apply static fail. check in failed. event: %{public}d, share id: %{public}s", event, shareId_.c_str());
        return false;
    }
    AddLazyLoadCallback();
    return TakeOff(event, overlay, current, ticket, option);
}

void SharedTransitionStatic::AddLazyLoadCallback()
{
    auto current = GetCurrentSharedElement().Upgrade();
    if (!current) {
        LOGE("Add Lazy load Callback failed. current is null.");
        return;
    }
    current->SetSizeModified([effectWeak = WeakClaim(this)]() {
        auto effect = effectWeak.Upgrade();
        if (!effect) {
            LOGE("Fix static shared element position failed. effect is null");
            return;
        }
        auto tweenSeatElement = effect->tweenSeatElement_.Upgrade();
        if (!tweenSeatElement) {
            LOGE("Fix static shared element position failed. tween element is null");
            return;
        }
        auto positionedElement =
            AceType::DynamicCast<PositionedElement>(tweenSeatElement->GetElementParent().Upgrade());
        if (!positionedElement) {
            LOGE("Fix static shared element position failed. positioned element is null");
            return;
        }
        auto positionedRender = AceType::DynamicCast<RenderPositioned>(positionedElement->GetRenderNode());
        if (!positionedRender) {
            LOGE("Fix static shared element position failed. positioned render is null");
            return;
        }
        auto dest = effect->currentWorking_.Upgrade();
        if (!dest) {
            return;
        }
        auto offset = dest->GetGlobalOffset();
        positionedRender->SetTop(Dimension(offset.GetY(), DimensionUnit::PX));
        positionedRender->SetLeft(Dimension(offset.GetX(), DimensionUnit::PX));
    });
}

} // namespace OHOS::Ace
