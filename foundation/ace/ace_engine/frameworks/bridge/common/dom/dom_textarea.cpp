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

#include "frameworks/bridge/common/dom/dom_textarea.h"

#include <iostream>

#include "frameworks/bridge/common/dom/input/dom_textfield_util.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t TEXTAREA_MAXLENGTH_VALUE_DEFAULT = std::numeric_limits<uint32_t>::max();
constexpr Dimension BOX_HOVER_RADIUS = 18.0_vp;

} // namespace

DOMTextarea::DOMTextarea(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    textAreaChild_ = AceType::MakeRefPtr<TextFieldComponent>();
    textAreaChild_->SetTextInputType(TextInputType::MULTILINE);
    textAreaChild_->SetTextEditController(AceType::MakeRefPtr<TextEditController>());
    textAreaChild_->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());
}

void DOMTextarea::ResetInitializedStyle()
{
    InitializeStyle();
}

void DOMTextarea::InitializeStyle()
{
    auto boxComponent = GetBoxComponent();
    auto component = textAreaChild_;
    auto theme = GetTheme<TextFieldTheme>();
    if (!boxComponent || !component || !theme) {
        return;
    }

    component->SetTextMaxLines(TEXTAREA_MAXLENGTH_VALUE_DEFAULT);
    component->SetCursorColor(theme->GetCursorColor());
    component->SetPlaceholderColor(theme->GetPlaceholderColor());
    component->SetFocusBgColor(theme->GetFocusBgColor());
    component->SetFocusPlaceholderColor(theme->GetFocusPlaceholderColor());
    component->SetFocusTextColor(theme->GetFocusTextColor());
    component->SetBgColor(theme->GetBgColor());
    component->SetTextColor(theme->GetTextColor());
    component->SetSelectedColor(theme->GetSelectedColor());
    component->SetHoverColor(theme->GetHoverColor());
    component->SetPressColor(theme->GetPressColor());
    textStyle_.SetTextColor(theme->GetTextColor());
    textStyle_.SetFontSize(theme->GetFontSize());
    textStyle_.SetFontWeight(theme->GetFontWeight());
    std::vector<std::string> textareaFontFamilyValueDefault = {
        "sans-serif",
    };
    textStyle_.SetFontFamilies(textareaFontFamilyValueDefault);
    component->SetTextStyle(textStyle_);
    component->SetCountTextStyle(theme->GetCountTextStyle());
    component->SetOverCountStyle(theme->GetOverCountStyle());
    component->SetCountTextStyleOuter(theme->GetCountTextStyleOuter());
    component->SetOverCountStyleOuter(theme->GetOverCountStyleOuter());

    component->SetErrorBorderWidth(theme->GetErrorBorderWidth());
    component->SetErrorBorderColor(theme->GetErrorBorderColor());

    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetPadding(theme->GetPadding());
    backDecoration->SetBackgroundColor(theme->GetBgColor());
    defaultRadius_ = theme->GetBorderRadius();
    backDecoration->SetBorderRadius(defaultRadius_);
    if (boxComponent->GetBackDecoration()) {
        backDecoration->SetImage(boxComponent->GetBackDecoration()->GetImage());
        backDecoration->SetGradient(boxComponent->GetBackDecoration()->GetGradient());
    }
    component->SetDecoration(backDecoration);
    component->SetIconSize(theme->GetIconSize());
    component->SetIconHotZoneSize(theme->GetIconHotZoneSize());

    boxComponent->SetBackDecoration(backDecoration);
    boxComponent->SetPadding(theme->GetPadding());
}

bool DOMTextarea::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const DOMTextareaMap textAreaAttrMap = {
        { DOM_AUTO_FOCUS, [](const std::string& val,
                              DOMTextarea& textarea) { textarea.textAreaChild_->SetAutoFocus(StringToBool(val)); } },
        { DOM_TEXTAREA_VALUE,
            [](const std::string& val, DOMTextarea& textarea) { textarea.textAreaChild_->SetValue(val); } },
        { DOM_DISABLED, [](const std::string& val,
                        DOMTextarea& textarea) { textarea.textAreaChild_->SetEnabled(!StringToBool(val)); } },
        { DOM_INPUT_ENTERKEYTYPE,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetAction(ConvertStrToTextInputAction(val));
            } },
        { DOM_TEXTAREA_PLACEHOLDER,
            [](const std::string& val, DOMTextarea& textarea) { textarea.textAreaChild_->SetPlaceholder(val); } },
        { DOM_TEXTAREA_MAXLENGTH,
            [](const std::string& val, DOMTextarea& textarea) {
                int32_t tmp = StringUtils::StringToInt(val);
                textarea.textAreaChild_->SetMaxLength(
                    tmp >= 0 ? (uint32_t)(tmp) : std::numeric_limits<uint32_t>::max());
            } },
        { DOM_TEXTAREA_MAXLINES,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetTextMaxLines(std::max(StringToInt(val), 1));
            } },
        { DOM_TEXTAREA_OBSCURE,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetObscure(StringToBool(val));
            } },
        { DOM_TEXTAREA_EXTEND, [](const std::string& val,
                               DOMTextarea& textarea) { textarea.textAreaChild_->SetExtend(StringToBool(val)); } },
        { DOM_TEXTAREA_SHOW_COUNTER,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetShowCounter(StringToBool(val));
            } },
        { DOM_ICON_SRC,
            [](const std::string& val, DOMTextarea& textarea) { textarea.textAreaChild_->SetIconImage(val); } },
        { DOM_INPUT_SELECTED_START,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetSelectedStart(StringToInt(val));
            } },
        { DOM_INPUT_SELECTED_END,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetSelectedEnd(StringToInt(val)); } },
        { DOM_INPUT_SOFT_KEYBOARD_ENABLED,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetSoftKeyboardEnabled(StringToBool(val));
            } },
    };
    auto textareaAttrIter = textAreaAttrMap.find(attr.first);
    if (textareaAttrIter != textAreaAttrMap.end()) {
        textareaAttrIter->second(attr.second, *this);
        return true;
    }
    return false;
}

bool DOMTextarea::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    if (!textAreaChild_) {
        return false;
    }
    static const DOMTextareaMap textAreaStyleMap = {
        { DOM_BACKGROUND_COLOR,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetBgColor(textarea.ParseColor(val));
                textarea.textAreaChild_->SetFocusBgColor(textarea.ParseColor(val));
            } },
        { DOM_TEXTAREA_COLOR,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textStyle_.SetTextColor(textarea.ParseColor(val));
                textarea.textAreaChild_->SetFocusTextColor(textarea.ParseColor(val));
            } },
        { DOM_TEXTAREA_FONT_SIZE,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textStyle_.SetFontSize(textarea.ParseDimension(val));
            } },
        { DOM_TEXTAREA_FONT_WEIGHT,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textStyle_.SetFontWeight(ConvertStrToFontWeight(val));
            } },
        { DOM_TEXTAREA_PLACEHOLDER_COLOR,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetPlaceholderColor(textarea.ParseColor(val));
                textarea.textAreaChild_->SetFocusPlaceholderColor(textarea.ParseColor(val));
            } },
        { DOM_TEXTAREA_FONT_FAMILY,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textStyle_.SetFontFamilies(textarea.ParseFontFamilies(val));
            } },
        { DOM_TEXT_ALLOW_SCALE, [](const std::string& val,
                                DOMTextarea& textarea) { textarea.textStyle_.SetAllowScale(StringToBool(val)); } },
        { DOM_TEXTAREA_CURSOR_COLOR,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetCursorColor(textarea.ParseColor(val));
            } },
        { DOM_CARET_COLOR,
            [](const std::string& val, DOMTextarea& textarea) {
                textarea.textAreaChild_->SetCursorColor(textarea.ParseColor(val));
            } },
        { DOM_PADDING,
            [](const std::string& val, DOMTextarea& textarea) {
                Edge padding;
                if (Edge::FromString(val, padding)) {
                    textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
                }
            } },
        { DOM_PADDING_LEFT,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                padding.SetLeft(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_RIGHT,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                padding.SetRight(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_TOP,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                padding.SetTop(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_BOTTOM,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                padding.SetBottom(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_START,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                textarea.IsRightToLeft() ? padding.SetRight(textarea.ParseDimension(val))
                                         : padding.SetLeft(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
        { DOM_PADDING_END,
            [](const std::string& val, DOMTextarea& textarea) {
                auto padding = textarea.textAreaChild_->GetDecoration()->GetPadding();
                textarea.IsRightToLeft() ? padding.SetLeft(textarea.ParseDimension(val))
                                         : padding.SetRight(textarea.ParseDimension(val));
                textarea.textAreaChild_->GetDecoration()->SetPadding(padding);
            } },
    };
    auto textareaStyleIter = textAreaStyleMap.find(style.first);
    if (textareaStyleIter != textAreaStyleMap.end()) {
        textareaStyleIter->second(style.second, *this);
        return true;
    }
    hasBoxRadius_ = DOMTextFieldUtil::IsRadiusStyle(style.first);
    return false;
}

bool DOMTextarea::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(const RefPtr<TextFieldComponent>&, const EventMarker&)> eventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](const RefPtr<TextFieldComponent>& component, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(true);
                component->SetOnTap(eventMarker);
            } },
        { DOM_CHANGE, [](const RefPtr<TextFieldComponent>& component,
                      const EventMarker& event) { component->SetOnTextChange(event); } },
        { DOM_CLICK,
            [](const RefPtr<TextFieldComponent>& component, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(false);
                component->SetOnTap(eventMarker);
            } },
        { DOM_LONG_PRESS, [](const RefPtr<TextFieldComponent>& component,
                          const EventMarker& event) { component->SetOnLongPress(event); } },
        { DOM_INPUT_EVENT_OPTION_SELECT, [](const RefPtr<TextFieldComponent>& component,
                                         const EventMarker& event) { component->SetOnOptionsClick(event); } },
        { DOM_INPUT_EVENT_SEARCH, [](const RefPtr<TextFieldComponent>& component,
                                  const EventMarker& event) { component->SetOnSearch(event); } },
        { DOM_INPUT_EVENT_SELECT_CHANGE, [](const RefPtr<TextFieldComponent>& component,
                                             const EventMarker& event) { component->SetOnSelectChange(event); } },
        { DOM_INPUT_EVENT_SHARE, [](const RefPtr<TextFieldComponent>& component,
                                 const EventMarker& event) { component->SetOnShare(event); } },
        { DOM_INPUT_EVENT_TRANSLATE, [](const RefPtr<TextFieldComponent>& component,
                                     const EventMarker& event) { component->SetOnTranslate(event); } },
    };
    auto operatorIter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (operatorIter != -1) {
        eventOperators[operatorIter].value(textAreaChild_, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMTextarea::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == DOM_INPUT_METHOD_DELETE) {
        auto textField = AceType::DynamicCast<TextFieldComponent>(textAreaChild_);
        if (!textField) {
            return;
        }
        auto controller = textField->GetTextFieldController();
        if (!controller) {
            return;
        }
        controller->Delete();
    }
}

void DOMTextarea::OnRequestFocus(bool shouldFocus)
{
    if (!textAreaChild_) {
        return;
    }
    auto textFieldController = textAreaChild_->GetTextFieldController();
    if (!textFieldController) {
        return;
    }
    textFieldController->Focus(shouldFocus);
}

void DOMTextarea::PrepareSpecializedComponent()
{
    RefPtr<BoxComponent> boxComponent = GetBoxComponent();
    if (!boxComponent || !textAreaChild_) {
        return;
    }
    boxComponent_->SetMouseAnimationType(HoverAnimationType::OPACITY);
    textAreaChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    textAreaChild_->SetTextStyle(textStyle_);
    textAreaChild_->SetInputOptions(inputOptions_);
    textAreaChild_->SetImageFill(GetImageFill());
    UpdateDecoration();
    boxComponent->SetPadding(Edge());
    boxComponent->SetDeliverMinToChild(true);
    auto theme = GetTheme<TextFieldTheme>();
    if (boxComponent_->GetHeightDimension().Value() < 0.0 && theme) {
        boxComponent->SetHeight(theme->GetHeight().Value(), theme->GetHeight().Unit());
    }
    textAreaChild_->SetHeight(boxComponent_->GetHeightDimension());
    if (textAreaChild_->IsExtend()) {
        boxComponent_->SetHeight(-1.0, DimensionUnit::PX);
    }
}

void DOMTextarea::UpdateDecoration()
{
    RefPtr<BoxComponent> boxComponent = GetBoxComponent();
    if (!boxComponent || !textAreaChild_) {
        return;
    }
    RefPtr<Decoration> backDecoration = boxComponent->GetBackDecoration();

    // set box border properties to child component
    RefPtr<Decoration> decoration = textAreaChild_->GetDecoration();
    if (backDecoration) {
        Border boxBorder = backDecoration->GetBorder();
        if (decoration) {
            if (hasBoxRadius_) {
                decoration->SetBorder(boxBorder);
            } else {
                Border border = decoration->GetBorder();
                border.SetLeftEdge(boxBorder.Left());
                border.SetRightEdge(boxBorder.Right());
                border.SetTopEdge(boxBorder.Top());
                border.SetBottomEdge(boxBorder.Bottom());
                decoration->SetBorder(border);
            }
            textAreaChild_->SetOriginBorder(decoration->GetBorder());
        }
        // clear box properties
        if (backDecoration->GetImage() || backDecoration->GetGradient().IsValid()) {
            // clear box properties except background image
            backDecoration->SetBackgroundColor(Color::TRANSPARENT);
            Border border;
            if (!hasBoxRadius_) {
                border.SetBorderRadius(defaultRadius_);
            } else {
                border.SetTopLeftRadius(boxBorder.TopLeftRadius());
                border.SetTopRightRadius(boxBorder.TopRightRadius());
                border.SetBottomLeftRadius(boxBorder.BottomLeftRadius());
                border.SetBottomRightRadius(boxBorder.BottomRightRadius());
            }
            backDecoration->SetBorder(border);
        } else {
            backDecoration = AceType::MakeRefPtr<Decoration>();
            backDecoration->SetBorderRadius(Radius(BOX_HOVER_RADIUS));
            boxComponent->SetBackDecoration(backDecoration);
        }
    }
}

} // namespace OHOS::Ace::Framework
