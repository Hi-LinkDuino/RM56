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

#include "core/components/scroll/flutter_render_multi_child_scroll.h"

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/common/painter/flutter_scroll_bar_painter.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

void FlutterRenderMultiChildScroll::Paint(RenderContext& context, const Offset& offset)
{
    // child position changed when layout, need layout again
    if (!NearEqual(GetMainOffset(currentOffset_), offsetBeforeLayout_)) {
        MarkNeedLayout();
        return;
    }
    RenderNode::Paint(context, offset);

    // paint custom effect
    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas != nullptr) {
        // paint custom effect
        if (scrollEffect_) {
            scrollEffect_->Paint(context, viewPort_, offset);
        }

        // paint scrollBar
        if (scrollBar_ && scrollBar_->NeedPaint()) {
            bool needPaint = false;
            if (scrollBar_->GetFirstLoad() || scrollBar_->IsActive() ||
                scrollBar_->GetDisplayMode() == DisplayMode::ON) {
                scrollBarOpacity_ = UINT8_MAX;
                needPaint = true;
            } else {
                if (scrollBarOpacity_ != 0) {
                    needPaint = true;
                }
            }
            if (needPaint) {
                scrollBar_->UpdateScrollBarRegion(offset, GetLayoutSize(), lastOffset_, GetEstimatedHeight());
                RefPtr<FlutterScrollBarPainter> scrollBarPainter = AceType::MakeRefPtr<FlutterScrollBarPainter>();
                scrollBarPainter->PaintBar(
                    canvas, offset, GetPaintRect(), scrollBar_, GetGlobalOffset(), scrollBarOpacity_);
                if (scrollBar_->GetFirstLoad()) {
                    scrollBar_->SetFirstLoad(false);
                    scrollBar_->HandleScrollBarEnd();
                }
            }
        }
    }
}

} // namespace OHOS::Ace