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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_FLUTTER_RENDER_TOOL_BAR_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_FLUTTER_RENDER_TOOL_BAR_ITEM_H

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/tool_bar/render_tool_bar_item.h"
#include "core/pipeline/layers/clip_layer.h"
#include "core/pipeline/layers/picture_layer.h"

namespace OHOS::Ace {

class FlutterRenderToolBarItem : public RenderToolBarItem {
    DECLARE_ACE_TYPE(FlutterRenderToolBarItem, RenderToolBarItem);

public:
    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void DrawFocus();
    void DrawShape(flutter::Canvas& canvas, const Rect& paintRect, const Color& color, double radius);

    RefPtr<Flutter::ClipLayer> clipLayer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_FLUTTER_RENDER_TOOL_BAR_ITEM_H
