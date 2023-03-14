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

#include "frameworks/core/components/svg/flutter_render_svg_ellipse.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvgEllipse::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgEllipse::Paint(RenderContext& context, const Offset& offset)
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

    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(GetTransformMatrix4());
    }

    SkAutoCanvasRestore save(skCanvas, false);
    PaintMaskLayer(context, offset, offset);

    SkPath path;
    GetPath(path);
    UpdateGradient(fillState_);

    RenderInfo renderInfo = { AceType::Claim(this), offset, opacity_, true };
    FlutterSvgPainter::SetFillStyle(skCanvas, path, fillState_, renderInfo);
    FlutterSvgPainter::SetStrokeStyle(skCanvas, path, strokeState_, renderInfo);
    RenderNode::Paint(context, offset);
}

void FlutterRenderSvgEllipse::PaintDirectly(RenderContext& context, const Offset& offset)
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

    SkPath path;
    GetPath(path);
    UpdateGradient(fillState_);
    FlutterSvgPainter::SetFillStyle(skCanvas, path, fillState_, opacity_);
    FlutterSvgPainter::SetStrokeStyle(skCanvas, path, strokeState_, opacity_);
    if (NeedTransform()) {
        skCanvas->restore();
    }
}

void FlutterRenderSvgEllipse::UpdateMotion(const std::string& path, const std::string& rotate, double percent)
{
    if (!transformLayer_) {
        LOGE("transformLayer is null");
        return;
    }
    bool isSuccess = true;
    auto motionMatrix = FlutterSvgPainter::CreateMotionMatrix(path, rotate, percent, isSuccess);
    if (isSuccess) {
        auto transform = FlutterRenderTransform::GetTransformByOffset(motionMatrix, GetGlobalOffset());
        transformLayer_->Update(transform);
    }
}

Rect FlutterRenderSvgEllipse::GetPaintBounds(const Offset& offset)
{
    SkPath path;
    GetPath(path);
    auto& bounds = path.getBounds();
    return Rect(bounds.left(), bounds.top(), bounds.width(), bounds.height());
}

void FlutterRenderSvgEllipse::GetPath(SkPath& path)
{
    double rx = 0.0;
    if (GreatOrEqual(rx_.Value(), 0.0)) {
        rx = ConvertDimensionToPx(rx_, LengthType::HORIZONTAL);
    } else {
        if (GreatNotEqual(ry_.Value(), 0.0)) {
            rx = ConvertDimensionToPx(ry_, LengthType::VERTICAL);
        }
    }
    double ry = 0.0;
    if (GreatOrEqual(ry_.Value(), 0.0)) {
        ry = ConvertDimensionToPx(ry_, LengthType::VERTICAL);
    } else {
        if (GreatNotEqual(rx_.Value(), 0.0)) {
            ry = ConvertDimensionToPx(rx_, LengthType::HORIZONTAL);
        }
    }
    SkRect rect = SkRect::MakeXYWH(ConvertDimensionToPx(cx_, LengthType::HORIZONTAL) - rx,
        ConvertDimensionToPx(cy_, LengthType::VERTICAL) - ry, rx + rx, ry + ry);
    path.addOval(rect);
}

void FlutterRenderSvgEllipse::OnGlobalPositionChanged()
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(UpdateTransformMatrix4());
    }
    RenderNode::OnGlobalPositionChanged();
}

} // namespace OHOS::Ace
