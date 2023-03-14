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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_RENDER_SLIDING_PANEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_RENDER_SLIDING_PANEL_H

#include "core/animation/animator.h"
#include "core/components/box/render_box.h"
#include "core/components/drag_bar/render_drag_bar.h"
#include "core/components/panel/sliding_panel_component.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class ACE_EXPORT SlidingPanelSizeChangeEvent : public BaseEventInfo, public EventToJSONStringAdapter {
    DECLARE_RELATIONSHIP_OF_CLASSES(SlidingPanelSizeChangeEvent, BaseEventInfo, EventToJSONStringAdapter);

public:
    SlidingPanelSizeChangeEvent(PanelMode mode, double width, double height)
        : BaseEventInfo("SlidingPanelSizeChangeEvent"), mode_(mode), width_(width), height_(height)
    {}

    ~SlidingPanelSizeChangeEvent() = default;

    double GetWidth() const
    {
        return width_;
    }

    double GetHeight() const
    {
        return height_;
    }

    PanelMode GetMode() const
    {
        return mode_;
    }

    std::string ToJSONString() const override;

private:
    PanelMode mode_ = PanelMode::HALF;
    double width_ = 0.0;
    double height_ = 0.0;
};

class RenderSlidingPanel : public RenderNode {
    DECLARE_ACE_TYPE(RenderSlidingPanel, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void OnPaintFinish() override;
    void PerformLayout() override;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;

    void ResetContentHeight();
    void OnAnimationStop();

    double GetBlankHeight() const
    {
        return blankHeight_;
    }

    const RefPtr<RenderDragBar>& GetDragBar() const
    {
        return dragBar_;
    }

    // used for inspector node in PC preview
    int32_t GetPanelId() const
    {
        return panelId_;
    }

    void SetPanelId(int32_t panelId)
    {
        panelId_ = panelId;
    }

    PanelType GetPanelType() const
    {
        return type_;
    }

    PanelMode GetPanelMode() const
    {
        return mode_;
    }

    bool HasDragBar() const
    {
        if (dragBar_) {
            return true;
        }
        return false;
    }

    const Dimension& GetFullHeight() const
    {
        return fullHeight_.first;
    }

    const Dimension& GetHalfHeight() const
    {
        return halfHeight_.first;
    }

    const Dimension& GetMiniHeight() const
    {
        return miniHeight_.first;
    }

    void UpdateTouchRect() override;

    void Dump() override;

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    RefPtr<RenderDragBar> dragBar_;
    double blankHeight_ = 0.0;
    PanelMode mode_ = PanelMode::FULL;
    PanelMode previousMode_ = PanelMode::HALF;
    PanelType type_ = PanelType::FOLDABLE_BAR;
    bool hasBoxStyle_ = false;
    std::function<void(const std::shared_ptr<BaseEventInfo>&)> onSizeChange_;

private:
    void InitializeRecognizer();
    bool InitializeLayoutProps();
    void SetDragBarCallBack();
    void FireSizeChangeEvent();
    void HandleDragStart(const Offset& startPoint);
    void HandleDragUpdate(const Offset& currentPoint);
    void HandleDragEnd(const Offset& endPoint, double velocity);
    void CalculateModeTypeMini(double dragLen, double velocity);
    void CalculateModeTypeFold(double dragLen, double velocity);
    void CalculateModeTypeTemp(double dragLen, double velocity);
    void AnimateTo(double blankHeight, PanelMode mode);
    void AppendBlankHeightAnimation(double blankHeight, PanelMode mode);
    void InnerLayout();
    void CheckHeightValidity();

    RefPtr<RenderNode> boxForBlank_;
    RefPtr<Animator> animator_;
    RefPtr<DragRecognizer> dragDetector_;
    std::unordered_map<PanelMode, double> defaultBlankHeights_;
    double minBlankHeight_ = 0.0;
    bool isFirstUpdate_ = true;
    bool isFirstLayout_ = true;
    Offset dragStartPoint_;
    double dragStartBlankHeight_ = 0.0;
    bool isAnimating_ = false;
    double fullHalfBoundary_ = 0.0;
    double halfMiniBoundary_ = 0.0;
    double fullMiniBoundary_ = 0.0;
    // used for inspector node in PC preview
    int32_t panelId_ = -1;
    std::pair<Dimension, bool> miniHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> halfHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> fullHeight_ = { 0.0_vp, false };
    Size previousSize_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_RENDER_SLIDING_PANEL_H
