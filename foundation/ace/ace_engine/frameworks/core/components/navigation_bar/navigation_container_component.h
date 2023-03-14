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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_CONTAINER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_CONTAINER_COMPONENT_H

#include "core/components/tab_bar/tab_controller.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

struct ToolBarItem : public virtual AceType {
    DECLARE_ACE_TYPE(ToolBarItem, AceType);

public:
    ToolBarItem() = default;
    ToolBarItem(const std::string& value, const std::string& icon, const EventMarker& action)
        : value(value), icon(icon), action(action)
    {}
    ~ToolBarItem() = default;

    std::string value;
    std::string icon;
    EventMarker action;
    EventMarker actionWithParam;
};

enum class NavigationTitleMode {
    FREE = 0,
    FULL,
    MINI,
};

struct ACE_EXPORT NavigationDeclaration : public virtual AceType {
    DECLARE_ACE_TYPE(NavigationDeclaration, AceType);

public:
    bool HasToolBar()
    {
        return (!hideToolbar && !toolbarItems.empty()) || (!hideToolbar && toolBarBuilder);
    }

    RefPtr<Component> customTitle;
    std::string title;
    std::string subTitle;
    bool hideBarBackButton = false;
    RefPtr<Component> customMenus;
    std::list<RefPtr<ToolBarItem>> menuItems;
    NavigationTitleMode titleMode = NavigationTitleMode::MINI;
    bool hideBar = false;
    EventMarker titleModeChangedEvent;

    std::list<RefPtr<ToolBarItem>> toolbarItems;
    RefPtr<Component> toolBarBuilder;
    bool hideToolbar = false;
    AnimationOption animationOption;
};

class ACE_EXPORT NavigationContainerComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(NavigationContainerComponent, ComponentGroup);

public:
    NavigationContainerComponent() : declaration_(AceType::MakeRefPtr<NavigationDeclaration>()) {}
    NavigationContainerComponent(const RefPtr<Component>& navigationbar, const RefPtr<Component>& content)
        : ComponentGroup()
    {
        ComponentGroup::AppendChild(navigationbar);
        ComponentGroup::AppendChild(content);
    }
    ~NavigationContainerComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    static uint32_t GetGlobalTabControllerId();
    static RefPtr<ComposedComponent> BuildToolBar(
        const RefPtr<NavigationDeclaration>& declaration, const RefPtr<TabController>& controller);

    RefPtr<TabController> GetTabController()
    {
        return tabController_;
    }
    RefPtr<NavigationDeclaration> GetDeclaration()
    {
        return declaration_;
    }
    void Build(const WeakPtr<PipelineContext>& context, int32_t menuCount);

    void SetMenuCount(int32_t menuCount)
    {
        menuCount_ = menuCount;
    }

    int32_t GetMenuCount() const
    {
        return menuCount_;
    }

private:
    bool NeedSection() const;

    RefPtr<NavigationDeclaration> declaration_;
    RefPtr<TabController> tabController_;
    int32_t menuCount_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_CONTAINER_COMPONENT_H
