/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_BAR_H

#include "core/components/navigation_bar/navigation_bar_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework  {

class DomNavigationBar final : public DOMNode {
    DECLARE_ACE_TYPE(DomNavigationBar, DOMNode);

public:
    DomNavigationBar(NodeId nodeId, const std::string& nodeName);
    ~DomNavigationBar() override = default;

    void InitializeStyle() override;

    void ResetInitializedStyle() override;

    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

    void PrepareSpecializedComponent() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return navigationBarComponent_;
    }

protected:
#ifndef WEARABLE_PRODUCT
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
#endif
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

private:
    RefPtr<NavigationBarComponent> navigationBarComponent_;
    RefPtr<NavigationBarData> navigationBarData_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_BAR_H
