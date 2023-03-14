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

#include "core/components/list/rosen_render_list.h"
#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {

void RosenRenderList::Paint(RenderContext& context, const Offset& offset)
{
    RenderNode::Paint(context, offset);
    PaintStickyItem(context, offset);
}

void RosenRenderList::PerformLayout()
{
    RenderList::PerformLayout();
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    rsNode->SetClipToBounds(true);
}

void RosenRenderList::SyncGeometryProperties()
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    if (makeCardTransition_) {
        auto pipelineContext = context_.Upgrade();
        if (!pipelineContext) {
            rsNode->SetBounds(0.0, 0.0, viewPort_.Width(), viewPort_.Height());
        } else {
            rsNode->SetBounds(-GetGlobalOffset().GetX(), -shiftHeight_, pipelineContext->GetRootWidth(),
                viewPort_.Height() + shiftHeight_);
        }
    } else {
        rsNode->SetBounds(0.0, 0.0, viewPort_.Width(), viewPort_.Height());
    }
    Offset paintOffset = GetPaintOffset();
    Size paintSize = GetLayoutSize();
    rsNode->SetFrame(paintOffset.GetX(), paintOffset.GetY(), paintSize.Width(), paintSize.Height());
}

void RosenRenderList::PaintChild(const RefPtr<RenderNode>& child, RenderContext& context, const Offset& offset)
{
    // sticky items paint individually by PaintStickyItem
    RefPtr<RenderListItem> listItem = RenderListItem::GetRenderListItem(child);
    if (listItem && !listItem->GetClonedBySticky() && listItem->GetVisible()) {
        context.PaintChild(child, offset);
    }
}

void RosenRenderList::PaintStickyItem(RenderContext& context, const Offset& offset)
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
