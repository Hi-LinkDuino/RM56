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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_CONTENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_CONTENT_H

#include "core/components/tab_bar/tab_content_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_tabs.h"

namespace OHOS::Ace::Framework {

class DOMTabContent final : public DOMNode {
    DECLARE_ACE_TYPE(DOMTabContent, DOMNode);

public:
    DOMTabContent(NodeId nodeId, const std::string& nodeName);
    ~DOMTabContent() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return tabContentChild_;
    }

    void SetVertical(bool vertical_)
    {
        tabContentChild_->SetVertical(vertical_);
    }

protected:
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    void PrepareSpecializedComponent() override;

private:
    RefPtr<TabContentComponent> tabContentChild_;
    EventMarker changeEventId_;
    uint32_t index_ = 0;
    uint32_t controllerId_ = 0;
    bool scrollable_ = true;
    bool vertical_ = false;

    void PrepareChangeListener(const RefPtr<DOMTabs>& parentNode);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_CONTENT_H
