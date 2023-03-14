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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_ELEMENT_H

#include "core/animation/shared_transition_effect.h"
#include "core/components/box/box_component.h"
#include "core/components/box/box_element.h"
#include "core/components/box/render_box.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/shared_transition/shared_transition_component.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class BoxComponent;
class PageElement;
class RenderBox;

class SharedTransitionElement : public ComposedElement, public FlushEvent {
    DECLARE_ACE_TYPE(SharedTransitionElement, ComposedElement);

public:
    using SizeModifiedCallback = std::function<void()>;
    explicit SharedTransitionElement(const ComposeId& id) : ComposedElement(id) {}
    ~SharedTransitionElement() override;

    void Update() override;
    // Prepare playing Shared Transition, Shared Transition Component prepare to show up in overlay.
    // Passenger wears a suit and aboard shuttle, where passenger means Shared Transition Component and shuttle means
    // overlay.
    bool AboardShuttle(Offset& ticket);
    RefPtr<Component> GetPassengerComponent() const
    {
        return passengerComponent_;
    }

    RefPtr<Element> GetPassengerElement() const
    {
        return passengerElement_;
    }
    // After finish Shared Transition, Shared Transition cleared from overlay and come back to source.
    // Passenger arrives at destination and teleports back home
    void GetOffShuttle();
    // When Shared Transition plays, only shared transition component in overlay can be seen, others will be invisible
    // on their own page.
    // When Shared Transition finished, shared transition component is visible on its own page, and overlay is cleared.
    void SetVisible(bool visible);
    const ShareId& GetShareId() const;
    const RefPtr<SharedTransitionEffect>& GetEffect() const;
    Size GetSuitSize() const;
    Offset GetGlobalOffset() const;
    float GetOpacity() const;
    int32_t GetZIndex() const;
    RefPtr<Element> GetContentElement() const;
    bool IsEnablePopEnter() const;
    bool IsEnablePushEnter() const;
    bool IsEnablePopExit() const;
    bool IsEnablePushExit() const;

    const TweenOption& GetOption() const
    {
        return option_;
    }

    void PerformBuild() override;

    void SetSizeModified(SizeModifiedCallback&& sizeModifiedCallback);

protected:
    RefPtr<Component> BuildChild() override;
    void OnPostFlush() override;

private:
    void Register();
    RefPtr<RenderBox> GetRenderPassengerWithPajamas() const;
    RefPtr<RenderBox> GetRenderBox();
    RefPtr<PageElement> SearchParentPage() const;

    ShareId shareId_;
    ShareId oldShareId_;
    TweenOption option_;
    RefPtr<BoxComponent> passengerComponent_; // Set When Build Passenger Aboard, Reset When Destruct.
    RefPtr<BoxElement> passengerElement_;     // Set When Passenger Aboard, Reset When GetOff Shuttle
    RefPtr<RenderBox> passengerRender_;       // Set When Passenger Aboard, Reset When GetOff Shuttle
    RefPtr<SharedTransitionEffect> effect_;
    WeakPtr<PageElement> pageElement_;
    Dimension passengerHeight_;
    Dimension passengerWidth_;
    bool enablePopEnter_ = true;
    bool enablePopExit_ = true;
    bool enablePushEnter_ = true;
    bool enablePushExit_ = true;
    SizeModifiedCallback sizeModifiedCallback_;
    float opacity_ = 1.0f;
    int32_t zIndex_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHARED_TRANSITION_SHARED_TRANSITION_ELEMENT_H
