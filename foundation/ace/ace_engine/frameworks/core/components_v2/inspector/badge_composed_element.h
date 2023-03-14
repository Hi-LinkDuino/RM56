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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BADGE_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BADGE_COMPOSED_ELEMENT_H

#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"
#include "core/components/badge/badge_element.h"
#include "core/components/badge/render_badge.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT BadgeComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(BadgeComposedElement, InspectorComposedElement)

public:
    explicit BadgeComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~BadgeComposedElement() override = default;

    void Dump(void) override;
    std::unique_ptr<OHOS::Ace::JsonValue> ToJsonObject() const override;
    std::string GetCount(void) const;
    std::string GetMaxCount(void) const;
    std::string GetBadgePosition(void) const;
    std::string GetLabel(void) const;
    std::string GetStyle() const;
    void AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void DeleteChildWithSlot(int32_t slot) override;

private:
    OHOS::Ace::RefPtr<OHOS::Ace::RenderBadge> GetRenderBadge() const;
};

} // namespace OHOS::Ace::V2

#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_BADGE_COMPOSED_ELEMENT_H