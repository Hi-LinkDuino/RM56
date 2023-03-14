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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_ITEM_H

#include "include/core/SkCanvas.h"

#include "core/components/tool_bar/render_tool_bar_item.h"

namespace OHOS::Ace {
class RosenRenderToolBarItem : public RenderToolBarItem {
    DECLARE_ACE_TYPE(RosenRenderToolBarItem, RenderToolBarItem);

public:
    void Paint(RenderContext& context, const Offset& offset) override;
    void Update(const RefPtr<Component>& component) override;

private:
    void DrawFocus();
    void DrawShape(SkCanvas& canvas, const Rect& paintRect, const Color& color, double radius);
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_ITEM_H
