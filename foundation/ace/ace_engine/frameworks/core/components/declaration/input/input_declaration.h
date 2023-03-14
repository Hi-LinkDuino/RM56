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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_INPUT_INPUT_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_INPUT_INPUT_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct InputAttribute : Attribute {
    std::pair<std::string, bool> type = { "text", true };
};

class InputDeclaration : public Declaration {
    DECLARE_ACE_TYPE(InputDeclaration, Declaration);

public:
    InputDeclaration() = default;
    ~InputDeclaration() override = default;

    void InitializeStyle() override;

    void PrepareSpecializedDeclaration();

    const std::string& GetType() const
    {
        static std::string defaultType = "text";
        auto& attribute = static_cast<InputAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.IsValid() ? attribute.type.first : defaultType;
    }

    bool IsTypeChanged() const
    {
        auto& attribute = static_cast<InputAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.IsValid() ? attribute.type.second : true;
    }

    void SetIsTypeChanged(bool isTypeChanged) const
    {
        auto& attribute = static_cast<InputAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        attribute.type.second = true;
    }

    const RefPtr<Declaration>& GetSpecializedDeclaration() const
    {
        return specializedDeclaration_;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;
    void OnRequestFocus(bool shouldFocus) override;

private:
    void CreateSpecializedDeclaration();
    void PrepareTextField();

    RefPtr<Declaration> specializedDeclaration_;
    std::map<std::string, std::string> inputAttrs_;
    std::map<std::string, std::string> inputStyles_;
    std::map<std::string, std::string> inputEvents_;
    int32_t pageId_ = -1; // invalid id.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_INPUT_INPUT_DECLARATION_H
