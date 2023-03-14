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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_RENDER_SELECT_POPUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_RENDER_SELECT_POPUP_H

#include <vector>

#include "base/geometry/offset.h"
#include "core/components/box/render_box.h"
#include "core/components/option/render_option.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/root/render_root.h"
#include "core/components/scroll/render_scroll.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/slider/render_slider.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderSelectPopup : public RenderNode {
    DECLARE_ACE_TYPE(RenderSelectPopup, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    RenderSelectPopup();
    ~RenderSelectPopup() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;

    void UpdateRenders()
    {
        ClearReferenceRenders();
        GetReferenceRenders();
    }

    RefPtr<SelectPopupComponent> GetSelectPopupComponent()
    {
        return selectPopup_;
    }

    void SetIsWattingForAnimationStart(bool isWattingForAnimationStart)
    {
        isWattingForAnimationStart_ = isWattingForAnimationStart;
    }

    bool HandleMouseEvent(const MouseEvent& event) override;

    void ProcessTouchDown(const TouchEventInfo& info);
    void ProcessTouchUp(const TouchEventInfo& info);

protected:
    bool isFullScreen_ = true;
    bool isWattingForAnimationStart_ = false;
    Color tvBackColor_;
    Offset globalRightBottom_;
    Offset childPosition_;
    LayoutParam childLayoutParam_;
    RefPtr<RenderScroll> renderScroll_;
    std::vector<RefPtr<RenderOption>> renderOptions_;
    Dimension optionInterval_;
    Dimension rrectSize_;

    bool isContextMenu_ = false;
private:
    void AdjustTvChildVerticalLayout(const Size& size, double& y, double& height);
    void AdjustTvChildHorizontalLayout(const Size& size, double& x, double& width);
    void AdjustChildVerticalLayout(const Size& size, double& y, double& height);
    void AdjustChildHorizontalLayout(const Size& size, double& x, double& width);
    void AdjustChildLayout(Size& size);

    void CreateAnimation();
    void CreatePopupAnimation(bool isMenu);

    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleRawEvent(const Offset& clickPosition);

    void GetReferenceRenders();
    void GetReferenceRenders(const RefPtr<RenderNode>& render);
    void ClearReferenceRenders();

    bool ScreenDirectionSwitched();

    RefPtr<RawRecognizer> rawDetector_;
    RefPtr<FreeDragRecognizer> dragDetector_;
    RefPtr<LongPressRecognizer> longPressDetector_;
    RefPtr<ClickRecognizer> clickDetector_;
    std::size_t optionSize_ = SELECT_DEFAULT_OPTION_SIZE;
    RefPtr<SelectPopupComponent> selectPopup_;
    Offset selectLeftTop_;
    Offset selectRightBottom_;

    TouchRegion touchRegion_;
    Dimension minWidth_;
    Dimension verticalSpacing_;
    Dimension horizontalSpacing_;
    Dimension contentSpacing_;

    bool screenHorizontal_ = false;
    bool screenVertical_ = false;
    bool animationCreated_ = false;

    double normalPadding_ = 0.0;
    RefPtr<SelectTheme> theme_;

    RefPtr<RenderNode> renderRoot_;
    RefPtr<RenderPositioned> renderPositioned_;
    RefPtr<RenderBox> renderTitleBox_;
    Offset firstFingerDownOffset_;
    Offset firstFingerUpOffset_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_POPUP_RENDER_SELECT_POPUP_H
