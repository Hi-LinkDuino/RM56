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

#include "core/components/popup/popup_component_v2.h"

#include "core/components/align/align_component.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/button/button_theme.h"
#include "core/components/flex/flex_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/popup/popup_element_v2.h"
#include "core/components/text/text_component_v2.h"

namespace OHOS::Ace {
namespace {

constexpr Dimension DEFAULT_FONT_SIZE = 14.0_fp;
constexpr Dimension BUTTON_PADDING = 8.0_fp;
constexpr Dimension BUTTON_ZERO_PADDING = 0.0_fp;

} // namespace

RefPtr<Element> PopupComponentV2::CreateElement()
{
    return AceType::MakeRefPtr<PopupElementV2>(GetId());
}

void PopupComponentV2::Initialization(const RefPtr<ThemeManager>& themeManager, const WeakPtr<PipelineContext>& context)
{
    if (hasInitialization_) {
        return;
    }

    if (!themeManager) {
        LOGE("themeManager is null.");
        return;
    }

    themeManager_ = themeManager;
    context_ = context;
    auto popupTheme = themeManager_->GetTheme<PopupTheme>();
    if (!popupTheme) {
        LOGE("popupTheme is null.");
        return;
    }

    RefPtr<Component> child;
    if (customComponent_) {
        child = customComponent_;
    } else if (primaryButtonProperties_.showButton || secondaryButtonProperties_.showButton) {
        child = CreateChild();
        GetPopupParam()->SetHasAction(true);
    } else {
        child = CreateMessage();
    }

    auto box = AceType::MakeRefPtr<BoxComponent>();
    auto decoration = box->GetBackDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
        box->SetBackDecoration(decoration);
    }
    decoration->SetBorderRadius(popupTheme->GetRadius());
    if (!customComponent_) {
        auto padding = popupTheme->GetPadding();
        box->SetPadding(padding);
        GetPopupParam()->SetPadding(padding);
    }
    box->SetChild(child);
    GetPopupParam()->SetBorder(decoration->GetBorder());
    if (!GetPopupParam()->IsMaskColorSetted()) {
        GetPopupParam()->SetMaskColor(popupTheme->GetMaskColor());
    }
    if (!GetPopupParam()->IsBackgroundColorSetted()) {
        GetPopupParam()->SetBackgroundColor(popupTheme->GetBackgroundColor());
    }
    if (placementOnTop_) {
        GetPopupParam()->SetPlacement(Placement::TOP);
    }
    SetChild(box);
    hasInitialization_ = true;
}

const RefPtr<Component> PopupComponentV2::CreateChild()
{
    RefPtr<ColumnComponent> child;
    std::list<RefPtr<Component>> columnChildren;
    columnChildren.emplace_back(CreateMessage());
    columnChildren.emplace_back(CreateButtons());
    child = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_END, columnChildren);
    child->SetMainAxisSize(MainAxisSize::MIN);
    child->SetCrossAxisSize(CrossAxisSize::MIN);
    return child;
}

const RefPtr<Component> PopupComponentV2::CreateMessage()
{
    auto text = AceType::MakeRefPtr<TextComponentV2>(message_);
    auto textStyle = text->GetTextStyle();
    textStyle.SetAllowScale(false);
    textStyle.SetFontSize(DEFAULT_FONT_SIZE);
    textStyle.SetTextColor(Color::WHITE);
    text->SetTextStyle(std::move(textStyle));
    return text;
}

const RefPtr<Component> PopupComponentV2::CreateButtons()
{
    std::list<RefPtr<Component>> rowChildren;
    rowChildren.emplace_back(SetPadding(CreateButton(primaryButtonProperties_),
        Edge(BUTTON_PADDING, BUTTON_ZERO_PADDING, BUTTON_PADDING, BUTTON_ZERO_PADDING)));
    rowChildren.emplace_back(SetPadding(CreateButton(secondaryButtonProperties_),
        Edge(BUTTON_PADDING, BUTTON_ZERO_PADDING, BUTTON_PADDING, BUTTON_ZERO_PADDING)));
    auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_END, FlexAlign::CENTER, rowChildren);
    row->SetMainAxisSize(MainAxisSize::MIN);
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(row);
    return box;
}

const RefPtr<Component> PopupComponentV2::CreateButton(const ButtonProperties& buttonProperties)
{
    if (!buttonProperties.showButton) {
        return nullptr;
    }

    auto text = AceType::MakeRefPtr<TextComponent>(buttonProperties.value);
    if (!themeManager_) {
        LOGE("themeManager is null.");
        return nullptr;
    }

    auto buttonTheme = themeManager_->GetTheme<ButtonTheme>();
    if (!buttonTheme) {
        LOGE("buttonTheme is null.");
        return nullptr;
    }

    auto textStyle = text->GetTextStyle();
    textStyle.SetAllowScale(false);
    textStyle.SetFontSize(DEFAULT_FONT_SIZE);
    textStyle.SetTextColor(Color::WHITE);
    text->SetTextStyle(std::move(textStyle));
    std::list<RefPtr<Component>> buttonChildren;
    buttonChildren.emplace_back(text);
    auto buttonComponent = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    buttonComponent->SetType(ButtonType::CAPSULE);
    buttonComponent->SetDeclarativeFlag(true);
    buttonComponent->SetHeight(buttonTheme->GetHeight());
    buttonComponent->SetBackgroundColor(Color::TRANSPARENT);
    buttonComponent->SetClickFunction([action = buttonProperties.actionId, context = context_]() {
        auto func = AceAsyncEvent<void()>::Create(action, context);
        if (func) {
            func();
        }
    });
    return buttonComponent;
}

const RefPtr<Component> PopupComponentV2::SetPadding(const RefPtr<Component>& component, const Edge& edge)
{
    auto paddingComponent = AceType::MakeRefPtr<PaddingComponent>();
    paddingComponent->SetPadding(edge);
    paddingComponent->SetChild(component);
    return paddingComponent;
}

} // namespace OHOS::Ace