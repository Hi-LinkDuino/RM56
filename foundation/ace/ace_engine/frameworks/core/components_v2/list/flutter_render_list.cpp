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

#include "core/components_v2/list/flutter_render_list.h"

#include "base/utils/utils.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace::V2 {

RenderLayer FlutterRenderList::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Flutter::Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

bool FlutterRenderList::IsRepaintBoundary() const
{
    return true;
}

void FlutterRenderList::Paint(RenderContext& context, const Offset& offset)
{
    const auto& layoutSize = GetLayoutSize();
    if (layer_) {
        layer_->SetClip(0, layoutSize.Width(), 0, layoutSize.Height(), Flutter::Clip::HARD_EDGE);
    }

    for (const auto& child : items_) {
        if (child == currentStickyItem_ || child == selectedItem_) {
            continue;
        }
        PaintChild(child, context, offset);
    }

    const auto& divider = component_->GetItemDivider();
    if (divider && divider->color.GetAlpha() > 0x00 && GreatNotEqual(divider->strokeWidth.Value(), 0.0)) {
        auto canvas = ScopedCanvas::Create(context);
        auto skCanvas = canvas.GetSkCanvas();
        if (skCanvas == nullptr) {
            LOGE("skia canvas is null");
            return;
        }

        const double mainSize = GetMainSize(layoutSize);
        const double strokeWidth = NormalizePercentToPx(divider->strokeWidth, vertical_);
        const double halfSpaceWidth = std::max(spaceWidth_, strokeWidth) / 2.0;
        const double startCrossAxis = NormalizePercentToPx(divider->startMargin, !vertical_);
        const double endCrossAxis = GetCrossSize(layoutSize) - NormalizePercentToPx(divider->endMargin, !vertical_);
        const double topOffset = halfSpaceWidth + (strokeWidth / 2.0);
        const double bottomOffset = topOffset - strokeWidth;

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(divider->color.GetValue());
        paint.setStyle(SkPaint::Style::kStroke_Style);
        paint.setStrokeWidth(strokeWidth);

        for (const auto& child : items_) {
            if (child == selectedItem_) {
                continue;
            }

            double mainAxis = GetMainAxis(child->GetPosition());
            if (GreatOrEqual(mainAxis - topOffset, mainSize)) {
                break;
            }
            if (LessOrEqual(mainAxis - bottomOffset, 0.0)) {
                continue;
            }
            mainAxis -= halfSpaceWidth;
            if (vertical_) {
                skCanvas->drawLine(startCrossAxis, mainAxis, endCrossAxis, mainAxis, paint);
            } else {
                skCanvas->drawLine(mainAxis, startCrossAxis, mainAxis, endCrossAxis, paint);
            }
        }

        if (selectedItem_) {
            double mainAxis = targetMainAxis_ - halfSpaceWidth;
            if (vertical_) {
                skCanvas->drawLine(startCrossAxis, mainAxis, endCrossAxis, mainAxis, paint);
            } else {
                skCanvas->drawLine(mainAxis, startCrossAxis, mainAxis, endCrossAxis, paint);
            }
        }
    }

    if (currentStickyItem_) {
        PaintChild(currentStickyItem_, context, offset);
    }

    if (selectedItem_) {
        selectedItem_->SetPosition(MakeValue<Offset>(selectedItemMainAxis_, 0.0));
        PaintChild(selectedItem_, context, offset);
    }

    // Notify scroll bar to update.
    if (scrollBarProxy_) {
        scrollBarProxy_->NotifyScrollBar(AceType::WeakClaim(this));
    }
}

} // namespace OHOS::Ace::V2
