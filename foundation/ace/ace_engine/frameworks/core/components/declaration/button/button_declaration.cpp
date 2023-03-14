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

#include "frameworks/core/components/declaration/button/button_declaration.h"

#include "base/log/event_report.h"
#include "core/common/ace_application_info.h"
#include "core/components/button/button_theme.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t METHOD_SET_PROGRESS_ARGS_SIZE = 1;

} // namespace

using namespace Framework;

void ButtonDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_BUTTON_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_BUTTON_STYLE);
    AddSpecializedEvent(DeclarationConstants::DEFAULT_BUTTON_EVENT);
    AddSpecializedRemoteMessageEvent(DeclarationConstants::DEFAULT_BUTTON_EVENT);
    AddSpecializedMethod(DeclarationConstants::DEFAULT_BUTTON_METHOD);
}

void ButtonDeclaration::InitializeStyle()
{
    buttonController_ = AceType::MakeRefPtr<ButtonProgressController>();
    auto& style = MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
    if (!style.IsValid()) {
        return;
    }
    RefPtr<ButtonTheme> buttonTheme = GetTheme<ButtonTheme>();
    if (!buttonTheme) {
        return;
    }
    style.padding = buttonTheme->GetPadding();
    style.blendOpacity = buttonTheme->GetBgDisabledAlpha();
    style.borderEdge.SetColor(buttonTheme->GetDownloadBorderColor());
    style.progressColor = buttonTheme->GetProgressColor();
    style.diameter = buttonTheme->GetProgressDiameter();
    style.innerLeftPadding = buttonTheme->GetInnerPadding();
    style.minWidth = buttonTheme->GetMinWidth();
    style.radius = buttonTheme->GetRadius();
    style.backgroundColor = buttonTheme->GetBgColor();
    style.clickedColor = buttonTheme->GetClickedColor();
    style.focusColor = buttonTheme->GetBgFocusColor();
    style.hoverColor = buttonTheme->GetHoverColor();
    style.focusAnimationColor = buttonTheme->GetBgFocusColor();
    style.progressFocusColor = buttonTheme->GetProgressFocusColor();
    style.textFocusColor = buttonTheme->GetTextFocusColor();
    style.textStyle = buttonTheme->GetTextStyle();
    style.textStyle.SetAdaptTextSize(style.textStyle.GetFontSize(), buttonTheme->GetMinFontSize());
    style.textStyle.SetMaxLines(buttonTheme->GetTextMaxLines());
    style.textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    style.textStyle.SetTextAlign(TextAlign::LEFT);
}

bool ButtonDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(ButtonDeclaration&, const std::string&)> buttonAttrOperators[] = {
        { DOM_BUTTON_AUTO_FOCUS, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.isAutoFocus = StringToBool(value);
        } },
        { DOM_DISABLED, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.isDisabled = StringToBool(value);
        } },
        { DOM_BUTTON_ICON, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.iconSrc = value;
        } },
        { DOM_PLACEMENT, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.placement = value;
        } },
        { DOM_BUTTON_TYPE, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.buttonType = value;
        } },
        { DOM_BUTTON_TEXT_DATA, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.textData = value;
        } },
        { DOM_BUTTON_WAITING, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonAttr = button.MaybeResetAttribute<ButtonAttribute>(AttributeTag::SPECIALIZED_ATTR);
            buttonAttr.isWaiting = StringToBool(value);
        } },
    };
    auto operatorIter = BinarySearchFindIndex(buttonAttrOperators, ArraySize(buttonAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        LOGD("Button attrs : %{public}s = %{public}s", attr.first.c_str(), attr.second.c_str());
        buttonAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool ButtonDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // Static linear map should be sorted by key.
    const static LinearMapNode<void (*)(ButtonDeclaration&, const std::string&)> buttonStyleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetAllowScale(StringToBool(value));
        } },
        { DOM_BUTTON_DEFAULT_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.backgroundColor = button.ParseColor(value);
            buttonStyle.bgColorDefined = true;
        } },
        { DOM_BUTTON_BORDER_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.borderEdge.SetColor(button.ParseColor(value));
        } },
        { DOM_BUTTON_BORDER_WIDTH, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.borderEdge.SetWidth(button.ParseDimension(value));
        } },
        { DOM_BUTTON_ICON_DIRECTION, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.matchTextDirection = StringToBool(value);
        } },
        { DOM_BUTTON_CLICKED_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.clickedColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_PROGRESS_DIAMETER, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.diameter = button.ParseDimension(value);
        } },
        { DOM_BUTTON_DISABLE_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.disabledColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_TEXT_DISABLE_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.disabledTextColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_FOCUS_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.focusColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_FONT_FAMILY, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetFontFamilies(button.ParseFontFamilies(value));
        } },
        { DOM_BUTTON_FONT_SIZE, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetFontSize(button.ParseDimension(value));
            buttonStyle.fontSizeDefined = true;
        } },
        { DOM_BUTTON_FONT_STYLE, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetFontStyle(ConvertStrToFontStyle(value));
        } },
        { DOM_BUTTON_FONT_WEIGHT, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetFontWeight(ConvertStrToFontWeight(value));
        } },
        { DOM_BUTTON_HEIGHT, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.height = button.ParseDimension(value);
            buttonStyle.heightDefined = true;
        } },
        { DOM_BUTTON_ICON_HEIGHT, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.iconHeight = button.ParseDimension(value);
        } },
        { DOM_BUTTON_ICON_WIDTH, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.iconWidth = button.ParseDimension(value);
        } },
        { DOM_BUTTON_INNER_PADDING, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.innerLeftPadding = button.ParseDimension(value);
        } },
        { DOM_BUTTON_MIN_WIDTH, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.minWidth = button.ParseDimension(value);
        } },
        { DOM_BUTTON_PROGRESS_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.progressColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_PROGRESS_FOCUS_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.progressFocusColor = button.ParseColor(value);
        } },
        { DOM_BUTTON_RRECT_RADIUS, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.radius = button.ParseDimension(value);
            buttonStyle.radiusDefined = true;
        } },
        { DOM_BUTTON_TEXT_COLOR, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.textStyle.SetTextColor(button.ParseColor(value));
            buttonStyle.textColorDefined = true;
        } },
        { DOM_BUTTON_WIDTH, [](ButtonDeclaration& button, const std::string& value) {
            auto& buttonStyle = button.MaybeResetStyle<ButtonStyle>(StyleTag::SPECIALIZED_STYLE);
            buttonStyle.width = button.ParseDimension(value);
        } },
    };
    auto operatorIter =
        BinarySearchFindIndex(buttonStyleOperators, ArraySize(buttonStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        LOGD("Button styles : %{public}s = %{public}s", style.first.c_str(), style.second.c_str());
        buttonStyleOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

bool ButtonDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    if (event == DOM_CLICK) {
        EventMarker eventMarker(eventId, event, pageId);
        eventMarker.SetCatchMode(false);
        SetClickedEventId(eventMarker);
        return true;
    } else if (event == DOM_CATCH_BUBBLE_CLICK) {
        EventMarker eventMarker(eventId, event, pageId);
        eventMarker.SetCatchMode(true);
        SetClickedEventId(eventMarker);
        return true;
    }
    return false;
}

void ButtonDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (!buttonController_) {
        EventReport::SendComponentException(ComponentExcepType::BUTTON_COMPONENT_ERR);
        return;
    }
    if (method == DOM_BUTTON_METHOD_SET_PROGRESS) {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        if ((!argsValue) || (!argsValue->IsArray()) || (argsValue->GetArraySize() != METHOD_SET_PROGRESS_ARGS_SIZE)) {
            return;
        }
        std::unique_ptr<JsonValue> progressValue = argsValue->GetArrayItem(0)->GetValue("progress");
        if ((!progressValue) || (!progressValue->IsNumber())) {
            return;
        }
        uint32_t progress = progressValue->GetUInt();
        buttonController_->SetProgress(progress);
    }
}

} // namespace OHOS::Ace
