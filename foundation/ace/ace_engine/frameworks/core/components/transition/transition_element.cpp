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

#include "core/components/transition/transition_element.h"

#include "core/components/box/render_box_base.h"
#include "core/components/transform/transform_element.h"
#include "core/components/transition/transition_component.h"
#include "core/components/tween/tween_component.h"
#include "core/components/tween/tween_element.h"

namespace OHOS::Ace {

void TransitionElement::Update()
{
    ComposedElement::Update();
    if (!component_) {
        LOGE("Update failed. component is null");
        return;
    }
    const auto transitionComponent = AceType::DynamicCast<TransitionComponent>(component_);
    if (!transitionComponent) {
        LOGE("transition element update failed. transition component is null.");
        return;
    }
    optionMap_[TransitionOptionType::TRANSITION_IN] = transitionComponent->GetTransitionInOption();
    optionMap_[TransitionOptionType::TRANSITION_OUT] = transitionComponent->GetTransitionOutOption();

    if (transitionComponent->IsOptionChanged()) {
        transitionOption_ = transitionComponent->GetTransitionOption();
        transitionComponent->MarkOptionChanged(false);
    }

    auto pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        if (!controller_) {
            controller_ = AceType::MakeRefPtr<Animator>(context_);
            LOGD("set simulation controller to transition component when update.");
        } else {
            controller_->Stop();
            controller_->ClearInterpolators();
        }
    }
}

void TransitionElement::PerformBuild()
{
    bool build = true;
    if (!hasBuildChild_) {
        ComposedElement::PerformBuild();
        hasBuildChild_ = true;
        build = false;
    }

    if (transitionOption_.IsValid()) {
        ReplaceAnimation(transitionOption_);
    } else {
        transitionOption_ = TweenOption();
        if (build) {
            ComposedElement::PerformBuild();
        }
        return;
    }

    if (controller_) {
        SetController(controller_);
        ApplyAnimation(transitionOption_);
        controller_->Play();
    }
    transitionOption_ = TweenOption();
    if (build) {
        ComposedElement::PerformBuild();
    }
}

void TransitionElement::ReplaceAnimation(TweenOption& transitionOption)
{
    auto& propertyAnimationMap = transitionOption.GetFloatPropertyAnimation();
    auto& colorAniamtion = transitionOption.GetColorAnimation();
    auto& opacityAniamtion = transitionOption.GetOpacityAnimation();
    auto elementBox = GetChildBox();
    if (!elementBox) {
        LOGE("box element get failed.");
        return;
    }
    auto renderBox = AceType::DynamicCast<RenderBoxBase>(elementBox->GetRenderNode());
    if (!renderBox) {
        LOGE("box render get failed.");
        return;
    }
    for (auto&& [propertyAnimatableType, propertyAnimation] : propertyAnimationMap) {
        if (propertyAnimatableType == PropertyAnimatableType::PROPERTY_WIDTH) {
            renderBox->UpdateStyleFromRenderNode(propertyAnimatableType);
            float width = renderBox->GetWidth();
            auto keyframeWidthBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0f, width);
            AceType::DynamicCast<KeyframeAnimation<float>>
                (propertyAnimationMap[PropertyAnimatableType::PROPERTY_WIDTH])->ReplaceKeyframe(keyframeWidthBegin);
        }
        if (propertyAnimatableType == PropertyAnimatableType::PROPERTY_HEIGHT) {
            renderBox->UpdateStyleFromRenderNode(propertyAnimatableType);
            float height = renderBox->GetHeight();
            auto keyframeHeightBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0f, height);
            AceType::DynamicCast<KeyframeAnimation<float>>
                (propertyAnimationMap[PropertyAnimatableType::PROPERTY_HEIGHT])->ReplaceKeyframe(keyframeHeightBegin);
        }
    }
    if (colorAniamtion) {
        renderBox->UpdateStyleFromRenderNode(PropertyAnimatableType::PROPERTY_BACK_DECORATION_COLOR);
        Color color = renderBox->GetColor();
        auto keyframeColorBegin = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, color);
        AceType::DynamicCast<KeyframeAnimation<Color>>(colorAniamtion)->ReplaceKeyframe(keyframeColorBegin);
    }

    auto display = GetChildDisplay();
    if (display && opacityAniamtion) {
        auto renderDisplay = AceType::DynamicCast<RenderDisplay>(display->GetRenderNode());
        if (renderDisplay) {
            renderDisplay->UpdateOpacity();
            auto opacity = renderDisplay->GetOpacity() * (1.0 / UINT8_MAX);
            auto keyframeOpacityBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0f, opacity);
            AceType::DynamicCast<KeyframeAnimation<float>>(opacityAniamtion)->ReplaceKeyframe(keyframeOpacityBegin);
        }
    }
}

RefPtr<DisplayElement> TransitionElement::GetChildDisplay() const
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("transition option get failed. no tween found.");
        return nullptr;
    }
    return AceType::DynamicCast<DisplayElement>(tween->GetFirstChild());
}

RefPtr<TransformElement> TransitionElement::GetChildTransform() const
{
    auto display = GetChildDisplay();
    if (!display) {
        LOGE("transition option get failed. no display found.");
        return nullptr;
    }
    return AceType::DynamicCast<TransformElement>(display->GetFirstChild());
}

RefPtr<BoxBaseElement> TransitionElement::GetChildBox() const
{
    auto elementDisplay = GetChildDisplay();
    if (!elementDisplay) {
        LOGE("display element get failed.");
        return nullptr;
    }
    auto elementTransform = AceType::DynamicCast<TransformElement>(elementDisplay->GetFirstChild());
    if (!elementTransform) {
        LOGE("transform element get failed.");
        return nullptr;
    }
    return AceType::DynamicCast<BoxBaseElement>(elementTransform->GetFirstChild());
}

void TransitionElement::ApplyAnimation(TweenOption& transitionOption)
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("transition option get failed. no tween found.");
        return;
    }

    tween->SetOption(transitionOption);
    if (!tween->ApplyKeyframes()) {
        LOGW("Apply transition option failed. tween apply option fail.");
    }
    tween->ApplyOptions();
}

void TransitionElement::SetController(const RefPtr<Animator>& controller)
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("set controller failed. no tween found.");
        return;
    }
    tween->SetController(controller);
}

RefPtr<Animator> TransitionElement::GetController() const
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("get controller failed. no tween found.");
        return nullptr;
    }
    return tween->GetController();
}

void TransitionElement::SetTouchable(bool enable)
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("set touchable failed. no tween found. enable: %{public}d", enable);
        return;
    }
    tween->SetTouchable(enable);
}

void TransitionElement::SwitchTransitionOption(TransitionOptionType type, bool needApplyOption)
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("Switch transition option failed. no tween found. direction: %{public}d", type);
        return;
    }
    optionMap_[type].ClearListeners();
    // If never set before, use empty option instead.
    tween->SetOption(optionMap_[type]);
    if (!tween->ApplyKeyframes()) {
        LOGW("Apply transition option failed. tween apply option fail.");
    }
    if (needApplyOption) {
        tween->ApplyOptions();
    }
}

RefPtr<Component> TransitionElement::BuildChild()
{
    RefPtr<TransitionComponent> transition = AceType::DynamicCast<TransitionComponent>(component_);
    if (transition) {
        RefPtr<TweenComponent> tweenComponent =
            AceType::MakeRefPtr<TweenComponent>(TweenComponent::AllocTweenComponentId(), transition->GetName());
        tweenComponent->SetChild(ComposedElement::BuildChild());
        tweenComponent->SetIsFirstFrameShow(transition->IsFirstFrameShow());
        return tweenComponent;
    } else {
        LOGE("no transition component found. return empty child.");
        return nullptr;
    }
}

void TransitionElement::SetWrapHidden(bool hidden)
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("set wrap hidden failed. no tween found. hidden: %{public}d", hidden);
        return;
    }
    tween->SetWrapHidden(hidden);
}

void TransitionElement::AddPreFlush()
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("Add pre flush failed. no tween found.");
        return;
    }
    tween->AddPreFlush();
}

void TransitionElement::SkipPostFlush()
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("Skip post flush failed. no tween found.");
        return;
    }
    tween->SkipPostFlush();
}

RefPtr<TweenElement> TransitionElement::GetChildTween() const
{
    if (children_.empty()) {
        LOGW("get child tween failed. no child yet.");
        return nullptr;
    }
    const auto& child = children_.front();
    if (!child) {
        LOGW("get child tween failed. null child.");
        return nullptr;
    }
    auto tween = AceType::DynamicCast<TweenElement>(child);
    if (!tween) {
        LOGW("get child tween failed. null tween.");
        return nullptr;
    }
    return tween;
}

void TransitionElement::SetTransition(const TweenOption& inOption, const TweenOption& outOption)
{
    optionMap_[TransitionOptionType::TRANSITION_IN] = inOption;
    optionMap_[TransitionOptionType::TRANSITION_OUT] = outOption;
}

void TransitionElement::SetSharedTransition(const TweenOption& inOption, const TweenOption& outOption)
{
    optionMap_[TransitionOptionType::TRANSITION_SHARED_IN] = inOption;
    optionMap_[TransitionOptionType::TRANSITION_SHARED_OUT] = outOption;
}

RefPtr<Element> TransitionElement::GetContentElement() const
{
    auto tween = GetChildTween();
    if (!tween) {
        LOGE("get content element failed. no tween found.");
        return nullptr;
    }
    return tween->GetContentElement();
}

void TransitionElement::ResetPageTransitionAnimation() const
{
    // reset opacity
    auto display = GetChildDisplay();
    if (display) {
        auto renderDisplay = AceType::DynamicCast<RenderDisplay>(display->GetRenderNode());
        if (renderDisplay) {
            renderDisplay->UpdateOpacity(UINT8_MAX);
        }
    }
    // reset transform
    auto transform = GetChildTransform();
    if (transform) {
        auto renderTransform = AceType::DynamicCast<RenderTransform>(transform->GetRenderNode());
        if (renderTransform) {
            renderTransform->ResetTransform();
        }
    }
}

} // namespace OHOS::Ace
