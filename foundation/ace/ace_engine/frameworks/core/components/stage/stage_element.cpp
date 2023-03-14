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

#include "core/components/stage/stage_element.h"

#include "base/log/ace_trace.h"
#include "base/utils/system_properties.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/shared_transition_controller.h"
#include "core/components/display/display_element.h"
#include "core/components/display/render_display.h"
#include "core/components/page/page_element.h"
#include "core/components/page_transition/page_transition_element.h"
#include "core/components/stage/render_stage.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t POP_TO_LEAST_COUNT = 2;

void StartSharedController(WeakPtr<PipelineContext> contextWeak, TransitionEvent event, int32_t duration)
{
    auto context = contextWeak.Upgrade();
    if (!context) {
        LOGE("Get Shared Controller failed. context is null.");
        return;
    }
    auto sharedTransitionController = context->GetSharedTransitionController();
    if (!sharedTransitionController) {
        LOGE("Get Shared Controller failed. shared transition controller is null.");
        return;
    }
    if (!sharedTransitionController->HasSharedTransition(event)) {
        return;
    }

    for (const auto& controller : sharedTransitionController->GetAnimators()) {
        controller->AttachScheduler(context);
        if (controller->GetDuration() == 0) {
            controller->SetDuration(duration);
        }
        controller->Forward();
    }
}

RefPtr<Animator> GetCardController(WeakPtr<PipelineContext> contextWeak, const ComposeId& composeId)
{
    auto context = contextWeak.Upgrade();
    if (!context) {
        LOGE("Get card controller failed. context is null.");
        return nullptr;
    }
    auto cardTransitionController = context->GetCardTransitionController();
    if (!cardTransitionController) {
        LOGE("Get card controller failed. shared transition controller is null.");
        return nullptr;
    }
    if (!cardTransitionController->GetCardRect(composeId).GetRect().IsValid()) {
        return nullptr;
    }
    return cardTransitionController->GetAnimator();
}

RRect GetCardRect(WeakPtr<PipelineContext> contextWeak, const ComposeId& composeId)
{
    auto context = contextWeak.Upgrade();
    if (!context) {
        LOGE("Get card controller failed. context is null.");
        return RRect();
    }
    auto cardTransitionController = context->GetCardTransitionController();
    if (!cardTransitionController) {
        LOGE("Get card controller failed. shared transition controller is null.");
        return RRect();
    }
    return cardTransitionController->GetCardRect(composeId);
}

} // namespace

bool StageElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (!IsCurrentFocus()) {
        LOGE("stage is not current focus.");
        return false;
    }
    if (itLastFocusNode_ != focusNodes_.end() && (*itLastFocusNode_)->HandleKeyEvent(keyEvent)) {
        return true;
    }
    if (FocusNode::OnKeyEvent(keyEvent)) {
        return true;
    }

    return false;
}

void StageElement::MarkDirty()
{
    isWaitingForBuild_ = true;
    StackElement::MarkDirty();
}

bool StageElement::CanRouterPage()
{
    if (isWaitingForBuild_) {
        LOGE("StageElement: waiting for performBuild. Not ready for router page.");
        return false;
    }
    if (pendingOperation_ != StackOperation::NONE) {
        LOGE("StageElement: waiting for pending operation: %{public}d. Not ready for router page.", pendingOperation_);
        return false;
    }
    if (!IsTransitionStop()) {
        LOGE("router page failed. transition animation is not complete.");
        return false;
    }
    return true;
}

void StageElement::PostponePageTransition()
{
    postponePageTransition_ = true;
}

void StageElement::LaunchPageTransition()
{
    postponePageTransition_ = false;
    auto context = context_.Upgrade();
    if (pendingOperation_ != StackOperation::NONE && context) {
        context->AddPostFlushListener(AceType::Claim(this));
    }
}

bool StageElement::CanPushPage()
{
    return CanRouterPage();
}

void StageElement::PushPage(const RefPtr<Component>& newComponent)
{
    if (!CanPushPage()) {
        return;
    }
    operation_ = StackOperation::PUSH_PAGE;
    newComponent_ = newComponent;
    MarkDirty();
}

bool StageElement::CanPopPage()
{
    if (!CanRouterPage()) {
        return false;
    }

    if (children_.empty() || children_.size() == 1) {
        LOGE("StageElement: no extra page for pop.");
        return false;
    }
    return true;
}

void StageElement::Pop()
{
    if (!CanPopPage()) {
        return;
    }
    operation_ = StackOperation::POP;
    MarkDirty();
}

void StageElement::PopToPage(int32_t pageId)
{
    if (!CanPopPage()) {
        return;
    }
    operation_ = StackOperation::POP_TO_PAGE;
    directedPageId_ = pageId;
    MarkDirty();
}

void StageElement::RestorePopPage(const RefPtr<Component>& newComponent)
{
    operation_ = StackOperation::RESTORE;
    newComponent_ = newComponent;
    MarkDirty();
}

bool StageElement::CanReplacePage()
{
    if (!CanRouterPage()) {
        return false;
    }
    if (children_.empty()) {
        LOGE("StageElement: no extra page for replace.");
        return false;
    }
    return true;
}

void StageElement::Replace(const RefPtr<Component>& newComponent)
{
    if (!CanReplacePage()) {
        return;
    }
    operation_ = StackOperation::REPLACE;
    newComponent_ = newComponent;
    MarkDirty();
}

bool StageElement::ClearOffStage()
{
    if (!CanPopPage()) {
        return false;
    }
    operation_ = StackOperation::CLEAR;
    MarkDirty();
    return true;
}

void StageElement::PerformBuild()
{
    LOGD("StageElement: PerformBuild, operation: %{public}d", operation_);
    switch (operation_) {
        case StackOperation::NONE:
            break;
        case StackOperation::RESTORE: {
            RestorePop();
            break;
        }
        case StackOperation::PUSH_PAGE: {
            PerformPushPage();
            auto render = GetRenderNode();
            if (render) {
                render->MarkNeedLayout();
            }
            break;
        }
        case StackOperation::POP:
            PerformPop();
            break;
        case StackOperation::REPLACE: {
            PerformReplace();
            auto render = GetRenderNode();
            if (render) {
                render->MarkNeedLayout();
            }
            break;
        }
        case StackOperation::POP_TO_PAGE:
            PerformPopToPage();
            break;
        case StackOperation::CLEAR:
            PerformClear();
            break;
        default:
            break;
    }
    isWaitingForBuild_ = false;
}

void StageElement::RefreshFocus()
{
    // Process focus logic on the current top page when page changes
    if (IsFocusable()) {
        focusNodes_.back()->RequestFocus();
    } else {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("Pipeline context is nullptr");
            return;
        }
        context->RootLostFocus();
    }
}

bool StageElement::CheckPageTransitionElement(
    const RefPtr<PageTransitionElement>& transitionIn, const RefPtr<PageTransitionElement>& transitionOut)
{
    if (!transitionIn) {
        LOGW("transition in is empty, skip transition.");
        return false;
    }
    if (!transitionOut) {
        LOGW("transition out is empty, skip transition.");
        return false;
    }
    auto controllerIn = transitionIn->GetTransitionController();
    if (!controllerIn) {
        LOGW("controller in is null, skip transition.");
        return false;
    }
    auto controllerOut = transitionOut->GetTransitionController();
    if (!controllerOut) {
        LOGW("controller out is null, skip transition.");
        return false;
    }
    return true;
}

bool StageElement::PerformPushPageTransition(const RefPtr<Element>& elementIn, const RefPtr<Element>& elementOut)
{
    auto transitionIn = PageTransitionElement::GetTransitionElement(elementIn);
    auto transitionOut = PageTransitionElement::GetTransitionElement(elementOut);
    auto pageIn = AceType::DynamicCast<PageElement>(elementIn);
    auto pageOut = AceType::DynamicCast<PageElement>(elementOut);
    if (!CheckPageTransitionElement(transitionIn, transitionOut)) {
        LOGW("check page transition failed, skip push transition.");
        return false;
    }
    auto context = GetContext().Upgrade();
    if (context && context->GetIsDeclarative()) {
        transitionIn->SetDeclarativeDirection(TransitionDirection::TRANSITION_IN);
        transitionOut->SetDeclarativeDirection(TransitionDirection::TRANSITION_OUT);
    }
    if (!pageIn || !pageOut) {
        LOGE("push page failed. page in / out is null.");
        return false;
    }
    LOGD("notify push page event. page id: in: %{public}d, out: %{public}d", pageIn->GetPageId(), pageOut->GetPageId());
    NotifyPageTransitionListeners(TransitionEvent::PUSH_START, pageIn, pageOut);
    ACE_SCOPED_TRACE("PUSH_START");
    if (!InitTransition(transitionIn, transitionOut, TransitionEvent::PUSH_START)) {
        LOGW("init transition failed, skip push transition.");
        return false;
    }
    if ((!controllerIn_) || (!controllerOut_)) {
        LOGE("push page failed. controller in / out is null.");
        return false;
    }
    auto weak = AceType::WeakClaim(this);
    WeakPtr<PageElement> pageInWeak = pageIn;
    WeakPtr<PageElement> pageOutWeak = pageOut;
    controllerIn_->AddStopListener([weak, pageInWeak, pageOutWeak]() {
        auto stage = weak.Upgrade();
        if (stage) {
            stage->NotifyPageTransitionListeners(TransitionEvent::PUSH_END, pageInWeak, pageOutWeak);
            ACE_SCOPED_TRACE("PUSH_END");
            auto context = stage->context_.Upgrade();
            if (context) {
                context->OnPageShow();
            }
        }
    });
    // make stage untouchable when push page.
    PerformPushPageInStage(pageOut);
    LOGD("start push transition.");
    controllerIn_->Forward();
    controllerOut_->Forward();
    StartSharedController(context_, TransitionEvent::PUSH_START, controllerIn_->GetDuration());
    auto cardController = GetCardController(context_, pageIn->GetCardComposeId());
    if (cardController) {
        cardController->SetDuration(controllerIn_->GetDuration());
        cardController->Forward();
    }
    return true;
}

void StageElement::AddListenerForPopPage(
    const WeakPtr<PageElement>& pageInWeak, const WeakPtr<PageElement>& pageOutWeak)
{
    auto weak = AceType::WeakClaim(this);
    // Add stop listener to remove top page when transition done.
    controllerIn_->AddStopListener([weak, pageInWeak, pageOutWeak]() {
        auto stage = weak.Upgrade();
        auto elementIn = DynamicCast<Element>(pageInWeak.Upgrade());
        if (stage && elementIn) {
            // Remove top page.
            stage->UpdateChild(elementIn, nullptr);
            stage->MakeTopPageTouchable();
            stage->NotifyPageTransitionListeners(TransitionEvent::POP_END, pageInWeak, pageOutWeak);
            ACE_SCOPED_TRACE("POP_END");
            stage->RefreshFocus();
        }
    });
}

bool StageElement::PerformPopPageTransition(const RefPtr<Element>& elementIn, const RefPtr<Element>& elementOut)
{
    auto transitionIn = PageTransitionElement::GetTransitionElement(elementIn);
    auto transitionOut = PageTransitionElement::GetTransitionElement(elementOut);
    auto pageIn = AceType::DynamicCast<PageElement>(elementIn);
    auto pageOut = AceType::DynamicCast<PageElement>(elementOut);
    if (!CheckPageTransitionElement(transitionIn, transitionOut)) {
        LOGW("check page transition failed, skip pop transition.");
        return false;
    }
    auto context = GetContext().Upgrade();
    if (context && context->GetIsDeclarative()) {
        transitionIn->SetDeclarativeDirection(TransitionDirection::TRANSITION_OUT);
        transitionOut->SetDeclarativeDirection(TransitionDirection::TRANSITION_IN);
    }
    NotifyPageTransitionListeners(TransitionEvent::POP_START, pageIn, pageOut);
    ACE_SCOPED_TRACE("POP_START");
    if (!InitTransition(transitionIn, transitionOut, TransitionEvent::POP_START)) {
        LOGW("init transition failed, skip pop transition.");
        return false;
    }
    if (!pageIn || !pageOut) {
        LOGE("pop page failed. page in / out is null.");
        return false;
    }
    if ((!controllerIn_) || (!controllerOut_)) {
        LOGE("pop page failed. controller in / out is null.");
        return false;
    }
    AddListenerForPopPage(pageIn, pageOut);
    PerformPopPageInStage(pageOut, transitionOut);
#ifndef WEARABLE_PRODUCT
    PerformPopMultimodalScene(pageIn->GetPageId(), pageOut->GetPageId());
#endif
    LOGD("start pop transition.");
    RRect cardRRect = GetCardRect(context_, pageIn->GetCardComposeId());
    if (cardRRect.GetRect().IsValid()) {
        controllerIn_->Forward();
        controllerOut_->Forward();
    } else {
        if (context && context->GetIsDeclarative()) {
            if (transitionIn->GetIsCustomOption()) {
                controllerIn_->Forward();
            } else {
                controllerIn_->Backward();
            }
            if (transitionOut->GetIsCustomOption()) {
                controllerOut_->Forward();
            } else {
                controllerOut_->Backward();
            }
        } else {
            controllerIn_->Backward();
            controllerOut_->Backward();
        }
    }
    StartSharedController(context_, TransitionEvent::POP_START, controllerIn_->GetDuration());
    auto cardController = GetCardController(context_, pageIn->GetCardComposeId());
    if (cardController) {
        cardController->SetDuration(controllerIn_->GetDuration());
        cardController->Forward();
    }
    return true;
}

void StageElement::PerformPushPage()
{
    LOGD("start to push page.");
#ifndef WEARABLE_PRODUCT
    auto pageComponent = DynamicCast<PageComponent>(newComponent_);
    if (pageComponent) {
        PerformPushMultimodalScene(pageComponent->GetPageId());
    } else {
        LOGW("fail to perform push scene due to page component is null");
    }
#endif
    RefPtr<Element> topElement;
    if (children_.empty()) {
        LOGD("push first page, just update child, no transition.");
        NotifyPageTransitionListeners(TransitionEvent::PUSH_START, nullptr, nullptr);
        auto newElement = UpdateChild(nullptr, newComponent_);
        auto pageIn = AceType::DynamicCast<PageElement>(newElement);
        if (!pageIn) {
            LOGE("no page element found, do not notify page transition event.");
            return;
        }
        LOGD("notify push first page event. page id: in: %{public}d.", pageIn->GetPageId());
        NotifyPageTransitionListeners(TransitionEvent::PUSH_END, pageIn, nullptr);
        return;
    } else {
        topElement = children_.back();
    }
    auto pushedElement = UpdateChild(nullptr, newComponent_);
    MakeTopPageTouchable();
    auto transitionIn = PageTransitionElement::GetTransitionElement(pushedElement);
    auto transitionOut = PageTransitionElement::GetTransitionElement(topElement);
    if (!CheckPageTransitionElement(transitionIn, transitionOut)) {
        LOGE("check page transition failed, skip push transition.");
        return;
    }
    LOGD("set transition in hidden.");
    transitionIn->SetWrapHidden(true);
    transitionIn->SkipPostFlush();
    transitionOut->SkipPostFlush();
    auto context = context_.Upgrade();
    if (context) {
        if (!postponePageTransition_) {
            context->AddPostFlushListener(AceType::Claim(this));
        }
        pendingOperation_ = operation_;
    }
    RefreshFocus();
}

void StageElement::PerformPop()
{
    if (children_.size() <= 1) {
        LOGD("no enough element left in stage.");
        return;
    }
    auto context = context_.Upgrade();
    if (context) {
        if (!postponePageTransition_) {
            context->AddPostFlushListener(Claim(this));
        }
        pendingOperation_ = operation_;
    }
}

void StageElement::RestorePop()
{
    // add page before current page and run pop
    auto newChild = UpdateChildWithSlot(nullptr, newComponent_, -2, GetRenderSlot());
    operation_ = StackOperation::POP;
    PerformPop();
}

void StageElement::PerformReplace()
{
    if (children_.empty()) {
        LOGE("replace page failed. no page in stage now.");
        return;
    }
    auto newPage = DynamicCast<PageComponent>(newComponent_);
    auto oldElement = children_.back();
#ifndef WEARABLE_PRODUCT
    auto oldPage = DynamicCast<PageElement>(oldElement);
    if (newPage && oldPage) {
        PerformReplaceActiveScene(newPage->GetPageId(), oldPage->GetPageId());
    }
#endif
    UpdateChild(oldElement, nullptr);
    UpdateChild(nullptr, newComponent_);
    auto context = context_.Upgrade();
    if (context) {
        context->OnPageShow();
    }
    RefreshFocus();
}

void StageElement::PerformPopToPage()
{
    if (children_.empty()) {
        LOGE("pop page failed. no page in stage now.");
        return;
    }
    // check if top page matches
    auto topElement = *children_.rbegin();
    if (topElement) {
        auto topPage = AceType::DynamicCast<PageElement>(topElement);
        if (topPage && directedPageId_ == topPage->GetPageId()) {
            LOGW("already in target page. do not need to jump.");
            return;
        }
    }
    // skip top page and remove others. At least, need Top and End child to perform pop.
    for (auto iter = (++children_.rbegin()); children_.size() > POP_TO_LEAST_COUNT; iter = (++children_.rbegin())) {
        auto child = *iter;
        RefPtr<PageElement> page = AceType::DynamicCast<PageElement>(child);
        if (!page) {
            LOGW("try pop to page with wrapped display");
            RefPtr<DisplayElement> display = AceType::DynamicCast<DisplayElement>(child);
            if (display) {
                page = AceType::DynamicCast<PageElement>(display->GetFirstChild());
            }
        } else {
            if (directedPageId_ == page->GetPageId()) {
                break;
            }
#ifndef WEARABLE_PRODUCT
            PerformRemoveInactiveScene(page->GetPageId());
#endif
        }
        // remove child.
        UpdateChild(child, nullptr);
    }
    PerformPop();
}

void StageElement::PerformClear()
{
    if (children_.empty()) {
        LOGE("clear page failed. no page in stage now.");
        return;
    }
    for (auto iter = (++children_.rbegin()); iter != children_.rend(); iter = (++children_.rbegin())) {
        auto page = AceType::DynamicCast<PageElement>(*iter);
#ifndef WEARABLE_PRODUCT
        if (page) {
            PerformRemoveInactiveScene(page->GetPageId());
        }
#endif
        UpdateChild(*iter, nullptr);
    }
    RefreshFocus();
}

bool StageElement::IsTransitionStop() const
{
    if ((!controllerIn_) || (!controllerOut_)) {
        LOGD("controllerIn or controllerOut are null.");
        return true;
    }
    return ((controllerIn_->IsStopped()) && (controllerOut_->IsStopped()));
}

bool StageElement::InitTransition(const RefPtr<PageTransitionElement>& transition, TransitionDirection direction,
    TransitionEvent event, const RRect& cardRRect)
{
    if (!transition) {
        LOGE("init transition failed. transition is null. direction: %{public}d", direction);
        return false;
    }

    auto controller = transition->GetTransitionController();
    if (!controller) {
        LOGE("init transition failed. transition controller is null. direction: %{public}d", direction);
        return false;
    }
    if (!controller->IsStopped()) {
        controller->Stop();
    }

    auto deviceType = SystemProperties::GetDeviceType();
    // Reset status listener.
    controller->ClearAllListeners();
    auto context = GetContext().Upgrade();
    if (context && context->GetIsDeclarative()) {
        transition->LoadTransition();
        transition->ResetPageTransitionAnimation();
    }
    transition->SetTransition(deviceType, event, direction, cardRRect);
    transition->SetTransitionDirection(event, direction);
    controller->SetFillMode(FillMode::FORWARDS);
    transition->InitController(direction, event);
    return true;
}

bool StageElement::InitTransition(const RefPtr<PageTransitionElement>& transitionIn,
    const RefPtr<PageTransitionElement>& transitionOut, TransitionEvent event)
{
    auto parentElement = transitionIn->GetElementParent();
    RefPtr<PageElement> page = AceType::DynamicCast<PageElement>(parentElement.Upgrade());
    if (!page) {
        return false;
    }
    RRect cardRRect = GetCardRect(context_, page->GetCardComposeId());
    if (!InitTransition(transitionIn, TransitionDirection::TRANSITION_IN, event, cardRRect)) {
        LOGE("init transition in failed.");
        return false;
    }
    if (!InitTransition(transitionOut, TransitionDirection::TRANSITION_OUT, event, cardRRect)) {
        LOGE("init transition out failed.");
        return false;
    }
    transitionIn->AddPreFlush();
    transitionOut->AddPreFlush();
    controllerIn_ = transitionIn->GetTransitionController();
    controllerOut_ = transitionOut->GetTransitionController();
    return true;
}

bool StageElement::IsFocusable() const
{
    if (!FocusNode::IsFocusable()) {
        return false;
    }

    if (focusNodes_.empty()) {
        return false;
    }

    return focusNodes_.back()->IsFocusable();
}
#ifndef WEARABLE_PRODUCT
void StageElement::PerformPushMultimodalScene(int32_t pageId)
{
    auto context = GetContext().Upgrade();
    if (!context || !context->GetMultiModalManager()) {
        LOGE("fail to push multimodal scene due to manager get failed");
        return;
    }
    context->GetMultiModalManager()->PushActiveScene(pageId);
}

void StageElement::PerformPopMultimodalScene(int32_t poppedPageId, int32_t incomingPageId)
{
    auto context = GetContext().Upgrade();
    if (!context || !context->GetMultiModalManager()) {
        LOGE("fail to pop multimodal scene due to manager get failed");
        return;
    }
    context->GetMultiModalManager()->PopActiveScene(poppedPageId, incomingPageId);
}

void StageElement::PerformReplaceActiveScene(int32_t newPageId, int32_t replaceId)
{
    auto context = GetContext().Upgrade();
    if (!context || !context->GetMultiModalManager()) {
        LOGE("fail to replace multimodal scene due to manager get failed");
        return;
    }
    context->GetMultiModalManager()->ReplaceActiveScene(newPageId, replaceId);
}

void StageElement::PerformRemoveInactiveScene(int32_t pageId)
{
    auto context = GetContext().Upgrade();
    if (!context || !context->GetMultiModalManager()) {
        LOGE("fail to remove inactive multimodal scene due to manager get failed");
        return;
    }
    context->GetMultiModalManager()->RemoveInactiveScene(pageId);
}
#endif

void StageElement::ProcessStageInPageTransition()
{
    auto renderStage = GetRenderNode();
    if (renderStage && controllerIn_ && controllerOut_) {
        renderStage->SetDisableTouchEvent(true);
        auto weakRenderStage = WeakClaim(RawPtr(renderStage));
        auto lastStopController =
            controllerIn_->GetDuration() > controllerOut_->GetDuration() ? controllerIn_ : controllerOut_;
        lastStopController->AddStopListener([weakRenderStage] {
            auto stage = weakRenderStage.Upgrade();
            if (stage) {
                stage->SetDisableTouchEvent(false);
            }
        });
    }
}

void StageElement::PerformPushPageInStage(const WeakPtr<PageElement>& pageOutWeak)
{
    ProcessStageInPageTransition();
    controllerOut_->AddStopListener([pageOutWeak] {
        auto pageOut = pageOutWeak.Upgrade();
        if (pageOut) {
            pageOut->SetHidden(true);
        }
    });
}

void StageElement::PerformPopPageInStage(
    const RefPtr<PageElement>& pageOut, const RefPtr<PageTransitionElement>& transitionOut)
{
    ProcessStageInPageTransition();
    pageOut->SetHidden(false);
}

void StageElement::OnPostFlush()
{
    LOGD("StageElement: PostFlush, pending operation: %{public}d", pendingOperation_);
    if (children_.size() < POP_TO_LEAST_COUNT) {
        LOGE("Can not handle less than two pages.");
        return;
    }
    auto elementIter = children_.rbegin();
    auto topElement = *(elementIter++);
    auto nextTopElement = *(elementIter++);

    switch (pendingOperation_) {
        case StackOperation::PUSH_PAGE:
            PerformPushPageTransition(topElement, nextTopElement);
            break;
        case StackOperation::POP:
        case StackOperation::POP_TO_PAGE:
            PerformPopPageTransition(topElement, nextTopElement);
            break;
        default:
            break;
    }
    pendingOperation_ = StackOperation::NONE;
}

void StageElement::MakeTopPageTouchable()
{
    auto renderStage = GetRenderNode();
    if (!renderStage) {
        return;
    }
    const auto& children = renderStage->GetChildren();
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        // only top page can touch
        child->SetDisableTouchEvent(!(iter == children.rbegin()));
    }
}

void SectionStageElement::PushPage(const RefPtr<Component>& newComponent)
{
    AddAsOnlyPage(newComponent);
}

void SectionStageElement::Replace(const RefPtr<Component>& newComponent)
{
    AddAsOnlyPage(newComponent);
}

void SectionStageElement::AddAsOnlyPage(const RefPtr<Component>& newComponent)
{
    if (children_.empty()) {
        StageElement::PushPage(newComponent);
    } else {
        StageElement::Replace(newComponent);
    }
}

} // namespace OHOS::Ace
