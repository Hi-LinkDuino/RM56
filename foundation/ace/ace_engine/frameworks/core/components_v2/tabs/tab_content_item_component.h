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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_COMPONENT_H

#include "core/components_v2/tabs/tabs_component.h"
#include "core/pipeline/base/element.h"
#include "frameworks/core/components/flex/flex_component.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT TabContentItemComponent : public ColumnComponent {
    DECLARE_ACE_TYPE(TabContentItemComponent, ColumnComponent);

public:
    explicit TabContentItemComponent(std::list<RefPtr<Component>>& children);
    ~TabContentItemComponent() override = default;

    RefPtr<Element> CreateElement() override;

    WeakPtr<V2::TabsComponent> GetTabsComponent() const
    {
        return tabsComponent_;
    }

    void SetTabsComponent(const WeakPtr<TabsComponent>& tabsComponent)
    {
        tabsComponent_ = tabsComponent;
    }

    void SetBarIcon(const std::string& barIcon)
    {
        barIcon_ = barIcon;
    }

    const std::string& GetBarIcon() const
    {
        return barIcon_;
    }

    void SetBarText(const std::string& barText)
    {
        barText_ = barText;
    }

    const std::string& GetBarText() const
    {
        return barText_;
    }

private:
    WeakPtr<TabsComponent> tabsComponent_;
    std::string barIcon_;
    std::string barText_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_TABS_TAB_CONTENT_ITEM_COMPONENT_H
