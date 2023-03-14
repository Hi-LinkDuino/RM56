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

#include "frameworks/core/components/svg/flutter_render_svg_rect.h"

#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/components/svg/flutter_render_svg_filter.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvgRect::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgRect::Paint(RenderContext& context, const Offset& offset)
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

    if (transformLayer_) {
        if (NeedTransform()) {
            transformLayer_->Update(GetTransformMatrix4());
        }
        if (!filterId_.empty()) {
            auto filter = AceType::DynamicCast<FlutterRenderSvgFilter>(GetPatternFromRoot(filterId_));
            if (filter != nullptr) {
                SkPaint skPaint = filter->OnAsPaint();
                transformLayer_->SetFilter(skPaint);
            }
        }
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

void FlutterRenderSvgRect::PaintDirectly(RenderContext& context, const Offset& offset)
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

void FlutterRenderSvgRect::UpdateMotion(const std::string& path, const std::string& rotate, double percent)
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

Rect FlutterRenderSvgRect::GetPaintBounds(const Offset& offset)
{
    SkPath path;
    GetPath(path);
    auto& bounds = path.getBounds();
    return Rect(bounds.left(), bounds.top(), bounds.width(), bounds.height());
}

void FlutterRenderSvgRect::GetPath(SkPath& path)
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

    SkRRect rrect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(ConvertDimensionToPx(x_, LengthType::HORIZONTAL),
            ConvertDimensionToPx(y_, LengthType::VERTICAL), ConvertDimensionToPx(width_, LengthType::HORIZONTAL),
            ConvertDimensionToPx(height_, LengthType::VERTICAL)),
        rx, ry);
    path.addRRect(rrect);
}

void FlutterRenderSvgRect::OnGlobalPositionChanged()
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(UpdateTransformMatrix4());
    }
    RenderNode::OnGlobalPositionChanged();
}

} // namespace OHOS::Ace
