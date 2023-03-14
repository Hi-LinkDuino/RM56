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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_BAR_H

#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_bar_indicator_component.h"
#include "core/components/tab_bar/tab_bar_item_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_tabs.h"

namespace OHOS::Ace::Framework {

class DOMTabBar final : public DOMNode {
    DECLARE_ACE_TYPE(DOMTabBar, DOMNode);

public:
    DOMTabBar(NodeId nodeId, const std::string& nodeName);
    ~DOMTabBar() override = default;

    void InitializeStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return tabBarChild_;
    }

    void UpdateIndex(uint32_t currentIndex);

    void BindToTabs(const RefPtr<DOMNode>& tabsNode)
    {
        OnMounted(tabsNode);
    }

    void SetVertical(bool vertical_)
    {
        tabBarChild_->SetVertical(vertical_);
    }

protected:
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    void PrepareSpecializedComponent() override;
    void ResetInitializedStyle() override;

private:
    void PrepareChangeListener();
    void OnChildActive(const RefPtr<DOMNode>& node, bool active);
    Edge ParseEdge(const std::string& value) const;

    RefPtr<TabBarComponent> tabBarChild_;
    RefPtr<TabBarIndicatorComponent> tabBarIndicator_;
    uint32_t lastIndex_ = 0;
    uint32_t controllerId_ = 0;
    bool vertical_ = false;
    TabBarMode tabBarMode_ = TabBarMode::SCROLLABEL;
    Edge padding_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_TAB_BAR_H
