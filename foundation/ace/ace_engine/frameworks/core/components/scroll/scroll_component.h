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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_COMPONENT_H

#include "base/geometry/axis.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_position_controller.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/scroll_bar/scroll_bar_proxy.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ACE_EXPORT ScrollComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ScrollComponent, SoleChildComponent);

public:
    explicit ScrollComponent(const RefPtr<Component>& child);
    ~ScrollComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void InitScrollBar(const RefPtr<ScrollBarTheme>& theme, const std::pair<bool, Color>& scrollBarColor,
        const std::pair<bool, Dimension>& scrollBarWidth, EdgeEffect edgeEffect);

    void SetAxisDirection(Axis axisDirection)
    {
        if (axisDirection == Axis::FREE) {
            LOGE("Invalid direction: %{public}d", axisDirection);
            return;
        }
        axisDirection_ = axisDirection;
    }

    Axis GetAxisDirection() const
    {
        return axisDirection_;
    }

    void SetEnable(bool enable)
    {
        enable_ = enable;
    }

    bool GetEnable() const
    {
        return enable_;
    }

    void SetPadding(const Edge& padding)
    {
        if (padding.IsValid()) {
            padding_ = padding;
        } else {
            LOGE("Invalid padding input.");
        }
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    void SetScrollPositionController(const RefPtr<ScrollPositionController>& positionController)
    {
        positionController_ = positionController;
    }

    RefPtr<ScrollPositionController> GetScrollPositionController() const
    {
        return positionController_;
    }

    void SetScrollPage(bool scrollPage)
    {
        scrollPage_ = scrollPage;
    }

    bool GetScrollPage() const
    {
        return scrollPage_;
    }

    void SetScrollBar(const RefPtr<ScrollBar>& scrollBar)
    {
        scrollBar_ = scrollBar;
    }

    RefPtr<ScrollBar> GetScrollBar() const
    {
        return scrollBar_;
    }

    const RefPtr<ScrollEdgeEffect>& GetScrollEffect() const
    {
        return scrollEffect_;
    }

    void SetScrollEffect(const RefPtr<ScrollEdgeEffect>& scrollEffect)
    {
        isEffectSetted_ = true;
        scrollEffect_ = scrollEffect;
    }

    bool IsEffectSetted() const
    {
        return isEffectSetted_;
    }

    void SetOnReachStart(const EventMarker& onReachStart)
    {
        onReachStart_ = onReachStart;
    }

    void SetOnReachEnd(const EventMarker& onReachEnd)
    {
        onReachEnd_ = onReachEnd;
    }

    void SetOnReachTop(const EventMarker& onReachTop)
    {
        onReachTop_ = onReachTop;
    }

    void SetOnReachBottom(const EventMarker& onReachBottom)
    {
        onReachBottom_ = onReachBottom;
    }

    const EventMarker& GetOnReachStart() const
    {
        return onReachStart_;
    }

    const EventMarker& GetOnReachEnd() const
    {
        return onReachEnd_;
    }

    const EventMarker& GetOnReachTop() const
    {
        return onReachTop_;
    }

    const EventMarker& GetOnReachBottom() const
    {
        return onReachBottom_;
    }

    const EventMarker& GetOnScroll() const
    {
        return onScroll_;
    }

    void SetOnScroll(const EventMarker& onScroll)
    {
        onScroll_ = onScroll;
    }

    const EventMarker& GetOnScrollEdge() const
    {
        return onScrollEdge_;
    }

    void SetOnScrollEdge(const EventMarker& onScrollEdge)
    {
        onScrollEdge_ = onScrollEdge;
    }

    const EventMarker& GetOnScrollEnd() const
    {
        return onScrollEnd_;
    }

    void SetOnScrollEnd(const EventMarker& onScrollEnd)
    {
        onScrollEnd_ = onScrollEnd;
    }

    void SetDisplayMode(DisplayMode displayMode)
    {
        if (!scrollBar_) {
            return;
        }
        scrollBar_->SetDisplayMode(displayMode);
    }

    void SetScrollBarColor(const Color& scrollBarColor)
    {
        if (!scrollBar_) {
            return;
        }
        scrollBar_->SetForegroundColor(scrollBarColor);
    }

    void SetScrollBarWidth(const Dimension& scrollBarWidth)
    {
        if (!scrollBar_) {
            return;
        }
        scrollBar_->SetInactiveWidth(scrollBarWidth);
        scrollBar_->SetNormalWidth(scrollBarWidth);
        scrollBar_->SetActiveWidth(scrollBarWidth);
        scrollBar_->SetTouchWidth(scrollBarWidth);
    }

    void SetScrollBarProxy(const RefPtr<ScrollBarProxy>& scrollBarProxy)
    {
        scrollBarProxy_ = scrollBarProxy;
    }

    const RefPtr<ScrollBarProxy>& GetScrollBarProxy() const
    {
        return scrollBarProxy_;
    }

private:
    Edge padding_;
    Axis axisDirection_ = Axis::VERTICAL;
    RefPtr<ScrollPositionController> positionController_;
    bool scrollPage_ = false;
    RefPtr<ScrollBar> scrollBar_;
    bool enable_ = true;
    bool isEffectSetted_ = false;
    RefPtr<ScrollEdgeEffect> scrollEffect_;
    RefPtr<ScrollBarProxy> scrollBarProxy_;

    // for scroll reaching the edge event
    EventMarker onReachStart_;
    EventMarker onReachEnd_;
    EventMarker onReachTop_;
    EventMarker onReachBottom_;

    EventMarker onScroll_;
    EventMarker onScrollEdge_;
    EventMarker onScrollEnd_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_COMPONENT_H
