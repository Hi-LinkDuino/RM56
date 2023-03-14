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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_H

#include "core/components/stepper/stepper_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMStepper final : public DOMNode {
    DECLARE_ACE_TYPE(DOMStepper, DOMNode);

public:
    DOMStepper(NodeId nodeId, const std::string& nodeName);
    ~DOMStepper() override = default;

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return stepperComponent_;
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:
    RefPtr<StepperComponent> stepperComponent_;
    EventMarker finishEventId_;
    EventMarker skipEventId_;
    EventMarker changeEventId_;
    EventMarker nextEventId_;
    EventMarker backEventId_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_STEPPER_H
