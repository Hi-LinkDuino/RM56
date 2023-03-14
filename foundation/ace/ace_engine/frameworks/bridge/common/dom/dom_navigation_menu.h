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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_MENU_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_MENU_H

#include "core/components/menu/menu_component.h"
#include "core/components/navigation_bar/navigation_bar_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class DOMNavigationMenu final : public DOMNode {
    DECLARE_ACE_TYPE(DOMNavigationMenu, DOMNode);

public:
    DOMNavigationMenu(NodeId nodeId, const std::string& nodeName);
    ~DOMNavigationMenu() override = default;

    void InitializeStyle() override;

    void ResetInitializedStyle() override;

    RefPtr<Component> GetSpecializedComponent() override
    {
        return menuChild_;
    }

protected:
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void OnMounted(const RefPtr<DOMNode>& parentNode) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;

private:
    void UpdateNavigationBar();

    RefPtr<MenuComponent> menuChild_;
    RefPtr<NavigationBarComponent> navigationBar_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_NAVIGATION_MENU_H
