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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_ELEMENT_H

#include "core/animation/animator.h"
#include "core/animation/page_transition_listener.h"
#include "core/components/page/page_element.h"
#include "core/components/page_transition/page_transition_element.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

enum class StackOperation { NONE, PUSH_PAGE, POP, POP_TO_PAGE, REPLACE, CLEAR, RESTORE };

class StageElement : public StackElement, public PageTransitionListenable, public FlushEvent {
    DECLARE_ACE_TYPE(StageElement, StackElement, FlushEvent);

public:
    void PerformBuild() override;

    virtual void PushPage(const RefPtr<Component>& newComponent);
    void Pop();
    void PopToPage(int32_t pageId);
    void RestorePopPage(const RefPtr<Component>& newComponent);
    virtual void Replace(const RefPtr<Component>& newComponent);
    bool ClearOffStage();
    bool CanPopPage();
    bool CanPushPage();
    bool CanReplacePage();
    bool CanRouterPage();
    void PostponePageTransition();
    void LaunchPageTransition();
    void RefreshFocus();
    bool IsFocusable() const override;
    bool InitTransition(const RefPtr<PageTransitionElement>& transitionIn,
        const RefPtr<PageTransitionElement>& transitionOut, TransitionEvent event);

    StackOperation GetStackOperation() const
    {
        return operation_;
    }

    bool IsTransitionStop() const;

    bool isPageElement() override
    {
        return true;
    }

protected:
    void MarkDirty() override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;

private:
    void PerformPushPage();
    void PerformPop();
    void PerformReplace();
    void PerformPopToPage();
    void PerformClear();
#ifndef WEARABLE_PRODUCT
    void PerformPushMultimodalScene(int32_t pageId);
    void PerformPopMultimodalScene(int32_t poppedPageId, int32_t incomingPageId);
    void PerformReplaceActiveScene(int32_t newPageId, int32_t replaceId);
    void PerformRemoveInactiveScene(int32_t pageId);
#endif
    bool PerformPushPageTransition(const RefPtr<Element>& elementIn, const RefPtr<Element>& elementOut);
    bool PerformPopPageTransition(const RefPtr<Element>& elementIn, const RefPtr<Element>& elementOut);
    void AddListenerForPopPage(const WeakPtr<PageElement>& pageInWeak, const WeakPtr<PageElement>& pageOutWeak);
    static bool CheckPageTransitionElement(
        const RefPtr<PageTransitionElement>& transitionIn, const RefPtr<PageTransitionElement>& transitionOut);
    bool InitTransition(const RefPtr<PageTransitionElement>& transition, TransitionDirection direction,
        TransitionEvent event, const RRect& cardRect);
    void PerformPushPageInStage(const WeakPtr<PageElement>& pageOutWeak);
    void PerformPopPageInStage(const RefPtr<PageElement>& pageOut, const RefPtr<PageTransitionElement>& transitionOut);
    void ProcessStageInPageTransition();
    // Page transition parameters depend on the existence of shared element transitions,
    // Only after PerformBuild can we determine if there is a shared element transition.
    void OnPostFlush() override;
    void MakeTopPageTouchable();
    void RestorePop();

    StackOperation operation_ { StackOperation::NONE };
    StackOperation pendingOperation_ { StackOperation::NONE };
    bool postponePageTransition_ { false };

    RefPtr<Component> newComponent_;
    RefPtr<Animator> controllerIn_;  // Controller for transition in.
    RefPtr<Animator> controllerOut_; // Controller for transition out.
    int32_t directedPageId_ = 0;
    bool isWaitingForBuild_ = false;
};

class SectionStageElement : public StageElement {
    DECLARE_ACE_TYPE(SectionStageElement, StageElement);

public:
    void PushPage(const RefPtr<Component>& newComponent) override;
    void Replace(const RefPtr<Component>& newComponent) override;

private:
    void AddAsOnlyPage(const RefPtr<Component>& newComponent);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_STAGE_ELEMENT_H
