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

#include "core/components/shape/shape_container_element.h"

#include "core/components/shape/shape_component.h"
#include "core/components_v2/inspector/inspector_composed_component.h"
#include "core/pipeline/base/multi_composed_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

void ShapeContainerElement::PerformBuild()
{
    UpdateChildStyle();
    ComponentGroupElement::PerformBuild();
}

void ShapeContainerElement::UpdateChildStyle()
{
    auto shapeContainerComponent = AceType::DynamicCast<ShapeContainerComponent>(component_);
    if (!shapeContainerComponent) {
        LOGE("shapeContainerComponent is null");
        return;
    }
    const auto& children = shapeContainerComponent->GetChildren();
    for (const auto& child : children) {
        UpdateChildStyle(shapeContainerComponent, child);
    }
}

void ShapeContainerElement::UpdateChildStyle(const RefPtr<ShapeContainerComponent>& selfComponent,
    const RefPtr<Component>& component)
{
    auto child = component;
    auto inspectorComposedComponent = AceType::DynamicCast<V2::InspectorComposedComponent>(child);
    if (inspectorComposedComponent) {
        child = inspectorComposedComponent->GetChild();
    }
    while (child) {
        auto shapeComponent = AceType::DynamicCast<ShapeComponent>(child);
        if (shapeComponent) {
            shapeComponent->InheritShapeStyle(selfComponent->GetFillState(), selfComponent->GetStrokeState(),
                selfComponent->GetAntiAlias().second);
            break;
        }
        auto shapeContainerComponent = AceType::DynamicCast<ShapeContainerComponent>(child);
        if (shapeContainerComponent) {
            shapeContainerComponent->InheritShapeStyle(selfComponent->GetFillState(), selfComponent->GetStrokeState(),
                selfComponent->GetAntiAlias().second);
            break;
        }
        auto soleChildComponent = AceType::DynamicCast<SoleChildComponent>(child);
        if (soleChildComponent) {
            child = soleChildComponent->GetChild();
            continue;
        }

        auto multiComposedComponent = AceType::DynamicCast<MultiComposedComponent>(child);
        if (multiComposedComponent) {
            auto children = multiComposedComponent->GetChildren();
            for (const auto& childComponent : children) {
                UpdateChildStyle(selfComponent, childComponent);
            }
        }
        break;
    }
}

} // namespace OHOS::Ace