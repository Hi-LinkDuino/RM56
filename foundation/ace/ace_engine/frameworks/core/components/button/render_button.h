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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_RENDER_BUTTON_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_RENDER_BUTTON_H

#include "base/utils/system_properties.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/button/button_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/touch_listener/render_touch_listener.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr Dimension ARC_BUTTON_WIDTH = 169.0_vp;
constexpr Dimension ARC_BUTTON_HEIGHT = 48.0_vp;

// Definition for animation
constexpr double INIT_SCALE = 1.0;

class RenderButton : public RenderNode {
    DECLARE_ACE_TYPE(RenderButton, RenderNode);

public:
    ~RenderButton() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;
    void OnStatusStyleChanged(const VisualState state) override;

    void OnAttachContext() override
    {
        width_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderButton = weak.Upgrade();
            if (renderButton) {
                renderButton->MarkNeedLayout();
            }
        });
        height_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderButton = weak.Upgrade();
            if (renderButton) {
                renderButton->MarkNeedLayout();
            }
        });
        backgroundColor_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderButton = weak.Upgrade();
            if (renderButton) {
                renderButton->MarkNeedLayout();
            }
        });
        clickedColor_.SetContextAndCallback(context_, [weak = WeakClaim(this)] {
            auto renderButton = weak.Upgrade();
            if (renderButton) {
                renderButton->MarkNeedLayout();
            }
        });
    }

    void HandleFocusEvent(bool isFocus);
    void HandleClickEvent(const ClickInfo& info);
    void HandleClickEvent();
    void HandleKeyEnterEvent(const ClickInfo& info);
    void HandleKeyEnterEvent();
    void HandleRemoteMessageEvent(const ClickInfo& info);
    void HandleRemoteMessageEvent();
    void DisplayFocusAnimation();
    void PlayFocusAnimation(bool isFocus);
    void AnimateMouseHoverEnter() override;
    void AnimateMouseHoverExit() override;
    WeakPtr<RenderNode> CheckHoverNode() override;

    bool IsDisabled() const
    {
        return isDisabled_;
    }

    void SetClickedColor(const Color& clickColor)
    {
        // do not trigger animation
        clickedColor_.SetValue(clickColor.GetValue());
        setClickColor_ = true;
    }

    ButtonType GetButtonType() const
    {
        return type_;
    }

    bool GetStateEffect() const
    {
        return stateEffect_;
    }
    RefPtr<Component> GetComponent() override
    {
        return buttonComponent_;
    }
    Color GetClickedColor()
    {
        return clickedColor_;
    }

    Color GetBackgroundColor() const
    {
        return buttonComponent_->GetBackgroundColor();
    }

    void SendAccessibilityEvent();

protected:
    RenderButton();
    virtual Size Measure() = 0;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void Initialize();
    void InitAccessibilityEventListener();
    void HandleTouchEvent(bool isTouch);
    void HandleMoveEvent(const TouchEventInfo& info);
    void UpdateDownloadStyles(const RefPtr<ButtonComponent>& button);
    void SetProgress(uint32_t progress);
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void OnMouseClickDownAnimation() override;
    void OnMouseClickUpAnimation() override;

    bool NeedLayoutExtendToParant() const
    {
        return (layoutFlag_ & LAYOUT_FLAG_EXTEND_TO_PARENT) == LAYOUT_FLAG_EXTEND_TO_PARENT;
    }

    RefPtr<ButtonComponent> buttonComponent_;
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<Animator> progressController_;

    ButtonType type_ = ButtonType::NORMAL;
    Size buttonSize_;
    Offset offsetDelta_;
    double rrectRadius_ = 0.0;
    double widthDelta_ = 0.0;
    double progressWidth_ = 0.0;
    double progressPercent_ = 0.0;
    double progressDiameter_ = 0.0;
    double opacity_ = 1.0;
    double maskingOpacity_ = 0.0;
    double ratio_ = 0.0;
    double previousValue_ = 0.0;
    double percentChange_ = 0.0;
    float scale_ = 1.0f;
    bool stateEffect_ = true;

    bool needUpdateAnimation_ = false;
    bool isInnerBorder_ = false;
    bool isClicked_ = false;
    bool isDisabled_ = false;
    bool isFocus_ = false;
    bool isWatch_ = false;
    bool isTv_ = false;
    bool isPhone_ = false;
    bool widthDefined_ = false;
    bool heightDefined_ = false;
    bool progressDisplay_ = false;
    bool animationRunning_ = false;
    bool isLastFrame_ = false;
    bool isOpacityAnimation_ = false;
    bool isTouchAnimation_ = false;
    bool isHover_ = false;
    bool needFocusColor_ = false;
    bool needHoverColor_ = false;
    bool isMoveEventValid_ = false;
    bool setClickColor_ = false;
    uint32_t layoutFlag_ = 0;

    Color focusAnimationColor_;
    Color progressColor_;
    Color progressFocusColor_;
    Color defaultClickedColor_;
    AnimatableColor clickedColor_;
    AnimatableColor backgroundColor_;
    BorderEdge borderEdge_;
    std::function<void(const ClickInfo&)> onClickWithInfo_;
    std::function<void()> onClick_;
    std::chrono::steady_clock::time_point previousUpdateTime_ = std::chrono::steady_clock::now();
    std::chrono::duration<double> animationDuring_;

private:
    void UpdateAnimationParam(double value);
    void UpdateFocusAnimation(double value);
    void UpdateProgressAnimation();
    void PlayAnimation(double start, double end, int32_t duration, const FillMode& fillMode = FillMode::FORWARDS);
    void PlayTouchAnimation();
    void PlayClickAnimation();
    void PlayClickScaleAnimation(float keyTime, int32_t duration);
    void UpdateAccessibility();
    void SetChildrenLayoutSize();
    void SetChildrenAlignment();
    Size CalculateLayoutSize();
    bool NeedAdaptiveChild();
    bool NeedConstrain();
    void ResetController(RefPtr<Animator>& controller);
    void CreateFloatAnimation(RefPtr<KeyframeAnimation<float>>& floatAnimation, float beginValue, float endValue);

    AnimatableDimension width_;
    AnimatableDimension height_;
    Dimension minWidth_;
    bool valueChanged_ = false;
    bool isClickAnimation_ = false;
    double startValue_ = 0.0;
    double endValue_ = 0.0;
    Edge arcInitEdge_;
    Size layoutSize_;
    Size childrenSize_;

    RefPtr<Animator> controller_;
    RefPtr<Animator> hoverControllerEnter_;
    RefPtr<Animator> hoverControllerExit_;
    RefPtr<Animator> clickControllerDown_;
    RefPtr<Animator> clickControllerUp_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationEnter_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationExit_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationUp_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationDown_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_RENDER_BUTTON_H
