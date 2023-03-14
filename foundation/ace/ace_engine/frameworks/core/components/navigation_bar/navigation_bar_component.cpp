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

#include "core/components/navigation_bar/navigation_bar_component.h"

#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/display/display_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/navigation_bar/navigation_bar_element.h"
#include "core/components/padding/padding_component.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/stack/stack_component.h"
#include "core/components/transform/transform_component.h"
#include "core/gestures/click_recognizer.h"
#ifdef WEARABLE_PRODUCT
#include "core/components/box/box_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/image/image_component.h"
#include "core/components/text/text_component.h"
#endif

namespace OHOS::Ace {
namespace {

#ifndef WEARABLE_PRODUCT
constexpr double MIDDLE_ZONE_FLEX_GROW = 1.0;
constexpr double MIDDLE_ZONE_FLEX_SHRINK = 1.0;
// only for default
constexpr double LOGO_PADDING_RIGHT_VALUE = 16.0;
// only for tv
constexpr double TV_PADDING_BETWEEN_TITLE_AND_SUBTITLE = 12.0;
constexpr double WATCH_BAR_PADDING_BOTTOM = 8.0;
#endif

// only for watch
constexpr double WATCH_TITLE_LINE_PADDING_VALUE = 52.0;
const Dimension WATCH_TITLE_LINE_PADDING(WATCH_TITLE_LINE_PADDING_VALUE, DimensionUnit::VP);
constexpr double WATCH_SUBTITLE_LINE_PADDING_VALUE = 26.0;
const Dimension WATCH_SUBTITLE_LINE_PADDING(WATCH_SUBTITLE_LINE_PADDING_VALUE, DimensionUnit::VP);
constexpr double WATCH_TITLE_PADDING_TOP = 20.0;
constexpr double WATCH_SUBTITLE_PADDING_TOP = 2.0;
constexpr double WATCH_PADDING_BETWEEN_LOGO_AND_SUBTITLE = 8.0;
constexpr Dimension WATCH_TITLE_MIN_SIZE = 16.0_fp;
constexpr Dimension WATCH_SUBTITLE_MIN_SIZE = 13.0_fp;

RefPtr<TextComponent> BuildWatchTitleText(
    const std::string& value, const Color& color, const Dimension& fontSize, bool subtitle)
{
    auto text = AceType::MakeRefPtr<TextComponent>(value);
    TextStyle textStyle;
    textStyle.SetTextColor(color);
    textStyle.SetFontSize(fontSize);
    textStyle.SetAllowScale(false);
    textStyle.SetFontWeight(FontWeight::W500);
    if (subtitle) {
        textStyle.SetMaxLines(1);
        textStyle.SetAdaptMaxFontSize(fontSize);
        textStyle.SetAdaptMinFontSize(WATCH_SUBTITLE_MIN_SIZE);
    } else {
        textStyle.SetMaxLines(2);
        textStyle.SetAdaptMaxFontSize(fontSize);
        textStyle.SetAdaptMinFontSize(WATCH_TITLE_MIN_SIZE);
    }

    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    text->SetTextStyle(textStyle);
    return text;
}

} // namespace

NavigationBarComponent::NavigationBarComponent(const ComposeId& id, const std::string& name)
    : ComposedComponent(id, name), data_(AceType::MakeRefPtr<NavigationBarData>())
{}

RefPtr<Element> NavigationBarComponent::CreateElement()
{
    return AceType::MakeRefPtr<NavigationBarElement>(GetId());
}

#ifndef WEARABLE_PRODUCT
RefPtr<Component> PhoneTitleBarBuilder::BuildTitle(double padding)
{
    RefPtr<ColumnComponent> titleContainer = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::FLEX_START, FlexAlign::FLEX_START, std::list<RefPtr<Component>>());
    titleContainer->SetTextDirection(direction_);
    titleContainer->SetMainAxisSize(MainAxisSize::MIN);
    if (padding > 0) {
        titleContainer->AppendChild(BuildPadding(padding, true));
    }

    if (selectPopup_) {
        BuildSelectPopup(titleContainer);
    } else if (!title_.empty()) {
        auto titleText = BuildTitleText(title_, titleColor_, titleFontSize_, FontWeight::W500);
        auto transform = AceType::MakeRefPtr<TransformComponent>();
        transform->SetChild(titleText);
        transform->SetOriginDimension(DimensionOffset(Offset(0.0, 0.0)));
        titleComposed_ = GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "titleText", transform);
        titleContainer->AppendChild(titleComposed_);
    }
    if (!subTitle_.empty()) {
        titleContainer->AppendChild(BuildPadding(2, true));
        auto subTitleText = BuildTitleText(subTitle_, subTitleColor_, subTitleFontSize_, FontWeight::W400);
        auto display = AceType::MakeRefPtr<DisplayComponent>(subTitleText);
        subTitleComposed_ = GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", display);
        titleContainer->AppendChild(subTitleComposed_);
    }

    return AceType::MakeRefPtr<FlexItemComponent>(MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, titleContainer);
}

void TabBarBuilder::BuildTabBar(const RefPtr<ComponentGroup>& parent)
{
    tabBar_->InitNavigationBarStyle();
    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(tabBar_);
    box->SetDeliverMinToChild(false);
    parent->AppendChild(
        AceType::MakeRefPtr<FlexItemComponent>(MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, box));
}

void TitleBarMenuBuilder::MoveMenuItemsToBar(const RefPtr<ComponentGroup>& container)
{
    menu_->ClearOptions();
    uint32_t showInBarSize = 0;
    bool hasRoom = true;
    uint32_t mostShowInBarSize = theme_->GetMostMenuItemCountInBar();

    bool needAddPadding = false;
    for (const auto& menuItem : allMenuItems_) {
        hasRoom = hasRoom && ((showInBarSize < mostShowInBarSize - 1) ||
                    (showInBarSize == mostShowInBarSize - 1 && allMenuItems_.size() == mostShowInBarSize) ||
                    (showInBarSize < mostShowInBarSize && SystemProperties::GetDeviceType() == DeviceType::TV));
        if (menuItem->GetShowInNavigationBar() == ShowInNavigationBar::SHOW && hasRoom) {
            showInBarSize++;
            auto menuItemIcon = menuItem->GetIcon();
            if (!menuItemIcon) {
                LOGW("navigation bar menu item icon empty");
                continue;
            }

            IconImage menuIcon(menuItemIcon->GetSrc(), menuIconSize_, menuIconSize_, imageFill_);
            auto optionButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, menuIcon);
            if (theme_->GetMenuItemPadding().Value() > 0.0 && needAddPadding) {
                container->AppendChild(BuildPadding(theme_->GetMenuItemPadding().Value()));
            }
            container->AppendChild(
                GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "optionButton", optionButton));
            needAddPadding = true;

            NavigationBarComponent::MenuItemInBar menuItemInBar { menuItem->GetValue(),
                BackEndEventManager<void(const ClickInfo&)>::GetInstance().GetAvailableMarker(), optionButton };
            menuItemsInBar_.emplace(menuItemInBar.clickEvent.GetData().eventId, menuItemInBar);
        } else {
            menu_->AppendOption(menuItem);
        }
    }
}

void TitleBarMenuBuilder::AddCollapseMenu(const RefPtr<ComponentGroup>& container)
{
    IconImage moreIcon(theme_->GetMoreResourceId(), menuIconSize_, menuIconSize_);
    moreIcon.image->SetImageFill(theme_->GetMenuIconColor());
    moreButton_ = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, moreIcon);
    container->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "moreButton", moreButton_));
    container->AppendChild(menu_);
}

bool TitleBarMenuBuilder::AddMenu(const RefPtr<ComponentGroup>& container, bool canCollapse)
{
    if (menu_) {
        MoveMenuItemsToBar(container);
        if (menu_->GetOptionCount() > 0 && canCollapse) {
            AddCollapseMenu(container);
        }
        return true;
    }
    return false;
}

void PhoneTitleBarBuilder::BuildSelectPopup(const RefPtr<ComponentGroup>& parent)
{
    RefPtr<BoxComponent> selectBox = AceType::MakeRefPtr<BoxComponent>();
    if (!selectPopup_->IsFontSet()) {
        selectPopup_->SetFontSize(titleFontSize_);
    }
    selectPopup_->SetInnerPadding(Edge());
    selectPopup_->SetClickedColor(Color::TRANSPARENT);
    selectBox->SetChild(selectPopup_);

    auto transform = AceType::MakeRefPtr<TransformComponent>();
    transform->SetChild(selectBox);
    transform->SetOriginDimension(DimensionOffset(Offset(0.0, 0.0)));
    titleComposed_ = GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "select", transform);
    parent->AppendChild(titleComposed_);
}

void PhoneTitleBarBuilder::BuildTitleZone(const RefPtr<ComponentGroup>& parent, double padding)
{
    if (tabBar_) {
        if (parent->GetChildren().empty()) {
            padding_.SetLeft(Dimension(0, DimensionUnit::VP));
        }
        BuildTabBar(parent);
    } else {
        parent->AppendChild(BuildTitle(padding));
    }
}

RefPtr<Component> EmphasizeTitleBarBuilder::Build()
{
    RefPtr<RowComponent> menuContainer =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_END, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    menuContainer->SetTextDirection(direction_);
    auto menusBox = AceType::MakeRefPtr<BoxComponent>();
    menusBox->SetHeight(theme_->GetHeight().Value(), theme_->GetHeight().Unit());
    menusBox->SetChild(menuContainer);
    AddMenu(menuContainer);
    RefPtr<ColumnComponent> columnContainer =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, std::list<RefPtr<Component>>());
    columnContainer->SetTextDirection(direction_);
    columnContainer->SetCrossAxisSize(CrossAxisSize::MAX);
    columnContainer->AppendChild(BuildTitle());

    RefPtr<CollapsingNavigationBarComponent> collapsingNavigationBar =
        AceType::MakeRefPtr<CollapsingNavigationBarComponent>(AceType::Claim(this));
    collapsingNavigationBar->AppendChild(menusBox);
    collapsingNavigationBar->AppendChild(columnContainer);
    collapsingNavigationBar->SetMinHeight(theme_->GetHeight());

    if (!subTitle_.empty() && !tabBar_) {
        height_ = Dimension(134, DimensionUnit::VP);
    }
    if (tabBar_) {
        padding_.SetLeft(Dimension(0, DimensionUnit::VP));
    }
    return TitleBarBuilder::BuildContainer(collapsingNavigationBar);
}

void NormalTitleBarBuilder::BuildStartZone(const RefPtr<RowComponent>& parent)
{
    if (!startIconSrc_.empty()) {
        IconImage startIcon(startIconSrc_, menuIconSize_, menuIconSize_, imageFill_);
        auto startButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, startIcon, startClickMarker_);
        parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "startButton", startButton));
        parent->AppendChild(BuildPadding(theme_->GetTitleMinPadding().Value()));
        padding_.SetLeft(theme_->GetDefaultPaddingStart());
    } else if (backEnabled_) {
        IconImage backIcon(theme_->GetBackResourceId(), menuIconSize_, menuIconSize_);
        backIcon.image->SetTextDirection(direction_);
        backIcon.image->SetMatchTextDirection(true);
        backIcon.image->SetColor(theme_->GetMenuIconColor());
        auto backButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, backIcon, backClickMarker_);
        parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "backButton", backButton));
        parent->AppendChild(BuildPadding(theme_->GetTitleMinPadding().Value()));
        padding_.SetLeft(theme_->GetDefaultPaddingStart());
    }
}

void NormalTitleBarBuilder::BuildLogo(const RefPtr<RowComponent>& parent)
{
    if (!logoSrc_.empty()) {
        auto logoSize = theme_->GetLogoIconSize();
        IconImage logoIconImage(logoSrc_, logoSize, logoSize, imageFill_);
        parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "image", logoIconImage.image));
        parent->AppendChild(BuildPadding(LOGO_PADDING_RIGHT_VALUE));
        padding_.SetLeft(theme_->GetDefaultPaddingStart());
    }
}

void NormalTitleBarBuilder::BuildEndZone(const RefPtr<RowComponent>& parent)
{
    parent->AppendChild(BuildPadding(theme_->GetTitleMinPadding().Value()));
    if (!endIconSrc_.empty()) {
        IconImage endIcon(endIconSrc_, menuIconSize_, menuIconSize_, imageFill_);
        auto endButton = BuildIconButton(theme_, menuZoneSize_, menuZoneSize_, endIcon, endClickMarker_);
        parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "endButton", endButton));
        padding_.SetRight(theme_->GetDefaultPaddingEnd());
    } else if (AddMenu(parent)) {
        padding_.SetRight(theme_->GetDefaultPaddingEnd());
    } else {
        padding_.SetRight(theme_->GetMaxPaddingStart() - theme_->GetTitleMinPadding());
    }
}

RefPtr<Component> NormalTitleBarBuilder::Build()
{
    if (!theme_) {
        LOGW("theme is null");
        return nullptr;
    }
    padding_.SetLeft(theme_->GetMaxPaddingStart());
    padding_.SetRight(theme_->GetMaxPaddingEnd());

    RefPtr<RowComponent> container =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    container->SetTextDirection(direction_);
    container->SetUpdateType(UpdateType::REBUILD);

    if (backEnabled_) {
        titleFontSize_ = Dimension(20, DimensionUnit::VP);
    }

    BuildStartZone(container);
    BuildLogo(container);
    BuildTitleZone(container);
    BuildEndZone(container);
    return TitleBarBuilder::BuildContainer(container);
}

void NormalTitleBarBuilder::BindDefaultBackEvent(const WeakPtr<PipelineContext>& weakContext)
{
    if (!backClickMarker_.IsEmpty()) {
        return;
    }
    backClickMarker_ = BackEndEventManager<void(const ClickInfo&)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
        backClickMarker_, [weakContext](const ClickInfo& clickInfo) {
            auto context = weakContext.Upgrade();
            if (!context) {
                LOGW("context is empty");
                return;
            }
            context->CallRouterBackToPopPage();
        });
}

void TVTitleBarBuilder::BuildTitle(const RefPtr<ComponentGroup>& parent)
{
    if (!title_.empty()) {
        if (!titleFontSize_.IsValid()) {
            titleFontSize_ = theme_->GetTitleFontSize();
        }
        auto titleText = BuildTitleText(title_, titleColor_, titleFontSize_, FontWeight::W500);
        parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "titleText", titleText));
    }
}

void TVTitleBarBuilder::BuildSubTitle(const RefPtr<ComponentGroup>& parent, const RefPtr<ComponentGroup>& leftContainer)
{
    if (!subTitle_.empty()) {
        if (menu_) {
            leftContainer->AppendChild(BuildPadding(TV_PADDING_BETWEEN_TITLE_AND_SUBTITLE));
            auto subTitleText = BuildTitleText(subTitle_, subTitleColor_, subTitleFontSize_, FontWeight::W400);
            leftContainer->AppendChild(
                GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", subTitleText));
            parent->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(
                MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, leftContainer));
        } else {
            parent->AppendChild(AceType::MakeRefPtr<FlexItemComponent>(
                MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, leftContainer));
            auto subTitleText = BuildTitleText(subTitle_, subTitleColor_, subTitleFontSize_, FontWeight::W400);
            parent->AppendChild(
                GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", subTitleText));
        }
    } else {
        parent->AppendChild(
            AceType::MakeRefPtr<FlexItemComponent>(MIDDLE_ZONE_FLEX_GROW, MIDDLE_ZONE_FLEX_SHRINK, 0.0, leftContainer));
    }
}

RefPtr<Component> TVTitleBarBuilder::Build()
{
    RefPtr<RowComponent> container =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_END, std::list<RefPtr<Component>>());
    container->SetTextDirection(direction_);
    RefPtr<RowComponent> leftContainer =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_END, std::list<RefPtr<Component>>());
    leftContainer->SetTextDirection(direction_);

    BuildTitle(leftContainer);
    BuildSubTitle(container, leftContainer);
    AddMenu(container, false);

    padding_.SetLeft(theme_->GetMaxPaddingStart());
    padding_.SetRight(theme_->GetMaxPaddingEnd());
    padding_.SetBottom(Dimension(WATCH_BAR_PADDING_BOTTOM, DimensionUnit::VP));

    auto boxContainer = BuildContainer(container);
    boxContainer->SetAlignment(Alignment::BOTTOM_CENTER);
    return boxContainer;
}

#endif

void WatchTitleBarBuilder::BuildTitle(const RefPtr<ComponentGroup>& parent)
{
    parent->AppendChild(BuildPadding(WATCH_TITLE_PADDING_TOP, true));
    if (!titleFontSize_.IsValid()) {
        titleFontSize_ = theme_->GetTitleFontSize();
    }

    auto titleBox = AceType::MakeRefPtr<BoxComponent>();
    titleBox->SetPadding(
        Edge(WATCH_TITLE_LINE_PADDING.Value(), 0.0, WATCH_TITLE_LINE_PADDING.Value(), 0.0, DimensionUnit::VP));
    auto titleText = BuildWatchTitleText(title_, titleColor_, titleFontSize_, false);
    titleBox->SetChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "titleText", titleText));
    parent->AppendChild(titleBox);
}

void WatchTitleBarBuilder::BuildSubTitle(const RefPtr<ComponentGroup>& parent)
{
    if (!subTitle_.empty()) {
        parent->AppendChild(BuildPadding(WATCH_SUBTITLE_PADDING_TOP, true));

        auto subTitleBox = AceType::MakeRefPtr<BoxComponent>();
        subTitleBox->SetPadding(Edge(
            WATCH_SUBTITLE_LINE_PADDING.Value(), 0.0, WATCH_SUBTITLE_LINE_PADDING.Value(), 0.0, DimensionUnit::VP));
        if (!logoSrc_.empty()) {
            RefPtr<RowComponent> subContainer = AceType::MakeRefPtr<RowComponent>(
                FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
            subContainer->SetTextDirection(direction_);
            BuildLogo(subContainer);
            subContainer->AppendChild(BuildPadding(WATCH_PADDING_BETWEEN_LOGO_AND_SUBTITLE));
            auto subTitleText = BuildWatchTitleText(subTitle_, subTitleColor_, subTitleFontSize_, true);
            auto subTitleFlex = AceType::MakeRefPtr<FlexItemComponent>(
                1, 1, 0, GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", subTitleText));
            subContainer->AppendChild(subTitleFlex);
            subContainer->SetMainAxisSize(MainAxisSize::MIN);
            subTitleBox->SetChild(subContainer);
        } else {
            auto subTitleText = BuildWatchTitleText(subTitle_, subTitleColor_, subTitleFontSize_, true);
            subTitleBox->SetChild(
                GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "subTitleText", subTitleText));
        }
        parent->AppendChild(subTitleBox);
    }
}

void WatchTitleBarBuilder::BuildLogo(const RefPtr<ComponentGroup>& parent)
{
    auto logoSize = theme_->GetLogoIconSize();
    IconImage logoIconImage(logoSrc_, logoSize, logoSize);
    parent->AppendChild(GenerateAccessibilityComposed(StringUtils::StringToInt(id_), "image", logoIconImage.image));
}

RefPtr<Component> WatchTitleBarBuilder::Build()
{
    RefPtr<ColumnComponent> container =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    container->SetMainAxisSize(MainAxisSize::MIN);
    container->SetTextDirection(direction_);

    if (!title_.empty()) {
        BuildTitle(container);
    } else {
        LOGW("navigation bar no title, will don't show");
        return container;
    }

    BuildSubTitle(container);
    return container;
}

RefPtr<Component> NavigationBarComponent::Build(const WeakPtr<PipelineContext>& context)
{
    auto deviceType = SystemProperties::GetDeviceType();
    LOGD("build navigation bar for %{public}d", deviceType);
    RefPtr<TitleBarBuilder> builder;
    switch (deviceType) {
#ifndef WEARABLE_PRODUCT
        case DeviceType::TV:
            builder = AceType::MakeRefPtr<TVTitleBarBuilder>(data_);
            break;
        case DeviceType::PHONE:
            if (data_->type == NavigationBarType::EMPHASIZE) {
                builder = AceType::MakeRefPtr<EmphasizeTitleBarBuilder>(data_);
            } else {
                auto normalBuilder = AceType::MakeRefPtr<NormalTitleBarBuilder>(data_);
                normalBuilder->BindDefaultBackEvent(context);
                builder = normalBuilder;
            }
            break;
#endif
        case DeviceType::WATCH:
            builder = AceType::MakeRefPtr<WatchTitleBarBuilder>(data_);
            break;
        default:
            LOGW("build navigation bar failed, unknown device type");
            return nullptr;
    }

    builder->SetContext(context);
    builder->SetId(GetId());
    builder->SetTextDirection(direction_);
    auto component = builder->Build();
#ifndef WEARABLE_PRODUCT
    auto menuBuilder = AceType::DynamicCast<TitleBarMenuBuilder>(builder);
    if (menuBuilder) {
        moreButton_ = menuBuilder->GetMoreButton();
        menuItemsInBar_ = menuBuilder->GetMenuItemsInBar();
    }
#endif

    return component;
}

} // namespace OHOS::Ace
