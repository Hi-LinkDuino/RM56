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

#include "core/components/navigation_bar/navigation_container_component.h"

#include "core/components/box/box_component.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/navigation_bar/navigation_bar_component.h"
#include "core/components/navigation_bar/navigation_bar_component_v2.h"
#include "core/components/navigation_bar/navigation_container_element.h"
#include "core/components/navigation_bar/render_navigation_container.h"
#include "core/components/padding/padding_component.h"
#include "core/components/stage/stage_component.h"
namespace OHOS::Ace {

namespace {

static uint32_t g_navigationTabControllerId = 0;
constexpr int32_t BOTTOM_TAB_ICON_SIZE = 24;
constexpr int32_t BOTTOM_TAB_ICON_AND_TEXT_PADDING = 2;
constexpr double SECTION_INDEX_PART_WEIGHT = 1.0;
constexpr double SECTION_CONTENT_PART_WEIGHT = 2.0;

} // namespace

RefPtr<RenderNode> NavigationContainerComponent::CreateRenderNode()
{
#ifndef WEARABLE_PRODUCT
    return RenderNavigationContainer::Create();
#else
    return RenderFlex::Create();
#endif
}

RefPtr<Element> NavigationContainerComponent::CreateElement()
{
#ifndef WEARABLE_PRODUCT
    return AceType::MakeRefPtr<NavigationContainerElement>();
#else
    return AceType::MakeRefPtr<FlexElement>();
#endif
}

uint32_t NavigationContainerComponent::GetGlobalTabControllerId()
{
    return ++g_navigationTabControllerId;
}

RefPtr<ComposedComponent> NavigationContainerComponent::BuildToolBar(
    const RefPtr<NavigationDeclaration>& declaration, const RefPtr<TabController>& controller)
{
#ifndef WEARABLE_PRODUCT
    std::list<RefPtr<Component>> tabBarItems;
    for (const auto& item : declaration->toolbarItems) {
        if (!item->icon.empty()) {
            auto itemContainer = AceType::MakeRefPtr<ColumnComponent>(
                FlexAlign::CENTER, FlexAlign::CENTER, std::list<RefPtr<OHOS::Ace::Component>>());
            auto iconBox = AceType::MakeRefPtr<BoxComponent>();
            iconBox->SetChild(AceType::MakeRefPtr<ImageComponent>(item->icon));
            iconBox->SetHeight(BOTTOM_TAB_ICON_SIZE, DimensionUnit::VP);
            iconBox->SetWidth(BOTTOM_TAB_ICON_SIZE, DimensionUnit::VP);
            itemContainer->AppendChild(iconBox);
            auto padding = AceType::MakeRefPtr<PaddingComponent>();
            padding->SetPaddingTop(Dimension(BOTTOM_TAB_ICON_AND_TEXT_PADDING, DimensionUnit::VP));
            itemContainer->AppendChild(padding);
            itemContainer->AppendChild(AceType::MakeRefPtr<TextComponent>(item->value));
            tabBarItems.push_back(AceType::MakeRefPtr<TabBarItemComponent>(itemContainer));
        } else {
            tabBarItems.push_back(
                AceType::MakeRefPtr<TabBarItemComponent>(AceType::MakeRefPtr<TextComponent>(item->value)));
        }
    }
    auto tabBar = AceType::MakeRefPtr<TabBarComponent>(tabBarItems, controller);
    auto theme = AceType::MakeRefPtr<ThemeManager>()->GetTheme<TabTheme>();
    tabBar->InitBottomTabStyle(theme);

    auto component = declaration->toolBarBuilder;
    auto display = AceType::MakeRefPtr<DisplayComponent>();
    if (component) {
        display->SetChild(component);
    } else {
        display->SetChild(tabBar);
    }
    RefPtr<BoxComponent> tabBarBox = AceType::MakeRefPtr<BoxComponent>();
    tabBarBox->SetChild(display);
    tabBarBox->SetDeliverMinToChild(false);

    if (!declaration->HasToolBar()) {
        display->SetOpacity(0.0, declaration->animationOption);
        tabBarBox->SetHeight(Dimension(0.0, DimensionUnit::VP), declaration->animationOption);
    } else {
        display->SetOpacity(1.0, declaration->animationOption);
        tabBarBox->SetHeight(theme->GetDefaultHeight(), declaration->animationOption);
    }

    return AceType::MakeRefPtr<ComposedComponent>("navigation", "navigationToolBarComposed", tabBarBox);
#else
    return nullptr;
#endif
}

bool NavigationContainerComponent::NeedSection() const
{
    bool isSupportDeviceType = SystemProperties::GetDeviceType() == DeviceType::TABLET;
    bool isWideScreen = SystemProperties::GetDevcieOrientation() == DeviceOrientation::LANDSCAPE;
    return isSupportDeviceType && isWideScreen;
}

void NavigationContainerComponent::Build(const WeakPtr<PipelineContext>& context, int32_t menuCount)
{
    if (!declaration_) {
        return;
    }

    auto content = GetChildren();
    ClearChildren();

    auto originalContent = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, content);
    RefPtr<ColumnComponent> fixPart = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::FLEX_START, FlexAlign::FLEX_START, std::list<RefPtr<OHOS::Ace::Component>>());
    fixPart->AppendChild(NavigationBarBuilder(declaration_, "navigationBar", direction_).Build(context, menuCount));
    fixPart->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(1.0, 1.0, 0.0, originalContent));
    tabController_ = TabController::GetController(GetGlobalTabControllerId());
    fixPart->AppendChild(NavigationContainerComponent::BuildToolBar(declaration_, tabController_));

    if (NeedSection()) {
        auto rootContainer = AceType::MakeRefPtr<RowComponent>(
            FlexAlign::FLEX_START, FlexAlign::FLEX_START, std::list<RefPtr<OHOS::Ace::Component>>());
        fixPart->SetFlexWeight(SECTION_INDEX_PART_WEIGHT);
        rootContainer->AppendChild(fixPart);

        auto sectionPart = AceType::MakeRefPtr<StageComponent>(std::list<RefPtr<Component>>(), true);
        sectionPart->SetFlexWeight(SECTION_CONTENT_PART_WEIGHT);
        rootContainer->AppendChild(sectionPart);

        AppendChild(rootContainer);
    } else {
        AppendChild(fixPart);
    }
}

} // namespace OHOS::Ace
