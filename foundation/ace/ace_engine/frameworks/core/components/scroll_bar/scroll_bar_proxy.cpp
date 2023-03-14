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

#include "core/components/scroll_bar/scroll_bar_proxy.h"

#include "core/components/scroll/render_scroll.h"
#include "core/components/scroll_bar/render_scroll_bar.h"
#include "core/components_v2/grid/render_grid_scroll.h"
#include "core/components_v2/list/render_list.h"

namespace OHOS::Ace {

void ScrollBarProxy::RegisterScrollableNode(const ScrollableNodeInfo& scrollableNode)
{
    if (std::find(scrollableNodes_.begin(), scrollableNodes_.end(), scrollableNode) != scrollableNodes_.end()) {
        LOGE("scrollable node is already exist.");
        return;
    }
    scrollableNodes_.emplace_back(scrollableNode);
}

void ScrollBarProxy::RegisterScrollBar(const WeakPtr<RenderScrollBar>& scrollBar)
{
    if (std::find(scrollBars_.begin(), scrollBars_.end(), scrollBar) != scrollBars_.end()) {
        LOGE("scroll bar is already exist.");
        return;
    }
    scrollBars_.emplace_back(scrollBar);
}

void ScrollBarProxy::UnRegisterScrollableNode(const WeakPtr<RenderNode>& scrollableNode)
{
    auto iter = std::find_if(scrollableNodes_.begin(), scrollableNodes_.end(),
        [&scrollableNode](const ScrollableNodeInfo& info) { return scrollableNode == info.scrollableNode; });
    if (iter != scrollableNodes_.end()) {
        scrollableNodes_.erase(iter);
    }
}

void ScrollBarProxy::UnRegisterScrollBar(const WeakPtr<RenderScrollBar>& scrollBar)
{
    auto iter = std::find(scrollBars_.begin(), scrollBars_.end(), scrollBar);
    if (iter != scrollBars_.end()) {
        scrollBars_.erase(iter);
    }
}

void ScrollBarProxy::NotifyScrollableNode(double distance, const WeakPtr<RenderScrollBar>& weakScrollBar) const
{
    auto scrollBar = weakScrollBar.Upgrade();
    if (!scrollBar) {
        LOGE("ScrollBar has been released.");
        return;
    }

    auto scrollBarChild = scrollBar->GetLastChild();
    if (!scrollBarChild) {
        LOGE("ScrollBar has no child.");
        return;
    }

    for (const auto& [weakScrollableNode, onPositionChanged] : scrollableNodes_) {
        if (!onPositionChanged) {
            continue;
        }
        auto scrollable = weakScrollableNode.Upgrade();
        if (!scrollable || !CheckScrollable(scrollable)) {
            LOGE("Node is not scrollable node.");
            continue;
        }

        auto scrollableChild = scrollable->GetLastChild();
        if (!scrollableChild) {
            LOGE("Scrollable node has no child.");
            continue;
        }

        auto scrollBarAxis = scrollBar->GetAxis();
        Size scrollableSize = scrollable->GetLayoutSize();
        Size scrollableChildSize = GetScrollableChildSize(scrollable, scrollableChild->GetLayoutSize(), scrollBarAxis);
        Size scrollBarSize = scrollBar->GetLayoutSize();
        Size scrollBarChildSize = scrollBarChild->GetLayoutSize();
        auto scrollableAxis = GetScrollableAxis(scrollable);
        if (scrollableAxis != Axis::FREE && scrollBarAxis != Axis::FREE && scrollableAxis != scrollBarAxis) {
            LOGE("Axis of ScrollBar and Scroll is not match.");
            continue;
        }

        double value = 0.0;
        auto scrollableDeltaSize = scrollableChildSize - scrollableSize;
        auto scrollBarDeltaSize = scrollBarSize - scrollBarChildSize;
        if (scrollBarAxis == Axis::VERTICAL) {
            if (!NearZero(scrollBarDeltaSize.Height())) {
                value = distance * scrollableDeltaSize.Height() / scrollBarDeltaSize.Height();
            }
        } else {
            if (!NearZero(scrollBarDeltaSize.Width())) {
                value = distance * scrollableDeltaSize.Width() / scrollBarDeltaSize.Width();
            }
        }
        constexpr int32_t SCROLL_FROM_BAR = 6; // Source type of scroll.
        onPositionChanged(value, SCROLL_FROM_BAR);
    }
}

void ScrollBarProxy::NotifyScrollBar(const WeakPtr<RenderNode>& weakScrollableNode) const
{
    auto scrollable = weakScrollableNode.Upgrade();
    if (!scrollable || !CheckScrollable(scrollable)) {
        LOGE("Node is not scrollable node.");
        return;
    }

    auto scrollableChild = scrollable->GetLastChild();
    if (!scrollableChild) {
        LOGE("child of scrollable node is null");
        return;
    }

    for (const auto& weakScrollBar : scrollBars_) {
        auto scrollBar = weakScrollBar.Upgrade();
        if (!scrollBar) {
            LOGE("ScrollBar is released.");
            continue;
        }
        auto scrollBarChild = scrollBar->GetLastChild();
        if (!scrollBarChild) {
            LOGE("ScrollBar has no child.");
            continue;
        }
        auto scrollBarAxis = scrollBar->GetAxis();
        Axis scrollableAxis = Axis::NONE;
        Offset scrollableChildPosition = scrollableChild->GetPosition();
        Size scrollableSize = scrollable->GetLayoutSize();
        Size scrollableChildSize = scrollableChild->GetLayoutSize();
        Size scrollBarSize = scrollBar->GetLayoutSize();
        Size scrollBarChildSize = scrollBarChild->GetLayoutSize();
        AdjustParam(scrollable, scrollBarAxis, scrollableAxis, scrollableChildSize, scrollableChildPosition);

        if (scrollBarAxis != Axis::FREE && scrollableAxis != Axis::FREE && scrollBarAxis != scrollableAxis) {
            LOGE("Axis of ScrollBar and Scroll is not match.");
            continue;
        }

        Offset position;
        if (scrollBarAxis == Axis::VERTICAL) {
            if (LessOrEqual((scrollableChildSize - scrollableSize).Height(), 0.0) ||
                LessOrEqual((scrollBarSize - scrollBarChildSize).Height(), 0.0)) {
                continue;
            }
            auto positionY = scrollableChildPosition.GetY() * (scrollBarSize - scrollBarChildSize).Height() /
                             (scrollableChildSize - scrollableSize).Height();
            positionY = std::clamp(positionY, 0.0, (scrollBarSize - scrollBarChildSize).Height());
            position.SetY(positionY);
        } else {
            if (LessOrEqual((scrollableChildSize - scrollableSize).Width(), 0.0) ||
                LessOrEqual((scrollBarSize - scrollBarChildSize).Width(), 0.0)) {
                continue;
            }
            auto positionX = scrollableChildPosition.GetX() * (scrollBarSize - scrollBarChildSize).Width() /
                             (scrollableChildSize - scrollableSize).Width();
            positionX = std::clamp(positionX, 0.0, (scrollBarSize - scrollBarChildSize).Width());
            position.SetX(positionX);
        }
        scrollBarChild->SetPosition(position);
        scrollBar->MarkNeedRender();
    }
}

void ScrollBarProxy::StartScrollBarAnimator() const
{
    for (const auto& weakScrollBar : scrollBars_) {
        auto scrollBar = weakScrollBar.Upgrade();
        if (!scrollBar) {
            LOGE("ScrollBar is released.");
            continue;
        }
        if (scrollBar->GetDisplayMode() == DisplayMode::AUTO) {
            scrollBar->StartAnimator();
        }
    }
}

void ScrollBarProxy::StopScrollBarAnimator() const
{
    for (const auto& weakScrollBar : scrollBars_) {
        auto scrollBar = weakScrollBar.Upgrade();
        if (!scrollBar) {
            LOGE("ScrollBar is released.");
            continue;
        }
        scrollBar->StopAnimator();
    }
}

bool ScrollBarProxy::CheckScrollable(const RefPtr<RenderNode>& node) const
{
    return AceType::InstanceOf<RenderScroll>(node) || AceType::InstanceOf<V2::RenderGridScroll>(node) ||
           AceType::InstanceOf<V2::RenderList>(node);
}

Axis ScrollBarProxy::GetScrollableAxis(const RefPtr<RenderNode>& node) const
{
    auto renderScroll = AceType::DynamicCast<RenderScroll>(node);
    if (renderScroll) {
        return renderScroll->GetAxis();
    }
    auto renderGridScroll = AceType::DynamicCast<V2::RenderGridScroll>(node);
    if (renderGridScroll) {
        return renderGridScroll->GetAxis();
    }
    auto renderList = AceType::DynamicCast<V2::RenderList>(node);
    if (renderList) {
        return renderList->GetAxis();
    }
    return Axis::NONE;
}

Size ScrollBarProxy::GetScrollableChildSize(
    const RefPtr<RenderNode>& scrollable, const Size& scrollableChildSize, Axis scrollBarAxis) const
{
    Size result = scrollableChildSize;
    auto renderScroll = AceType::DynamicCast<RenderScroll>(scrollable);
    if (renderScroll) {
        scrollBarAxis == Axis::VERTICAL ? result.SetHeight(renderScroll->GetEstimatedHeight())
                                        : result.SetWidth(renderScroll->GetEstimatedHeight());
        return result;
    }
    auto renderGridScroll = AceType::DynamicCast<V2::RenderGridScroll>(scrollable);
    if (renderGridScroll) {
        scrollBarAxis == Axis::VERTICAL ? result.SetHeight(renderGridScroll->GetEstimatedHeight())
                                        : result.SetWidth(renderGridScroll->GetEstimatedHeight());
        return result;
    }
    auto renderList = AceType::DynamicCast<V2::RenderList>(scrollable);
    if (renderList) {
        scrollBarAxis == Axis::VERTICAL ? result.SetHeight(renderList->GetEstimatedHeight())
                                        : result.SetWidth(renderList->GetEstimatedHeight());
        return result;
    }
    return result;
}

void ScrollBarProxy::AdjustParam(const RefPtr<RenderNode>& scrollable, Axis scrollBarAxis, Axis& scrollableAxis,
    Size& scrollableChildSize, Offset& scrollableChildPosition) const
{
    auto renderScroll = AceType::DynamicCast<RenderScroll>(scrollable);
    if (renderScroll) {
        scrollBarAxis == Axis::VERTICAL ? scrollableChildSize.SetHeight(renderScroll->GetEstimatedHeight())
                                        : scrollableChildSize.SetWidth(renderScroll->GetEstimatedHeight());
        scrollableAxis = renderScroll->GetAxis();
        scrollableChildPosition = renderScroll->GetLastOffset();
        return;
    }

    auto renderGridScroll = AceType::DynamicCast<V2::RenderGridScroll>(scrollable);
    if (renderGridScroll) {
        scrollBarAxis == Axis::VERTICAL ? scrollableChildSize.SetHeight(renderGridScroll->GetEstimatedHeight())
                                        : scrollableChildSize.SetWidth(renderGridScroll->GetEstimatedHeight());
        scrollableAxis = renderGridScroll->GetAxis();
        scrollableChildPosition = renderGridScroll->GetLastOffset();
        return;
    }

    auto renderList = AceType::DynamicCast<V2::RenderList>(scrollable);
    if (renderList) {
        scrollBarAxis == Axis::VERTICAL ? scrollableChildSize.SetHeight(renderList->GetEstimatedHeight())
                                        : scrollableChildSize.SetWidth(renderList->GetEstimatedHeight());
        scrollableAxis = renderList->GetAxis();
        scrollableChildPosition = renderList->GetLastOffset();
    }
}

} // namespace OHOS::Ace