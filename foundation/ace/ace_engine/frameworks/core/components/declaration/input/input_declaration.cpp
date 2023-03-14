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

#include "core/components/declaration/input/input_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "core/components/declaration/textfield/textfield_declaration.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/text_field/textfield_theme.h"

namespace OHOS::Ace {
namespace {

// input type
constexpr char INPUT_TYPE_BUTTON[] = "button";
constexpr char INPUT_TYPE_CHECKBOX[] = "checkbox";
constexpr char INPUT_TYPE_RADIO[] = "radio";
constexpr char INPUT_TYPE_TEXT[] = "text";
constexpr char INPUT_TYPE_EMAIL[] = "email";
constexpr char INPUT_TYPE_DATE[] = "date";
constexpr char INPUT_TYPE_TIME[] = "time";
constexpr char INPUT_TYPE_NUMBER[] = "number";
constexpr char INPUT_TYPE_PASSWORD[] = "password";

std::set<std::string> g_textCategory;

// If type is changed between g_textCategory, there is no need to recreate components.
bool ShouldCreateNewComponent(const std::string& oldType, const std::string& newType)
{
    if (g_textCategory.empty()) {
        g_textCategory = std::set<std::string>({ INPUT_TYPE_TEXT, INPUT_TYPE_EMAIL, INPUT_TYPE_DATE, INPUT_TYPE_TIME,
            INPUT_TYPE_NUMBER, INPUT_TYPE_PASSWORD });
    }
    return g_textCategory.find(oldType) == g_textCategory.end() || g_textCategory.find(newType) == g_textCategory.end();
}

} // namespace

using namespace Framework;

void InputDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_INPUT_ATTR);
}

void InputDeclaration::InitializeStyle()
{
    if (specializedDeclaration_) {
        specializedDeclaration_->InitializeStyle();
    }
}

bool InputDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const std::set<std::string> inputCategory { INPUT_TYPE_BUTTON, INPUT_TYPE_CHECKBOX, INPUT_TYPE_RADIO,
        INPUT_TYPE_TEXT, INPUT_TYPE_EMAIL, INPUT_TYPE_DATE, INPUT_TYPE_TIME, INPUT_TYPE_NUMBER, INPUT_TYPE_PASSWORD };
    if (attr.first == DOM_INPUT_TYPE) {
        auto& specializedAttr = MaybeResetAttribute<InputAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (specializedAttr.IsValid()) {
            std::string typeName = attr.second;
            if (typeName.empty() || inputCategory.find(typeName) == inputCategory.end()) {
                typeName = INPUT_TYPE_TEXT;
            }
            specializedAttr.type.second = ShouldCreateNewComponent(specializedAttr.type.first, typeName);
            specializedAttr.type.first = typeName;
        }
        return true;
    }
    inputAttrs_[attr.first] = attr.second;
    return false;
}

bool InputDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    inputStyles_[style.first] = style.second;
    return false;
}

bool InputDeclaration::SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event)
{
    inputEvents_[event] = eventId;
    pageId_ = pageId;
    return false;
}

void InputDeclaration::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (g_textCategory.find(GetType()) != g_textCategory.end()) {
        auto declaration = AceType::DynamicCast<TextFieldDeclaration>(specializedDeclaration_);
        if (declaration) {
            declaration->CallSpecializedMethod(method, args);
        }
    }
}

void InputDeclaration::OnRequestFocus(bool shouldFocus)
{
    if (g_textCategory.find(GetType()) != g_textCategory.end()) {
        auto declaration = AceType::DynamicCast<TextFieldDeclaration>(specializedDeclaration_);
        if (declaration) {
            declaration->OnRequestFocus(shouldFocus);
        }
    }
}

void InputDeclaration::PrepareSpecializedDeclaration()
{
    CreateSpecializedDeclaration();
    auto declaration = AceType::DynamicCast<TextFieldDeclaration>(specializedDeclaration_);
    if (declaration) {
        for (const auto& attr : inputAttrs_) {
            declaration->SetSpecializedAttr(attr);
        }
        for (const auto& style : inputStyles_) {
            declaration->SetSpecializedStyle(style);
        }
        for (const auto& event : inputEvents_) {
            declaration->SetSpecializedEvent(pageId_, event.second, event.first);
        }
    }

    PrepareTextField();
}

void InputDeclaration::CreateSpecializedDeclaration()
{
    std::pair<std::string, bool> type = { INPUT_TYPE_TEXT, true };
    auto& specializedAttr = static_cast<InputAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
    if (specializedAttr.IsValid()) {
        type = specializedAttr.type;
        if (!type.second) {
            LOGD("type of input is not changed.");
            return;
        }
        specializedAttr.type.second = true;
    }

    if (specializedAttr.type.first == INPUT_TYPE_BUTTON) {
    } else if (specializedAttr.type.first == INPUT_TYPE_CHECKBOX) {
    } else if (specializedAttr.type.first == INPUT_TYPE_RADIO) {
    } else {
        specializedDeclaration_ = AceType::MakeRefPtr<TextFieldDeclaration>();
    }
    if (specializedDeclaration_) {
        specializedDeclaration_->BindPipelineContext(pipelineContext_);
        specializedDeclaration_->Init();
        specializedDeclaration_->InitializeStyle();
    }
}

void InputDeclaration::PrepareTextField()
{
    auto textFieldDeclaration = AceType::DynamicCast<TextFieldDeclaration>(specializedDeclaration_);
    if (!textFieldDeclaration) {
        return;
    }

    // Init disable style.
    if (!textFieldDeclaration->IsEnabled()) {
        auto theme = GetTheme<TextFieldTheme>();
        textFieldDeclaration->SetTextColor(theme->GetDisableTextColor());
        textFieldDeclaration->SetPlaceholderColor(theme->GetDisableTextColor());
    }

    // Set height with height of box.
    auto& sizeStyle = static_cast<CommonSizeStyle&>(GetStyle(StyleTag::COMMON_SIZE_STYLE));
    if (sizeStyle.IsValid() && GreatOrEqual(sizeStyle.height.Value(), 0.0)) {
        textFieldDeclaration->SetHeight(sizeStyle.height);
    }
}

} // namespace OHOS::Ace
