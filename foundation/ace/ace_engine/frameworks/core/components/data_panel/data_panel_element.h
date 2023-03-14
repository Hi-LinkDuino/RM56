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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_ELEMENT_H

#include "core/components/data_panel/data_panel_component.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class DataPanelElement : public RenderElement {
    DECLARE_ACE_TYPE(DataPanelElement, RenderElement);

public:
    DataPanelElement() = default;
    ~DataPanelElement() override = default;

    void Update() override
    {
        oldComponent_ = component_;
        RenderElement::Update();
    }

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        const auto newDataPanelComponent = AceType::DynamicCast<DataPanelComponent>(newComponent);
        if (!newDataPanelComponent) {
            return false;
        }
        const auto oldDataPanelComponent = AceType::DynamicCast<DataPanelComponent>(oldComponent_);
        if (!oldDataPanelComponent) {
            return false;
        }
        return (newDataPanelComponent->GetPanelType() == oldDataPanelComponent->GetPanelType()) &&
               Element::CanUpdate(newComponent);
    }

private:
    RefPtr<Component> oldComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_ELEMENT_H
