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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_RENDER_SEMI_MODAL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_RENDER_SEMI_MODAL_H

#include "core/animation/animator.h"
#include "core/components/box/render_box.h"
#include "core/components/drag_bar/render_drag_bar.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components/semi_modal/semi_modal_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderSemiModal : public RenderNode {
    DECLARE_ACE_TYPE(RenderSemiModal, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;

    void ResetContentHeight();
    void ExtendContentHeight();
    void OnExtendAnimationEnd();
    void OnAnimationStop();
    void AnimateToExitApp();
    void AnimateToFullWindow(int32_t duration);
    void OnNavigationBarHeightChanged(const Dimension& height);
    void OnStatusBarHeightChanged(const Dimension& height);

    double GetBlankHeight() const
    {
        return blankHeight_;
    }

    const RefPtr<RenderDragBar>& GetDragBar() const
    {
        return dragBar_;
    }

    void Dump() override;

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

private:
    void InitializeRecognizer();
    void HandleClick(const Offset& clickPosition);
    void HandleDragStart(const Offset& startPoint);
    void HandleDragUpdate(const Offset& currentPoint);
    void HandleDragEnd(const Offset& endPoint);
    void FirstLayout();
    void LayoutFullScreen(const RefPtr<RenderNode>& boxForContent);
    double GetScrollContentHeight();
    void AnimateTo(double blankHeight);
    void AppendBlankHeightAnimation(double blankHeight);
    void AppendContentRadiusAnimation(double radius);
    void UpdateDragImg();
    void UpdateMinBlankHeight();
    void UpdateDefaultBlankHeight();
    void ShowUpAnimation();
    void InnerLayout();
    bool IsPageReady() const;
    void MovePage(double delta);
    double UpdateTargetBlankHeight(double oldHeight);

    RefPtr<RenderNode> boxForBlank_;
    RefPtr<ClickRecognizer> clickDetector_;
    RefPtr<Animator> animator_;
    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<RenderDragBar> dragBar_;
    Rect blankTouchRegion_;
    WeakPtr<RenderNode> cachedPage_;
    double cachedContentHeight_ = 0.0;
    double blankHeight_ = -std::numeric_limits<double>::max();
    double normalBlankHeight_ = -std::numeric_limits<double>::max(); // save blank height when keyboard not show up
    double defaultBlankHeight_ = 0.0;
    double minBlankHeight_ = 0.0;
    bool isFirstUpdate_ = true;
    bool isFirstLayout_ = true;
    Offset dragStartPoint_;
    double dragStartBlankHeight_ = 0.0;
    bool canHandleDrag_ = false;
    bool forbiddenSwipe_ = true;
    bool isAnimating_ = false;
    bool isAnimatingToDefault_ = false;
    bool isFullScreen_ = false;
    double navigationHeight_ = 0.0;
    double statusBarHeight_ = 0.0;
    bool updateMinBlank_ = true;
    bool updateDefaultBlank_ = true;
    bool isExit_ = false;
    double inputHeight_ = 0.0;
    bool hasInputHeight_ = false;
    double maxWidth_ = 0.0;
    DeviceOrientation orientation_ = DeviceOrientation::ORIENTATION_UNDEFINED;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_RENDER_SEMI_MODAL_H
