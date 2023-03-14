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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_BUBBLE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_BUBBLE_COMPONENT_H

#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/popup/popup_component.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class BubbleComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(BubbleComponent, SoleChildComponent);

public:
    using StateChangeEvent = std::function<void(bool)>;

    explicit BubbleComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~BubbleComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetPopupParam(const RefPtr<PopupParam>& popupParam)
    {
        popupParam_ = popupParam;
    }

    const RefPtr<PopupParam>& GetPopupParam() const
    {
        return popupParam_;
    }

    void SetId(const ComposeId& id)
    {
        id_ = id;
    }

    const ComposeId& GetId() const
    {
        return id_;
    }

    void SetWeakStack(const WeakPtr<StackElement>& weakStack)
    {
        weakStack_ = weakStack;
    }

    const WeakPtr<StackElement>& GetWeakStack() const
    {
        return weakStack_;
    }

    const StateChangeEvent& GetStateChangeEvent() const
    {
        return stateChangeEvent_;
    }

    void SetStateChangeEvent(StateChangeEvent&& stateChangeEvent)
    {
        stateChangeEvent_ = std::move(stateChangeEvent);
    }

private:
    ComposeId id_;
    RefPtr<PopupParam> popupParam_;
    WeakPtr<StackElement> weakStack_;
    StateChangeEvent stateChangeEvent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_BUBBLE_COMPONENT_H
