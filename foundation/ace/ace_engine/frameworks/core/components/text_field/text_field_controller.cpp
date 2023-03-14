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

#include "core/components/text_field/text_field_controller.h"

#include "core/components/text_field/text_field_element.h"

namespace OHOS::Ace {

void TextFieldController::Focus(bool focus)
{
    auto textField = AceType::DynamicCast<TextFieldElement>(element_.Upgrade());
    if (!textField) {
        return;
    }
    if (focus) {
        textField->RequestKeyboard(true);
    } else {
        textField->CloseKeyboard(true);
        textField->LostSelfFocus();
    }
}

void TextFieldController::ShowError(const std::string& errorText)
{
    auto textField = AceType::DynamicCast<TextFieldElement>(element_.Upgrade());
    if (!textField) {
        return;
    }
    textField->ShowError(errorText);
}

void TextFieldController::Delete()
{
    auto textField = AceType::DynamicCast<TextFieldElement>(element_.Upgrade());
    if (!textField) {
        return;
    }
    textField->Delete();
}

void TextFieldController::CaretPosition(int32_t caretPosition)
{
    if (caretPosition < 0) {
        LOGW("Input caretPosition is not valid.");
        return;
    }

    if (setCaretPosition_) {
        setCaretPosition_(caretPosition);
    }
}

void TextFieldController::Insert(const std::string& args)
{
    auto textField = AceType::DynamicCast<TextFieldElement>(element_.Upgrade());
    if (!textField) {
        return;
    }
    textField->Insert(args);
}


}  // namespace OHOS::Ace
