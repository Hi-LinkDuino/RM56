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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ELEMENT_H

#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class GridLayoutElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(GridLayoutElement, ComponentGroupElement, FocusGroup);

public:
    void Update() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    void PerformBuild() override;

private:
    void ApplyRenderChild(const RefPtr<RenderElement>& renderChild) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_GRID_LAYOUT_ELEMENT_H
