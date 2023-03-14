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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_RENDER_TEXT_OVERLAY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_RENDER_TEXT_OVERLAY_H

#include <functional>

#include "core/components/box/render_box.h"
#include "core/components/clip/render_clip.h"
#include "core/components/select_popup/render_select_popup.h"
#include "core/components/slider/render_slider.h"
#include "core/components/text_field/render_text_field.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/overlay_show_option.h"
#include "core/pipeline/base/render_node.h"
#include "frameworks/base/utils/system_properties.h"

namespace OHOS::Ace {

const Offset DOT1_POSITION = Offset(6.25, 6.75);
const Offset DOT2_POSITION = Offset(17.75, 6.75);
const Offset DOT3_POSITION = Offset(6.25, 17.25);
const Offset DOT4_POSITION = Offset(17.75, 17.25);
const Dimension STROKE_MAX_WIDTH = 4.0_vp;
const Dimension STROKE_MIN_WIDTH = 1.5_vp;

using StartAnimationCallback = std::function<void(const TweenOption&, const TweenOption&, bool, bool)>;

class RenderTextOverlay : public RenderNode {
    DECLARE_ACE_TYPE(RenderTextOverlay, RenderNode)

public:
    RenderTextOverlay();
    ~RenderTextOverlay() override;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;

    void PopOverlay();
    void OnFocusChange(RenderStatus renderStatus);
    void SetOnRebuild(const std::function<void(bool, bool, bool, bool, bool)>& onRebuild);
    void SetStartAnimationCallback(const StartAnimationCallback& value);

    void SetIsAnimationStarted(bool isAnimationStart);
    bool IsAnimationStarted() const;
    void SetIsAnimationStopped(bool isAnimationStopped);
    double GetHorizonOffsetForAnimation() const;
    bool HandleMouseEvent(const MouseEvent& event) override;

protected:
    void OnPaintFinish() override;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void RestoreMoreButtonStyle();
    void InitRenderChild(const RefPtr<RenderNode>& render);
    void ResetRenderChild();
    bool HasToolBarOnly() const; // Has tool bar and has no menu.

    bool hasMenu_ = false;
    bool hasPoped_ = false;
    bool reverse_ = false;
    bool showMagnifier_ = false;
    bool showOverlay_ = true;
    bool isDragging_ = false;
    bool isSingleHandle_ = false;
    bool isStartDrag_ = false;
    bool isEndDrag_ = false;
    bool isTouchStartDrag_ = false;
    bool isTouchEndDrag_ = false;
    bool isAnimationStarted_ = true;
    bool isAnimationStopped_ = true;
    float rotateDegree_ = 0.0f;
    double clipWidth_ = 0.0;
    double lineHeight_ = 0.0;

    // Mark startHandle and endHandle index
    int32_t startIndex_ = 0;
    int32_t endIndex_ = 0;

    OverlayShowOption showOption_;

    Dimension handleRadius_;
    Dimension handleRadiusInner_;
    Dimension handleDiameter_;
    Dimension handleDiameterInner_;
    Dimension menuSpacingWithText_;
    Dimension menuSpacingWithHandle_;
    Dimension strokeWidth_ = STROKE_MAX_WIDTH;

    Color handleColor_;
    Color handleColorInner_;

    Offset startHandleOffset_;
    Offset endHandleOffset_;
    Offset startHandleCenter_;
    Offset endHandleCenter_;

    // dot offset align to arrow endings
    Offset dot1StartOffset_;
    Offset dot2StartOffset_;
    Offset dot3StartOffset_;
    Offset dot4StartOffset_;

    // line ending offset compare to dot position in animation
    Offset dot2Offset_;
    Offset dot3Offset_;
    Offset dot4Offset_;

    Offset moreButtonPosition_;

    Rect clipRect_;
    TouchRegion startHandleRegion_;
    TouchRegion endHandleRegion_;

    TextDirection textDirection_ = TextDirection::LTR;
    TextDirection realTextDirection_ = TextDirection::LTR;

    RefPtr<RenderBox> renderBox_;
    RefPtr<RenderClip> renderClip_;
    RefPtr<RenderSelectPopup> renderMenu_;
    RefPtr<ClickRecognizer> clickDetector_;
    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<RawRecognizer> touchDetector_;
    WeakPtr<RenderTextField> weakTextField_;
    RefPtr<Animator> controller_;
    WeakPtr<TextOverlayComponent> overlayComponent_;

private:
    void UpdateWeakTextField(const RefPtr<TextOverlayComponent>& overlay);
    void BindBackendEvent(const RefPtr<TextOverlayComponent>& overlay);
    void BindBackendEventV2(const RefPtr<TextOverlayComponent>& overlay);
    void RemoveBackendEvent(const RefPtr<TextOverlayComponent>& overlay);

    void HandleClick(const Offset& clickOffset);
    void HandleDragStart(const Offset& startOffset);
    void HandleDragUpdateAndEnd(const Offset& offset);

    void HandleCut();
    void HandleCopy();
    void HandlePaste();
    void HandleCopyAll();
    void HandleMoreButtonClick();

    void StartMoreAnimation(bool reverse);
    void BuildAndStartMoreButtonAnimation();

    void BuildStrokeWidthAnimation(const RefPtr<KeyframeAnimation<Dimension>>& widthAnimation, const Dimension& from,
            const Dimension& to, bool reverse);

    void ProcessFrictionAnimation(double value);
    void BuildFrictionAnimation(const RefPtr<KeyframeAnimation<double>>& animation, double from, double to);

    void ProcessEndPointAnimation(double value);
    void BuildEndPointOffsetAnimation(
            const RefPtr<KeyframeAnimation<double>>& offsetAnimation, double from, double to, bool reverse);

    Offset ComputeChildPosition(const RefPtr<RenderNode>& child);

    void InitAnimation();

    bool needStartTwinkling_ = true;
    bool needCloseKeyboard_ = true;
    bool isAnimationInited_ = false;
    bool animateUntilPaint_ = false;
    double horizonOffsetForAnimation_ = 0.0;
    double childRightBoundary_ = 0.0;

    TweenOption tweenOptionIn_;
    TweenOption tweenOptionOut_;
    TweenOption innerTweenOptionIn_;
    TweenOption innerTweenOptionOut_;

    CommonCallback onCut_;
    CommonCallback onCopy_;
    CommonCallback onPaste_;
    CopyAllCallback onCopyAll_;
    StartHandleMoveCallback onStartHandleMove_;
    EndHandleMoveCallback onEndHandleMove_;
    StartAnimationCallback startAnimation_;

    std::function<void(bool, bool)> onFocusChange_;
    std::function<void(bool, bool, bool, bool, bool)> onRebuild_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_RENDER_TEXT_OVERLAY_H
