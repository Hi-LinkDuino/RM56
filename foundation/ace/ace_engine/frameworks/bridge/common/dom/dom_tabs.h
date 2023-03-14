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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TABS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TABS_H

#include "core/components/flex/flex_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMTabs final : public DOMNode {
    DECLARE_ACE_TYPE(DOMTabs, DOMNode);

public:
    DOMTabs(NodeId nodeId, const std::string& nodeName);
    ~DOMTabs() override = default;

    uint32_t GetTabIndex() const
    {
        return tabIndex_;
    }

    uint32_t GetTabControllerId() const
    {
        return tabControllerId_;
    }

    const RefPtr<TabController>& GetTabController() const
    {
        return tabController_;
    }

    uint32_t GetTabPageId() const
    {
        return tabPageId_;
    }

    const std::string& GetTabEventId() const
    {
        return tabEventId_;
    }

    const std::string& GetTabEventType() const
    {
        return tabEventType_;
    }

    bool IsVertical() const
    {
        return vertical_;
    }

    RefPtr<Component> GetSpecializedComponent() override
    {
        return flexChild_;
    }

    void OnPageLoadFinish() override
    {
        if (tabController_) {
            tabController_->SetPageReady(true);
        }
    }

protected:
    void OnMounted(const RefPtr<DOMNode>& parentNode) override {};
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

private:
    static uint32_t GetGlobalTabControllerId();
    RefPtr<FlexComponent> CreateChild() const;

    RefPtr<FlexComponent> flexChild_;
    RefPtr<TabController> tabController_;

    // Record the tabs component info for its children (tab_bars and tab_contents)
    uint32_t tabIndex_ = 0;
    uint32_t tabControllerId_ = 0;
    uint32_t tabPageId_ = 0;
    std::string tabEventId_;
    std::string tabEventType_;

    // Record the tab is vertical or horizontal
    bool vertical_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TABS_H
