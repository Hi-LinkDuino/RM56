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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_H

#include <vector>

#include "base/memory/ace_type.h"
#include "core/components/common/properties/edge.h"
#include "core/components/scroll/scrollable.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_bar_size_animation.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderTabBar : public RenderNode {
    DECLARE_ACE_TYPE(RenderTabBar, RenderNode)

public:
    ~RenderTabBar() override = default;
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    using UpdateIndexFunc = std::function<void(int32_t index)>;
    void RegisterCallback(const UpdateIndexFunc& callback)
    {
        callback_ = callback;
    }

    bool IsRightToLeft() const
    {
        return GetTextDirection() == TextDirection::RTL && !isVertical_;
    }
    void SetIndex(int32_t index, bool force = false);
    void UpdateIndicatorStyle(const RefPtr<Component>& component);
    void HandleFocusEvent(bool focus);

    void OnPaintFinish() override;

    int32_t GetTabsSize() const
    {
        return tabsSize_;
    }

    TabBarMode GetBarMode() const
    {
        return mode_;
    }

    BarPosition GetBarPosition() const
    {
        return barPosition_;
    }

protected:
    RenderTabBar();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    LayoutParam MakeInnerLayoutParam() const;
    LayoutParam MakeIndicatorLayoutParam(const RefPtr<RenderNode>& item) const;
    Offset MakeIndicatorOffset(const RefPtr<RenderNode>& item) const;

    double MaxScrollableWidth() const;
    double MaxScrollableHeight() const;

    Size GetTabItemChildLayoutSize(const RefPtr<RenderNode>& child) const;
    void ApplyGradientColor();
    TabBarIndicatorStyle GetIndicatorStyle(const RefPtr<BoxComponent>& component) const;
    void InitScrollableOffset(TabBarMode mode);

    TabBarMode mode_ = TabBarMode::FIXED;
    TabBarIndicatorType indicatorSize_ = TabBarIndicatorType::TAB;
    int32_t index_ = 0;
    int32_t tabsSize_ = 0;
    // mark update offset flag, which caused by index changed
    bool needUpdateOffset_ = false;
    std::vector<Offset> tabItemOffsets_;
    std::vector<double> tabsWidth_;
    std::vector<double> tabsHeight_;

    Offset scrollableOffset_;
    double tabBarWidth_ = 0.0;
    double actualWidth_ = 0.0;
    double actualHeight_ = 0.0;

    Edge padding_ { 0.0 };
    Edge indicatorPadding_ { 0.0 };

    RefPtr<RenderNode> indicator_;
    TabBarIndicatorStyle indicatorStyle_ = TabBarIndicatorStyle::DEFAULT;
    Dimension activeIndicatorMinWidth_;
    bool initialUpdate_ = true;
    bool isFirstLayout_ = true;
    bool onFocused_ = false;
    Color focusAnimationColor_ = Color::WHITE;
    Dimension focusRadiusDimension_;
    Dimension gradientWidth_;

private:
    bool isVertical_ = false;

    void Initialize();
    void PerformLayoutChildren(const LayoutParam& innerLayoutParam);
    void LayoutChildren();
    void UpdatePosition();
    void HandleClickedEvent(const ClickInfo& info);
    void AccessibilityScroll(const bool isAdd);
    void AccessibilityClick();
    void InitAccessibilityEventListener();
    bool HandleScrollablePosition(double);
    bool IsScrollable() const
    {
        return TabBarMode::SCROLLABEL == mode_ || TabBarMode::FIXED_START == mode_;
    }
    RefPtr<RenderNode> GetChildByIndex(int32_t index) const;

    void FlushIndex(const RefPtr<TabController>& controller);

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<Scrollable> scrollable_;

    UpdateIndexFunc callback_;
    RefPtr<TabBarSizeAnimation> tabBarSizeAnimation_;
    int accessibilityIndex_ = -1;
    BarPosition barPosition_ = BarPosition::START;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_H
