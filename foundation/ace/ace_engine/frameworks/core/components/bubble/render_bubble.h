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

#include "core/components/bubble/bubble_component.h"
#include "core/components/common/properties/edge.h"
#include "core/components/slider/render_slider.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_RENDER_BUBBLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_RENDER_BUBBLE_H

namespace OHOS::Ace {

class RenderBubble : public RenderNode {
    DECLARE_ACE_TYPE(RenderBubble, RenderNode);

public:
    ~RenderBubble() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool PopBubble();
    void FirePopEvent();
    bool HandleMouseEvent(const MouseEvent& event) override;

protected:
    enum class ErrorPositionType {
        NORMAL = 0,
        TOP_LEFT_ERROR,
        BOTTOM_RIGHT_ERROR,
    };

    RenderBubble();

    void OnHiddenChanged(bool hidden) override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    virtual void UpdateBorderRadius() {}
    void HandleTouch();
    Offset GetChildPosition(const Size& childSize);
    Offset GetPositionWithPlacement(const Size& childSize, const Offset& topPosition, const Offset& bottomPosition,
        const Offset& topArrowPosition, const Offset& bottomArrowPosition);
    Offset FitToScreen(const Offset& fitPosition, const Size& childSize);
    ErrorPositionType GetErrorPositionType(const Offset& childOffset, const Size& childSize);
    void UpdateAccessibilityInfo(Size size, Offset offset);
    void InitAccessibilityEventListener();

    static const Dimension BUBBLE_SPACING;

    bool isShow_ = true;
    bool enableArrow_ = true;
    // Is there has enough space for showing arrow.
    bool showTopArrow_ = true;
    bool showBottomArrow_ = true;
    bool useCustom_ = false;
    Edge padding_;
    Edge margin_;
    Border border_;
    Size childSize_;
    Size targetSize_;
    Offset childOffset_;
    Offset targetOffset_;
    Offset arrowPosition_;
    Dimension arrowOffset_;
    Color maskColor_;
    Color backgroundColor_;
    Placement placement_ = Placement::BOTTOM;
    Placement arrowPlacement_ = Placement::TOP;
    ComposeId targetId_;
    std::function<void(const std::string&)> onVisibilityChange_;
    RefPtr<RawRecognizer> rawDetector_;
    RefPtr<BubbleComponent> bubbleComponent_;
    WeakPtr<StackElement> weakStack_;

private:
    void InitArrowState();
    // Get size and position of target by targetId.
    void InitTargetSizeAndPosition();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUBBLE_RENDER_BUBBLE_H
