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

#include "core/components/text_field/text_field_component.h"

#include "core/components/text_field/render_text_field.h"
#include "core/components/text_field/text_field_element.h"

namespace OHOS::Ace {

TextFieldComponent::TextFieldComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<TextFieldDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> TextFieldComponent::CreateRenderNode()
{
    return RenderTextField::Create();
}

RefPtr<Element> TextFieldComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextFieldElement>();
}

const std::string& TextFieldComponent::GetValue() const
{
    return declaration_->GetValue();
}

void TextFieldComponent::SetValue(const std::string& value)
{
    declaration_->SetValue(value);
}

bool TextFieldComponent::IsValueUpdated() const
{
    return declaration_->IsValueUpdated();
}

const std::string& TextFieldComponent::GetPlaceholder() const
{
    return declaration_->GetPlaceholder();
}

void TextFieldComponent::SetPlaceholder(const std::string& placeholder)
{
    declaration_->SetPlaceholder(placeholder);
}

const Color& TextFieldComponent::GetPlaceholderColor() const
{
    return declaration_->GetPlaceholderColor();
}

void TextFieldComponent::SetPlaceholderColor(const Color& placeholderColor)
{
    declaration_->SetPlaceholderColor(placeholderColor);
}

void TextFieldComponent::SetTextMaxLines(uint32_t textMaxLines)
{
    declaration_->SetTextMaxLines(textMaxLines);
}

TextAlign TextFieldComponent::GetTextAlign() const
{
    return declaration_->GetTextAlign();
}

void TextFieldComponent::SetTextAlign(TextAlign textAlign)
{
    declaration_->SetTextAlign(textAlign);
}

uint32_t TextFieldComponent::GetTextMaxLines() const
{
    return declaration_->GetTextMaxLines();
}

const TextStyle& TextFieldComponent::GetTextStyle() const
{
    return declaration_->GetTextStyle();
}

void TextFieldComponent::SetTextStyle(const TextStyle& textStyle)
{
    declaration_->SetTextStyle(textStyle);
}

const TextStyle& TextFieldComponent::GetErrorTextStyle() const
{
    return declaration_->GetErrorTextStyle();
}

void TextFieldComponent::SetErrorTextStyle(const TextStyle& errorTextStyle)
{
    declaration_->SetErrorTextStyle(errorTextStyle);
}

const Dimension& TextFieldComponent::GetErrorSpacing() const
{
    return declaration_->GetErrorSpacing();
}

void TextFieldComponent::SetErrorSpacing(const Dimension& errorSpacing)
{
    declaration_->SetErrorSpacing(errorSpacing);
}

bool TextFieldComponent::GetErrorIsInner() const
{
    return declaration_->GetErrorIsInner();
}

void TextFieldComponent::SetErrorIsInner(bool errorIsInner)
{
    declaration_->SetErrorIsInner(errorIsInner);
}

const Dimension& TextFieldComponent::GetErrorBorderWidth() const
{
    return declaration_->GetErrorBorderWidth();
}

void TextFieldComponent::SetErrorBorderWidth(const Dimension& errorBorderWidth)
{
    declaration_->SetErrorBorderWidth(errorBorderWidth);
}

const Color& TextFieldComponent::GetErrorBorderColor() const
{
    return declaration_->GetErrorBorderColor();
}

void TextFieldComponent::SetErrorBorderColor(const Color& errorBorderColor)
{
    declaration_->SetErrorBorderColor(errorBorderColor);
}

bool TextFieldComponent::NeedFade() const
{
    return declaration_->NeedFade();
}

void TextFieldComponent::SetNeedFade(bool needFade)
{
    declaration_->SetNeedFade(needFade);
}

RefPtr<Decoration> TextFieldComponent::GetDecoration() const
{
    return declaration_->GetDecoration();
}

void TextFieldComponent::SetDecoration(const RefPtr<Decoration>& decoration)
{
    declaration_->SetDecoration(decoration);
}

void TextFieldComponent::SetOriginBorder(const Border& originBorder)
{
    declaration_->SetOriginBorder(originBorder);
}

const Border& TextFieldComponent::GetOriginBorder() const
{
    return declaration_->GetOriginBorder();
}

bool TextFieldComponent::ShowCursor() const
{
    return declaration_->ShowCursor();
}

void TextFieldComponent::SetShowCursor(bool show)
{
    declaration_->SetShowCursor(show);
}

bool TextFieldComponent::NeedObscure() const
{
    return declaration_->NeedObscure();
}

void TextFieldComponent::SetObscure(bool obscure)
{
    declaration_->SetObscure(obscure);
}

bool TextFieldComponent::IsEnabled() const
{
    return declaration_->IsEnabled();
}

void TextFieldComponent::SetEnabled(bool enable)
{
    declaration_->SetEnabled(enable);
}

TextInputType TextFieldComponent::GetTextInputType() const
{
    return declaration_->GetTextInputType();
}

void TextFieldComponent::SetTextInputType(TextInputType type)
{
    declaration_->SetTextInputType(type);
}

TextInputAction TextFieldComponent::GetAction() const
{
    return declaration_->GetAction();
}

void TextFieldComponent::SetAction(TextInputAction action)
{
    declaration_->SetAction(action);
}

void TextFieldComponent::SetCursorColor(const Color& color)
{
    declaration_->SetCursorColor(color);
}

const Color& TextFieldComponent::GetCursorColor()
{
    return declaration_->GetCursorColor();
}

void TextFieldComponent::SetCursorRadius(const Dimension& radius)
{
    declaration_->SetCursorRadius(radius);
}

const Dimension& TextFieldComponent::GetCursorRadius() const
{
    return declaration_->GetCursorRadius();
}

bool TextFieldComponent::IsCursorColorSet() const
{
    return declaration_->IsCursorColorSet();
}

const std::string& TextFieldComponent::GetActionLabel() const
{
    return declaration_->GetActionLabel();
}

void TextFieldComponent::SetActionLabel(const std::string& actionLabel)
{
    declaration_->SetActionLabel(actionLabel);
}

uint32_t TextFieldComponent::GetMaxLength() const
{
    return declaration_->GetMaxLength();
}

void TextFieldComponent::SetMaxLength(uint32_t maxLength)
{
    declaration_->SetMaxLength(maxLength);
}

bool TextFieldComponent::IsTextLengthLimited() const
{
    return declaration_->IsTextLengthLimited();
}

const Dimension& TextFieldComponent::GetHeight() const
{
    return declaration_->GetHeight();
}

void TextFieldComponent::SetHeight(const Dimension& height)
{
    declaration_->SetHeight(height);
}

bool TextFieldComponent::GetAutoFocus() const
{
    return declaration_->GetAutoFocus();
}

void TextFieldComponent::SetAutoFocus(bool autoFocus)
{
    declaration_->SetAutoFocus(autoFocus);
}

bool TextFieldComponent::IsExtend() const
{
    return declaration_->IsExtend();
}

void TextFieldComponent::SetExtend(bool extend)
{
    declaration_->SetExtend(extend);
}

bool TextFieldComponent::ShowEllipsis() const
{
    return declaration_->ShowEllipsis();
}

void TextFieldComponent::SetShowEllipsis(bool showEllipsis)
{
    declaration_->SetShowEllipsis(showEllipsis);
}

bool TextFieldComponent::IsSoftKeyboardEnabled() const
{
    return declaration_->IsSoftKeyboardEnabled();
}

void TextFieldComponent::SetSoftKeyboardEnabled(bool softKeyboardEnabled)
{
    declaration_->SetSoftKeyboardEnabled(softKeyboardEnabled);
}

bool TextFieldComponent::ShowPasswordIcon() const
{
    return declaration_->ShowPasswordIcon();
}

void TextFieldComponent::SetShowPasswordIcon(bool showPasswordIcon)
{
    declaration_->SetShowPasswordIcon(showPasswordIcon);
}

const std::optional<Color>& TextFieldComponent::GetImageFill() const
{
    return declaration_->GetImageFill();
}

void TextFieldComponent::SetImageFill(const std::optional<Color>& color)
{
    declaration_->SetImageFill(color);
}

const std::string& TextFieldComponent::GetIconImage() const
{
    return declaration_->GetIconImage();
}

void TextFieldComponent::SetIconImage(const std::string& iconImage)
{
    declaration_->SetIconImage(iconImage);
}

const std::string& TextFieldComponent::GetShowIconImage() const
{
    return declaration_->GetShowIconImage();
}

void TextFieldComponent::SetShowIconImage(const std::string& showImage)
{
    declaration_->SetShowIconImage(showImage);
}

const std::string& TextFieldComponent::GetHideIconImage() const
{
    return declaration_->GetHideIconImage();
}

void TextFieldComponent::SetHideIconImage(const std::string& hideImage)
{
    declaration_->SetHideIconImage(hideImage);
}

const Dimension& TextFieldComponent::GetIconSize() const
{
    return declaration_->GetIconSize();
}

void TextFieldComponent::SetIconSize(const Dimension& iconSize)
{
    declaration_->SetIconSize(iconSize);
}

const Dimension& TextFieldComponent::GetIconHotZoneSize() const
{
    return declaration_->GetIconHotZoneSize();
}

void TextFieldComponent::SetIconHotZoneSize(const Dimension& iconHotZoneSize)
{
    declaration_->SetIconHotZoneSize(iconHotZoneSize);
}

const EventMarker& TextFieldComponent::GetOnTextChange() const
{
    return declaration_->GetOnTextChange();
}

void TextFieldComponent::SetOnTextChange(const EventMarker& onTextChange)
{
    declaration_->SetOnTextChange(onTextChange);
}

void TextFieldComponent::SetOnTextChangeFunction(std::function<void(const std::string&)>&& onTextChangeCallback)
{
    declaration_->SetOnTextChangeFunction(std::move(onTextChangeCallback));
}

const EventMarker& TextFieldComponent::GetOnSelectChange() const
{
    return declaration_->GetOnSelectChange();
}

void TextFieldComponent::SetOnSelectChange(const EventMarker& onSelectChange)
{
    declaration_->SetOnSelectChange(onSelectChange);
}

const EventMarker& TextFieldComponent::GetOnFinishInput() const
{
    return declaration_->GetOnFinishInput();
}

void TextFieldComponent::SetOnFinishInput(const EventMarker& onFinishInput)
{
    declaration_->SetOnFinishInput(onFinishInput);
}

const EventMarker& TextFieldComponent::GetOnTap() const
{
    return declaration_->GetOnTap();
}

void TextFieldComponent::SetOnTap(const EventMarker& onTap)
{
    declaration_->SetOnTap(onTap);
}

const EventMarker& TextFieldComponent::GetOnLongPress() const
{
    return declaration_->GetOnLongPress();
}

void TextFieldComponent::SetOnLongPress(const EventMarker& onLongPress)
{
    declaration_->SetOnLongPress(onLongPress);
}

RefPtr<TextEditController> TextFieldComponent::GetTextEditController() const
{
    return declaration_->GetTextEditController();
}

void TextFieldComponent::SetTextEditController(const RefPtr<TextEditController>& controller)
{
    declaration_->SetTextEditController(controller);
}

RefPtr<TextFieldController> TextFieldComponent::GetTextFieldController() const
{
    return declaration_->GetTextFieldController();
}

void TextFieldComponent::SetTextFieldController(const RefPtr<TextFieldController>& controller)
{
    declaration_->SetTextFieldController(controller);
}

void TextFieldComponent::SetFocusBgColor(const Color& focusBgColor)
{
    declaration_->SetFocusBgColor(focusBgColor);
}

const Color& TextFieldComponent::GetFocusBgColor()
{
    return declaration_->GetFocusBgColor();
}

void TextFieldComponent::SetFocusPlaceholderColor(const Color& focusPlaceholderColor)
{
    declaration_->SetFocusPlaceholderColor(focusPlaceholderColor);
}

const Color& TextFieldComponent::GetFocusPlaceholderColor()
{
    return declaration_->GetFocusPlaceholderColor();
}

void TextFieldComponent::SetFocusTextColor(const Color& focusTextColor)
{
    declaration_->SetFocusTextColor(focusTextColor);
}

const Color& TextFieldComponent::GetFocusTextColor()
{
    return declaration_->GetFocusTextColor();
}

void TextFieldComponent::SetBgColor(const Color& bgColor)
{
    declaration_->SetBgColor(bgColor);
}

const Color& TextFieldComponent::GetBgColor()
{
    return declaration_->GetBgColor();
}

void TextFieldComponent::SetTextColor(const Color& textColor)
{
    declaration_->SetTextColor(textColor);
}

const Color& TextFieldComponent::GetTextColor()
{
    return declaration_->GetTextColor();
}

void TextFieldComponent::SetWidthReserved(const Dimension& widthReserved)
{
    declaration_->SetWidthReserved(widthReserved);
}

const Dimension& TextFieldComponent::GetWidthReserved() const
{
    return declaration_->GetWidthReserved();
}

const Color& TextFieldComponent::GetSelectedColor() const
{
    return declaration_->GetSelectedColor();
}

void TextFieldComponent::SetSelectedColor(const Color& selectedColor)
{
    declaration_->SetSelectedColor(selectedColor);
}

const Color& TextFieldComponent::GetHoverColor() const
{
    return declaration_->GetHoverColor();
}

void TextFieldComponent::SetHoverColor(const Color& hoverColor)
{
    declaration_->SetHoverColor(hoverColor);
}

const Color& TextFieldComponent::GetPressColor() const
{
    return declaration_->GetPressColor();
}

void TextFieldComponent::SetPressColor(const Color& pressColor)
{
    declaration_->SetPressColor(pressColor);
}

void TextFieldComponent::SetBlockRightShade(bool blockRightShade)
{
    declaration_->SetBlockRightShade(blockRightShade);
}

bool TextFieldComponent::GetBlockRightShade() const
{
    return declaration_->GetBlockRightShade();
}

void TextFieldComponent::SetIsVisible(bool isVisible)
{
    declaration_->SetIsVisible(isVisible);
}

bool TextFieldComponent::IsVisible() const
{
    return declaration_->IsVisible();
}

void TextFieldComponent::SetResetToStart(bool resetToStart)
{
    declaration_->SetResetToStart(resetToStart);
}

bool TextFieldComponent::GetResetToStart() const
{
    return declaration_->GetResetToStart();
}

bool TextFieldComponent::HasSetResetToStart() const
{
    return declaration_->HasSetResetToStart();
}

void TextFieldComponent::SetShowCounter(bool showCounter)
{
    declaration_->SetShowCounter(showCounter);
}

bool TextFieldComponent::ShowCounter() const
{
    return declaration_->ShowCounter();
}

void TextFieldComponent::SetCountTextStyle(const TextStyle& countTextStyle)
{
    declaration_->SetCountTextStyle(countTextStyle);
}

const TextStyle& TextFieldComponent::GetCountTextStyle() const
{
    return declaration_->GetCountTextStyle();
}

void TextFieldComponent::SetOverCountStyle(const TextStyle& overCountStyle)
{
    declaration_->SetOverCountStyle(overCountStyle);
}

const TextStyle& TextFieldComponent::GetOverCountStyle() const
{
    return declaration_->GetOverCountStyle();
}

void TextFieldComponent::SetCountTextStyleOuter(const TextStyle& countTextStyleOuter)
{
    declaration_->SetCountTextStyleOuter(countTextStyleOuter);
}

const TextStyle& TextFieldComponent::GetCountTextStyleOuter() const
{
    return declaration_->GetCountTextStyleOuter();
}

void TextFieldComponent::SetOverCountStyleOuter(const TextStyle& overCountStyleOuter)
{
    declaration_->SetOverCountStyleOuter(overCountStyleOuter);
}

const TextStyle& TextFieldComponent::GetOverCountStyleOuter() const
{
    return declaration_->GetOverCountStyleOuter();
}

const TextSelection& TextFieldComponent::GetSelection() const
{
    return declaration_->GetSelection();
}

void TextFieldComponent::SetSelectedStart(int32_t selectedStart)
{
    declaration_->SetSelectedStart(selectedStart);
}

void TextFieldComponent::SetSelectedEnd(int32_t selectedEnd)
{
    declaration_->SetSelectedEnd(selectedEnd);
}

void TextFieldComponent::SetInputOptions(const std::vector<InputOption>& inputOptions)
{
    declaration_->SetInputOptions(inputOptions);
}

const std::vector<InputOption>& TextFieldComponent::GetInputOptions() const
{
    return declaration_->GetInputOptions();
}

const EventMarker& TextFieldComponent::GetOnOptionsClick() const
{
    return declaration_->GetOnOptionsClick();
}

void TextFieldComponent::SetOnOptionsClick(const EventMarker& onOptionsClick)
{
    declaration_->SetOnOptionsClick(onOptionsClick);
}

const EventMarker& TextFieldComponent::GetOnTranslate() const
{
    return declaration_->GetOnTranslate();
}

void TextFieldComponent::SetOnTranslate(const EventMarker& onTranslate)
{
    declaration_->SetOnTranslate(onTranslate);
}

const EventMarker& TextFieldComponent::GetOnShare() const
{
    return declaration_->GetOnShare();
}

void TextFieldComponent::SetOnShare(const EventMarker& onShare)
{
    declaration_->SetOnShare(onShare);
}

const EventMarker& TextFieldComponent::GetOnSearch() const
{
    return declaration_->GetOnSearch();
}

void TextFieldComponent::SetOnSearch(const EventMarker& onSearch)
{
    declaration_->SetOnSearch(onSearch);
}

void TextFieldComponent::SetPlaceHoldStyle(const TextStyle& style)
{
    declaration_->SetPlaceHoldStyle(style);
}

const TextStyle& TextFieldComponent::GetPlaceHoldStyle() const
{
    return declaration_->GetPlaceHoldStyle();
}

void TextFieldComponent::SetEditingStyle(const TextStyle& style)
{
    declaration_->SetEditingStyle(style);
}

const TextStyle& TextFieldComponent::GetEditingStyle() const
{
    return declaration_->GetEditingStyle();
}

const std::string& TextFieldComponent::GetInputFilter() const
{
    return declaration_->GetInputFilter();
}

void TextFieldComponent::SetInputFilter(const std::string& inputFilter)
{
    declaration_->SetInputFilter(inputFilter);
}

} // namespace OHOS::Ace
