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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_CONTAINER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_CONTAINER_ELEMENT_H

#include "core/components/shape/shape_container_component.h"
#include "frameworks/core/pipeline/base/component_group_element.h"
#include "frameworks/core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class ShapeContainerElement : public ComponentGroupElement {
    DECLARE_ACE_TYPE(ShapeContainerElement, ComponentGroupElement);

public:
    void PerformBuild() override;

private:
    void UpdateChildStyle();
    void UpdateChildStyle(const RefPtr<ShapeContainerComponent>& shapeContainerComponent,
        const RefPtr<Component>& component);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_SHAPE_CONTAINER_ELEMENT_H
