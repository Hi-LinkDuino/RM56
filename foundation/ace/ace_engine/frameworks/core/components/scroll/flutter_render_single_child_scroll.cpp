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

#include "core/components/scroll/flutter_render_single_child_scroll.h"

#include "flutter/lib/ui/painting/canvas.h"

#include "core/components/common/painter/flutter_scroll_bar_painter.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

RenderLayer FlutterRenderSingleChildScroll::GetRenderLayer()
{
    if (!layer_) {
        // layer should add parent padding size to avoid clip
        auto maxWidth = GetLayoutParam().GetMaxSize().Width();
        auto maxHeight = GetLayoutParam().GetMaxSize().Height();
        if (NearEqual(maxWidth, Size::INFINITE_SIZE)) {
            maxWidth = viewPort_.Width();
        }
        if (NearEqual(maxHeight, Size::INFINITE_SIZE)) {
            maxHeight = viewPort_.Height();
        }
        layer_ = AceType::MakeRefPtr<Flutter::ClipLayer>(0.0, maxWidth, 0.0, maxHeight, Flutter::Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderSingleChildScroll::Paint(RenderContext& context, const Offset& offset)
{
    auto maxWidth = viewPort_.Width();
    auto maxHeight = viewPort_.Height();
    auto parent = GetParent().Upgrade();
    if (parent) {
        auto parentBox = AceType::DynamicCast<RenderBox>(parent);
        if (parentBox) {
            maxWidth += parentBox->GetPaddingSize().Width();
            maxHeight += parentBox->GetPaddingSize().Height();
        }
    }
    layer_->SetClip(0.0, maxWidth, 0.0, maxHeight, Flutter::Clip::HARD_EDGE);
    RenderNode::Paint(context, offset);

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("canvas is null");
        return;
    }

    // paint custom effect
    if (scrollEffect_) {
        scrollEffect_->Paint(context, viewPort_, offset);
    }

    // paint scrollBar
    if (scrollBar_ && scrollBar_->NeedPaint()) {
        bool needPaint = false;
        if (scrollBar_->GetFirstLoad() || scrollBar_->IsActive() || scrollBar_->GetDisplayMode() == DisplayMode::ON) {
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

    // Notify scroll bar to update.
    if (scrollBarProxy_) {
        scrollBarProxy_->NotifyScrollBar(AceType::WeakClaim(this));
    }
}

}