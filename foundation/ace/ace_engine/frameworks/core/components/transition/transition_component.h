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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSITION_TRANSITION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSITION_TRANSITION_COMPONENT_H

#include "core/components/common/properties/tween_option.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class TransitionComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(TransitionComponent, ComposedComponent);

public:
    TransitionComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {}
    TransitionComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name) {}

    ~TransitionComponent() override = default;

    RefPtr<Element> CreateElement() override;

    void MarkOptionChanged(bool change)
    {
        optionChanged_ = change;
    }

    bool IsOptionChanged() const
    {
        return optionChanged_;
    }

    void SetTransitionOption(const TweenOption& transition)
    {
        MarkOptionChanged(true);
        transitionOption_ = std::move(transition);
    }

    const TweenOption& GetTransitionOption() const
    {
        return transitionOption_;
    }

    void SetTransitionOption(const TweenOption& in, const TweenOption& out);
    const TweenOption& GetTransitionInOption() const;

    const TweenOption& GetTransitionOutOption() const;

    static ComposeId AllocTransitionComponentId();

    bool IsFirstFrameShow() const;
    void SetIsFirstFrameShow(bool isFirstFrameShow);

private:
    TweenOption inOption_;  // In option
    TweenOption outOption_; // Out option
    TweenOption transitionOption_; // transition option
    bool optionChanged_ = false;
    bool isFirstFrameShow_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRANSITION_TRANSITION_COMPONENT_H
