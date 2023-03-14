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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_REFRESH_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_REFRESH_COMPOSED_ELEMENT_H

#include "core/components/refresh/refresh_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"


namespace OHOS::Ace::V2 {

class ACE_EXPORT RefreshComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(RefreshComposedElement, InspectorComposedElement)

public:
    explicit RefreshComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~RefreshComposedElement() override = default;

    void Dump() override;

    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetRefreshing() const;
    std::string GetRefreshOffset() const;
    std::string GetFriction() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return RefreshElement::TypeId();
    }

    void AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void DeleteChildWithSlot(int32_t slot) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_REFRESH_COMPOSED_ELEMENT_H