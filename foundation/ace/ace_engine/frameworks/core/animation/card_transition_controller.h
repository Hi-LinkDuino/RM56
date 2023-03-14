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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CARD_TRANSITION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CARD_TRANSITION_CONTROLLER_H

#include "base/memory/ace_type.h"
#include "core/animation/animation_pub.h"
#include "core/components/page/page_element.h"
#include "core/components/transform/render_transform.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class Animator;

class CardTransitionController : public AceType {
DECLARE_ACE_TYPE(CardTransitionController, AceType);

public:
    explicit CardTransitionController(const WeakPtr<PipelineContext>& context);
    ~CardTransitionController() override = default;
    void RegisterTransitionListener();
    RRect GetCardRect(const ComposeId& composeId) const;

    const RefPtr<Animator>& GetAnimator() const
    {
        return controller_;
    }

private:
    void PrepareTransition();
    void CreateCardAnimation(const RefPtr<Element>& cardComposeElement, const Offset& globalOffset, const RRect& rrect);
    void CreateCardOpacityAnimation(RefPtr<RenderNode>& displayRender);
    void CreateCardTranslateAnimation(const WeakPtr<RenderTransform>& weakTransform, const Offset& globalOffset,
        const Offset& marginOffset);
    void CreateCardScaleAnimation(const WeakPtr<RenderTransform>& weakTransform, const RRect& rrect);
    void CreateCardListAnimation(const RefPtr<RenderNode>& renderNode);
    void CreateExternalAnimation(double height, double cardOffsetY);
    RefPtr<Element> GetCardTransformElement(const RefPtr<Element>& element);
    void AddPrepareListener(const WeakPtr<RenderTransform>& weakTransform);
    void AddStopListener(const WeakPtr<RenderTransform>& weakTransform);

    WeakPtr<PipelineContext> context_;
    WeakPtr<PageElement> pageDest_;
    WeakPtr<PageElement> pageSrc_;
    RefPtr<Animator> controller_;
    TransitionEvent event_ { TransitionEvent::PUSH_START };
    uint64_t currentTimestamp_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_CARD_TRANSITION_CONTROLLER_H
