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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TABS_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TABS_COMPONENT_H

#include "core/components/tab_bar/tab_controller.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"
#include "frameworks/core/components/flex/flex_component.h"
#include "frameworks/core/components/flex/flex_item_component.h"
#include "frameworks/core/components/tab_bar/tab_bar_component.h"
#include "frameworks/core/components/tab_bar/tab_content_component.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT TabsComponent : public FlexComponent {
    DECLARE_ACE_TYPE(TabsComponent, FlexComponent);

public:
    explicit TabsComponent(std::list<RefPtr<Component>>& children, BarPosition barPosition = BarPosition::START,
        const RefPtr<TabController>& controller = nullptr);
    ~TabsComponent() override = default;

    RefPtr<Element> CreateElement() override;

    void SetTabsController(const RefPtr<TabController>& controller)
    {
        controller_ = controller;
    }

    RefPtr<TabController> GetTabsController() const
    {
        return controller_;
    }

    RefPtr<TabContentComponent> GetTabContentChild() const
    {
        return tabContent_;
    }

    RefPtr<TabBarComponent> GetTabBarChild() const
    {
        return tabBar_;
    }

    void AppendChild(const RefPtr<Component>& child) override;

    void RemoveChild(const RefPtr<Component>& child) override;

private:
    std::list<RefPtr<Component>> tabContentChildren_;
    std::list<RefPtr<Component>> tabBarChildren_;
    RefPtr<TabController> controller_;
    RefPtr<TabBarIndicatorComponent> tabBarIndicator_;
    RefPtr<TabContentComponent> tabContent_;
    RefPtr<TabBarComponent> tabBar_;
    RefPtr<FlexItemComponent> flexItem_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TABS_COMPONENT_H
