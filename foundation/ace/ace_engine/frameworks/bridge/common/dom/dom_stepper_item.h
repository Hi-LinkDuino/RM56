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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_ITEM_H

#include "core/components/stepper/stepper_item_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMStepperItem final : public DOMNode {
    DECLARE_ACE_TYPE(DOMStepperItem, DOMNode);

public:
    DOMStepperItem(NodeId nodeId, const std::string& nodeName, int32_t index);
    ~DOMStepperItem() override = default;

    void SetLabel(const StepperLabels& label)
    {
        label_ = label;
    }

    const StepperLabels& GetLabel() const
    {
        return label_;
    }

    const TextStyle& GetTextStyle() const
    {
        return textStyle_;
    }

    RefPtr<Component> GetSpecializedComponent() override
    {
        return stepperItemComponent_;
    }

    int32_t GetItemIndex() const
    {
        return itemIndex_;
    }

    void InitializeStyle() override;

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;
    RefPtr<Component> CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components) override;
    void ResetInitializedStyle() override;

private:
    void AddStepperItem(const RefPtr<DOMNode>& node, int32_t slot);
    void RemoveStepperItem(const RefPtr<DOMNode>& node);

    RefPtr<StepperItemComponent> stepperItemComponent_;
    TextStyle textStyle_;
    StepperLabels label_;
    EventMarker appearEventId_;
    EventMarker disappearEventId_;
    int32_t itemIndex_ = -1;
    FlexDirection flexDirection_ { FlexDirection::ROW };
    FlexAlign flexMainAlign_ { FlexAlign::FLEX_START };
    FlexAlign flexCrossAlign_ { FlexAlign::FLEX_START };
    RefPtr<FlexComponent> flexComponent_;
    RefPtr<DisplayComponent> displayComponent_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_ITEM_H
