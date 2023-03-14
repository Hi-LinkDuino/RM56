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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWITCH_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWITCH_H

#include "core/components/checkable/checkable_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMSwitch final : public DOMNode {
    DECLARE_ACE_TYPE(DOMSwitch, DOMNode);

public:
    DOMSwitch(NodeId nodeId, const std::string& nodeName);
    ~DOMSwitch() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return switchChild_;
    }

    void InitializeStyle() override;

    void ResetInitializedStyle() override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void PrepareSpecializedComponent() override;

private:
    std::string textOn_;
    std::string textOff_;
    TextStyle textStyle_;
    EventMarker changeEvent_;
    RefPtr<SwitchComponent> switchChild_;

    void PrepareCheckedListener();
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SWITCH_H
