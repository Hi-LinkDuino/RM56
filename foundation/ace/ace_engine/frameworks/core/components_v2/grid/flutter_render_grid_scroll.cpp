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
#include "core/components_v2/grid/flutter_render_grid_scroll.h"

#include "core/components/common/painter/flutter_scroll_bar_painter.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace::V2 {

using namespace Flutter;

RenderLayer FlutterRenderGridScroll::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderGridScroll::Paint(RenderContext& context, const Offset& offset)
{
    LOGD("Paint %{public}lf  %{public}lf", GetLayoutSize().Width(), GetLayoutSize().Height());
    layer_->SetClip(0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    RenderNode::Paint(context, offset);

    // Notify scroll bar to update.
    if (scrollBarProxy_) {
        scrollBarProxy_->NotifyScrollBar(AceType::WeakClaim(this));
    }

    // render scroll bar
    if (!scrollBar_ || !scrollBar_->NeedPaint()) {
        return;
    }
    bool needPaint = false;
    if (scrollBar_->GetFirstLoad() || scrollBar_->IsActive() || scrollBar_->GetDisplayMode() == DisplayMode::ON) {
        scrollBarOpacity_ = UINT8_MAX;
        needPaint = true;
    } else {
        if (scrollBarOpacity_ != 0) {
            needPaint = true;
        }
    }
    if (!needPaint) {
        return;
    }
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    Offset lastOffset = isVertical_ ? Offset(0, lastOffset_) : Offset(lastOffset_, 0);
    scrollBar_->UpdateScrollBarRegion(offset, GetLayoutSize(), lastOffset, GetEstimatedHeight());
    RefPtr<FlutterScrollBarPainter> scrollPainter = AceType::MakeRefPtr<FlutterScrollBarPainter>();
    scrollPainter->PaintBar(canvas, offset, GetPaintRect(), scrollBar_, GetGlobalOffset(), scrollBarOpacity_);
    if (scrollBar_->GetFirstLoad()) {
        scrollBar_->SetFirstLoad(false);
        scrollBar_->HandleScrollBarEnd();
    }
}

} // namespace OHOS::Ace::V2
