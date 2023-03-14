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

#include "core/components/grid_layout/rosen_render_grid_layout.h"

#include "core/pipeline/base/rosen_render_context.h"
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {

void RosenRenderGridLayout::Update(const RefPtr<Component>& component)
{
    RenderGridLayout::Update(component);
    auto rsNode = GetRSNode();
    if (rsNode == nullptr) {
        return;
    }
    rsNode->SetClipToFrame(true);
}

void RosenRenderGridLayout::Paint(RenderContext& context, const Offset& offset)
{
    for (const auto& child : GetChildren()) {
        auto gridLayoutItem = FindChildOfClass<RenderGridLayoutItem>(child);
        if (gridLayoutItem && gridLayoutItem->IsSelected()) {
            PaintItemZone(context, child->GetPaintRect());
        }
        PaintChild(child, context, offset);
    }
    PaintSelectedZone(context);
}

void RosenRenderGridLayout::PaintSelectedZone(RenderContext& context)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (canvas == nullptr) {
        LOGE("skia canvas is null");
        return;
    }

    SkRect skRect = SkRect::MakeXYWH(mouseStartOffset_.GetX(), mouseStartOffset_.GetY(),
        mouseEndOffset_.GetX()-mouseStartOffset_.GetX(), mouseEndOffset_.GetY()-mouseStartOffset_.GetY());

    SkPaint fillGeometry;
    fillGeometry.setAntiAlias(true);
    fillGeometry.setStyle(SkPaint::Style::kFill_Style);
    fillGeometry.setColor(0x1A000000);

    SkPaint strokeGeometry;
    strokeGeometry.setAntiAlias(true);
    strokeGeometry.setStyle(SkPaint::Style::kStroke_Style);
    strokeGeometry.setColor(0x33FFFFFF);
    strokeGeometry.setStrokeWidth(NormalizeToPx(1.0_vp));

    canvas->drawRect(skRect, fillGeometry);
    canvas->drawRect(skRect, strokeGeometry);
}

void RosenRenderGridLayout::PaintItemZone(RenderContext& context, const Rect& paintRect)
{
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (canvas == nullptr) {
        LOGE("skia canvas is null");
        return;
    }

    SkRect skRect = SkRect::MakeXYWH(paintRect.GetOffset().GetX(), paintRect.GetOffset().GetY(),
        paintRect.Width(), paintRect.Height());

    SkPaint fillGeometry;
    fillGeometry.setAntiAlias(true);
    fillGeometry.setStyle(SkPaint::Style::kFill_Style);
    fillGeometry.setColor(0x1A0A59f7);

    canvas->drawRect(skRect, fillGeometry);
}

} // namespace OHOS::Ace
