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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_ELEMENT_H

#include "core/components/box/box_element.h"
#include "core/components/common/properties/page_transition_option.h"
#include "core/components/page_transition/page_transition_info.h"
#include "core/components/stack/stack_component.h"
#include "core/components/stack/stack_element.h"
#include "core/components/transition/transition_element.h"
#include "core/components/tween/tween_element.h"

namespace OHOS::Ace {

class ACE_EXPORT PageTransitionElement : public StackElement {
    DECLARE_ACE_TYPE(PageTransitionElement, StackElement);

public:
    void Update() override;
    void PerformBuild() override;

    void SetTransitionDirection(TransitionEvent event, TransitionDirection direction);
    void SetTransitionController();
    const RefPtr<Animator>& GetTransitionController() const;
    void LoadTransition();
    void ResetPageTransitionAnimation();
    void SetTransition(DeviceType deviceType, TransitionEvent event, TransitionDirection direction,
        const RRect& rrect);
    void SetWrapHidden(bool hidden);
    void AddPreFlush();
    void SkipPostFlush();
    void InitController(TransitionDirection direction, TransitionEvent event);
    RefPtr<Element> GetContentElement() const;
    const RefPtr<TransitionElement>& GetContentTransitionElement() const;
    const RefPtr<TransitionElement>& GetBackgroundTransitionElement() const;
    void SetTouchable(bool enable);
    void InitTransitionClip();
    TweenOption ProcessPageTransition(const RefPtr<PageTransition>& pageTransition, TransitionEvent event);
    RefPtr<PageTransition> GetCurrentPageTransition(TransitionEvent event, TransitionDirection direction) const;
    void SetDeclarativeDirection(TransitionDirection direction)
    {
        direction_ = direction;
    }
    bool GetIsCustomOption() const
    {
        return isCustomOption_;
    }

    // element only support page now
    static RefPtr<PageTransitionElement> GetTransitionElement(const RefPtr<Element>& element);
    static PageTransitionType GetPageTransitionType(TransitionEvent event, TransitionDirection direction);

private:
    void UpdateTransitionOption();
    void BuildSeparatedChild(const RefPtr<StackComponent>& component);
    void BuildCombinedChild(const RefPtr<StackComponent>& component);

    RefPtr<TransitionElement> contentTransition_;         // element to animate page content
    RefPtr<TransitionElement> backgroundTransition_;      // element to animate background
    RefPtr<TransitionElement> frontDecorationTransition_; // element to animate front decoration opacity
    RefPtr<Animator> controller_;                         // animator for content and background
    TweenOption contentInOption_;
    TweenOption contentOutOption_;
    TweenOption sharedInOption_;
    TweenOption sharedOutOption_;
    bool isRightToLeft_ = false;
    bool isCustomOptionOut_ = false;
    bool isCustomOptionIn_ = false;
    std::unordered_map<PageTransitionType, RefPtr<PageTransition>> pageTransitions_;
    TransitionDirection direction_ = TransitionDirection::TRANSITION_IN;
    bool isCustomOption_ = false;
    RefPtr<Animation<float>> floatAnimation_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_ELEMENT_H
