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

#include "core/components/declaration/textarea/textarea_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/dom/input/dom_textfield_util.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/text_field/textfield_theme.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t TEXTAREA_MAXLENGTH_VALUE_DEFAULT = std::numeric_limits<uint32_t>::max();

} // namespace

using namespace Framework;

using TextareaMap = std::unordered_map<std::string, void (*)(const std::string&, TextareaDeclaration&)>;

TextareaDeclaration::TextareaDeclaration()
{
    textFieldDeclaration_ = AceType::MakeRefPtr<TextFieldDeclaration>();
    textFieldDeclaration_->BindPipelineContext(pipelineContext_);
    textFieldDeclaration_->Init();
    textFieldDeclaration_->SetTextInputType(TextInputType::MULTILINE);
    textFieldDeclaration_->SetTextEditController(AceType::MakeRefPtr<TextEditController>());
    textFieldDeclaration_->SetTextFieldController(AceType::MakeRefPtr<TextFieldController>());
}

void TextareaDeclaration::InitializeStyle()
{
    auto theme = GetTheme<TextFieldTheme>();
    if (!theme || !textFieldDeclaration_) {
        return;
    }

    textFieldDeclaration_->SetTextMaxLines(TEXTAREA_MAXLENGTH_VALUE_DEFAULT);
    textFieldDeclaration_->SetCursorColor(theme->GetCursorColor());
    textFieldDeclaration_->SetPlaceholderColor(theme->GetPlaceholderColor());
    textFieldDeclaration_->SetFocusBgColor(theme->GetFocusBgColor());
    textFieldDeclaration_->SetFocusPlaceholderColor(theme->GetFocusPlaceholderColor());
    textFieldDeclaration_->SetFocusTextColor(theme->GetFocusTextColor());
    textFieldDeclaration_->SetBgColor(theme->GetBgColor());
    textFieldDeclaration_->SetTextColor(theme->GetTextColor());
    textFieldDeclaration_->SetSelectedColor(theme->GetSelectedColor());
    textFieldDeclaration_->SetHoverColor(theme->GetHoverColor());
    textFieldDeclaration_->SetPressColor(theme->GetPressColor());
    textFieldDeclaration_->GetTextStyle().SetTextColor(theme->GetTextColor());
    textFieldDeclaration_->GetTextStyle().SetFontSize(theme->GetFontSize());
    textFieldDeclaration_->GetTextStyle().SetFontWeight(theme->GetFontWeight());
    textFieldDeclaration_->GetTextStyle().SetFontFamilies({
        "sans-serif",
    });
    textFieldDeclaration_->SetCountTextStyle(theme->GetCountTextStyle());
    textFieldDeclaration_->SetOverCountStyle(theme->GetOverCountStyle());
    textFieldDeclaration_->SetCountTextStyleOuter(theme->GetCountTextStyleOuter());
    textFieldDeclaration_->SetOverCountStyleOuter(theme->GetOverCountStyleOuter());

    textFieldDeclaration_->SetErrorBorderWidth(theme->GetErrorBorderWidth());
    textFieldDeclaration_->SetErrorBorderColor(theme->GetErrorBorderColor());

    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetPadding(theme->GetPadding());
    backDecoration->SetBackgroundColor(theme->GetBgColor());
    backDecoration->SetBorderRadius(theme->GetBorderRadius());
    textFieldDeclaration_->SetDecoration(backDecoration);
    textFieldDeclaration_->SetIconSize(theme->GetIconSize());
    textFieldDeclaration_->SetIconHotZoneSize(theme->GetIconHotZoneSize());
}

bool TextareaDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return textFieldDeclaration_->SetSpecializedAttr(attr);
}

bool TextareaDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return textFieldDeclaration_->SetSpecializedStyle(style);
}

bool TextareaDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    return textFieldDeclaration_->SetSpecializedEvent(pageId, eventId, event);
}

void TextareaDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    textFieldDeclaration_->CallSpecializedMethod(method, args);
}

void TextareaDeclaration::OnRequestFocus(bool shouldFocus)
{
    textFieldDeclaration_->OnRequestFocus(shouldFocus);
}

} // namespace OHOS::Ace