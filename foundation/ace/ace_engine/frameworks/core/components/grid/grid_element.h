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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ELEMENT_H

#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class GridElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(GridElement, ComponentGroupElement, FocusGroup);

public:
    void PerformBuild() override;
    void Update() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    void HandleOnFocus();
    void HandleOnBlur();

private:
    RefPtr<RenderNode> CreateRenderNode() override;
    void ApplyRenderChild(const RefPtr<RenderElement>& renderChild) override;

    void BuildGridData(int32_t index, int32_t count);

    std::list<RefPtr<Component>> newGridItems_;
    using RequestItem = std::function<void(const std::string&, std::string&)>;
    RequestItem requestItem_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_ELEMENT_H
