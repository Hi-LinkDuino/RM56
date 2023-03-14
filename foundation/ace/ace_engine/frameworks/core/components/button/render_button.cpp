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

#include "core/components/button/render_button.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "core/accessibility/accessibility_node.h"
#include "core/animation/keyframe_animation.h"
#include "core/common/frontend.h"
#include "core/components/button/button_theme.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/theme/theme_manager.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

// Watch button definitions
constexpr Dimension TEXT_BUTTON_MAX_WIDTH = 116.5_vp;

// Download button definitions
constexpr double DOWNLOAD_FULL_PERCENT = 100.0;
constexpr double MAX_TRANSITION_TIME = 5000.0;
constexpr double MIN_TRANSITION_TIME = 200.0;
constexpr double MILLISECOND_PER_PERCENT = 20.0;
constexpr double SECOND_TO_MILLISECOND = 1000.0;

// Definition for animation
constexpr double TV_EXPAND_SCALE = 1.05;
constexpr double TV_REDUCE_SCALE = 0.95;
constexpr double WATCH_SCALE = 0.8;
constexpr double MASKING_ANIMATION_RATIO = 10.0;
constexpr float KEY_TIME_START = 0.0f;
constexpr float KEY_TIME_MID = 0.5f;
constexpr float KEY_TIME_END = 1.0f;
constexpr float PRESS_UP_OPACITY = 1.0f;
constexpr float PRESS_DOWN_OPACITY = 0.95f;
constexpr int32_t WATCH_DURATION_DOWN = 200;
constexpr int32_t WATCH_DURATION_UP = 250;
constexpr int32_t TV_CLICK_DURATION = 200;
constexpr int32_t TV_FOCUS_SCALE_DURATION = 100;
constexpr int32_t HOVER_ANIMATION_DURATION = 250;
constexpr int32_t PRESS_ANIMATION_DURATION = 100;

} // namespace

RenderButton::RenderButton()
{
    Initialize();
}

void RenderButton::Initialize()
{
    auto wp = AceType::WeakClaim(this);
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp](const TouchEventInfo&) {
        auto button = wp.Upgrade();
        if (button) {
            button->HandleTouchEvent(true);
        }
    });
    touchRecognizer_->SetOnTouchUp([wp](const TouchEventInfo&) {
        auto button = wp.Upgrade();
        if (button) {
            button->HandleTouchEvent(false);
        }
    });
    touchRecognizer_->SetOnTouchCancel([wp](const TouchEventInfo&) {
        auto button = wp.Upgrade();
        if (button) {
            button->HandleTouchEvent(false);
        }
    });
    touchRecognizer_->SetOnTouchMove([wp](const TouchEventInfo& info) {
        auto button = wp.Upgrade();
        if (button) {
            button->HandleMoveEvent(info);
        }
    });

    clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer_->SetOnClick([wp](const ClickInfo& info) {
        auto button = wp.Upgrade();
        if (button) {
            const auto context = button->GetContext().Upgrade();
            if (context && context->GetIsDeclarative()) {
                button->HandleClickEvent(info);
            } else {
                button->HandleClickEvent();
            }
        }
    });
    clickRecognizer_->SetRemoteMessage([wp](const ClickInfo& info) {
        auto button = wp.Upgrade();
        if (button) {
            const auto context = button->GetContext().Upgrade();
            if (context && context->GetIsDeclarative()) {
                button->HandleRemoteMessageEvent(info);
            } else {
                button->HandleRemoteMessageEvent();
            }
        }
    });
}

void RenderButton::InitAccessibilityEventListener()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }
    refNode->AddSupportAction(AceAction::ACTION_ACCESSIBILITY_FOCUS);

    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionFocusImpl([weakPtr]() {
        auto button = weakPtr.Upgrade();
        if (button) {
            button->HandleFocusEvent(true);
        }
    });
}

void RenderButton::OnPaintFinish()
{
    if (isFocus_) {
        if (isTv_) {
            DisplayFocusAnimation();
        }
        if (isPhone_) {
            UpdateFocusAnimation(INIT_SCALE);
        }
    }
    UpdateAccessibility();
    InitAccessibilityEventListener();
}

void RenderButton::UpdateAccessibility()
{
    const auto& context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto viewScale = context->GetViewScale();
    if (NearZero(viewScale)) {
        return;
    }
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    Offset globalOffset = GetGlobalOffset();
    if (isTv_ && isFocus_) {
        Size size = GetLayoutSize();
        Offset scaleCenter =
            Offset(globalOffset.GetX() + size.Width() / 2.0, globalOffset.GetY() + size.Height() / 2.0);
        accessibilityNode->SetScaleCenter(scaleCenter);
        accessibilityNode->SetScale(TV_EXPAND_SCALE);
    }
#endif
    accessibilityNode->SetMarginSize(Size());
    if (!GetAccessibilityText().empty()) {
        accessibilityNode->SetAccessibilityLabel(GetAccessibilityText());
    }
}

void RenderButton::HandleTouchEvent(bool isTouch)
{
    isClicked_ = isTouch;
    if (isClicked_) {
        OnStatusStyleChanged(VisualState::PRESSED);
        isMoveEventValid_ = true;
    } else {
        OnStatusStyleChanged(VisualState::NORMAL);
    }
    if (isMoveEventValid_ || isWatch_) {
        PlayTouchAnimation();
    }
}

void RenderButton::HandleMoveEvent(const TouchEventInfo& info)
{
    if (!isMoveEventValid_) {
        return;
    }
    if (info.GetTouches().empty()) {
        return;
    }
    const auto& locationInfo = info.GetTouches().front();
    double moveDeltaX = locationInfo.GetLocalLocation().GetX();
    double moveDeltaY = locationInfo.GetLocalLocation().GetY();
    if ((moveDeltaX < 0 || moveDeltaX > buttonSize_.Width()) || (moveDeltaY < 0 || moveDeltaY > buttonSize_.Height())) {
        isClicked_ = false;
        MarkNeedRender();
        OnStatusStyleChanged(VisualState::NORMAL);
        isMoveEventValid_ = false;
    }
}

void RenderButton::HandleClickEvent(const ClickInfo& info)
{
    if (!buttonComponent_) {
        return;
    }
    auto onClickWithInfo =
        AceAsyncEvent<void(const ClickInfo&)>::Create(buttonComponent_->GetClickedEventId(), context_);
    if (onClickWithInfo) {
        onClickWithInfo(info);
    }
    PlayClickAnimation();
}

void RenderButton::HandleClickEvent()
{
    if (!buttonComponent_) {
        return;
    }
    auto onClick = AceAsyncEvent<void()>::Create(buttonComponent_->GetClickedEventId(), context_);
    if (onClick) {
        onClick();
    }
    PlayClickAnimation();
}

void RenderButton::HandleKeyEnterEvent(const ClickInfo& info)
{
    if (!buttonComponent_) {
        return;
    }
    auto onEnterWithInfo =
        AceAsyncEvent<void(const ClickInfo&)>::Create(buttonComponent_->GetKeyEnterEventId(), context_);
    if (onEnterWithInfo) {
        onEnterWithInfo(info);
    }
}

void RenderButton::HandleKeyEnterEvent()
{
    if (!buttonComponent_) {
        return;
    }
    auto onEnter = AceAsyncEvent<void()>::Create(buttonComponent_->GetKeyEnterEventId(), context_);
    if (onEnter) {
        onEnter();
    }
}

void RenderButton::HandleRemoteMessageEvent(const ClickInfo& info)
{
    if (!buttonComponent_) {
        return;
    }
    auto onRemoteMessagekWithInfo =
        AceAsyncEvent<void(const ClickInfo&)>::Create(buttonComponent_->GetRemoteMessageEventId(), context_);
    if (onRemoteMessagekWithInfo) {
        onRemoteMessagekWithInfo(info);
    }
    PlayClickAnimation();
}

void RenderButton::HandleRemoteMessageEvent()
{
    if (!buttonComponent_) {
        return;
    }
    auto onRemoteMessage = AceAsyncEvent<void()>::Create(buttonComponent_->GetRemoteMessageEventId(), context_);
    if (onRemoteMessage) {
        onRemoteMessage();
    }
    PlayClickAnimation();
}

void RenderButton::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if ((!touchRecognizer_) || (!clickRecognizer_)) {
        return;
    }
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchRecognizer_);
    result.emplace_back(clickRecognizer_);
}

void RenderButton::HandleFocusEvent(bool isFocus)
{
    isFocus_ = isFocus;
    needFocusColor_ = isFocus_ && isTv_;
    MarkNeedRender();
}

void RenderButton::DisplayFocusAnimation()
{
    if (!animationRunning_ && isTv_) {
        UpdateAnimationParam(TV_EXPAND_SCALE);
    }
}

WeakPtr<RenderNode> RenderButton::CheckHoverNode()
{
    return AceType::WeakClaim<RenderNode>(this);
}

void RenderButton::AnimateMouseHoverEnter()
{
    OnMouseHoverEnterTest();
}
void RenderButton::OnMouseHoverEnterTest()
{
    if (!buttonComponent_) {
        return;
    }
    ButtonType type = buttonComponent_->GetType();
    if (isPhone_ && ((type == ButtonType::TEXT) || (type == ButtonType::NORMAL))) {
        needHoverColor_ = true;
        MarkNeedRender();
    } else {
        ResetController(hoverControllerExit_);
        if (!hoverControllerEnter_) {
            hoverControllerEnter_ = AceType::MakeRefPtr<Animator>(context_);
        }
        scaleAnimationEnter_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        CreateFloatAnimation(scaleAnimationEnter_, 1.0, 1.05);
        hoverControllerEnter_->AddInterpolator(scaleAnimationEnter_);
        hoverControllerEnter_->SetDuration(HOVER_ANIMATION_DURATION);
        hoverControllerEnter_->Play();
        hoverControllerEnter_->SetFillMode(FillMode::FORWARDS);
    }
}

void RenderButton::AnimateMouseHoverExit()
{
    OnMouseHoverExitTest();
}
void RenderButton::OnMouseHoverExitTest()
{
    if (needHoverColor_) {
        needHoverColor_ = false;
        MarkNeedRender();
    } else {
        ResetController(hoverControllerEnter_);
        if (!hoverControllerExit_) {
            hoverControllerExit_ = AceType::MakeRefPtr<Animator>(context_);
        }
        scaleAnimationExit_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        auto begin = scale_;
        CreateFloatAnimation(scaleAnimationExit_, begin, 1.0);
        hoverControllerExit_->AddInterpolator(scaleAnimationExit_);
        hoverControllerExit_->SetDuration(HOVER_ANIMATION_DURATION);
        hoverControllerExit_->Play();
        hoverControllerExit_->SetFillMode(FillMode::FORWARDS);
    }
}

void RenderButton::OnMouseClickDownAnimation()
{
    if (!needHoverColor_) {
        ResetController(clickControllerUp_);
        if (!clickControllerDown_) {
            clickControllerDown_ = AceType::MakeRefPtr<Animator>(context_);
        }
        scaleAnimationDown_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        auto begin = scale_;
        CreateFloatAnimation(scaleAnimationDown_, begin, 1.0);
        clickControllerDown_->AddInterpolator(scaleAnimationDown_);
        clickControllerDown_->SetDuration(HOVER_ANIMATION_DURATION);
        clickControllerDown_->Play();
        clickControllerDown_->SetFillMode(FillMode::FORWARDS);
    }
}

void RenderButton::OnMouseClickUpAnimation()
{
    if (!needHoverColor_) {
        ResetController(clickControllerDown_);
        if (!clickControllerUp_) {
            clickControllerUp_ = AceType::MakeRefPtr<Animator>(context_);
        }
        scaleAnimationUp_ = AceType::MakeRefPtr<KeyframeAnimation<float>>();
        auto begin = scale_;
        CreateFloatAnimation(scaleAnimationUp_, begin, 1.05);
        clickControllerUp_->AddInterpolator(scaleAnimationUp_);
        clickControllerUp_->SetDuration(HOVER_ANIMATION_DURATION);
        clickControllerUp_->Play();
        clickControllerUp_->SetFillMode(FillMode::FORWARDS);
    }
}

void RenderButton::CreateFloatAnimation(
    RefPtr<KeyframeAnimation<float>>& floatAnimation, float beginValue, float endValue)
{
    if (!floatAnimation) {
        return;
    }
    auto keyframeBegin = AceType::MakeRefPtr<Keyframe<float>>(0.0, beginValue);
    auto keyframeEnd = AceType::MakeRefPtr<Keyframe<float>>(1.0, endValue);
    floatAnimation->AddKeyframe(keyframeBegin);
    floatAnimation->AddKeyframe(keyframeEnd);
    floatAnimation->AddListener([weakButton = AceType::WeakClaim(this)](float value) {
        auto button = weakButton.Upgrade();
        if (button) {
            button->isHover_ = true;
            button->scale_ = value;
            button->MarkNeedRender();
        }
    });
}

void RenderButton::ResetController(RefPtr<Animator>& controller)
{
    if (controller) {
        if (!controller->IsStopped()) {
            controller->Stop();
        }
        controller->ClearInterpolators();
    }
}

void RenderButton::Update(const RefPtr<Component>& component)
{
    const RefPtr<ButtonComponent> button = AceType::DynamicCast<ButtonComponent>(component);
    if (!button) {
        LOGE("Update error, button component is null");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    buttonComponent_ = button;
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    auto theme = GetTheme<ButtonTheme>();
    if (theme) {
        defaultClickedColor_ = theme->GetClickedColor();
    }

    width_ = buttonComponent_->GetWidth();
    height_ = buttonComponent_->GetHeight();
    buttonComponent_->FitTextHeight(height_);
    layoutFlag_ = button->GetLayoutFlag();
    // No animation happens on first setting, will animate from background color on click
    clickedColor_ = AnimatableColor(button->GetClickedColor());
    backgroundColor_.SetValue(button->GetBackgroundColor().GetValue());
    stateEffect_ = button->GetStateEffect();
    isWatch_ = (SystemProperties::GetDeviceType() == DeviceType::WATCH);
    isTv_ = (SystemProperties::GetDeviceType() == DeviceType::TV);
    isPhone_ = (SystemProperties::GetDeviceType() == DeviceType::PHONE);
    auto catchMode =
        buttonComponent_->GetClickedEventId().IsEmpty() || buttonComponent_->GetClickedEventId().GetCatchMode();
    static const int32_t bubbleModeVersion = 6;
    auto pipeline = context_.Upgrade();
    if (!catchMode) {
        if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            catchMode = false;
        } else {
            catchMode = true;
        }
    }
    auto catchModeButton = buttonComponent_->GetCatchMode();
    clickRecognizer_->SetUseCatchMode(catchMode && catchModeButton);
    SetAccessibilityText(button->GetAccessibilityText());
    UpdateDownloadStyles(button);

    OnStatusStyleChanged(disabled_ ? VisualState::DISABLED : VisualState::NORMAL);
    MarkNeedLayout();
}

void RenderButton::PerformLayout()
{
    if (!buttonComponent_) {
        LOGE("Fail to perform layout due to buttonComponent is null");
        return;
    }
    minWidth_ = buttonComponent_->GetMinWidth();
    type_ = buttonComponent_->GetType();
    widthDefined_ = GreatOrEqual(buttonComponent_->GetWidth().Value(), 0.0);
    heightDefined_ = GreatOrEqual(buttonComponent_->GetHeight().Value(), 0.0);
    if (type_ == ButtonType::ARC) {
        width_ = ARC_BUTTON_WIDTH;
        height_ = ARC_BUTTON_HEIGHT;
    }
    buttonSize_ = Size(NormalizePercentToPx(width_, false), NormalizePercentToPx(height_, true));
    rrectRadius_ = NormalizeToPx(buttonComponent_->GetRectRadius());
    layoutSize_ = Measure();
    SetChildrenLayoutSize();
    SetLayoutSize(CalculateLayoutSize());
    SetChildrenAlignment();
    buttonSize_ = GetLayoutSize() - Size(widthDelta_, widthDelta_);
}

void RenderButton::SetChildrenLayoutSize()
{
    LayoutParam innerLayoutParam;
    bool isWatchText = (isWatch_ && (type_ == ButtonType::TEXT));
    double maxWidth = buttonSize_.Width();
    if (NearEqual(buttonSize_.Width(), 0.0)) {
        maxWidth = isWatchText ? NormalizeToPx(TEXT_BUTTON_MAX_WIDTH) : GetLayoutParam().GetMaxSize().Width();
        maxWidth -= widthDelta_;
    }
    double height = buttonSize_.Height();
    if (buttonComponent_->GetDeclarativeFlag()) {
        if (!heightDefined_ && type_ == ButtonType::NORMAL) {
            height = GetLayoutParam().GetMaxSize().Height();
        }
    }
    innerLayoutParam.SetMaxSize(Size(maxWidth, height));
    for (const auto& child : GetChildren()) {
        child->Layout(innerLayoutParam);
        childrenSize_.SetWidth(child->GetLayoutSize().Width());
        childrenSize_.SetHeight(child->GetLayoutSize().Height());
    }
}

Size RenderButton::CalculateLayoutSize()
{
    Size layoutSize;
    if (NeedAdaptiveChild()) {
        double layoutWidth = widthDefined_ ? layoutSize_.Width() : childrenSize_.Width();
        double layoutHeight = heightDefined_ ? layoutSize_.Height() : childrenSize_.Height();
        layoutSize = Size(layoutWidth, layoutHeight);
    } else {
        if (NearEqual(buttonSize_.Width(), 0.0)) {
            double width =
                (childrenSize_.Width() > NormalizeToPx(minWidth_)) ? childrenSize_.Width() : NormalizeToPx(minWidth_);
            layoutSize = Size(width, buttonSize_.Height()) + Size(widthDelta_, widthDelta_);
        } else {
            layoutSize = layoutSize_;
        }
    }
    if (NeedConstrain()) {
        layoutSize = GetLayoutParam().Constrain(layoutSize);
    }
    return layoutSize;
}

bool RenderButton::NeedAdaptiveChild()
{
    if ((type_ == ButtonType::TEXT) && isWatch_) {
        return true;
    }
    if ((type_ == ButtonType::ICON) || (type_ == ButtonType::NORMAL)) {
        return true;
    }
    return false;
}

bool RenderButton::NeedConstrain()
{
    if (type_ == ButtonType::CIRCLE) {
        return false;
    }
    if (isWatch_) {
        if ((type_ == ButtonType::DOWNLOAD) || (type_ == ButtonType::ARC)) {
            return false;
        }
    }
    return true;
}

void RenderButton::SetChildrenAlignment()
{
    if (GetChildren().empty()) {
        return;
    }
    auto& child = GetChildren().front();
    Alignment alignment = (type_ == ButtonType::ARC) ? Alignment::TOP_CENTER : Alignment::CENTER;
    child->SetPosition(Alignment::GetAlignPosition(GetLayoutSize(), child->GetLayoutSize(), alignment));
}

void RenderButton::SetProgress(uint32_t progress)
{
    if (isWatch_) {
        if (progress >= static_cast<uint32_t>(round(DOWNLOAD_FULL_PERCENT))) {
            progressDisplay_ = false;
            return;
        }
        progressDisplay_ = progress > 0 ? true : false;
        progressPercent_ = progress / DOWNLOAD_FULL_PERCENT;
        progressWidth_ = buttonSize_.Width() * progressPercent_;
        MarkNeedRender();
        return;
    }
    needUpdateAnimation_ = false;
    if (!NearEqual(progress, previousValue_)) {
        animationDuring_ = std::chrono::steady_clock::now() - previousUpdateTime_;
        percentChange_ = progress - previousValue_;
        previousValue_ = progress;
        previousUpdateTime_ = std::chrono::steady_clock::now();
        needUpdateAnimation_ = true;
    }
    UpdateProgressAnimation();
    MarkNeedLayout();
}

void RenderButton::UpdateDownloadStyles(const RefPtr<ButtonComponent>& button)
{
    if (button->GetType() != ButtonType::DOWNLOAD) {
        return;
    }
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext) {
        if (!progressController_) {
            progressController_ = AceType::MakeRefPtr<Animator>(pipelineContext);
        }
    }
    progressColor_ = button->GetProgressColor();
    progressFocusColor_ = button->GetProgressFocusColor();
    progressDiameter_ = NormalizeToPx(button->GetProgressDiameter());
    const auto& buttonController = button->GetButtonController();
    if (buttonController) {
        buttonController->SetProgressCallback([weak = AceType::WeakClaim(this)](uint32_t progress) {
            auto renderButton = weak.Upgrade();
            if (renderButton) {
                renderButton->SetProgress(progress);
            }
        });
    }
}

void RenderButton::UpdateProgressAnimation()
{
    if (!needUpdateAnimation_) {
        return;
    }
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(progressPercent_, previousValue_, Curves::EASE_OUT);
    animation->AddListener([this](const double& value) {
        progressDisplay_ = GreatNotEqual(value, 0.0) ? true : false;
        progressPercent_ = value;
        progressWidth_ = buttonSize_.Width() * progressPercent_ / DOWNLOAD_FULL_PERCENT;
        if (GreatOrEqual(progressPercent_, DOWNLOAD_FULL_PERCENT)) {
            progressDisplay_ = false;
        }
        MarkNeedRender();
    });

    if (!progressController_) {
        return;
    }
    double change = percentChange_ * MILLISECOND_PER_PERCENT;
    double during = animationDuring_.count() * SECOND_TO_MILLISECOND;
    double duration = GreatNotEqual(std::abs(change), during) ? std::abs(change) : during;
    if (LessNotEqual(duration, MIN_TRANSITION_TIME) || (previousValue_ == DOWNLOAD_FULL_PERCENT)) {
        duration = MIN_TRANSITION_TIME;
    }
    if (GreatNotEqual(duration, MAX_TRANSITION_TIME)) {
        duration = MAX_TRANSITION_TIME;
    }
    progressController_->ClearInterpolators();
    progressController_->AddInterpolator(animation);
    progressController_->SetDuration(static_cast<int32_t>(round(duration)));
    progressController_->SetIteration(1);
    progressController_->Stop();
    progressController_->Play();
}

void RenderButton::UpdateAnimationParam(double value)
{
    UpdateFocusAnimation(value);
    if (!animationRunning_) {
        return;
    }

    if (isWatch_ || isTv_) {
        scale_ = value;
    }
    if (isOpacityAnimation_) {
        opacity_ = fabs((value - INIT_SCALE) * ratio_);
    }
    if (isTouchAnimation_) {
        maskingOpacity_ = fabs((value - INIT_SCALE) * ratio_ / MASKING_ANIMATION_RATIO);
    }
    if (!valueChanged_ && (!NearEqual(value, startValue_))) {
        valueChanged_ = true;
    }
    if ((!isClickAnimation_ && NearEqual(value, endValue_)) || (valueChanged_ && NearEqual(value, startValue_))) {
        isLastFrame_ = true;
        valueChanged_ = false;
        isClickAnimation_ = false;
    }
    MarkNeedRender();
}

void RenderButton::UpdateFocusAnimation(double value)
{
    auto context = context_.Upgrade();
    if (!context || !isFocus_) {
        return;
    }
    Size sizeDelta = buttonSize_ * (value - INIT_SCALE);
    Size layoutSize = GetLayoutSize() + sizeDelta;
    Offset buttonOffset = Offset(sizeDelta.Width() / 2.0, sizeDelta.Height() / 2.0);
    double radius = rrectRadius_;
    if (!buttonComponent_) {
        return;
    }
    ButtonType type = buttonComponent_->GetType();
    if ((type == ButtonType::CIRCLE) || (type == ButtonType::CAPSULE)) {
        radius = layoutSize.Height() / 2.0;
    }
    if (isPhone_ && ((type == ButtonType::TEXT) || (type == ButtonType::NORMAL))) {
        context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(0, 0), layoutSize), Radius(radius)),
            buttonComponent_->GetFocusAnimationColor(), GetGlobalOffset() - buttonOffset, true);
        return;
    }
    context->ShowFocusAnimation(RRect::MakeRRect(Rect(Offset(0, 0), layoutSize), Radius(radius)),
        buttonComponent_->GetFocusAnimationColor(), GetGlobalOffset() - buttonOffset);
    context->ShowShadow(
        RRect::MakeRRect(Rect(Offset(0, 0), layoutSize), Radius(radius)), GetGlobalOffset() - buttonOffset);
}

void RenderButton::PlayAnimation(double start, double end, int32_t duration, const FillMode& fillMode)
{
    animationRunning_ = true;
    startValue_ = start;
    endValue_ = end;
    if (!NearEqual(start, end)) {
        ratio_ = INIT_SCALE / (end - start);
    }
    auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(start, end, Curves::FRICTION);
    animation->AddListener(Animation<double>::ValueCallback([weak = AceType::WeakClaim(this)](double value) {
        auto button = weak.Upgrade();
        if (button) {
            button->UpdateAnimationParam(value);
        }
    }));
    if (!controller_) {
        return;
    }
    controller_->ClearInterpolators();
    controller_->SetDuration(duration);
    controller_->SetFillMode(fillMode);
    controller_->AddInterpolator(animation);
    controller_->Play();
}

void RenderButton::PlayTouchAnimation()
{
    if (isWatch_) {
        isTouchAnimation_ = true;
        if (isClicked_) {
            PlayAnimation(INIT_SCALE, WATCH_SCALE, WATCH_DURATION_DOWN);
        } else {
            PlayAnimation(WATCH_SCALE, INIT_SCALE, WATCH_DURATION_UP);
        }
    } else {
        isTouchAnimation_ = true;
        if (isClicked_) {
            PlayAnimation(PRESS_UP_OPACITY, PRESS_DOWN_OPACITY, PRESS_ANIMATION_DURATION);
        } else {
            PlayAnimation(PRESS_DOWN_OPACITY, PRESS_UP_OPACITY, PRESS_ANIMATION_DURATION);
        }
    }
}

void RenderButton::PlayClickScaleAnimation(float keyTime, int32_t duration)
{
    auto startFrame = AceType::MakeRefPtr<Keyframe<double>>(KEY_TIME_START, TV_EXPAND_SCALE);
    auto midFrame = AceType::MakeRefPtr<Keyframe<double>>(keyTime, TV_REDUCE_SCALE);
    auto endFrame = AceType::MakeRefPtr<Keyframe<double>>(KEY_TIME_END, TV_EXPAND_SCALE);
    midFrame->SetCurve(Curves::FRICTION);
    endFrame->SetCurve(Curves::FRICTION);
    auto animation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    animation->AddKeyframe(startFrame);
    animation->AddKeyframe(midFrame);
    animation->AddKeyframe(endFrame);
    animation->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto button = weak.Upgrade();
        if (button) {
            button->UpdateAnimationParam(value);
        }
    });

    if (!controller_) {
        return;
    }
    controller_->ClearInterpolators();
    controller_->AddInterpolator(animation);
    controller_->SetDuration(duration);
    controller_->Play();
}

void RenderButton::PlayClickAnimation()
{
    if (isPhone_ || isWatch_) {
        return;
    }
    if (!isFocus_) {
        return;
    }
    animationRunning_ = true;
    isClickAnimation_ = true;
    startValue_ = TV_EXPAND_SCALE;
    endValue_ = TV_REDUCE_SCALE;
    PlayClickScaleAnimation(KEY_TIME_MID, TV_CLICK_DURATION);
}

void RenderButton::PlayFocusAnimation(bool isFocus)
{
    if (isWatch_) {
        return;
    }
    if (isPhone_) {
        UpdateFocusAnimation(INIT_SCALE);
        return;
    }
    if (!isOpacityAnimation_ && isTv_) {
        isOpacityAnimation_ = true;
    }
    if (isFocus) {
        PlayAnimation(INIT_SCALE, TV_EXPAND_SCALE, TV_FOCUS_SCALE_DURATION);
    } else {
        PlayAnimation(TV_EXPAND_SCALE, INIT_SCALE, TV_FOCUS_SCALE_DURATION);
    }
}

void RenderButton::OnStatusStyleChanged(const VisualState state)
{
    RenderNode::OnStatusStyleChanged(state);
    if (buttonComponent_ == nullptr || !buttonComponent_->HasStateAttributes()) {
        return;
    }

    for (auto attribute : buttonComponent_->GetStateAttributes()->GetAttributesForState(state)) {
        switch (attribute->id_) {
            case ButtonStateAttribute::COLOR: {
                auto colorState =
                    AceType::DynamicCast<StateAttributeValue<ButtonStateAttribute, AnimatableColor>>(attribute);
                if (state == VisualState::PRESSED) {
                    LOGD("Click color start %{public}x  end %{public}x", backgroundColor_.GetValue(),
                        colorState->value_.GetValue());
                    SetClickedColor(backgroundColor_);  // starting animation color
                    clickedColor_ = colorState->value_; // End color
                    setClickColor_ = true;
                } else {
                    LOGD("background color start %{public}x  end %{public}x", clickedColor_.GetValue(),
                        colorState->value_.GetValue());
                    backgroundColor_.SetValue(clickedColor_.GetValue()); // Start value
                    backgroundColor_ = colorState->value_;               // End value and animate
                }
            } break;

            case ButtonStateAttribute::RADIUS: {
                auto radiusState =
                    AceType::DynamicCast<StateAttributeValue<ButtonStateAttribute, Dimension>>(attribute);
                buttonComponent_->SetRectRadius(radiusState->value_);
            } break;

            case ButtonStateAttribute::HEIGHT: {
                auto valueState =
                    AceType::DynamicCast<StateAttributeValue<ButtonStateAttribute, AnimatableDimension>>(attribute);
                buttonComponent_->SetHeight(valueState->value_);
                height_ = valueState->value_;
            } break;

            case ButtonStateAttribute::WIDTH: {
                auto valueState =
                    AceType::DynamicCast<StateAttributeValue<ButtonStateAttribute, AnimatableDimension>>(attribute);
                buttonComponent_->SetWidth(valueState->value_);
                width_ = valueState->value_;
            } break;
            default:
                break;
        }
    }
    MarkNeedLayout();
}

void RenderButton::SendAccessibilityEvent()
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }

    auto context = context_.Upgrade();
    if (context) {
        AccessibilityEvent radioEvent;
        radioEvent.nodeId = accessibilityNode->GetNodeId();
        radioEvent.eventType = "click";
        context->SendEventToAccessibility(radioEvent);
    }
}

} // namespace OHOS::Ace
