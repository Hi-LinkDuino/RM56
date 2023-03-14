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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BUTTON_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BUTTON_COMPOSED_ELEMENT_H

#include "core/components/button/render_button.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"
#include "core/components/button/button_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT ButtonComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(ButtonComposedElement, InspectorComposedElement)

public:
    explicit ButtonComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~ButtonComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetButtonType() const;
    std::string GetStateEffect() const;
    std::string GetLabel() const;
    std::string GetFontSize() const;
    std::string GetFontWeight() const;
    std::string GetFontColor() const;
    std::string GetBackgroundColor() const override;
    std::string ConvertButtonTypeToString(ButtonType buttonType) const;

    AceType::IdType GetTargetTypeId() const override
    {
        return ButtonElement::TypeId();
    }
    void AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void DeleteChildWithSlot(int32_t slot) override;

private:
    RefPtr<RenderButton> GetRenderButton() const;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BUTTON_COMPOSED_ELEMENT_H