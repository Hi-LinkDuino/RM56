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

#include "core/components/page_transition/page_transition_element.h"

#include "base/utils/system_properties.h"
#include "core/animation/shared_transition_controller.h"
#include "core/components/clip/clip_component.h"
#include "core/components/clip/clip_element.h"
#include "core/components/clip/render_clip.h"
#include "core/components/page/page_element.h"
#include "core/components/page_transition/page_transition_component.h"
#include "core/components/transition/transition_component.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t CHILDREN_SIZE_WHEN_SPLIT = 2;

TransitionOptionType GetOptionType(bool hasSharedTransition, TransitionDirection direction)
{
    if (hasSharedTransition) {
        if (direction == TransitionDirection::TRANSITION_IN) {
            return TransitionOptionType::TRANSITION_SHARED_IN;
        } else {
            return TransitionOptionType::TRANSITION_SHARED_OUT;
        }
    } else {
        if (direction == TransitionDirection::TRANSITION_IN) {
            return TransitionOptionType::TRANSITION_IN;
        } else {
            return TransitionOptionType::TRANSITION_OUT;
        }
    }
}

} // namespace

void PageTransitionElement::Update()
{
    StackElement::Update();
    UpdateTransitionOption();
}

void PageTransitionElement::PerformBuild()
{
    // PageTransitionElement only have two children. one is content, the other is background.
    if (!children_.empty()) {
        LOGE("perform build failed. not empty children. size: %{public}u, skip perform build.",
            static_cast<int32_t>(children_.size()));
        return;
    }

    auto pageTransitionComponent = AceType::DynamicCast<PageTransitionComponent>(component_);
    if (!pageTransitionComponent) {
        LOGE("PageTransitionElement::PerformBuild: get PageTransitionComponent failed!");
        return;
    }

    if (!controller_) {
        LOGD("create animator.");
        controller_ = AceType::MakeRefPtr<Animator>(context_);
    }
    UpdateTransitionOption();

    if (pageTransitionComponent->GetSeparation()) {
        BuildSeparatedChild(pageTransitionComponent);
    } else {
        BuildCombinedChild(pageTransitionComponent);
    }
    SetTransitionController();
}

void PageTransitionElement::SetTransitionController()
{
    if (!controller_) {
        LOGE("set transition controller failed. controller is null");
        return;
    }

    // stop controller first.
    if (!controller_->IsStopped()) {
        controller_->Stop();
    }

    if (contentTransition_) {
        auto contentController = contentTransition_->GetController();
        if (contentController && (!contentController->IsStopped())) {
            contentController->Stop();
        }
        contentTransition_->SetController(controller_);
    }
    if (frontDecorationTransition_) {
        auto frontController = frontDecorationTransition_->GetController();
        if (frontController && (!frontController->IsStopped())) {
            frontController->Stop();
        }
        frontDecorationTransition_->SetController(controller_);
    }
    if (backgroundTransition_) {
        auto backgroundController = backgroundTransition_->GetController();
        if (backgroundController && (!backgroundController->IsStopped())) {
            backgroundController->Stop();
        }
        backgroundTransition_->SetController(controller_);
    }
}

void PageTransitionElement::SetTransitionDirection(TransitionEvent event, TransitionDirection direction)
{
    if (!controller_) {
        LOGE("set transition direction failed. controller is empty.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("set transition direction failed. context is empty.");
        return;
    }
    auto sharedController = context->GetSharedTransitionController();
    if (!sharedController) {
        LOGE("set transition direction failed. shared controller is null.");
        return;
    }
    controller_->ClearInterpolators();

    // stop controller first.
    if (!controller_->IsStopped()) {
        controller_->Stop();
    }
    TransitionOptionType optionType;
    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::TV) {
        // no shared transition UI standard on tv, just use default page transition parameters
        optionType = GetOptionType(false, direction);
    } else {
        bool hasShared = sharedController->HasSharedTransition(event);
        optionType = GetOptionType(hasShared, direction);
    }
    if (contentTransition_) {
        LOGD("set content transition option type: %{public}d", optionType);
        contentTransition_->SwitchTransitionOption(optionType);
    }
    if (frontDecorationTransition_) {
        LOGD("set front transition option type: %{public}d", direction);
        frontDecorationTransition_->SwitchTransitionOption(optionType);
    }
    if (backgroundTransition_) {
        LOGD("set background transition option type: %{public}d", optionType);
        backgroundTransition_->SwitchTransitionOption(optionType);
    }
    if (context && context->GetIsDeclarative()) {
        if (floatAnimation_) {
            controller_->AddInterpolator(std::move(floatAnimation_));
            controller_->SetAllowRunningAsynchronously(false);
        } else {
            controller_->SetAllowRunningAsynchronously(true);
        }
    }
}

const RefPtr<Animator>& PageTransitionElement::GetTransitionController() const
{
    return controller_;
}

void PageTransitionElement::UpdateTransitionOption()
{
    if (!component_) {
        LOGE("update transition option failed. component is null.");
        return;
    }
    auto transitionComponent = AceType::DynamicCast<PageTransitionComponent>(component_);
    if (!transitionComponent) {
        LOGE("update transition option failed. transition is null.");
        return;
    }
    isRightToLeft_ = transitionComponent->GetTextDirection() == TextDirection::RTL;

    LOGI("Use user-defined transition parameters.");
    contentInOption_ = transitionComponent->GetContentTransitionInOption();
    contentOutOption_ = transitionComponent->GetContentTransitionOutOption();
    pageTransitions_ = transitionComponent->GetPageTransitions();

    sharedInOption_ = contentInOption_;
    sharedOutOption_ = contentOutOption_;
    isCustomOptionIn_ = contentInOption_.IsValid();
    isCustomOptionOut_ = contentOutOption_.IsValid();
}

RefPtr<PageTransitionElement> PageTransitionElement::GetTransitionElement(const RefPtr<Element>& element)
{
    // first try with page element.
    RefPtr<PageElement> page = AceType::DynamicCast<PageElement>(element);
    if (!page) {
        return nullptr;
    }
    LOGD("try to get transition element from page element.");
    auto child = page->GetFirstChild();
    return AceType::DynamicCast<PageTransitionElement>(child);
}

void PageTransitionElement::SetTouchable(bool enable)
{
    if (backgroundTransition_) {
        backgroundTransition_->SetTouchable(enable);
    }

    if (contentTransition_) {
        contentTransition_->SetTouchable(enable);
    }
}

void PageTransitionElement::InitTransitionClip()
{
    if (!contentTransition_) {
        LOGE("InitTransitionClip failed, content transition is null.");
        return;
    }
    auto clipElement = contentTransition_->GetContentElement();
    if (AceType::InstanceOf<ClipElement>(clipElement)) {
        RefPtr<RenderClip> clipRender = DynamicCast<RenderClip>(clipElement->GetRenderNode());
        if (clipRender) {
            clipRender->SetWidth(0.0);
            clipRender->SetHeight(0.0);
        }
    }
}

void PageTransitionElement::InitController(TransitionDirection direction, TransitionEvent event)
{
    if (!controller_) {
        LOGE("init controller failed. controller is null.");
        return;
    }
    if (event == TransitionEvent::PUSH_END || event == TransitionEvent::POP_END) {
        LOGE("init controller failed. event can not be handled. event: %{public}d", event);
        return;
    }
    SetTouchable(false);
    if ((direction == TransitionDirection::TRANSITION_OUT) && (event == TransitionEvent::PUSH_START)) {
        LOGD("No need to make it touchable after transition done, because page will not on the top of the stage.");
        return;
    }
    if ((direction == TransitionDirection::TRANSITION_IN) && (event == TransitionEvent::POP_START)) {
        LOGD("No need to make it touchable after transition done, because page will be destroyed.");
        return;
    }
    auto weak = AceType::WeakClaim(this);
    controller_->AddStopListener([weak]() {
        LOGD("transition complete, prepare to clear it");
        auto transition = weak.Upgrade();
        if (transition) {
            LOGD("transition complete, clear it");
            transition->SetTouchable(true);
        }
    });
}

void PageTransitionElement::SetWrapHidden(bool hidden)
{
    if (contentTransition_) {
        contentTransition_->SetWrapHidden(hidden);
    }

    if (backgroundTransition_) {
        backgroundTransition_->SetWrapHidden(hidden);
    }
}

void PageTransitionElement::AddPreFlush()
{
    if (contentTransition_) {
        contentTransition_->AddPreFlush();
    }

    if (backgroundTransition_) {
        backgroundTransition_->AddPreFlush();
    }
}

void PageTransitionElement::SkipPostFlush()
{
    if (contentTransition_) {
        contentTransition_->SkipPostFlush();
    }

    if (backgroundTransition_) {
        backgroundTransition_->SkipPostFlush();
    }
}

RefPtr<Element> PageTransitionElement::GetContentElement() const
{
    if (!contentTransition_) {
        LOGE("get content element failed. content tween is null.");
        return nullptr;
    }
    auto element = contentTransition_->GetContentElement();
    if (AceType::InstanceOf<ClipElement>(element)) {
        auto transition = DynamicCast<TransitionElement>(element->GetFirstChild());
        if (transition) {
            auto frontDecorationBox = transition->GetContentElement();
            if (frontDecorationBox) {
                return frontDecorationBox->GetFirstChild();
            }
        }
        return transition;
    }
    return element;
}

void PageTransitionElement::LoadTransition()
{
    auto pageElement = GetPageElement();
    if (!pageElement) {
        return;
    }
    auto componentUpdated = pageElement->CallPageTransitionFunction();
    // save origin component
    auto componentOrigin = component_;
    component_ = componentUpdated;
    // update with updated component
    UpdateTransitionOption();
    // restore origin component
    component_ = componentOrigin;
}

void PageTransitionElement::ResetPageTransitionAnimation()
{
    if (contentTransition_) {
        contentTransition_->ResetPageTransitionAnimation();
    }
}

void PageTransitionElement::SetTransition(
    DeviceType deviceType, TransitionEvent event, TransitionDirection direction, const RRect& rrect)
{
    auto tweenOption =
        TransitionTweenOptionFactory::CreateTransitionTweenOption(deviceType, event, isRightToLeft_, rrect, context_);
    if (!tweenOption) {
        LOGE("TransitionTweenOption is null.");
        return;
    }
    bool isSetOutOption = false;
    int32_t duration = tweenOption->GetTransitionContentInOption().GetDuration();
    int32_t delay = 0;
    if (direction == TransitionDirection::TRANSITION_OUT) {
        if (isCustomOptionOut_) {
            if (contentOutOption_.HasDurationChanged()) {
                duration = contentOutOption_.GetDuration();
            }
            isSetOutOption = true;
        } else {
            contentOutOption_ = tweenOption->GetTransitionContentOutOption();
            sharedOutOption_ = tweenOption->GetSharedOutOption();
        }
    }
    if (direction == TransitionDirection::TRANSITION_IN) {
        if (isCustomOptionIn_) {
            if (contentInOption_.HasDurationChanged()) {
                duration = contentInOption_.GetDuration();
            }
        } else {
            contentInOption_ = tweenOption->GetTransitionContentInOption();
            sharedInOption_ = tweenOption->GetSharedInOption();
        }
    }
    auto context = GetContext().Upgrade();
    if (context && context->GetIsDeclarative()) {
        auto pageTransition = GetCurrentPageTransition(event, direction_);
        isCustomOption_ = false;
        isSetOutOption = true;
        if (direction == TransitionDirection::TRANSITION_OUT) {
            if (pageTransition) {
                contentOutOption_ = ProcessPageTransition(pageTransition, event);
                if (contentOutOption_.HasDurationChanged()) {
                    duration = contentOutOption_.GetDuration();
                }
                delay = contentOutOption_.GetDelay();
                isCustomOption_ = true;
                sharedOutOption_ = contentOutOption_;
            }
        } else {
            if (pageTransition) {
                contentInOption_ = ProcessPageTransition(pageTransition, event);
                if (contentInOption_.HasDurationChanged()) {
                    duration = contentInOption_.GetDuration();
                }
                delay = contentInOption_.GetDelay();
                isCustomOption_ = true;
                sharedInOption_ = contentInOption_;
            }
        }
    }
    if (controller_) {
        controller_->SetDuration(duration);
        if (context && context->GetIsDeclarative() && delay >= 0) {
            controller_->SetStartDelay(delay);
        }
    }
    if (contentTransition_) {
        contentTransition_->SetTransition(contentInOption_, contentOutOption_);
        contentTransition_->SetSharedTransition(sharedInOption_, sharedOutOption_);
    }
    if (frontDecorationTransition_ && !isSetOutOption) {
        // do not need option in.
        TweenOption optionIn;
        frontDecorationTransition_->SetTransition(optionIn, tweenOption->GetTransitionFrontDecorationOption());
        frontDecorationTransition_->SetSharedTransition(
            optionIn, tweenOption->GetSharedTransitionFrontDecorationOption());
    }
    if (backgroundTransition_) {
        backgroundTransition_->SetTransition(
            tweenOption->GetTransitionBackgroundInOption(), tweenOption->GetTransitionBackgroundOutOption());
    }
}

void PageTransitionElement::BuildCombinedChild(const RefPtr<StackComponent>& component)
{
    auto pageTransitionComponent = AceType::DynamicCast<PageTransitionComponent>(component);
    if (!pageTransitionComponent) {
        LOGE("Get PageTransitionComponent failed!");
        return;
    }
    // create transition for content
    auto box = AceType::MakeRefPtr<BoxComponent>();
    Component::MergeRSNode(box);
    auto front = AceType::MakeRefPtr<Decoration>();
    front->SetBackgroundColor(Color::FromRGBO(0, 0, 0, 0.0));
    box->SetFrontDecoration(front);
    box->SetChild(pageTransitionComponent->GetContent());
    auto transition = AceType::MakeRefPtr<TransitionComponent>(
        TransitionComponent::AllocTransitionComponentId(), "frontDecoration_transition", box);

    auto clip = AceType::MakeRefPtr<ClipComponent>(transition);
    Component::MergeRSNode(clip);
    auto contentTransitionComponent = AceType::MakeRefPtr<TransitionComponent>(
        TransitionComponent::AllocTransitionComponentId(), "page_transition_content", clip);

    // add transition for content
    pageTransitionComponent->AppendChild(contentTransitionComponent);
    StackElement::PerformBuild();

    if (children_.size() != 1) {
        LOGE("the children size is error.");
        return;
    }
    auto childIter = children_.begin();
    auto child = *childIter;

    // child for content.
    contentTransition_ = AceType::DynamicCast<TransitionElement>(child);
    auto frontElement = contentTransition_->GetContentElement();
    if (frontElement) {
        frontDecorationTransition_ = DynamicCast<TransitionElement>(frontElement->GetFirstChild());
    }
}

void PageTransitionElement::BuildSeparatedChild(const RefPtr<StackComponent>& component)
{
    auto pageTransitionComponent = AceType::DynamicCast<PageTransitionComponent>(component);
    if (!pageTransitionComponent) {
        LOGE("BuildSeparatedChild : get PageTransitionComponent failed!");
        return;
    }
    // add transition for background
    pageTransitionComponent->AppendChild(
        AceType::MakeRefPtr<TransitionComponent>(TransitionComponent::AllocTransitionComponentId(),
            "page_transition_background", pageTransitionComponent->GetBackground()));

    // create transition for content
    auto contentTransitionComponent =
        AceType::MakeRefPtr<TransitionComponent>(TransitionComponent::AllocTransitionComponentId(),
            "page_transition_content", pageTransitionComponent->GetContent());

    // add transition for content
    pageTransitionComponent->AppendChild(contentTransitionComponent);
    StackElement::PerformBuild();

    if (children_.size() != CHILDREN_SIZE_WHEN_SPLIT) {
        LOGE("the children size is error.");
        return;
    }
    auto childIter = children_.begin();
    auto child = *childIter;

    // child for background
    backgroundTransition_ = AceType::DynamicCast<TransitionElement>(child);
    child = *(++childIter);

    // child for content.
    contentTransition_ = AceType::DynamicCast<TransitionElement>(child);
}

const RefPtr<TransitionElement>& PageTransitionElement::GetContentTransitionElement() const
{
    return contentTransition_;
}

const RefPtr<TransitionElement>& PageTransitionElement::GetBackgroundTransitionElement() const
{
    return backgroundTransition_;
}

TweenOption PageTransitionElement::ProcessPageTransition(
    const RefPtr<PageTransition>& pageTransition, TransitionEvent event)
{
    auto tweenOption = pageTransition->GetTweenOption();
    // 1.SlideEffect
    auto transitionDeclarativeTweenOption = TransitionDeclarativeTweenOption(isRightToLeft_, GetContext());
    transitionDeclarativeTweenOption.CreateSlideEffectAnimation(
        tweenOption, pageTransition->GetSlideEffect(), pageTransition->GetType(), direction_);
    // 2. callback
    auto onExitHandler = pageTransition->GetOnExitHandler();
    auto onEnterHandler = pageTransition->GetOnEnterHandler();
    RouteType type = RouteType::PUSH;
    if (event == TransitionEvent::POP_START || event == TransitionEvent::POP_END) {
        type = RouteType::POP;
    }
    if (onExitHandler || onEnterHandler) {
        floatAnimation_ = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, 1.0f, pageTransition->GetCurve());
        if (onExitHandler) {
            floatAnimation_->AddListener(
                [type, onExitHandler](const float& progress) { onExitHandler(type, progress); });
        }
        if (onEnterHandler) {
            floatAnimation_->AddListener(
                [type, onEnterHandler](const float& progress) { onEnterHandler(type, progress); });
        }
    }
    // 3. delay curve
    return tweenOption;
}

RefPtr<PageTransition> PageTransitionElement::GetCurrentPageTransition(
    TransitionEvent event, TransitionDirection direction) const
{
    if (pageTransitions_.empty()) {
        return nullptr;
    }
    auto type = GetPageTransitionType(event, direction);
    auto pos = pageTransitions_.find(type);
    if (pos != pageTransitions_.end()) {
        return pos->second;
    }

    if (direction == TransitionDirection::TRANSITION_IN) {
        type = PageTransitionType::ENTER;
    } else {
        type = PageTransitionType::EXIT;
    }
    pos = pageTransitions_.find(type);
    if (pos != pageTransitions_.end()) {
        return pos->second;
    }
    return nullptr;
}

PageTransitionType PageTransitionElement::GetPageTransitionType(TransitionEvent event, TransitionDirection direction)
{
    if (direction == TransitionDirection::TRANSITION_IN) {
        if (event == TransitionEvent::POP_START) {
            return PageTransitionType::ENTER_POP;
        } else {
            return PageTransitionType::ENTER_PUSH;
        }
    } else {
        if (event == TransitionEvent::POP_START) {
            return PageTransitionType::EXIT_POP;
        } else {
            return PageTransitionType::EXIT_PUSH;
        }
    }
}

} // namespace OHOS::Ace
