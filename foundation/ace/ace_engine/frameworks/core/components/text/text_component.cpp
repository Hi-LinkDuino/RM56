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

#include "core/components/text/text_component.h"

#include "core/components/text/render_text.h"
#include "core/components/text/text_element.h"

namespace OHOS::Ace {

TextComponent::TextComponent(const std::string& data) : ComponentGroup(std::list<RefPtr<Component>>())
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<TextDeclaration>();
        declaration_->Init();
    }
    SetData(data);
}

RefPtr<RenderNode> TextComponent::CreateRenderNode()
{
    return RenderText::Create();
}

RefPtr<Element> TextComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextElement>();
}

uint32_t TextComponent::Compare(const RefPtr<Component>& component) const
{
    auto text = AceType::DynamicCast<TextComponent>(component);
    if (!text) {
        return static_cast<uint32_t>(UpdateRenderType::LAYOUT);
    }
    uint32_t updateType = static_cast<uint32_t>(UpdateRenderType::NONE);
    TextStyle declarationStyle = declaration_->GetTextStyle();
    TextStyle textStyle = text->GetTextStyle();
    updateType |= static_cast<uint32_t>(declarationStyle.GetMaxLines() == textStyle.GetMaxLines() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetLineHeight() == textStyle.GetLineHeight() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetAdaptMinFontSize() == textStyle.GetAdaptMinFontSize() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetAdaptMaxFontSize() == textStyle.GetAdaptMaxFontSize() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetLetterSpacing() == textStyle.GetLetterSpacing() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetBaselineOffset() == textStyle.GetBaselineOffset() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetTextAlign() == textStyle.GetTextAlign() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetTextOverflow() == textStyle.GetTextOverflow() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(textStyle.GetTextColor() == declarationStyle.GetTextColor() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetFontFamilies() == textStyle.GetFontFamilies() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetTextDecoration() == textStyle.GetTextDecoration() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetTextDecorationColor() ==
        textStyle.GetTextDecorationColor() ? UpdateRenderType::NONE : UpdateRenderType::PAINT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetFontStyle() == textStyle.GetFontStyle() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetFontWeight() == textStyle.GetFontWeight() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    updateType |= static_cast<uint32_t>(declarationStyle.GetTextCase() == textStyle.GetTextCase() ?
        UpdateRenderType::NONE : UpdateRenderType::LAYOUT);
    return updateType;
}

const std::string& TextComponent::GetData() const
{
    return declaration_->GetData();
}

void TextComponent::SetData(const std::string& data)
{
    declaration_->SetData(data);
}

const TextStyle& TextComponent::GetTextStyle() const
{
    return declaration_->GetTextStyle();
}

void TextComponent::SetTextStyle(const TextStyle& textStyle)
{
    declaration_->SetTextStyle(textStyle);
}

const Color& TextComponent::GetFocusColor() const
{
    return declaration_->GetFocusColor();
}

void TextComponent::SetFocusColor(const Color& focusColor)
{
    declaration_->SetFocusColor(focusColor);
}

bool TextComponent::GetMaxWidthLayout() const
{
    return declaration_->IsMaxWidthLayout();
}

void TextComponent::SetMaxWidthLayout(bool isMaxWidthLayout)
{
    declaration_->SetIsMaxWidthLayout(isMaxWidthLayout);
}

bool TextComponent::GetAutoMaxLines() const
{
    return declaration_->GetAutoMaxLines();
}

void TextComponent::SetAutoMaxLines(bool autoMaxLines)
{
    declaration_->SetAutoMaxLines(autoMaxLines);
}

bool TextComponent::IsChanged() const
{
    return declaration_->IsChanged();
}

void TextComponent::SetIsChanged(bool isChanged)
{
    declaration_->SetIsChanged(isChanged);
}

void TextComponent::SetOnClick(const EventMarker& onClick)
{
    declaration_->SetClickEvent(onClick);
}

void TextComponent::SetRemoteMessageEvent(const EventMarker& eventId)
{
    declaration_->SetRemoteMessageEvent(eventId);
}

void TextComponent::SetDeclaration(const RefPtr<TextDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

const RefPtr<TextDeclaration>& TextComponent::GetDeclaration() const
{
    return declaration_;
}

Dimension TextComponent::GetDeclarationHeight() const
{
    auto& sizeStyle = static_cast<CommonSizeStyle&>(declaration_->GetStyle(StyleTag::COMMON_SIZE_STYLE));
    if (sizeStyle.IsValid()) {
        return sizeStyle.height;
    }
    return Dimension(-1.0);
}

} // namespace OHOS::Ace
