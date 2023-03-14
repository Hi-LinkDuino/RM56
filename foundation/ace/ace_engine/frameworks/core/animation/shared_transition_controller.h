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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_CONTROLLER_H

#include <list>

#include "base/memory/ace_type.h"
#include "core/animation/animation_pub.h"
#include "core/components/page/page_element.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class Animator;
class SharedTransitionElement;
class SharedTransitionEffect;

class SharedTransitionController : public AceType {
    DECLARE_ACE_TYPE(SharedTransitionController, AceType);

public:
    explicit SharedTransitionController(const WeakPtr<PipelineContext>& context);
    ~SharedTransitionController() override = default;
    void RegisterTransitionListener();
    bool HasSharedTransition(TransitionEvent event);
    const std::list<RefPtr<Animator>>& GetAnimators() const
    {
        return controllers_;
    }

private:
    void StartSharedTransition();
    void KickoffSharedTransition(TransitionEvent event, RefPtr<OverlayElement>& overlay);
    bool PrepareTransition(RefPtr<OverlayElement> overlay, bool preCheck = false);
    bool CheckAndCreateTransition(
        std::vector<RefPtr<SharedTransitionEffect>>& effects, RefPtr<OverlayElement>& overlay);
    bool PrepareEachTransition(
        const ShareId& shareId, RefPtr<SharedTransitionEffect>& effect, RefPtr<OverlayElement>& overlay);
    WeakPtr<PipelineContext> context_;
    WeakPtr<PageElement> pageDest_;
    WeakPtr<PageElement> pageSrc_;
    std::list<RefPtr<Animator>> controllers_;
    int32_t stopControllerCount_ = 0;
    TransitionEvent event_ { TransitionEvent::PUSH_START };
    bool hasSharedTransition_ = { false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SHARED_TRANSITION_CONTROLLER_H
