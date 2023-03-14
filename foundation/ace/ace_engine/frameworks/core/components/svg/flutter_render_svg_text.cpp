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

#include "frameworks/core/components/svg/flutter_render_svg_text.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/svg/flutter_render_svg_text_path.h"
#include "frameworks/core/components/svg/flutter_render_svg_tspan.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RefPtr<RenderNode> FlutterRenderSvgText::Create()
{
    FlutterSvgPainter::CheckFontType();
    return AceType::MakeRefPtr<FlutterRenderSvgText>();
}

RenderLayer FlutterRenderSvgText::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgText::Paint(RenderContext& context, const Offset& offset)
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(GetTransformMatrix4());
    }

    DrawOffset drawOffset = { offset, offset, false };
    DrawText(context, drawOffset);
}

void FlutterRenderSvgText::PaintDirectly(RenderContext& context, const Offset& offset)
{
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
    if (NeedTransform()) {
        skCanvas->save();
        skCanvas->concat(FlutterSvgPainter::ToSkMatrix(GetTransformMatrix4()));
    }

    DrawOffset drawOffset = { offset, offset, false };
    DrawText(context, drawOffset);

    if (NeedTransform()) {
        skCanvas->restore();
    }
}

Rect FlutterRenderSvgText::GetPaintBounds(const Offset& offset)
{
    DrawOffset drawOffset = { Offset(), offset, false };
    Rect bounds;
    MeasureTextBounds(drawOffset, bounds);
    return bounds;
}

void FlutterRenderSvgText::MeasureTextBounds(DrawOffset& drawOffset, Rect& bounds)
{
    UpdateDrawOffset(drawOffset);
    if (!textData_.empty()) {
        drawOffset.current = OnMeasureTextBounds(drawOffset, bounds);
        drawOffset.isTspan = true;
    }

    const auto& children = GetChildren();
    if (!children.empty()) {
        for (const auto& child : children) {
            auto textSpan = AceType::DynamicCast<FlutterRenderSvgTspan>(child);
            if (textSpan) {
                textSpan->MeasureTextBounds(drawOffset, bounds);
                continue;
            }

            auto textPath = AceType::DynamicCast<FlutterRenderSvgTextPath>(child);
            if (textPath) {
                drawOffset.current = textPath->MeasureTextPathBounds(drawOffset.svg, bounds);
                drawOffset.isTspan = true;
            }
        }
    }
}

void FlutterRenderSvgText::DrawText(RenderContext& context, DrawOffset& drawOffset)
{
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
    PaintMaskLayer(context, drawOffset.svg, drawOffset.current);

    // update current offset by attribute
    UpdateDrawOffset(drawOffset);

    if (!textData_.empty()) {
        drawOffset.current = OnDrawText(context, drawOffset);
        drawOffset.isTspan = true;
    }

    const auto& children = GetChildren();
    if (!children.empty()) {
        for (const auto& child : children) {
            auto textSpan = AceType::DynamicCast<FlutterRenderSvgTspan>(child);
            if (textSpan) {
                textSpan->DrawText(context, drawOffset);
                continue;
            }

            auto textPath = AceType::DynamicCast<FlutterRenderSvgTextPath>(child);
            if (textPath) {
                drawOffset.current = textPath->PaintTextPath(context, drawOffset.svg);
                drawOffset.isTspan = true;
            }
        }
    }
}

Offset FlutterRenderSvgText::OnDrawText(RenderContext& context, const DrawOffset& drawOffset)
{
    Offset offset = drawOffset.current;
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

    std::string text = isDrawSpace_ ? " " + textData_ : textData_;
    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, text, opacity_ };
    TextDrawInfo textDrawInfo = { offset, rotate_ };
    offset = FlutterSvgPainter::UpdateText(skCanvas, svgTextInfo, textDrawInfo);
    return offset;
}

Offset FlutterRenderSvgText::OnMeasureTextBounds(const DrawOffset& drawOffset, Rect& bounds)
{
    Offset offset = drawOffset.current;
    std::string text = isDrawSpace_ ? " " + textData_ : textData_;
    SvgTextInfo svgTextInfo = { fillState_, strokeState_, textStyle_, text, opacity_ };
    TextDrawInfo textDrawInfo = { offset, rotate_ };
    offset = FlutterSvgPainter::MeasureTextBounds(svgTextInfo, textDrawInfo, bounds);
    return offset;
}

void FlutterRenderSvgText::UpdateDrawOffset(DrawOffset& drawOffset)
{
    double x = ConvertDimensionToPx(x_, LengthType::HORIZONTAL);
    double dx = ConvertDimensionToPx(dx_, LengthType::HORIZONTAL);
    double y = ConvertDimensionToPx(y_, LengthType::VERTICAL);
    double dy = ConvertDimensionToPx(dy_, LengthType::VERTICAL);
    drawOffset.current = drawOffset.current + Offset(x + dx, y + dy);
    drawOffset.isTspan = false;
}

void FlutterRenderSvgText::OnGlobalPositionChanged()
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(UpdateTransformMatrix4());
    }
    RenderNode::OnGlobalPositionChanged();
}

} // namespace OHOS::Ace
