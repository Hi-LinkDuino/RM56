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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_GRID_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_GRID_COMPOSED_ELEMENT_H

#include "core/components/common/properties/scroll_bar.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/grid_layout_element.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components_v2/grid/grid_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT GridComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(GridComposedElement, InspectorComposedElement)

public:
    explicit GridComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~GridComposedElement() override = default;

    void Dump() override;

    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetColumnsTemplate() const;
    std::string GetRowsTemplate() const;
    std::string GetColumnsGap() const;
    std::string GetRowsGap() const;
    std::string GetScrollBarWidth() const;
    std::string GetScrollBarColor() const;
    std::string GetScrollBar() const;
    std::string GetGridEditMode() const;
    std::string GetGridMaxCount() const;
    std::string GetGridMinCount() const;
    std::string GetGridCellLength() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return GridLayoutElement::TypeId();
    }

private:
    std::string DisplayModeToString(DisplayMode displayMode) const;
    void AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent) override;
    void DeleteChildWithSlot(int32_t slot) override;

};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_GRID_COMPOSED_ELEMENT_H