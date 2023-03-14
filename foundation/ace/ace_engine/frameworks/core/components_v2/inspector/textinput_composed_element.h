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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTINPUT_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTINPUT_COMPOSED_ELEMENT_H

#include "core/components/text_field/render_text_field.h"
#include "core/components/text_field/text_field_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT TextInputComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(TextInputComposedElement, InspectorComposedElement)

public:
    explicit TextInputComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~TextInputComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    AceType::IdType GetTargetTypeId() const override
    {
        return TextFieldElement::TypeId();
    }

    std::string GetPlaceholder() const;
    std::string GetText() const;
    std::string GetTextinputType() const;
    std::string GetPlaceholderColor() const;
    std::string GetPlaceholderFont() const;
    std::string GetEnterKeyType() const;
    std::string GetCaretColor() const;
    std::string GetTextInputFontColor() const;
    std::string GetTextInputFontSize() const;
    std::string GetTextInputFontStyle() const;
    std::string GetTextInputFontWeight() const;
    std::string GetTextInputFontFamily() const;
    std::string GetTextMaxLength() const;
    std::string GetTextInputFilter() const;

private:
    RefPtr<RenderTextField> GetRenderTextField() const;
    std::string ConvertFontFamily(const std::vector<std::string>& fontFamily) const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_TEXTINPUT_COMPOSED_ELEMENT_H
