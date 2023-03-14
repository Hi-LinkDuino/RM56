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

#include "frameworks/core/components/svg/flutter_render_svg_text_path.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/svg/flutter_render_svg_text.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RefPtr<RenderNode> FlutterRenderSvgTextPath::Create()
{
    return AceType::MakeRefPtr<FlutterRenderSvgTextPath>();
}

RenderLayer FlutterRenderSvgTextPath::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgTextPath::Paint(RenderContext& context, const Offset& offset)
{
    // The element text is the general entry for text rendering
    return;
}

Offset FlutterRenderSvgTextPath::PaintTextPath(RenderContext& context, const Offset& offset)
{
    double length = FlutterSvgPainter::GetPathLength(path_);
    double startOffset = ConvertDimensionToPx(startOffset_, length);
    PathOffset pathOffset = { offset, startOffset, startOffset, path_};
    DrawTextPath(context, pathOffset);

    return FlutterSvgPainter::GetPathOffset(path_, pathOffset.current);
}

Offset FlutterRenderSvgTextPath::MeasureTextPathBounds(const Offset& offset, Rect& bounds)
{
    double length = FlutterSvgPainter::GetPathLength(path_);
    double startOffset = ConvertDimensionToPx(startOffset_, length);
    PathOffset pathOffset = { offset, startOffset, startOffset, path_};

    if (!pathOffset.path.empty()) {
        if (!textData_.empty()) {
            pathOffset.current = OnMeasureTextPathBounds(pathOffset, bounds);
        }
        const auto& children = GetChildren();
        if (!children.empty()) {
            for (const auto& child : children) {
                auto textSpan = AceType::DynamicCast<FlutterRenderSvgTspan>(child);
                if (textSpan) {
                    textSpan->MeasureTextPathBounds(pathOffset, bounds);
                }
            }
        }
    }

    return FlutterSvgPainter::GetPathOffset(path_, pathOffset.current);
}

Rect FlutterRenderSvgTextPath::GetPaintBounds(const Offset& offset)
{
    Rect bounds;
    MeasureTextPathBounds(Offset(), bounds);
    return bounds;
}

void FlutterRenderSvgTextPath::DrawTextPath(RenderContext& context, PathOffset& pathOffset)
{
    if (pathOffset.path.empty()) {
        LOGW("draw path is emtpy.");
        return;
    }

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return;
    }
    SkAutoCanvasRestore save(skCanvas, false);
    PaintMaskLayer(context, pathOffset.svg, pathOffset.svg);

    if (!textData_.empty()) {
        pathOffset.current = OnDrawTextPath(context, pathOffset);
    }

    const auto& children = GetChildren();
    if (!children.empty()) {
        for (const auto& child : children) {
            auto textSpan = AceType::DynamicCast<FlutterRenderSvgTspan>(child);
            if (textSpan) {
                textSpan->DrawTextPath(context, pathOffset);
            }
        }
    }
}

double FlutterRenderSvgTextPath::OnDrawTextPath(RenderContext& context, const PathOffset& pathOffset)
{
    double offset = pathOffset.current;
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (!canvas) {
        LOGE("Paint canvas is null");
        return offset;
    }
    SkCanvas* skCanvas = canvas->canvas();
    if (!skCanvas) {
        LOGE("Paint skCanvas is null");
        return offset;
    }

    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, textData_, opacity_ };
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, 0.0 };
    offset = FlutterSvgPainter::UpdateTextPath(skCanvas, svgTextInfo, pathDrawInfo);
    return offset;
}

double FlutterRenderSvgTextPath::OnMeasureTextPathBounds(const PathOffset& pathOffset, Rect& bounds)
{
    double offset = pathOffset.current;
    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, textData_, opacity_ };
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, 0.0 };
    offset = FlutterSvgPainter::MeasureTextPathBounds(svgTextInfo, pathDrawInfo, bounds);
    return offset;
}

void FlutterRenderSvgTextPath::OnNotifyRender()
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
