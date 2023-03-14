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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_V2_H

#include "core/components/menu/menu_component.h"
#include "core/components/navigation_bar/navigation_bar_component_base.h"
#include "core/components/navigation_bar/navigation_container_component.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class NavigationBarBuilder : public NavigationBarComponentBase {
public:
    NavigationBarBuilder(const RefPtr<NavigationDeclaration>& declaration, ComposeId id, TextDirection direction)
        : declaration_(declaration), theme_(AceType::MakeRefPtr<ThemeManager>()->GetTheme<NavigationBarTheme>()),
          id_(id), direction_(direction), menuZoneSize_(theme_->GetMenuZoneSize()),
          menuIconSize_(theme_->GetMenuIconSize())
    {}
    ~NavigationBarBuilder() override = default;

    RefPtr<Component> Build(const WeakPtr<PipelineContext>& context, int32_t menuCount)
    {
        context_ = context;
        menuCount_ = menuCount;
        RefPtr<Component> navigationBar;
        if (declaration_->titleMode == NavigationTitleMode::MINI) {
            navigationBar = BuildMiniLayer();
        } else if (declaration_->titleMode == NavigationTitleMode::FULL) {
            navigationBar = BuildFullLayer();
        } else if (declaration_->titleMode == NavigationTitleMode::FREE) {
            navigationBar = BuildFreeModeBar();
        } else {
            LOGE("unknown navigation title mode");
        }
        return navigationBar;
    }

private:
    RefPtr<Component> BuildMiniLayer();
    RefPtr<Component> BuildFullLayer();
    RefPtr<Component> BuildFreeModeBar();
    RefPtr<Component> BuildTitle();
    bool AddMenu(const RefPtr<ComponentGroup>& container);
    RefPtr<Component> BuildAnimationContainer(
        const RefPtr<Component>& content, const Dimension& height, const Edge& edge);
    void BindMoreButtonClickEvent();
    void AddOption(const RefPtr<ToolBarItem>& menuItem);

    RefPtr<NavigationDeclaration> declaration_;
    RefPtr<NavigationBarTheme> theme_;
    ComposeId id_;
    TextDirection direction_ = TextDirection::LTR;
    Dimension menuZoneSize_;
    Dimension menuIconSize_;

    RefPtr<ComposedComponent> titleComposed_;
    RefPtr<ComposedComponent> subTitleComposed_;
    RefPtr<ButtonComponent> moreButton_;
    RefPtr<MenuComponent> menu_;
    EventMarker backClickMarker_;
    int32_t menuCount_ = 0;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATION_BAR_NAVIGATION_BAR_COMPONENT_V2_H
