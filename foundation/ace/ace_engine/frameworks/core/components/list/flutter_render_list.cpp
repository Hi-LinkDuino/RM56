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

#include "core/components/list/flutter_render_list.h"

#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderList::GetRenderLayer()
{
    if (!layer_) {
        layer_ =
            AceType::MakeRefPtr<ClipLayer>(0, GetLayoutSize().Width(), 0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderList::Paint(RenderContext& context, const Offset& offset)
{
    if (makeCardTransition_) {
        auto pipelineContext = context_.Upgrade();
        if (!pipelineContext) {
            layer_->SetClip(0.0, viewPort_.Width(), 0.0, viewPort_.Height(), Clip::HARD_EDGE);
        } else {
            layer_->SetClip(-GetGlobalOffset().GetX(), pipelineContext->GetRootWidth(), -shiftHeight_,
                viewPort_.Height() + shiftHeight_, Clip::HARD_EDGE);
        }
    } else {
        layer_->SetClip(0.0, viewPort_.Width(), 0.0, viewPort_.Height(), Clip::HARD_EDGE);
    }
    RenderNode::Paint(context, offset);
    PaintStickyItem(context, offset);
}

void FlutterRenderList::PaintChild(const RefPtr<RenderNode>& child, RenderContext& context, const Offset& offset)
{
    // sticky items paint individually by PaintStickyItem
    RefPtr<RenderListItem> listItem = RenderListItem::GetRenderListItem(child);
    if (listItem && !listItem->GetClonedBySticky() && listItem->GetVisible()) {
        context.PaintChild(child, offset);
    }
}

void FlutterRenderList::PaintStickyItem(RenderContext& context, const Offset& offset)
{
    bool watchMode = SystemProperties::GetDeviceType() == DeviceType::WATCH;
    RefPtr<RenderNode> stickyItem = GetStickyItem();
    if (stickyItem) {
        RefPtr<RenderListItem> listItem = RenderListItem::GetRenderListItem(stickyItem);
        if (listItem && listItem->GetSticky()) {
            listItem->SetVisible(true);
            listItem->SetRedraw(watchMode);
            listItem->SetPosition(offset + GetStickyItemOffset());
            listItem->Paint(context, offset + GetStickyItemOffset());
            listItem->SetRedraw(false);
        }
        if (!watchMode) {
            return;
        }
        RefPtr<RenderNode> stickyNext = GetStickyNext();
        if (stickyNext) {
            RefPtr<RenderListItem> listItemNext = RenderListItem::GetRenderListItem(stickyNext);
            if (listItemNext) {
                if (listItem && listItem->GetStickyMode() == StickyMode::OPACITY) {
                    listItemNext->SetNearByOpacitySticky(true);
                } else {
                    listItemNext->SetNearByOpacitySticky(false);
                }
                listItemNext->SetVisible(true);
                listItemNext->SetRedraw(watchMode);
                listItemNext->SetPosition(Offset());
                listItemNext->Paint(context, offset + GetStickyNextOffset());
                listItemNext->SetRedraw(false);
            }
        }
    }
}

} // namespace OHOS::Ace
