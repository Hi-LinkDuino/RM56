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

#include "core/components/tab_bar/tab_bar_component.h"

#include "base/utils/system_properties.h"
#include "core/components/tab_bar/render_tab_bar.h"
#include "core/components/tab_bar/tab_bar_element.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ACTIVE_TEXT_SIZE = 24;
constexpr int32_t INACTIVE_TEXT_SIZE = 18;
constexpr double ACTIVE_TEXT_OPACITY = 0.9;
constexpr double INACTIVE_TEXT_OPACITY = 0.6;
constexpr Dimension BOTTOM_PADING(12, DimensionUnit::VP);

constexpr int32_t BOTTOM_TAB_TEXT_SIZE = 10;
constexpr double BOTTOM_TAB_INACTIVE_TEXT_OPACITY = 0.4;

} // namespace

TabBarComponent::TabBarComponent(const std::list<RefPtr<Component>>& tabItems, const RefPtr<TabController>& controller,
    const RefPtr<BoxComponent>& indicator)
    : ComponentGroup(tabItems)
{
    controller_ = controller;
    indicator_ = indicator;
    focusIndicator_ = indicator;
}

RefPtr<Element> TabBarComponent::CreateElement()
{
    return AceType::MakeRefPtr<TabBarElement>();
}

RefPtr<RenderNode> TabBarComponent::CreateRenderNode()
{
    return RenderTabBar::Create();
}

void TabBarComponent::InitStyle(const RefPtr<TabTheme>& theme)
{
    if (!theme) {
        return;
    }

    focusRadiusDimension_ = theme->GetFocusIndicatorRadius();
    focusAnimationColor_ = theme->GetFocusIndicatorColor();
    gradientWidth_ = theme->GetGradientWidth();
    activeIndicatorMinWidth_ = theme->GetActiveIndicatorMinWidth();

    Dimension labelPaddingDimension = theme->GetLabelPadding();
    SetLabelPadding(
        Edge(labelPaddingDimension.Value(), 0.0, labelPaddingDimension.Value(), 0.0, labelPaddingDimension.Unit()));

    auto paddingDimension = theme->GetPadding();
    SetPadding(Edge(paddingDimension.Value(), 0.0, paddingDimension.Value(), 0.0, paddingDimension.Unit()));

    auto indicatorPadding = Edge(0.0, theme->GetActiveIndicatorPadding().Value(), 0.0,
        theme->GetActiveIndicatorPadding().Value(), theme->GetActiveIndicatorPadding().Unit());
    indicator_ = AceType::MakeRefPtr<TabBarIndicatorComponent>(
        indicatorPadding, theme->GetActiveIndicatorColor(), theme->GetActiveIndicatorWidth());
    indicator_->SetPadding(indicatorPadding);

    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::TV) {
        RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
        Border border;
        border.SetBorderRadius(Radius(theme->GetFocusIndicatorRadius()));
        backDecoration->SetBorder(border);
        backDecoration->SetBackgroundColor(theme->GetFocusIndicatorColor());

        focusIndicator_ = AceType::MakeRefPtr<TabBarIndicatorComponent>(backDecoration);
        auto focusIndicatorPadding = Edge(theme->GetFocusIndicatorHorizontalPadding().Value(),
            theme->GetFocusIndicatorVerticalPadding().Value(), theme->GetFocusIndicatorHorizontalPadding().Value(),
            theme->GetFocusIndicatorVerticalPadding().Value(), theme->GetFocusIndicatorVerticalPadding().Unit());
        focusIndicator_->SetPadding(focusIndicatorPadding);
    } else if (deviceType == DeviceType::PHONE) {
        focusIndicator_->SetPadding(indicatorPadding);
    }
}

void TabBarComponent::InitNavigationBarStyle()
{
    usingDefaultStyle_ = true;
    indicator_ = nullptr;
    activeTextStyle_.SetTextColor(Color::FromRGBO(0, 0, 0, ACTIVE_TEXT_OPACITY));
    activeTextStyle_.SetFontSize(Dimension(ACTIVE_TEXT_SIZE, DimensionUnit::VP));
    activeTextStyle_.SetMaxLines(1);
    activeTextStyle_.SetTextOverflow(TextOverflow::CLIP);

    inactiveTextStyle_.SetTextColor(Color::FromRGBO(0, 0, 0, BOTTOM_TAB_INACTIVE_TEXT_OPACITY));
    inactiveTextStyle_.SetFontSize(Dimension(INACTIVE_TEXT_SIZE, DimensionUnit::VP));
    inactiveTextStyle_.SetMaxLines(1);
    inactiveTextStyle_.SetTextOverflow(TextOverflow::CLIP);
    labelPadding_.SetBottom(BOTTOM_PADING);
    itemAlignment_ = Alignment::BOTTOM_CENTER;
    mode_ = TabBarMode::FIXED_START;
}

void TabBarComponent::InitBottomTabStyle(const RefPtr<TabTheme>& theme)
{
    usingDefaultStyle_ = true;
    indicator_ = nullptr;
    activeColor_ = theme->GetActiveIndicatorColor();
    activeTextStyle_.SetTextColor(activeColor_);
    activeTextStyle_.SetFontSize(Dimension(BOTTOM_TAB_TEXT_SIZE, DimensionUnit::VP));
    activeTextStyle_.SetMaxLines(1);
    activeTextStyle_.SetTextOverflow(TextOverflow::CLIP);

    inactiveColor_ = Color::FromRGBO(0, 0, 0, INACTIVE_TEXT_OPACITY);
    inactiveTextStyle_.SetTextColor(inactiveColor_);
    inactiveTextStyle_.SetFontSize(Dimension(BOTTOM_TAB_TEXT_SIZE, DimensionUnit::VP));
    inactiveTextStyle_.SetMaxLines(1);
    inactiveTextStyle_.SetTextOverflow(TextOverflow::CLIP);
}

void TabBarComponent::BuildItems(std::list<RefPtr<TabBarItemComponent>>& items)
{
    int32_t currentIndex = 0;
    int32_t activeIndex = controller_ ? controller_->GetIndex() : 0;
    for (const auto& pos : GetChildren()) {
        RefPtr<TabBarItemComponent> box = AceType::DynamicCast<TabBarItemComponent>(pos);
        if (!box) {
            box = AceType::MakeRefPtr<TabBarItemComponent>(pos);
        }
        if (usingDefaultStyle_) {
            if (currentIndex++ == activeIndex) {
                box->UpdateStyle(activeTextStyle_, activeColor_);
            } else {
                box->UpdateStyle(inactiveTextStyle_, inactiveColor_);
            }
        }
        box->SetPadding(labelPadding_);
        if (mode_ == TabBarMode::FIXED) {
            box->SetFlex(BoxFlex::FLEX_XY);
        } else {
            if (vertical_) {
                box->SetFlex(BoxFlex::FLEX_X);
            } else {
                box->SetFlex(BoxFlex::FLEX_Y);
            }
        }
        box->SetDeliverMinToChild(false);
        box->SetAlignment(itemAlignment_);
        items.push_back(box);
    }
}

} // namespace OHOS::Ace