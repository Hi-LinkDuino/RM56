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
#include "core/components_v2/grid/rosen_render_grid_scroll.h"

#include "render_service_client/core/ui/rs_node.h"

#include "core/components/common/painter/rosen_scroll_bar_painter.h"
#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace::V2 {

void RosenRenderGridScroll::Update(const RefPtr<Component>& component)
{
    RenderGridScroll::Update(component);
    auto rsNode = GetRSNode();
    if (rsNode == nullptr) {
        return;
    }
    rsNode->SetClipToFrame(true);
}

void RosenRenderGridScroll::Paint(RenderContext& context, const Offset& offset)
{
    LOGD("Paint %{public}lf  %{public}lf", GetLayoutSize().Width(), GetLayoutSize().Height());
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
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    auto canvas = renderContext->GetCanvas();
    auto rsNode = renderContext->GetRSNode();
    if (!canvas || !rsNode) {
        LOGE("canvas is null");
        return;
    }
    rsNode->SetPaintOrder(true);
    Offset lastOffset = isVertical_ ? Offset(0, lastOffset_) : Offset(lastOffset_, 0);
    scrollBar_->UpdateScrollBarRegion(offset, GetLayoutSize(), lastOffset, GetEstimatedHeight());
    RefPtr<RosenScrollBarPainter> scrollPainter = AceType::MakeRefPtr<RosenScrollBarPainter>();
    scrollPainter->PaintBar(canvas, offset, GetPaintRect(), scrollBar_, GetGlobalOffset(), scrollBarOpacity_);
    if (scrollBar_->GetFirstLoad()) {
        scrollBar_->SetFirstLoad(false);
        scrollBar_->HandleScrollBarEnd();
    }
}

} // namespace OHOS::Ace::V2
