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

#include "frameworks/core/components/svg/flutter_render_svg_mask.h"

#include "frameworks/core/components/common/painter/flutter_svg_painter.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvgMask::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgMask::Paint(RenderContext& context, const Offset& offset)
{
    return;
}

void FlutterRenderSvgMask::PaintMaskLayer(RenderContext& context, const Offset& offset, const Rect& paintRect)
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

    SkRect maskBounds = SkRect::MakeXYWH(
        SkDoubleToScalar(paintRect.Left() + ParseUnitsAttr(x_, paintRect.Width())),
        SkDoubleToScalar(paintRect.Top() + ParseUnitsAttr(y_, paintRect.Height())),
        SkDoubleToScalar(ParseUnitsAttr(width_, paintRect.Width())),
        SkDoubleToScalar(ParseUnitsAttr(height_, paintRect.Height())));

    // create mask layer
    skCanvas->saveLayer(maskBounds, nullptr);
    {
        // render mask content
        SkAutoCanvasRestore save(skCanvas, false);
        FlutterSvgPainter::SetMask(skCanvas);
        PaintDirectly(context, offset);
    }

    // create content layer and render content
    SkPaint maskPaint;
    maskPaint.setBlendMode(SkBlendMode::kSrcIn);
    skCanvas->saveLayer(maskBounds, &maskPaint);
    skCanvas->clipRect(maskBounds, true);
}

} // namespace OHOS::Ace
