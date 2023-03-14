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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_RENDER_DRAG_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_RENDER_DRAG_BAR_H

#include "core/animation/animator.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderDragBar : public RenderNode {
    DECLARE_ACE_TYPE(RenderDragBar, RenderNode);

public:
    using ClickArrowCallback = std::function<void()>;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void ShowArrow(bool show);
    void ShowInPanelMode(PanelMode mode);
    void UpdateDrawPoint();
    void AnimateToStatusBarPadding(int32_t duration);
    void SetStatusBarHeight(double height);
    void HandleClick(const Offset& clickPosition);
    void HandleTouchDown(const Offset& downPoint);
    void HandleTouchMove(const Offset& movePoint);
    void HandleTouchUp();

    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void OnPaintFinish() override;
    void OnStatusChanged(RenderStatus renderStatus) override;

    PanelMode GetPanelMode() const
    {
        return showMode_;
    }

    bool HasClickArrowCallback() const
    {
        return (clickArrowCallback_ != nullptr);
    }

    void SetClickArrowCallback(const ClickArrowCallback& callback)
    {
        clickArrowCallback_ = callback;
    }

    void SetFullScreenMode(bool fullScreenMode)
    {
        fullScreenMode_ = fullScreenMode;
    }

    double GetStatusBarHeight() const
    {
        return statusBarHeight_.Value();
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    Offset iconOffset_;
    Offset barLeftPoint_;
    Offset barCenterPoint_;
    Offset barRightPoint_;
    Color barColor_ = Color(0xffb3b3b3);
    double barWidth_ = 4.0;
    double scaleX_ = 1.0;
    double scaleY_ = 1.0;
    double opacity_ = 1.0;
    uint8_t alpha_ = UINT8_MAX;
    PanelMode showMode_ = PanelMode::HALF;
    bool hasDragBar_ = true;

    Offset dragOffset_;
    Offset downPoint_;
    double dragRangeX_ = 0.0;
    double dragRangeY_ = 0.0;
    double scaleIcon_ = 1.0;
    double scaleWidth_ = 1.0;

private:
    void InitializeRecognizer();
    void DoStyleAnimation(const Offset& left, const Offset& center, const Offset& right);
    void FadingOut();
    void Stretching();

    Rect imgTouchRegion_;
    ClickArrowCallback clickArrowCallback_;
    Dimension hotRegionHeight_ = 0.0_vp;  // height in vp
    Dimension statusBarHeight_ = 0.0_vp;  // height in vp
    RenderStatus renderStatus_ = RenderStatus::DEFAULT;
    bool fullScreenMode_ = false;

    RefPtr<ClickRecognizer> clickDetector_;
    RefPtr<RawRecognizer> touchDetector_;
    RefPtr<Animator> animator_;
    RefPtr<Animator> barTouchController_;
    RefPtr<Animator> barRangeController_;
    RefPtr<Animator> barStyleController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_RENDER_DRAG_BAR_H
