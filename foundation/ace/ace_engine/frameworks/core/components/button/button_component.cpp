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

#include "core/components/button/button_component.h"

#include "core/components/button/button_element.h"
#include "core/components/button/button_theme.h"
#include "core/components/button/render_button.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace {

ButtonComponent::ButtonComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<ButtonDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> ButtonComponent::CreateRenderNode()
{
    return RenderButton::Create();
}

RefPtr<Element> ButtonComponent::CreateElement()
{
    return AceType::MakeRefPtr<ButtonElement>();
}

RefPtr<ButtonComponent> ButtonBuilder::Build(const RefPtr<ThemeManager>& themeManager, const std::string& text)
{
    auto buttonTheme = AceType::DynamicCast<ButtonTheme>(themeManager->GetTheme(ButtonTheme::TypeId()));
    if (!buttonTheme) {
        TextStyle defaultStyle;
        return ButtonBuilder::Build(themeManager, text, defaultStyle);
    }
    TextStyle textStyle = buttonTheme->GetTextStyle();
    textStyle.SetAdaptTextSize(buttonTheme->GetMaxFontSize(), buttonTheme->GetMinFontSize());
    textStyle.SetMaxLines(buttonTheme->GetTextMaxLines());
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    return ButtonBuilder::Build(themeManager, text, textStyle);
}

RefPtr<ButtonComponent> ButtonBuilder::Build(const RefPtr<ThemeManager>& themeManager, const std::string& text,
    TextStyle& textStyle, const Color& textFocusColor, bool useTextFocus)
{
    auto textComponent = AceType::MakeRefPtr<TextComponent>(text);
    auto padding = AceType::MakeRefPtr<PaddingComponent>();
    padding->SetChild(textComponent);
    Component::MergeRSNode(padding, textComponent);
    std::list<RefPtr<Component>> buttonChildren;
    buttonChildren.emplace_back(padding);
    auto buttonComponent = AceType::MakeRefPtr<ButtonComponent>(buttonChildren);
    auto buttonTheme = AceType::DynamicCast<ButtonTheme>(themeManager->GetTheme(ButtonTheme::TypeId()));
    if (!buttonTheme) {
        return buttonComponent;
    }
    if (useTextFocus) {
        textComponent->SetFocusColor(textFocusColor);
    } else {
        textComponent->SetFocusColor(buttonTheme->GetTextFocusColor());
    }
    textComponent->SetTextStyle(textStyle);
    padding->SetPadding(buttonTheme->GetPadding());
    buttonComponent->SetHeight(buttonTheme->GetHeight());
    buttonComponent->SetRectRadius(buttonTheme->GetHeight() / 2.0);
    buttonComponent->SetBackgroundColor(buttonTheme->GetBgColor());
    buttonComponent->SetClickedColor(buttonTheme->GetClickedColor());
    buttonComponent->SetFocusColor(buttonTheme->GetBgFocusColor());
    buttonComponent->SetFocusAnimationColor(buttonTheme->GetBgFocusColor());
    return buttonComponent;
}

bool ButtonComponent::GetDisabledState() const
{
    return declaration_->GetDisabledState();
}

bool ButtonComponent::GetWaitingState() const
{
    return declaration_->GetWaitingState();
}

bool ButtonComponent::GetAutoFocusState() const
{
    return declaration_->GetAutoFocusState();
}

bool ButtonComponent::GetRadiusState() const
{
    return declaration_->GetRadiusState();
}

bool ButtonComponent::GetCatchMode() const
{
    return isCatchMode_;
}

const Dimension& ButtonComponent::GetMinWidth() const
{
    return declaration_->GetMinWidth();
}

const Dimension& ButtonComponent::GetRectRadius() const
{
    return declaration_->GetRectRadius();
}

const Dimension& ButtonComponent::GetProgressDiameter() const
{
    return declaration_->GetProgressDiameter();
}

const Color& ButtonComponent::GetBackgroundColor() const
{
    return declaration_->GetBackgroundColor();
}

const Color& ButtonComponent::GetClickedColor() const
{
    return declaration_->GetClickedColor();
}

const Color& ButtonComponent::GetDisabledColor() const
{
    return declaration_->GetDisabledColor();
}

const Color& ButtonComponent::GetFocusColor() const
{
    return declaration_->GetFocusColor();
}

const Color& ButtonComponent::GetHoverColor() const
{
    return declaration_->GetHoverColor();
}

const Color& ButtonComponent::GetProgressColor() const
{
    return declaration_->GetProgressColor();
}

const Color& ButtonComponent::GetProgressFocusColor() const
{
    return declaration_->GetProgressFocusColor();
}

const Color& ButtonComponent::GetFocusAnimationColor() const
{
    return declaration_->GetFocusAnimationColor();
}

const BorderEdge& ButtonComponent::GetBorderEdge() const
{
    return declaration_->GetBorderEdge();
}

const EventMarker& ButtonComponent::GetClickedEventId() const
{
    return declaration_->GetClickedEventId();
}

const EventMarker& ButtonComponent::GetKeyEnterEventId() const
{
    return keyEnterId_;
}

const EventMarker& ButtonComponent::GetRemoteMessageEventId() const
{
    return declaration_->GetRemoteMessageEventId();
}

RefPtr<ButtonProgressController> ButtonComponent::GetButtonController() const
{
    return declaration_->GetButtonController();
}

void ButtonComponent::SetDisabledState(bool state)
{
    declaration_->SetDisabledState(state);
}

void ButtonComponent::SetWaitingState(bool state)
{
    declaration_->SetWaitingState(state);
}

void ButtonComponent::SetAutoFocusState(bool state)
{
    declaration_->SetAutoFocusState(state);
}

void ButtonComponent::SetRadiusState(bool state)
{
    declaration_->SetRadiusState(state);
}

void ButtonComponent::SetCatchMode(bool catchMode)
{
    isCatchMode_ = catchMode;
}

void ButtonComponent::SetMinWidth(const Dimension& width)
{
    declaration_->SetMinWidth(width);
}

void ButtonComponent::SetRectRadius(const Dimension& radius)
{
    declaration_->SetRectRadius(radius);
}

void ButtonComponent::SetProgressDiameter(const Dimension& diameter)
{
    declaration_->SetProgressDiameter(diameter);
}

void ButtonComponent::SetBackgroundColor(const Color& color)
{
    declaration_->SetBackgroundColor(color);

}

void ButtonComponent::SetClickedColor(const Color& color)
{
    declaration_->SetClickedColor(color);
}

void ButtonComponent::SetDisabledColor(const Color& color)
{
    declaration_->SetDisabledColor(color);
}

void ButtonComponent::SetFocusColor(const Color& color)
{
    declaration_->SetFocusColor(color);
}

void ButtonComponent::SetHoverColor(const Color& color)
{
    declaration_->SetHoverColor(color);
}

void ButtonComponent::SetProgressColor(const Color& color)
{
    declaration_->SetProgressColor(color);
}

void ButtonComponent::SetProgressFocusColor(const Color& color)
{
    declaration_->SetProgressFocusColor(color);
}

void ButtonComponent::SetFocusAnimationColor(const Color& color)
{
    declaration_->SetFocusAnimationColor(color);
}

void ButtonComponent::SetBorderEdge(const BorderEdge& borderEdge)
{
    declaration_->SetBorderEdge(borderEdge);
}

void ButtonComponent::SetClickedEventId(const EventMarker& eventId)
{
    declaration_->SetClickedEventId(eventId);
}

void ButtonComponent::SetKeyEnterEventId(const EventMarker& eventId)
{
    keyEnterId_ = eventId;
}

void ButtonComponent::SetRemoteMessageEventId(const EventMarker& eventId)
{
    declaration_->SetRemoteMessageEventId(eventId);
}

void ButtonComponent::SetClickFunction(std::function<void()>&& clickCallback)
{
    declaration_->SetClickedFunction(std::move(clickCallback));
}

void ButtonComponent::SetDeclaration(const RefPtr<ButtonDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

void ButtonComponent::FitTextHeight(AnimatableDimension& height)
{
    if (isDeclareHeight_) {
        return;
    }
    auto padding = AceType::DynamicCast<PaddingComponent>(GetChildren().front());
    if (padding == nullptr) {
        LOGW("Padding component get failed");
        return;
    }
    auto text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    if (text == nullptr) {
        LOGW("Text component get failed");
        return;
    }
    auto fontSize = text->GetTextStyle().GetFontSize();
    if (height.Value() < fontSize.Value()) {
        height = fontSize;
    }
}

uint32_t ButtonComponent::Compare(const RefPtr<Component>& component) const
{
    auto button = AceType::DynamicCast<ButtonComponent>(component);
    if (!button) {
        return static_cast<uint32_t>(UpdateRenderType::LAYOUT);
    }
    uint32_t updateType = static_cast<uint32_t>(UpdateRenderType::NONE);
    updateType |= static_cast<uint32_t>(button->GetWidth() == width_ ? UpdateRenderType::NONE :
        UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(button->GetHeight() == height_ ? UpdateRenderType::NONE :
        UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(button->GetMinWidth() == declaration_->GetMinWidth() ? UpdateRenderType::NONE :
        UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(button->GetType() == type_ ? UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(button->GetRadiusState() == declaration_->GetRadiusState() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(button->GetStateEffect() == stateEffect_ ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    if (updateType == static_cast<uint32_t>(UpdateRenderType::LAYOUT)) {
        return updateType;
    }
    updateType |= static_cast<uint32_t>(button->GetBackgroundColor() == declaration_->GetBackgroundColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetClickedColor() == declaration_->GetClickedColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetDisabledColor() == declaration_->GetDisabledColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetFocusColor() == declaration_->GetFocusColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetHoverColor() == declaration_->GetHoverColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetProgressColor() == declaration_->GetProgressColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetProgressFocusColor() == declaration_->GetProgressFocusColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(button->GetFocusAnimationColor() == declaration_->GetFocusAnimationColor() ?
        UpdateRenderType::NONE : UpdateRenderType::PAINT);
    return updateType;
}

} // namespace OHOS::Ace
