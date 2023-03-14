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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTAREA_TEXTAREA_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTAREA_TEXTAREA_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "core/components/declaration/textfield/textfield_declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/components/text_field/text_field_controller.h"

namespace OHOS::Ace {

class TextareaDeclaration : public Declaration {
    DECLARE_ACE_TYPE(TextareaDeclaration, Declaration);

public:
    TextareaDeclaration();
    ~TextareaDeclaration() override = default;

    void InitializeStyle() override;

    bool HasBoxRadius() const
    {
        return textFieldDeclaration_->HasBoxRadius();
    }

    const RefPtr<TextFieldDeclaration>& GetTextFieldDeclaration() const
    {
        return textFieldDeclaration_;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void OnRequestFocus(bool shouldFocus) override;

private:
    RefPtr<TextFieldDeclaration> textFieldDeclaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTAREA_TEXTAREA_DECLARATION_H
