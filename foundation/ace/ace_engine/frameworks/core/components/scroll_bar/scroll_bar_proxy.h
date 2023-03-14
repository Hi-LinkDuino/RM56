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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_PROXY_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

struct ScrollableNodeInfo {
    WeakPtr<RenderNode> scrollableNode;
    std::function<bool(double, int32_t source)> onPositionChanged;

    bool operator==(const ScrollableNodeInfo& info) const
    {
        return scrollableNode == info.scrollableNode;
    }
};

class RenderScrollBar;
class ACE_EXPORT ScrollBarProxy : public AceType {
    DECLARE_ACE_TYPE(ScrollBarProxy, AceType);

public:
    ScrollBarProxy() = default;
    ~ScrollBarProxy() override = default;

    // Register scrollable node and scroll bar, scrollable node and scroll bar communicate through proxy.
    void RegisterScrollableNode(const ScrollableNodeInfo& scrollableNode);
    void RegisterScrollBar(const WeakPtr<RenderScrollBar>& scrollBar);

    // UnRegister scrollable node and scroll bar.
    void UnRegisterScrollableNode(const WeakPtr<RenderNode>& scrollableNode);
    void UnRegisterScrollBar(const WeakPtr<RenderScrollBar>& scrollBar);

    /*
     * Notify scrollable node to update state, called by scroll bar.
     * @param distance absolute distance that scroll bar has scrolled.
     */
    void NotifyScrollableNode(double distance, const WeakPtr<RenderScrollBar>& weakScrollBar) const;

    /*
     * Notify scroll bar to update state, called by scrollable node.
     * @param distance absolute distance that scrollable node has scrolled.
     */
    void NotifyScrollBar(const WeakPtr<RenderNode>& weakScrollableNode) const;

    /*
     * Start animation of ScrollBar.
     */
    void StartScrollBarAnimator() const;

    /*
     * Stop animation of ScrollBar, and show ScrollBar if needed, when scrollable node is scrolling.
     */
    void StopScrollBarAnimator() const;

private:
    bool CheckScrollable(const RefPtr<RenderNode>& node) const;
    Axis GetScrollableAxis(const RefPtr<RenderNode>& node) const;
    Size GetScrollableChildSize(
        const RefPtr<RenderNode>& scrollable, const Size& scrollableChildSize, Axis scrollBarAxis) const;
    void AdjustParam(const RefPtr<RenderNode>& scrollable, Axis scrollBarAxis, Axis& scrollableAxis,
        Size& scrollableChildSize, Offset& scrollableChildPosition) const;

    std::list<ScrollableNodeInfo> scrollableNodes_;  // Scrollable nodes, like list, grid, scroll, etc.
    std::list<WeakPtr<RenderScrollBar>> scrollBars_; // ScrollBar should effect with scrollable node.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_PROXY_H
