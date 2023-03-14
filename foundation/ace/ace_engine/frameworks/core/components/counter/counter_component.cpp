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

#include "core/components/counter/counter_component.h"

#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/counter/counter_element.h"
#include "core/components/counter/render_counter.h"
#include "core/components/padding/padding_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t CLICKED_BLEND_COLOR = 0x19000000;
constexpr char COUNTER_BUTTON_ADD[] = "+";
constexpr char COUNTER_BUTTON_LESS[] = "-";
constexpr double COUNTER_BORDER_WIDTH = 1.0;
constexpr double COUNTER_RADIUS_DELTA = 2.0;
constexpr int32_t COUNTER_ITEM_SIZE_DIFF = 2;
constexpr double STATE_FAIELD_OPACITY = 0.3;

} // namespace

RefPtr<RenderNode> CounterComponent::CreateRenderNode()
{
    return RenderCounter::Create();
}

RefPtr<Element> CounterComponent::CreateElement()
{
    return AceType::MakeRefPtr<CounterElement>();
}

RefPtr<Component> CounterComponent::BuildChild(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return nullptr;
    }

    counterTheme_ = AceType::DynamicCast<CounterTheme>(themeManager->GetTheme(CounterTheme::TypeId()));
    if (!counterTheme_) {
        return nullptr;
    }

    // build up counter_component.
    RefPtr<BoxComponent> counterBox = AceType::MakeRefPtr<BoxComponent>();
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    BorderEdge edge(Color::GRAY, Dimension(COUNTER_BORDER_WIDTH, DimensionUnit::VP), BorderStyle::NONE);
    Border border(edge);
    border.SetBorderRadius(Radius(GetControlRadius()));
    backDecoration->SetBorder(border);
    counterBox->SetBackDecoration(backDecoration);
    counterBox->SetHeight(GetHeight());
    counterBox->SetWidth(GetWidth());
    std::list<RefPtr<Component>> rowChildren;
    RefPtr<RowComponent> counterRow =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, rowChildren);
    counterRow->SetMainAxisSize(MainAxisSize::MIN);
    BuildControl(counterTheme_, counterRow, decreButtonComponent_, std::string(COUNTER_BUTTON_LESS), true);
    counterRow->AppendChild(BuildDivider());
    RefPtr<BoxComponent> contentBox = AceType::MakeRefPtr<BoxComponent>();
    BuildContent(counterTheme_, counterRow, contentBox);
    counterRow->AppendChild(BuildDivider());
    BuildControl(counterTheme_, counterRow, increButtonComponent_, std::string(COUNTER_BUTTON_ADD), false);
    counterBox->SetChild(counterRow);

    displayComponent_ = AceType::MakeRefPtr<DisplayComponent>(counterBox);
    if (propState_ == false) {
        displayComponent_->SetOpacity(STATE_FAIELD_OPACITY);
    }

    return displayComponent_;
}

RefPtr<DividerComponent> CounterComponent::BuildDivider()
{
    auto divider = AceType::MakeRefPtr<DividerComponent>();
    divider->SetStrokeWidth(Dimension(COUNTER_BORDER_WIDTH, DimensionUnit::VP));
    divider->SetVertical(true);
    divider->SetDividerColor(Color::GRAY);
    return divider;
}

void CounterComponent::BuildControl(const RefPtr<CounterTheme>& counterTheme, const RefPtr<RowComponent>& row,
    RefPtr<ButtonComponent>& button, std::string content, bool isLeft)
{
    std::list<RefPtr<Component>> buttonChildren;
    auto textComponent = AceType::MakeRefPtr<TextComponent>(content);
    textComponent->SetTextStyle(counterTheme->GetContentTextStyle());
    buttonChildren.emplace_back(textComponent);
    button = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    button->SetHeight(Dimension(GetHeight().Value() - COUNTER_RADIUS_DELTA * COUNTER_BORDER_WIDTH, GetHeight().Unit()));
    button->SetWidth(
        Dimension(GetControlWidth().Value() - COUNTER_RADIUS_DELTA * COUNTER_BORDER_WIDTH, GetControlWidth().Unit()));
    button->SetBackgroundColor(propBackgroundColor_);
    button->SetClickedColor(Color(CLICKED_BLEND_COLOR));
    button->SetType(ButtonType::CUSTOM);

    // set up radius and click function for button.
    if (isLeft) {
        std::array<Radius, 4> radii = {
            Radius(Dimension(GetControlRadius().Value() - COUNTER_BORDER_WIDTH, GetControlRadius().Unit())),
            Radius(0.0_vp),
            Radius(0.0_vp),
            Radius(Dimension(GetControlRadius().Value() - COUNTER_BORDER_WIDTH, GetControlRadius().Unit()))
        };
        button->SetRectRadii(radii);
        if (propState_) {
            button->SetClickFunction([weak = AceType::WeakClaim(this)] {
                auto spThis = weak.Upgrade();
                if (spThis) {
                    V2::ResumeEventCallback(spThis, &CounterComponent::GetOnDec);
                }
            });
        } else {
            button->SetBackgroundColor(Color::GRAY);
        }
    } else {
        std::array<Radius, 4> radii = {
            Radius(0.0_vp),
            Radius(Dimension(GetControlRadius().Value() - COUNTER_BORDER_WIDTH, GetControlRadius().Unit())),
            Radius(Dimension(GetControlRadius().Value() - COUNTER_BORDER_WIDTH, GetControlRadius().Unit())),
            Radius(0.0_vp)
        };
        button->SetRectRadii(radii);
        if (propState_) {
            button->SetClickFunction([weak = AceType::WeakClaim(this)] {
                auto spThis = weak.Upgrade();
                if (spThis) {
                    V2::ResumeEventCallback(spThis, &CounterComponent::GetOnInc);
                }
            });
        } else {
            button->SetBackgroundColor(Color::GRAY);
        }
    }

    row->AppendChild(button);
}

void CounterComponent::BuildContent(
    const RefPtr<CounterTheme>& counterTheme, const RefPtr<RowComponent>& row, RefPtr<BoxComponent>& box)
{
    box->SetAlignment(Alignment::CENTER);
    if (!GetChildren().empty()) {
        box->SetChild(GetChildren().front());
    }

    if (GreatNotEqual(GetHeight().Value() - COUNTER_RADIUS_DELTA * COUNTER_BORDER_WIDTH, 0.0)) {
        box->SetHeight(
            Dimension(GetHeight().Value() - COUNTER_RADIUS_DELTA * COUNTER_BORDER_WIDTH, GetHeight().Unit()));
    } else {
        box->SetHeight(Dimension(0.0, DimensionUnit::VP));
    }
    if (GreatNotEqual(GetWidth().Value() - COUNTER_ITEM_SIZE_DIFF * GetControlWidth().Value(), 0.0)) {
        box->SetWidth(
            Dimension(GetWidth().Value() - COUNTER_ITEM_SIZE_DIFF * GetControlWidth().Value(), GetWidth().Unit()));
    } else {
        box->SetWidth(Dimension(0.0, DimensionUnit::VP));
    }

    box->SetOverflow(Overflow::FORCE_CLIP);
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(propBackgroundColor_);
    box->SetBackDecoration(backDecoration);
    row->AppendChild(box);
}

} // namespace OHOS::Ace
