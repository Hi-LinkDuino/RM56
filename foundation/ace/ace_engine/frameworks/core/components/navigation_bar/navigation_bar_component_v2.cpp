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

#include "core/components/navigation_bar/navigation_bar_component_v2.h"

#include "base/utils/system_properties.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/navigation_bar/navigation_bar_element.h"
#include "core/components/padding/padding_component.h"
#include "core/components/stack/stack_component.h"
#include "core/components/transform/transform_component.h"
#include "core/event/ace_event_helper.h"
#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace {
namespace {

constexpr double MIDDLE_ZONE_FLEX_GROW = 1.0;
constexpr double MIDDLE_ZONE_FLEX_SHRINK = 1.0;
constexpr double MENU_DEFAULT_HEIGHT = 56.0;

} // namespace

RefPtr<Component> NavigationBarBuilder::BuildMiniLayer()
{
    Edge padding;
    RefPtr<RowComponent> container =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    container->SetTextDirection(direction_);

    if (!declaration_->hideBarBackButton) {
        IconImage backIcon(theme_->GetBackResourceId(), menuIconSize_, menuIconSize_);
        backIcon.image->SetTextDirection(direction_);
        backIcon.image->SetMatchTextDirection(true);
        backIcon.image->SetColor(theme_->GetMenuIconColor());
        auto backButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, backIcon, backClickMarker_);
        EventMarker clickEventId([weakContext = context_](const BaseEventInfo* info) {
            auto context = weakContext.Upgrade();
            if (!context) {
                LOGW("context is empty");
                return;
            }
            context->CallRouterBackToPopPage();
        });
        backButton->SetClickedEventId(clickEventId);
        container->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "backButton", backButton));
        container->AppendChild(BuildPadding(theme_->GetTitleMinPadding().Value()));
        padding.SetLeft(theme_->GetDefaultPaddingStart());
    } else {
        container->AppendChild(BuildPadding(theme_->GetMaxPaddingStart().Value()));
    }

    container->AppendChild(BuildTitle());
    container->SetUpdateType(UpdateType::REBUILD);

    if (AddMenu(container)) {
        padding.SetRight(theme_->GetDefaultPaddingEnd());
    } else {
        padding.SetRight(theme_->GetMaxPaddingStart() - theme_->GetTitleMinPadding());
    }

    return BuildAnimationContainer(container, theme_->GetHeight(), padding);
}

RefPtr<Component> NavigationBarBuilder::BuildFullLayer()
{
    RefPtr<ColumnComponent> columnContainer =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, std::list<RefPtr<Component>>());
    columnContainer->SetTextDirection(direction_);
    columnContainer->SetCrossAxisSize(CrossAxisSize::MAX);
    columnContainer->AppendChild(BuildTitle());

    RefPtr<RowComponent> menuContainer =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_END, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    menuContainer->SetTextDirection(direction_);
    AddMenu(menuContainer);
    auto menusBox = AceType::MakeRefPtr<BoxComponent>();
    menusBox->SetHeight(theme_->GetHeight().Value(), theme_->GetHeight().Unit());
    menusBox->SetChild(menuContainer);

    RefPtr<ColumnComponent> container =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, std::list<RefPtr<Component>>());
    container->SetTextDirection(direction_);
    container->SetCrossAxisSize(CrossAxisSize::MAX);
    container->AppendChild(menusBox);
    container->AppendChild(columnContainer);

    Edge padding;
    padding.SetLeft(theme_->GetMaxPaddingStart());
    padding.SetRight(theme_->GetDefaultPaddingEnd());
    return BuildAnimationContainer(container, theme_->GetHeightEmphasize(), padding);
}

RefPtr<Component> NavigationBarBuilder::BuildFreeModeBar()
{
    Edge padding;
    padding.SetLeft(theme_->GetMaxPaddingStart());
    padding.SetRight(theme_->GetMaxPaddingStart());
    RefPtr<ColumnComponent> columnContainer =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, std::list<RefPtr<Component>>());
    columnContainer->SetTextDirection(direction_);
    columnContainer->SetCrossAxisSize(CrossAxisSize::MAX);
    columnContainer->AppendChild(BuildTitle());
    auto columnBox = AceType::MakeRefPtr<BoxComponent>();
    columnBox->SetChild(columnContainer);
    columnBox->SetPadding(padding);

    padding.SetLeft(theme_->GetDefaultPaddingEnd());
    padding.SetRight(theme_->GetDefaultPaddingEnd());
    RefPtr<RowComponent> menuContainer =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_END, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    menuContainer->SetTextDirection(direction_);
    AddMenu(menuContainer);
    auto menusBox = AceType::MakeRefPtr<BoxComponent>();
    menusBox->SetHeight(theme_->GetHeight().Value(), theme_->GetHeight().Unit());
    menusBox->SetChild(menuContainer);
    menusBox->SetPadding(padding);

    auto height = !declaration_->subTitle.empty() ? Dimension(134, DimensionUnit::VP) : theme_->GetHeightEmphasize();
    RefPtr<CollapsingNavigationBarComponent> collapsingNavigationBar =
        AceType::MakeRefPtr<CollapsingNavigationBarComponent>(
            titleComposed_, subTitleComposed_, declaration_->titleModeChangedEvent);
    collapsingNavigationBar->AppendChild(menusBox);
    collapsingNavigationBar->AppendChild(columnBox);
    collapsingNavigationBar->SetMinHeight(theme_->GetHeight());

    return BuildAnimationContainer(collapsingNavigationBar, height, Edge());
}

RefPtr<Component> NavigationBarBuilder::BuildAnimationContainer(
    const RefPtr<Component>& content, const Dimension& height, const Edge& padding)
{
    auto boxContainer = AceType::MakeRefPtr<BoxComponent>();
    boxContainer->SetChild(content);
    boxContainer->SetPadding(padding);
    auto display = AceType::MakeRefPtr<DisplayComponent>();
    display->SetChild(boxContainer);
    if (!declaration_->hideBar) {
        boxContainer->SetHeight(height, declaration_->animationOption);
        display->SetOpacity(1.0, declaration_->animationOption);
    } else {
        boxContainer->SetHeight(Dimension(0.0, height.Unit()), declaration_->animationOption);
        display->SetOpacity(0.0, declaration_->animationOption);
    }
    return display;
}

RefPtr<Component> NavigationBarBuilder::BuildTitle()
{
    RefPtr<ColumnComponent> titleContainer = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::FLEX_START, FlexAlign::FLEX_START, std::list<RefPtr<Component>>());
    titleContainer->SetTextDirection(direction_);
    titleContainer->SetMainAxisSize(MainAxisSize::MIN);

    if (declaration_->customTitle || !declaration_->title.empty()) {
        RefPtr<Component> title;
        if (declaration_->customTitle) {
            title = declaration_->customTitle;
        } else {
            title = BuildTitleText(
                declaration_->title, theme_->GetTitleColor(), theme_->GetTitleFontSize(), FontWeight::W500);
        }
        auto transform = AceType::MakeRefPtr<TransformComponent>();
        transform->SetChild(title);
        transform->SetOriginDimension(DimensionOffset(Offset(0.0, 0.0)));
        titleComposed_ = GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "titleText", transform);
        titleContainer->AppendChild(titleComposed_);
    }
    if (!declaration_->subTitle.empty()) {
        titleContainer->AppendChild(BuildPadding(2, true));
        auto subTitleText = BuildTitleText(
            declaration_->subTitle, theme_->GetSubTitleColor(), theme_->GetSubTitleFontSize(), FontWeight::W400);
        auto display = AceType::MakeRefPtr<DisplayComponent>(subTitleText);
        subTitleComposed_ = GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", display);
        titleContainer->AppendChild(subTitleComposed_);
    }

    return AceType::MakeRefPtr<FlexItemComponent>(MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, titleContainer);
}

bool NavigationBarBuilder::AddMenu(const RefPtr<ComponentGroup>& container)
{
    if (declaration_->customMenus) {
        LOGE("arg is customMenus.");
        container->AppendChild(declaration_->customMenus);
        return true;
    }
    uint32_t showInBarSize = 0;
    bool hasRoom = true;
    uint32_t mostShowInBarSize =
        menuCount_ > 0 ? static_cast<uint32_t>(menuCount_) : theme_->GetMostMenuItemCountInBar();
    bool needAddPadding = false;
    menu_ = AceType::MakeRefPtr<MenuComponent>("", "navigationMenu");
    auto ctx = context_.Upgrade();
    if (ctx) {
        auto themeManager = ctx->GetThemeManager();
        if (themeManager) {
            menu_->SetTheme(themeManager->GetTheme<SelectTheme>());
        }
    }
    auto menusSize = declaration_->menuItems.size();
    for (const auto& menuItem : declaration_->menuItems) {
        hasRoom = hasRoom && ((showInBarSize < mostShowInBarSize - 1) ||
            (showInBarSize == mostShowInBarSize - 1 && menusSize == mostShowInBarSize) ||
            (showInBarSize < mostShowInBarSize && SystemProperties::GetDeviceType() == DeviceType::TV));
        if (hasRoom) {
            showInBarSize++;
            IconImage menuIcon(menuItem->icon, menuIconSize_, menuIconSize_);
            auto optionButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, menuIcon);
            if (theme_->GetMenuItemPadding().Value() > 0.0 && needAddPadding) {
                container->AppendChild(BuildPadding(theme_->GetMenuItemPadding().Value()));
            }
            optionButton->SetClickedEventId(menuItem->actionWithParam);
            container->AppendChild(
                GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "optionButton", optionButton));
            needAddPadding = true;
        } else {
            AddOption(menuItem);
        }
    }
    if (menu_->GetOptionCount() > 0) {
        // add collapse menu
        IconImage moreIcon(theme_->GetMoreResourceId(), menuIconSize_, menuIconSize_);
        moreIcon.image->SetColor(theme_->GetMenuIconColor());
        moreButton_ = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, moreIcon);
        container->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "moreButton", moreButton_));
        container->AppendChild(menu_);
    }
    BindMoreButtonClickEvent();
    return showInBarSize > 0 || menu_->GetOptionCount() > 0;
}

void NavigationBarBuilder::AddOption(const RefPtr<ToolBarItem>& menuItem)
{
    auto optionComponent = AceType::MakeRefPtr<OptionComponent>();
    auto ctx = context_.Upgrade();
    if (ctx) {
        auto themeManager = ctx->GetThemeManager();
        if (themeManager) {
            optionComponent->InitTheme(themeManager);
        }
    }
    optionComponent->SetText(AceType::MakeRefPtr<TextComponent>(menuItem->value));
    optionComponent->SetValue(menuItem->value);
    optionComponent->SetIcon(AceType::MakeRefPtr<ImageComponent>(menuItem->icon));
    optionComponent->SetClickEvent(menuItem->action);
    menu_->AppendOption(optionComponent);
}

void NavigationBarBuilder::BindMoreButtonClickEvent()
{
    if (!moreButton_) {
        return;
    }
    EventMarker clickEventId([menu = menu_, context = context_](const BaseEventInfo* info) {
        if (!menu) {
            return;
        }
        auto showMenuFunc = menu->GetTargetCallback();
        if (!showMenuFunc) {
            return;
        }
        auto ctx = context.Upgrade();
        if (!ctx) {
            return;
        }
        double morePopupOffsetX = 0.0;
        morePopupOffsetX = ctx->GetStageRect().Width();
        showMenuFunc("", Offset(morePopupOffsetX, MENU_DEFAULT_HEIGHT));
    });
    moreButton_->SetClickedEventId(clickEventId);
}

} // namespace OHOS::Ace
