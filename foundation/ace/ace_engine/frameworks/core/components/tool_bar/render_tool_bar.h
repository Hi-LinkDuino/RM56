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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_H

#include <vector>

#include "core/components/common/layout/grid_layout_info.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components/common/properties/edge.h"
#include "core/components/flex/render_flex.h"
#include "core/components/tool_bar/tool_bar_component.h"

namespace OHOS::Ace {

class RenderToolBar : public RenderNode {
    DECLARE_ACE_TYPE(RenderToolBar, RenderNode)

public:
    RenderToolBar();
    ~RenderToolBar() override = default;
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

protected:
    double GetMaxWidthBasedOnGridType(
        const RefPtr<GridColumnInfo>& info, GridSizeType type) const;

private:
    void LayoutChildren();
    LayoutParam MakeInnerLayoutParam() const;

    int32_t toolBarsSize_ { 0 };
    double toolBarHeight_ { 0.0 };
    mutable double toolBarWidth_ { 0.0 };
    mutable double actualWidth_ { 0.0 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_H
