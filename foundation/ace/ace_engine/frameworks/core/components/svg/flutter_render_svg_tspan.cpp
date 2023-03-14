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

#include "frameworks/core/components/svg/flutter_render_svg_tspan.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/svg/flutter_render_svg_text_path.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

RefPtr<RenderNode> FlutterRenderSvgTspan::Create()
{
    return AceType::MakeRefPtr<FlutterRenderSvgTspan>();
}

void FlutterRenderSvgTspan::Paint(RenderContext& context, const Offset& offset)
{
    // The element text is the general entry for text rendering
    return;
}

void FlutterRenderSvgTspan::UpdateDrawOffset(DrawOffset& drawOffset)
{
    double x = ConvertDimensionToPx(x_, LengthType::HORIZONTAL);
    double dx = ConvertDimensionToPx(dx_, LengthType::HORIZONTAL);
    double y = ConvertDimensionToPx(y_, LengthType::VERTICAL);
    double dy = ConvertDimensionToPx(dy_, LengthType::VERTICAL);

    double offsetX = hasX_ ? (drawOffset.svg.GetX() + x + dx) : (drawOffset.current.GetX() + dx);
    double offsetY = hasY_ ? (drawOffset.svg.GetY() + y + dy) : (drawOffset.current.GetY() + dy);
    drawOffset.current = Offset(offsetX, offsetY);
    if (!hasX_ && drawOffset.isTspan) {
        // keep space between tspans
        isDrawSpace_ = true;
    } else {
        isDrawSpace_ = false;
    }
    drawOffset.isTspan = true;
}

void FlutterRenderSvgTspan::UpdatePathOffset(PathOffset& pathOffset)
{
    double length = FlutterSvgPainter::GetPathLength(pathOffset.path);
    double x = ConvertDimensionToPx(x_, length);
    double dx = ConvertDimensionToPx(dx_, length);
    if (hasX_) {
        pathOffset.current = pathOffset.start + x + dx;
    } else {
        if (pathOffset.current != pathOffset.start) {
            // keep space between tspans
            isDrawSpace_ = true;
        } else {
            isDrawSpace_ = false;
        }
        pathOffset.current += dx;
    }
}

void FlutterRenderSvgTspan::DrawTextPath(RenderContext& context, PathOffset& pathOffset)
{
    if (pathOffset.path.empty()) {
        LOGW("draw path is empty.");
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
    PaintMaskLayer(context, pathOffset.svg, GetPaintRect());

    if (!textData_.empty()) {
        // update draw offset by attribute
        UpdatePathOffset(pathOffset);
        // update offset after draw
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
        return;
    }
}

void FlutterRenderSvgTspan::MeasureTextPathBounds(PathOffset& pathOffset, Rect& bounds)
{
    if (pathOffset.path.empty()) {
        LOGW("draw path is empty.");
        return;
    }

    if (!textData_.empty()) {
        // update draw offset by attribute
        UpdatePathOffset(pathOffset);
        // update offset after draw
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

double FlutterRenderSvgTspan::OnDrawTextPath(RenderContext& context, const PathOffset& pathOffset)
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
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, rotate_ };
    offset = FlutterSvgPainter::UpdateTextPath(skCanvas, svgTextInfo, pathDrawInfo);
    return offset;
}

double FlutterRenderSvgTspan::OnMeasureTextPathBounds(const PathOffset& pathOffset, Rect& bounds)
{
    double offset = pathOffset.current;
    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, textData_, opacity_ };
    PathDrawInfo pathDrawInfo = { pathOffset.path, pathOffset.current, rotate_ };
    offset = FlutterSvgPainter::MeasureTextPathBounds(svgTextInfo, pathDrawInfo, bounds);
    return offset;
}

void FlutterRenderSvgTspan::OnNotifyRender()
{
    // notify parent node of text or textpath to render.
    RefPtr<RenderSvgBase> node = nullptr;
    auto parent = AceType::DynamicCast<RenderSvgBase>(GetParent().Upgrade());
    while (parent != nullptr) {
        if (AceType::InstanceOf<FlutterRenderSvgTspan>(parent) || AceType::InstanceOf<RenderSvgTextPath>(parent)) {
            node = parent;
            parent = AceType::DynamicCast<RenderSvgBase>(parent->GetParent().Upgrade());
        } else if (AceType::InstanceOf<RenderSvgText>(parent)) {
            // text or textpath set render mark.
            parent->MarkNeedRender(true);
            return;
        }
    }

    // tspan has no parent node of text or textpath. the top level of tspan set render mark.
    if (node) {
        node->MarkNeedRender(true);
    } else {
        MarkNeedRender(true);
    }
}

} // namespace OHOS::Ace
