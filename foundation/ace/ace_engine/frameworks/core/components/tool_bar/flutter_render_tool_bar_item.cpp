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

#include "core/components/tool_bar/flutter_render_tool_bar_item.h"

#include "third_party/skia/include/core/SkMaskFilter.h"

#include "core/components/transform/flutter_render_transform.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

// for focus
constexpr Dimension TOOL_BAR_FOCUS_DEL_OFFSET = 4.0_vp;
constexpr Dimension TOOL_BAR_FOCUS_DEL_SIZE = 8.0_vp;

using namespace Flutter;

RenderLayer FlutterRenderToolBarItem::GetRenderLayer()
{
    if (!clipLayer_) {
        clipLayer_ = AceType::MakeRefPtr<ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(clipLayer_);
}

void FlutterRenderToolBarItem::Paint(RenderContext& context, const Offset& offset)
{
    clipLayer_->SetClip(0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    auto renderContext = AceType::DynamicCast<FlutterRenderContext>(&context);
    if (!renderContext) {
        LOGE("Render context is null");
        return;
    }
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null");
        return;
    }

    DrawShape(*canvas, Rect(offset, GetLayoutSize()), GetEventEffectColor(), NormalizeToPx(rrectRadius_));

    if (onFocus_) {
        DrawFocus();
    }
    if (onHover_) {
        DrawShape(*canvas, Rect(offset, GetLayoutSize()), hoverColor_, NormalizeToPx(rrectRadius_));
    }

    RenderToolBarItem::Paint(context, offset);
}

void FlutterRenderToolBarItem::DrawFocus()
{
    Offset offset =
        GetPosition() + Offset(NormalizeToPx(TOOL_BAR_FOCUS_DEL_OFFSET), NormalizeToPx(TOOL_BAR_FOCUS_DEL_OFFSET));
    Size layoutSize =
        GetLayoutSize() - Size(NormalizeToPx(TOOL_BAR_FOCUS_DEL_SIZE), NormalizeToPx(TOOL_BAR_FOCUS_DEL_SIZE));
    Offset globalOffset =
        GetGlobalOffset() + Offset(NormalizeToPx(TOOL_BAR_FOCUS_DEL_OFFSET), NormalizeToPx(TOOL_BAR_FOCUS_DEL_OFFSET));
    RequestFocusAnimation(globalOffset, Rect(offset, layoutSize), Radius(rrectRadius_));
}

void FlutterRenderToolBarItem::DrawShape(
    flutter::Canvas& canvas, const Rect& paintRect, const Color& color, double radius)
{
    flutter::Paint paint;
    flutter::RRect rRect;
    flutter::PaintData paintData;

    paint.paint()->setColor(color.GetValue());
    rRect.sk_rrect.setRectXY(SkRect::MakeIWH(paintRect.Width(), paintRect.Height()), radius, radius);
    rRect.sk_rrect.offset(paintRect.GetOffset().GetX(), paintRect.GetOffset().GetY());
    canvas.drawRRect(rRect, paint, paintData);
}

} // namespace OHOS::Ace