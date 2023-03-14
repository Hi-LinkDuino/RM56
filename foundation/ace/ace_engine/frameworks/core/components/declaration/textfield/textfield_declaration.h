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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTFIELD_TEXTFIELD_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTFIELD_TEXTFIELD_DECLARATION_H

#include "core/common/ime/text_edit_controller.h"
#include "core/common/ime/text_input_action.h"
#include "core/common/ime/text_input_type.h"
#include "core/common/ime/text_selection.h"
#include "core/components/common/properties/input_option.h"
#include "core/components/declaration/common/declaration.h"
#include "core/components/declaration/input/input_declaration.h"
#include "core/components/text_field/text_field_controller.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/components/text_field/text_field_controller.h"

namespace OHOS::Ace {

struct TextFieldAttribute : Attribute {
    bool isValueUpdated = false;
    std::string value;
    std::string placeholder;
    std::string inputFilter;
    TextSelection selection;
    Dimension widthReserved;
    // Obscure the text, for example, password.
    bool obscure = false;
    // Whether show counter, should work together with maxLength.
    bool showCounter = false;
    // Whether height of textfield can auto-extend.
    bool extend = false;
    bool autoFocus = false;
    bool showEllipsis = false;
    bool blockRightShade = false;
    bool isVisible = true;
    bool resetToStart = true;
    bool hasSetResetToStart = false;
    bool showCursor = true;
    bool enabled = true;
    bool needFade = false;
    bool lengthLimited = false;
    uint32_t maxLength = std::numeric_limits<uint32_t>::max();
    uint32_t textMaxLines = 1;

    bool softKeyboardEnabled = true;
    // Type of input method.
    TextInputType keyboard = TextInputType::TEXT;
    // Action when "enter" pressed.
    TextInputAction action = TextInputAction::UNSPECIFIED;
    std::string actionLabel;

    // Attribute about header-icon of text-field.
    std::string iconImage;
    Dimension iconSize;
    Dimension iconHotZoneSize;
    // Icon to control password show or hide.
    std::string showImage;
    std::string hideImage;
    bool showPasswordIcon = true;

    // Menu options of text overlay
    std::vector<InputOption> inputOptions;
};

struct TextFieldStyle : Style {
    Dimension height;

    // style about cursor
    Dimension cursorRadius;
    Color cursorColor;
    bool cursorColorIsSet = false;

    TextAlign textAlign = TextAlign::START;
    TextStyle textStyle;
    TextStyle countTextStyle;
    TextStyle overCountStyle;
    TextStyle countTextStyleOuter;
    TextStyle overCountStyleOuter;
    TextStyle editingStyle;
    TextStyle placeHoldStyle;

    Color textColor;
    Color focusTextColor;
    Color placeholderColor;
    Color focusPlaceholderColor;
    Color bgColor;
    Color focusBgColor;
    Color selectedColor;
    Color hoverColor;
    Color pressColor;

    // Style about error text
    // Whether error text show inner or under.
    bool errorIsInner = false;
    TextStyle errorTextStyle;
    // Spacing between error text and input text.
    Dimension errorSpacing;
    Dimension errorBorderWidth;
    Color errorBorderColor;
    Border originBorder;

    RefPtr<Decoration> decoration;
};

struct TextFieldEvent : Event {
    // Trigger when text is changed.
    EventMarker onTextChange;
    // Trigger when selection is changed
    EventMarker onSelectChange;
    // Trigger when user press "enter"
    EventMarker onFinishInput;
    // Trigger when user click
    EventMarker onTap;
    // Trigger when user long press
    EventMarker onLongPress;
    // Trigger when user click option of menu
    EventMarker onOptionsClick;
    // Trigger when user click translate button of text overlay
    EventMarker onTranslate;
    // Trigger when user click share button of text overlay
    EventMarker onShare;
    // Trigger when user click search button of text overlay
    EventMarker onSearch;
};

struct TextFieldMethod : Method {
    void Delete(const RefPtr<TextFieldController>& textFieldController) const
    {
        if (textFieldController) {
            textFieldController->Delete();
        }
    }
};

class TextFieldDeclaration : public Declaration {
    DECLARE_ACE_TYPE(TextFieldDeclaration, Declaration);

public:
    TextFieldDeclaration() = default;
    ~TextFieldDeclaration() override = default;

    void InitializeStyle() override;

    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void OnRequestFocus(bool shouldFocus) override;

    const std::string& GetInputFilter() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.inputFilter;
    }

    void SetInputFilter(const std::string& inputFilter)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.inputFilter = inputFilter;
    }

    const std::string& GetValue() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.value;
    }

    void SetValue(const std::string& value)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.value = value;
        attribute.isValueUpdated = true;
    }

    bool IsValueUpdated() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isValueUpdated;
    }

    void SetIsValueUpdated(bool isValueUpdated)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isValueUpdated = isValueUpdated;
    }

    void SetPlaceHoldStyle(const TextStyle& textstyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.placeHoldStyle = textstyle;
    }

    void SetEditingStyle(const TextStyle& textstyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.editingStyle = textstyle;
    }

    const TextStyle& GetPlaceHoldStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.placeHoldStyle;
    }

    const TextStyle& GetEditingStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.editingStyle;
    }

    const std::string& GetPlaceholder() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.placeholder;
    }

    void SetPlaceholder(const std::string& placeholder)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.placeholder = placeholder;
    }

    const TextSelection& GetSelection() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.selection;
    }

    void SetSelectedStart(int32_t selectedStart)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.selection.baseOffset = selectedStart;
    }

    void SetSelectedEnd(int32_t selectedEnd)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.selection.extentOffset = selectedEnd;
    }

    const Color& GetPlaceholderColor() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.placeholderColor;
    }

    void SetPlaceholderColor(const Color& placeholderColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.placeholderColor = placeholderColor;
    }

    void SetTextMaxLines(uint32_t textMaxLines)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.textMaxLines = textMaxLines;
    }

    uint32_t GetTextMaxLines() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.textMaxLines;
    }

    TextAlign GetTextAlign() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textAlign;
    }

    void SetTextAlign(TextAlign textAlign)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textAlign = textAlign;
    }

    TextStyle& GetTextStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textStyle;
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textStyle = textStyle;
    }

    const TextStyle& GetErrorTextStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.errorTextStyle;
    }

    void SetErrorTextStyle(const TextStyle& errorTextStyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.errorTextStyle = errorTextStyle;
    }

    const Dimension& GetErrorSpacing() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.errorSpacing;
    }

    void SetErrorSpacing(const Dimension& errorSpacing)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.errorSpacing = errorSpacing;
    }

    bool GetErrorIsInner() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.errorIsInner;
    }

    void SetErrorIsInner(bool errorIsInner)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.errorIsInner = errorIsInner;
    }

    const Dimension& GetErrorBorderWidth() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.errorBorderWidth;
    }

    void SetErrorBorderWidth(const Dimension& errorBorderWidth)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.errorBorderWidth = errorBorderWidth;
    }

    const Color& GetErrorBorderColor() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.errorBorderColor;
    }

    void SetErrorBorderColor(const Color& errorBorderColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.errorBorderColor = errorBorderColor;
    }

    bool NeedFade() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.needFade;
    }

    void SetNeedFade(bool needFade)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.needFade = needFade;
    }

    bool IsSoftKeyboardEnabled() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.softKeyboardEnabled;
    }

    void SetSoftKeyboardEnabled(bool softKeyboardEnabled)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.softKeyboardEnabled = softKeyboardEnabled;
    }

    RefPtr<Decoration> GetDecoration() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.decoration;
    }

    void SetDecoration(const RefPtr<Decoration>& decoration)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.decoration = decoration;
    }

    void SetOriginBorder(const Border& originBorder)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.originBorder = originBorder;
    }

    const Border& GetOriginBorder() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.originBorder;
    }

    bool ShowCursor() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showCursor;
    }

    void SetShowCursor(bool showCursor)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showCursor = showCursor;
    }

    bool NeedObscure() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.obscure;
    }

    void SetObscure(bool obscure)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.obscure = obscure;
    }

    bool IsEnabled() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.enabled;
    }

    void SetEnabled(bool enable)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.enabled = enable;
    }

    TextInputType GetTextInputType() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.keyboard;
    }

    void SetTextInputType(TextInputType type)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.keyboard = type;
    }

    TextInputAction GetAction() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.action;
    }

    void SetAction(TextInputAction action)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.action = action;
    }

    void SetCursorColor(const Color& color)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.cursorColor = color;
        style.cursorColorIsSet = true;
    }

    const Color& GetCursorColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.cursorColor;
    }

    bool IsCursorColorSet() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.cursorColorIsSet;
    }

    void SetCursorRadius(const Dimension& radius)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.cursorRadius = radius;
    }

    const Dimension& GetCursorRadius() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.cursorRadius;
    }

    const std::string& GetActionLabel() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.actionLabel;
    }

    void SetActionLabel(const std::string& actionLabel)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.actionLabel = actionLabel;
    }

    uint32_t GetMaxLength() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.maxLength;
    }

    void SetMaxLength(uint32_t maxLength)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.maxLength = maxLength;
        attribute.lengthLimited = true;
    }

    bool IsTextLengthLimited() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.lengthLimited;
    }

    const Dimension& GetHeight() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.height;
    }

    void SetHeight(const Dimension& height)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.height = height;
    }

    bool GetAutoFocus() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.autoFocus;
    }

    void SetAutoFocus(bool autoFocus)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.autoFocus = autoFocus;
    }

    bool IsExtend() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.extend;
    }

    void SetExtend(bool extend)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.extend = extend;
    }

    bool ShowEllipsis() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showEllipsis;
    }

    void SetShowEllipsis(bool showEllipsis)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showEllipsis = showEllipsis;
    }

    bool ShowPasswordIcon() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showPasswordIcon;
    }

    void SetShowPasswordIcon(bool showPasswordIcon)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showPasswordIcon = showPasswordIcon;
    }

    const std::optional<Color>& GetImageFill() const
    {
        auto& imageStyle = static_cast<CommonImageStyle&>(GetStyle(StyleTag::COMMON_IMAGE_STYLE));
        return imageStyle.imageFill;
    }

    void SetImageFill(const std::optional<Color>& color)
    {
        auto& imageStyle = MaybeResetStyle<CommonImageStyle>(StyleTag::COMMON_IMAGE_STYLE);
        if (color != std::nullopt) {
            imageStyle.imageFill = color.value();
        }
    }

    const std::string& GetIconImage() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iconImage;
    }

    void SetIconImage(const std::string& iconImage)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconImage = iconImage;
    }

    const std::string& GetShowIconImage() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showImage;
    }

    void SetShowIconImage(const std::string& showImage)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showImage = showImage;
    }

    const std::string& GetHideIconImage() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.hideImage;
    }

    void SetHideIconImage(const std::string& hideImage)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.hideImage = hideImage;
    }

    const Dimension& GetIconSize() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iconSize;
    }

    void SetIconSize(const Dimension& iconSize)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconSize = iconSize;
    }

    const Dimension& GetIconHotZoneSize() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iconHotZoneSize;
    }

    void SetIconHotZoneSize(const Dimension& iconHotZoneSize)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iconHotZoneSize = iconHotZoneSize;
    }

    void SetFocusBgColor(const Color& focusBgColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.focusBgColor = focusBgColor;
    }

    const Color& GetFocusBgColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusBgColor;
    }

    void SetFocusPlaceholderColor(const Color& focusPlaceholderColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.focusPlaceholderColor = focusPlaceholderColor;
    }

    const Color& GetFocusPlaceholderColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusPlaceholderColor;
    }

    void SetFocusTextColor(const Color& focusTextColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.focusTextColor = focusTextColor;
    }

    const Color& GetFocusTextColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.focusTextColor;
    }

    void SetBgColor(const Color& bgColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.bgColor = bgColor;
    }

    const Color& GetBgColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.bgColor;
    }

    void SetTextColor(const Color& textColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textColor = textColor;
    }

    const Color& GetTextColor()
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textColor;
    }

    void SetWidthReserved(const Dimension& widthReserved)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.widthReserved = widthReserved;
    }

    const Dimension& GetWidthReserved() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.widthReserved;
    }

    const Color& GetSelectedColor() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.selectedColor;
    }

    void SetSelectedColor(const Color& selectedColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.selectedColor = selectedColor;
    }

    const Color& GetHoverColor() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.hoverColor;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.hoverColor = hoverColor;
    }

    const Color& GetPressColor() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.pressColor;
    }

    void SetPressColor(const Color& pressColor)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.pressColor = pressColor;
    }

    void SetBlockRightShade(bool blockRightShade)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.blockRightShade = blockRightShade;
    }

    bool GetBlockRightShade() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.blockRightShade;
    }

    void SetIsVisible(bool isVisible)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isVisible = isVisible;
    }

    bool IsVisible() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isVisible;
    }

    void SetResetToStart(bool resetToStart)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.resetToStart = resetToStart;
        attribute.hasSetResetToStart = true;
    }

    bool GetResetToStart() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.resetToStart;
    }

    bool HasSetResetToStart() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.hasSetResetToStart;
    }

    void SetShowCounter(bool showCounter)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.showCounter = showCounter;
    }

    bool ShowCounter() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.showCounter;
    }

    void SetCountTextStyle(const TextStyle& countTextStyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.countTextStyle = countTextStyle;
    }

    const TextStyle& GetCountTextStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.countTextStyle;
    }

    void SetOverCountStyle(const TextStyle& overCountStyle)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.overCountStyle = overCountStyle;
    }

    const TextStyle& GetOverCountStyle() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.overCountStyle;
    }

    void SetCountTextStyleOuter(const TextStyle& countTextStyleOuter)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.countTextStyleOuter = countTextStyleOuter;
    }

    const TextStyle& GetCountTextStyleOuter() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.countTextStyleOuter;
    }

    void SetOverCountStyleOuter(const TextStyle& overCountStyleOuter)
    {
        auto& style = MaybeResetStyle<TextFieldStyle>(StyleTag::SPECIALIZED_STYLE);
        style.overCountStyleOuter = overCountStyleOuter;
    }

    const TextStyle& GetOverCountStyleOuter() const
    {
        auto& style = static_cast<TextFieldStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.overCountStyleOuter;
    }

    void SetInputOptions(const std::vector<InputOption>& inputOptions)
    {
        auto& attribute = MaybeResetAttribute<TextFieldAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.inputOptions = inputOptions;
    }

    const std::vector<InputOption>& GetInputOptions() const
    {
        auto& attribute = static_cast<TextFieldAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.inputOptions;
    }

    const EventMarker& GetOnTextChange() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onTextChange;
    }

    void SetOnTextChange(const EventMarker& onTextChange)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onTextChange = onTextChange;
    }

    void SetOnTextChangeFunction(std::function<void(const std::string&)>&& onTextChangeCallback)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onTextChange.SetUiStrFunction(std::move(onTextChangeCallback));
    }

    const EventMarker& GetOnSelectChange() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onSelectChange;
    }

    void SetOnSelectChange(const EventMarker& onSelectChange)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onSelectChange = onSelectChange;
    }

    const EventMarker& GetOnFinishInput() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onFinishInput;
    }

    void SetOnFinishInput(const EventMarker& onFinishInput)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onFinishInput = onFinishInput;
    }

    const EventMarker& GetOnTap() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onTap;
    }

    void SetOnTap(const EventMarker& onTap)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onTap = onTap;
    }

    const EventMarker& GetOnLongPress() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onLongPress;
    }

    void SetOnLongPress(const EventMarker& onLongPress)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onLongPress = onLongPress;
    }

    const EventMarker& GetOnOptionsClick() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onOptionsClick;
    }

    void SetOnOptionsClick(const EventMarker& onOptionsClick)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onOptionsClick = onOptionsClick;
    }

    const EventMarker& GetOnTranslate() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onTranslate;
    }

    void SetOnTranslate(const EventMarker& onTranslate)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onTranslate = onTranslate;
    }

    const EventMarker& GetOnShare() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onShare;
    }

    void SetOnShare(const EventMarker& onShare)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onShare = onShare;
    }

    const EventMarker& GetOnSearch() const
    {
        auto& event = static_cast<TextFieldEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.onSearch;
    }

    void SetOnSearch(const EventMarker& onSearch)
    {
        auto& event = MaybeResetEvent<TextFieldEvent>(EventTag::SPECIALIZED_EVENT);
        event.onSearch = onSearch;
    }

    const RefPtr<TextEditController>& GetTextEditController() const
    {
        return textEditController_;
    }

    void SetTextEditController(const RefPtr<TextEditController>& controller)
    {
        textEditController_ = controller;
    }

    const RefPtr<TextFieldController>& GetTextFieldController() const
    {
        return textFieldController_;
    }

    void SetTextFieldController(const RefPtr<TextFieldController>& controller)
    {
        textFieldController_ = controller;
    }

    bool HasBoxRadius() const
    {
        return hasBoxRadius_;
    }

protected:
    void InitSpecialized() override;

private:
    bool hasBoxRadius_ = false;
    RefPtr<TextEditController> textEditController_;
    RefPtr<TextFieldController> textFieldController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTFIELD_TEXTFIELD_DECLARATION_H