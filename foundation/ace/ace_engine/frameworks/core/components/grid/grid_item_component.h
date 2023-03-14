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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ITEM_COMPONENT_H

#include "core/components/grid/grid_item_element.h"
#include "core/components/grid/render_grid_item.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class GridItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(GridItemComponent, SoleChildComponent);

public:
    GridItemComponent() = default;
    explicit GridItemComponent(const RefPtr<Component>& child)
        : SoleChildComponent(child) {}
    ~GridItemComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override;

    void SetColumnSpan(int32_t columnSpan);

    int32_t GetColumnSpan() const
    {
        return columnSpan_;
    }

private:
    int32_t columnSpan_ = DEFAULT_COLUMN_SPAN;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ITEM_COMPONENT_H
