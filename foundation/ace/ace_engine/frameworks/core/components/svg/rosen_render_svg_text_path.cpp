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

#include "frameworks/core/components/svg/rosen_render_svg_text_path.h"

#include "core/pipeline/base/rosen_render_context.h"
#include "frameworks/core/components/common/painter/rosen_svg_painter.h"
#include "frameworks/core/components/svg/rosen_render_svg_text.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RosenRenderSvgTextPath::Create()
{
    return AceType::MakeRefPtr<RosenRenderSvgTextPath>();
}

void RosenRenderSvgTextPath::Paint(RenderContext& context, const Offset& offset)
{
    // The element text is the general entry for text rendering
    return;
}

Offset RosenRenderSvgTextPath::PaintTextPath(RenderContext& context, const Offset& offset)
{
    double length = RosenSvgPainter::GetPathLength(path_);
    double startOffset = ConvertDimensionToPx(startOffset_, length);
    PathOffset pathOffset = { offset, startOffset, startOffset, path_ };
    DrawTextPath(context, pathOffset);

    return RosenSvgPainter::GetPathOffset(path_, pathOffset.current);
}

Offset RosenRenderSvgTextPath::MeasureTextPathBounds(const Offset& offset, Rect& bounds)
{
    double length = RosenSvgPainter::GetPathLength(path_);
    double startOffset = ConvertDimensionToPx(startOffset_, length);
    PathOffset pathOffset = { offset, startOffset, startOffset, path_ };

    if (!pathOffset.path.empty()) {
        if (!textData_.empty()) {
            pathOffset.current = OnMeasureTextPathBounds(pathOffset, bounds);
        }
        const auto& children = GetChildren();
        if (!children.empty()) {
            for (const auto& child : children) {
                auto textSpan = AceType::DynamicCast<RosenRenderSvgTspan>(child);
                if (textSpan) {
                    textSpan->MeasureTextPathBounds(pathOffset, bounds);
                }
            }
        }
    }

    return RosenSvgPainter::GetPathOffset(path_, pathOffset.current);
}

Rect RosenRenderSvgTextPath::GetPaintBounds(const Offset& offset)
{
    Rect bounds;
    MeasureTextPathBounds(Offset(), bounds);
    return bounds;
}

void RosenRenderSvgTextPath::DrawTextPath(RenderContext& context, PathOffset& pathOffset)
{
    if (pathOffset.path.empty()) {
        LOGW("draw path is emtpy.");
        return;
    }

    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }

    SkAutoCanvasRestore save(canvas, false);
    PaintMaskLayer(context, pathOffset.svg, pathOffset.svg);

    if (!textData_.empty()) {
        pathOffset.current = OnDrawTextPath(context, pathOffset);
    }

    const auto& children = GetChildren();
    if (!children.empty()) {
        for (const auto& child : children) {
            auto textSpan = AceType::DynamicCast<RosenRenderSvgTspan>(child);
            if (textSpan) {
                textSpan->DrawTextPath(context, pathOffset);
            }
        }
    }
}

double RosenRenderSvgTextPath::OnDrawTextPath(RenderContext& context, const PathOffset& pathOffset)
{
    double offset = pathOffset.current;
    auto canvas = static_cast<RosenRenderContext*>(&context)->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return offset;
    }

    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, textData_, opacity_ };
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, 0.0 };
    offset = RosenSvgPainter::UpdateTextPath(canvas, svgTextInfo, pathDrawInfo);
    return offset;
}

double RosenRenderSvgTextPath::OnMeasureTextPathBounds(const PathOffset& pathOffset, Rect& bounds)
{
    double offset = pathOffset.current;
    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, textData_, opacity_ };
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, 0.0 };
    offset = RosenSvgPainter::MeasureTextPathBounds(svgTextInfo, pathDrawInfo, bounds);
    return offset;
}

void RosenRenderSvgTextPath::OnNotifyRender()
{
    // notify parent node of text to render.
    RefPtr<RenderSvgBase> node = nullptr;
    auto parent = AceType::DynamicCast<RenderSvgText>(GetParent().Upgrade());
    if (parent != nullptr) {
        // text set render mark.
        parent->MarkNeedRender(true);
        return;
    }

    // textpath has no parent node of text, set render mark.
    MarkNeedRender(true);
}

} // namespace OHOS::Ace
