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

#include "core/components/text_overlay/render_text_overlay.h"

#include "core/components/focus_collaboration/render_focus_collaboration.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {
namespace {

const Offset DOT1_OFFSET = Offset(-0.790737726, -1.290737726);
const Offset DOT2_OFFSET = Offset(-0.623475836, -1.290737726);
const Offset DOT3_OFFSET = Offset(-0.790737726, -0.123475836);
const Offset DOT4_OFFSET = Offset(0.790737726, 1.290737726);
const Offset END_POINT = DOT1_POSITION + DOT1_OFFSET;
constexpr Dimension TOOL_BAR_HEIGHT = 40.0_vp;
constexpr Dimension HANDLE_HOTZONE_DIAMETER = 20.0_vp;
constexpr Dimension MORE_BUTTON_SIZE = 40.0_vp;
constexpr Dimension ANIMATION_OFFSET_X = 16.0_vp;
constexpr double DEFAULT_SPACING = 10.0;
constexpr double FIFTY_PERCENT = 0.5;
constexpr double ROTATE_DEGREE = -45.0;
constexpr double CLIP_WIDTH = 1.0;
constexpr double OPACITY_KEYFRAME = 250.0 / 350.0; // Clip and translate animation is 350ms, opacity animation is 250ms.
constexpr float KEYFRAME_PERCENT_THIRTY = 0.33f;
constexpr float KEYFRAME_BEGINNING = 0.0f;
constexpr float KEYFRAME_ENDING = 1.0f;
constexpr int32_t MORE_ANIMATION_DURATION = 300; // Duration of more icon animation.
constexpr int32_t ANIMATION_DURATION = 350;      // Duration of clip and translate animation

} // namespace

RenderTextOverlay::RenderTextOverlay()
{
    clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
    clickDetector_->SetOnClick([weak = AceType::WeakClaim(this)](const ClickInfo& clickInfo) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->HandleClick(clickInfo.GetLocalLocation());
        }
    });

    dragDetector_ = AceType::MakeRefPtr<DragRecognizer>(Axis::FREE);
    dragDetector_->SetOnDragStart([weak = AceType::WeakClaim(this)](const DragStartInfo& startInfo) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->HandleDragStart(startInfo.GetLocalLocation());
        }
    });

    dragDetector_->SetOnDragUpdate([weak = AceType::WeakClaim(this)](const DragUpdateInfo& updateInfo) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->HandleDragUpdateAndEnd(updateInfo.GetLocalLocation());
        }
    });

    dragDetector_->SetOnDragEnd([weak = AceType::WeakClaim(this)](const DragEndInfo& endInfo) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->HandleDragUpdateAndEnd(endInfo.GetLocalLocation());
            overlay->isDragging_ = false;
        }
    });

    touchDetector_ = AceType::MakeRefPtr<RawRecognizer>();
    touchDetector_->SetOnTouchDown([weak = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->showMagnifier_ = true;
            auto startOffset = info.GetTouches().front().GetLocalLocation();
            if (overlay->startHandleRegion_.ContainsInRegion(startOffset.GetX(), startOffset.GetY())) {
                overlay->isTouchStartDrag_ = true;
                overlay->isTouchEndDrag_ = false;
            } else {
                overlay->isTouchStartDrag_ = false;
                overlay->isTouchEndDrag_ =
                    overlay->endHandleRegion_.ContainsInRegion(startOffset.GetX(), startOffset.GetY());
            }
        }
    });

    touchDetector_->SetOnTouchUp([weak = AceType::WeakClaim(this)](const TouchEventInfo&) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->showMagnifier_ = false;
        }
    });
}

RenderTextOverlay::~RenderTextOverlay()
{
    auto renderTextField = weakTextField_.Upgrade();
    if (renderTextField) {
        renderTextField->SetIsOverlayShowed(false, needStartTwinkling_);
    }
    auto spOverlayComponent = overlayComponent_.Upgrade();
    if (spOverlayComponent) {
        RemoveBackendEvent(spOverlayComponent);
    }
}

void RenderTextOverlay::Update(const RefPtr<Component>& component)
{
    auto overlay = AceType::DynamicCast<TextOverlayComponent>(component);
    if (!overlay) {
        return;
    }
    overlayComponent_ = overlay;
    overlay->SetPopOverlay([weak = WeakClaim(this)]() {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->needCloseKeyboard_ = false;
            overlay->PopOverlay();
        }
    });
    onFocusChange_ = overlay->GetOnFocusChange();
    onCut_ = overlay->GetOnCut();
    onCopy_ = overlay->GetOnCopy();
    onPaste_ = overlay->GetOnPaste();
    onCopyAll_ = overlay->GetOnCopyAll();
    startHandleOffset_ = overlay->GetStartHandleOffset();
    endHandleOffset_ = overlay->GetEndHandleOffset();
    onStartHandleMove_ = overlay->GetOnStartHandleMove();
    onEndHandleMove_ = overlay->GetOnEndHandleMove();
    isSingleHandle_ = overlay->GetIsSingleHandle() || (startHandleOffset_ == endHandleOffset_);
    lineHeight_ = overlay->GetLineHeight();
    handleDiameter_ = overlay->GetHandleDiameter();
    menuSpacingWithHandle_ = handleDiameter_;
    handleDiameterInner_ = overlay->GetHandleDiameterInner();
    handleRadius_ = handleDiameter_ / 2.0;
    handleRadiusInner_ = handleDiameterInner_ / 2.0;
    menuSpacingWithText_ = overlay->GetMenuSpacingWithText();
    handleColor_ = overlay->GetHandleColor();
    handleColorInner_ = overlay->GetHandleColorInner();
    clipRect_ = overlay->GetClipRect();
    textDirection_ = overlay->GetTextDirection();
    realTextDirection_ = overlay->GetRealTextDirection();
    BindBackendEvent(overlay);
    UpdateWeakTextField(overlay);
    MarkNeedLayout();
}

void RenderTextOverlay::BindBackendEvent(const RefPtr<TextOverlayComponent>& overlay)
{
    auto context = context_.Upgrade();
    if (context->GetIsDeclarative()) {
        BindBackendEventV2(overlay);
    } else {
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            overlay->GetCutButtonMarker(), [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->HandleCut();
                }
            });

        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            overlay->GetCopyButtonMarker(), [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->HandleCopy();
                }
            });

        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            overlay->GetPasteButtonMarker(), [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->HandlePaste();
                }
            });

        BackEndEventManager<void()>::GetInstance().BindBackendEvent(
            overlay->GetCopyAllButtonMarker(), [weak = WeakClaim(this)]() {
                auto overlay = weak.Upgrade();
                if (overlay) {
                    overlay->HandleCopyAll();
                }
            });
    }

    BackEndEventManager<void()>::GetInstance().BindBackendEvent(
        overlay->GetMoreButtonMarker(), [weak = WeakClaim(this)]() {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->HandleMoreButtonClick();
            }
        });
}

void RenderTextOverlay::BindBackendEventV2(const RefPtr<TextOverlayComponent>& overlay)
{
    BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
        overlay->GetCutButtonMarker(), [weak = WeakClaim(this)](const ClickInfo& info) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->HandleCut();
            }
        });

    BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
        overlay->GetCopyButtonMarker(), [weak = WeakClaim(this)](const ClickInfo& info) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->HandleCopy();
            }
        });

    BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
        overlay->GetPasteButtonMarker(), [weak = WeakClaim(this)](const ClickInfo& info) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->HandlePaste();
            }
        });

    BackEndEventManager<void(const ClickInfo& info)>::GetInstance().BindBackendEvent(
        overlay->GetCopyAllButtonMarker(), [weak = WeakClaim(this)](const ClickInfo& info) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->HandleCopyAll();
            }
        });
}

void RenderTextOverlay::RemoveBackendEvent(const RefPtr<TextOverlayComponent>& overlay)
{
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(overlay->GetCutButtonMarker());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(overlay->GetCopyButtonMarker());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(overlay->GetPasteButtonMarker());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(overlay->GetCopyAllButtonMarker());
    BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(overlay->GetMoreButtonMarker());
}

void RenderTextOverlay::UpdateWeakTextField(const RefPtr<TextOverlayComponent>& overlay)
{
    if (!overlay) {
        return;
    }
    weakTextField_ = overlay->GetWeakTextField();
    auto renderTextField = weakTextField_.Upgrade();
    if (!renderTextField) {
        return;
    }
    auto callback = [weak = WeakClaim(this)](const OverlayShowOption& option) {
        auto overlay = weak.Upgrade();
        if (!overlay) {
            return;
        }
        if (option.updateOverlayType == UpdateOverlayType::CLICK ||
            option.updateOverlayType == UpdateOverlayType::LONG_PRESS) {
            overlay->childRightBoundary_ = 0.0;
        }
        overlay->SetVisible(true);
        overlay->showOption_ = option;
        overlay->startHandleOffset_ = option.startHandleOffset;
        overlay->endHandleOffset_ = option.endHandleOffset;
        overlay->isSingleHandle_ = option.isSingleHandle;
        if (option.updateOverlayType == UpdateOverlayType::CLICK) {
            if (overlay->onRebuild_) {
                overlay->hasMenu_ = false;
                overlay->onRebuild_(true, false, false, false, false);
            }
        } else if (option.updateOverlayType == UpdateOverlayType::LONG_PRESS) {
            if (overlay->onRebuild_) {
                overlay->hasMenu_ = false;
                overlay->onRebuild_(false, true, false, true, false);
            }
        }
    };
    renderTextField->SetUpdateHandlePosition(callback);

    auto callbackDiameter = [weak = WeakClaim(this)](const double& value) {
        auto overlay = weak.Upgrade();
        if (!overlay) {
            LOGE("UpdateWeakTextField error, overlay is nullptr");
            return;
        }

        overlay->SetVisible(true);
        if (overlay->onRebuild_) {
            overlay->onRebuild_(overlay->isSingleHandle_, !overlay->isSingleHandle_, overlay->hasMenu_,
                !overlay->isSingleHandle_, false);
        }
        overlay->handleDiameter_ = Dimension(value, DimensionUnit::VP);
        overlay->handleRadius_ = overlay->handleDiameter_ * FIFTY_PERCENT;
    };
    renderTextField->SetUpdateHandleDiameter(callbackDiameter);

    auto callbackDiameterInner = [weak = WeakClaim(this)](const double& value) {
        auto overlay = weak.Upgrade();
        if (!overlay) {
            LOGE("UpdateWeakTextField error, overlay is nullptr");
            return;
        }

        overlay->SetVisible(true);
        if (overlay->onRebuild_) {
            overlay->onRebuild_(overlay->isSingleHandle_, !overlay->isSingleHandle_, overlay->hasMenu_,
                !overlay->isSingleHandle_, false);
        }
        overlay->handleDiameterInner_ = Dimension(value, DimensionUnit::VP);
        overlay->handleRadiusInner_ = overlay->handleDiameterInner_ * FIFTY_PERCENT;
    };
    renderTextField->SetUpdateHandleDiameterInner(callbackDiameterInner);

    auto onValueChange = [weak = WeakClaim(this)] {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->needCloseKeyboard_ = false;
            overlay->PopOverlay();
        }
    };
    renderTextField->SetOnValueChange(onValueChange);

    auto onKeyboardClose = [weak = WeakClaim(this)](bool forceCloseKeyboard) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->needCloseKeyboard_ = !forceCloseKeyboard;
            overlay->needStartTwinkling_ = !forceCloseKeyboard;
            overlay->PopOverlay();
        }
    };
    renderTextField->SetOnKeyboardClose(onKeyboardClose);

    auto onClipRectChanged = [weak = WeakClaim(this)](const Rect& clipRect) {
        auto overlay = weak.Upgrade();
        if (overlay && (overlay->clipRect_ != clipRect)) {
            overlay->clipRect_ = clipRect;
            overlay->MarkNeedLayout();
        }
    };
    renderTextField->SetOnClipRectChanged(onClipRectChanged);
}

void RenderTextOverlay::PerformLayout()
{
    double handleRadius = NormalizeToPx(handleRadius_);
    startHandleCenter_ = Offset(-handleRadius, handleRadius);
    endHandleCenter_ = Offset(handleRadius, handleRadius);

    if (!GetChildren().empty()) {
        const auto& child = GetChildren().front();
        if (child) {
            child->Layout(GetLayoutParam());
            child->SetPosition(ComputeChildPosition(child));
            if (NearZero(childRightBoundary_)) {
                childRightBoundary_ = child->GetPosition().GetX() + child->GetLayoutSize().Width();
            } else {
                child->SetPosition(
                    Offset(childRightBoundary_ - child->GetLayoutSize().Width(), child->GetPosition().GetY()));
            }
        } else {
            LOGE("child is null");
            return;
        }
        SetLayoutSize(GetLayoutParam().GetMaxSize());

        // If child size is changed, refresh animation, when there is tool bar only.
        if (HasToolBarOnly()) {
            double horizonOffsetForAnimation = child->GetLayoutSize().Width() - NormalizeToPx(MORE_BUTTON_SIZE);
            if (!NearEqual(horizonOffsetForAnimation, horizonOffsetForAnimation_)) {
                horizonOffsetForAnimation_ = horizonOffsetForAnimation;
                isAnimationInited_ = false;
            }
        }
        if (child && renderClip_) {
            renderClip_->SetShadowBoxOffset(Offset(
                std::max(
                    child->GetLayoutSize().Width() - horizonOffsetForAnimation_ - NormalizeToPx(MORE_BUTTON_SIZE), 0.0),
                0.0));
        }

        if (textDirection_ == TextDirection::RTL) {
            moreButtonPosition_ = child->GetGlobalOffset();
        } else {
            moreButtonPosition_ = child->GetGlobalOffset() + Offset(child->GetLayoutSize().Width(), 0.0);
        }
    }

    // Compute touch region of handle.
    double hotZoneDiameter = NormalizeToPx(HANDLE_HOTZONE_DIAMETER);
    double hotZoneRadius = hotZoneDiameter / 2.0;
    if (isSingleHandle_) {
        startHandleRegion_ = TouchRegion(startHandleOffset_ + Offset(-hotZoneRadius, 0.0),
            startHandleOffset_ + Offset(hotZoneRadius, hotZoneDiameter));
    } else {
        startHandleRegion_ = TouchRegion(startHandleOffset_ + Offset(-hotZoneRadius, -lineHeight_ - hotZoneDiameter),
            startHandleOffset_ + Offset(hotZoneRadius, -lineHeight_));
        endHandleRegion_ = TouchRegion(
            endHandleOffset_ + Offset(-hotZoneRadius, 0.0), endHandleOffset_ + Offset(hotZoneRadius, hotZoneDiameter));
    }

    InitAnimation();
}

Offset RenderTextOverlay::ComputeChildPosition(const RefPtr<RenderNode>& child)
{
    Offset startHandleOffset = startHandleOffset_;
    startHandleOffset.SetX(std::clamp(startHandleOffset.GetX(), clipRect_.Left(), clipRect_.Right()));
    startHandleOffset.SetY(std::clamp(startHandleOffset.GetY(), clipRect_.Top(), clipRect_.Bottom()));
    Offset endHandleOffset = endHandleOffset_;
    endHandleOffset.SetX(std::clamp(endHandleOffset.GetX(), clipRect_.Left(), clipRect_.Right()));
    endHandleOffset.SetY(std::clamp(endHandleOffset.GetY(), clipRect_.Top(), clipRect_.Bottom()));
    if (!NearEqual(startHandleOffset.GetY(), endHandleOffset.GetY())) {
        startHandleOffset.SetX(clipRect_.Left());
        endHandleOffset.SetX(clipRect_.Right());
    }

    // Calculate the spacing with text and handle, menu is fixed up the handle and text.
    double menuSpacingWithText = NormalizeToPx(menuSpacingWithText_);
    double menuSpacingWithHandle = NormalizeToPx(menuSpacingWithHandle_);
    double menuSpacing = isSingleHandle_ ? menuSpacingWithText : menuSpacingWithHandle + menuSpacingWithText;

    Offset childPosition =
        Offset((startHandleOffset.GetX() + endHandleOffset.GetX() - child->GetLayoutSize().Width()) / 2.0,
            startHandleOffset.GetY() - lineHeight_ - menuSpacing - NormalizeToPx(TOOL_BAR_HEIGHT));
    // Adjust position of overlay.
    if (LessOrEqual(childPosition.GetX(), 0.0)) {
        childPosition.SetX(DEFAULT_SPACING);
    } else if (GreatOrEqual(
        childPosition.GetX() + child->GetLayoutSize().Width(), GetLayoutParam().GetMaxSize().Width())) {
        childPosition.SetX(GetLayoutParam().GetMaxSize().Width() - child->GetLayoutSize().Width() - DEFAULT_SPACING);
    }
    if (LessNotEqual(childPosition.GetY(), 0.0)) {
        childPosition.SetY(endHandleOffset_.GetY() + menuSpacingWithHandle + menuSpacingWithText);
    }
    return childPosition;
}

void RenderTextOverlay::InitAnimation()
{
    if (isAnimationInited_) {
        return;
    }
    isAnimationInited_ = true;

    // Create tween option for in.
    // Add offset animation for outer tween.
    auto xCurveIn = AceType::MakeRefPtr<CurveAnimation<float>>(horizonOffsetForAnimation_, 0.0f, Curves::FRICTION);
    tweenOptionIn_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_OFFSET_X, xCurveIn);

    // Add opacity animation for outer tween.
    auto opacityKeyframeInFirst = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
    auto opacityKeyframeInSecond = AceType::MakeRefPtr<Keyframe<float>>(OPACITY_KEYFRAME, 1.0f);
    opacityKeyframeInSecond->SetCurve(Curves::SHARP);
    auto opacityKeyframeInThird = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f);
    auto opacityAnimationIn = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationIn->AddKeyframe(opacityKeyframeInFirst);
    opacityAnimationIn->AddKeyframe(opacityKeyframeInSecond);
    opacityAnimationIn->AddKeyframe(opacityKeyframeInThird);
    tweenOptionIn_.SetOpacityAnimation(opacityAnimationIn);
    tweenOptionIn_.SetDuration(ANIMATION_DURATION);
    tweenOptionIn_.SetFillMode(FillMode::FORWARDS);

    // Add translate animation for inner tween.
    auto xTranslateIn = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
        DimensionOffset(ANIMATION_OFFSET_X, 0.0_vp), DimensionOffset(0.0_vp, 0.0_vp), Curves::FRICTION);
    innerTweenOptionIn_.SetTranslateAnimations(AnimationType::TRANSLATE_X, xTranslateIn);
    innerTweenOptionIn_.SetDuration(ANIMATION_DURATION);
    innerTweenOptionIn_.SetFillMode(FillMode::FORWARDS);

    // Create tween option for out.
    // Add offset animation for outer tween.
    auto xCurveOut = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, horizonOffsetForAnimation_, Curves::FRICTION);
    tweenOptionOut_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_OFFSET_X, xCurveOut);

    // Add opacity animation for outer tween.
    auto opacityKeyframeOutFirst = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f);
    auto opacityKeyframeOutSecond = AceType::MakeRefPtr<Keyframe<float>>(OPACITY_KEYFRAME, 0.0f);
    opacityKeyframeOutSecond->SetCurve(Curves::SHARP);
    auto opacityKeyframeOutThird = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f);
    auto opacityAnimationOut = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimationOut->AddKeyframe(opacityKeyframeOutFirst);
    opacityAnimationOut->AddKeyframe(opacityKeyframeOutSecond);
    opacityAnimationOut->AddKeyframe(opacityKeyframeOutThird);
    tweenOptionOut_.SetOpacityAnimation(opacityAnimationOut);
    tweenOptionOut_.SetDuration(ANIMATION_DURATION);
    tweenOptionOut_.SetFillMode(FillMode::FORWARDS);

    // Create translate animation for inner tween.
    auto xTranslateOut = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
        DimensionOffset(0.0_vp, 0.0_vp), DimensionOffset(ANIMATION_OFFSET_X, 0.0_vp), Curves::FRICTION);
    innerTweenOptionOut_.SetTranslateAnimations(AnimationType::TRANSLATE_X, xTranslateOut);
    innerTweenOptionOut_.SetDuration(ANIMATION_DURATION);
    innerTweenOptionOut_.SetFillMode(FillMode::FORWARDS);
}

bool RenderTextOverlay::TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (GetDisableTouchEvent() || disabled_ || !isAnimationStopped_) {
        return false;
    }
    const auto localPoint = parentLocalPoint - GetPaintRect().GetOffset();
    if (!isSingleHandle_ || showOption_.showMenu) {
        for (auto iter = GetChildren().rbegin(); iter != GetChildren().rend(); ++iter) {
            const auto& child = *iter;
            if (child->TouchTest(globalPoint, localPoint, touchRestrict, result)) {
                return true;
            }
        }
    }
    if (startHandleRegion_.ContainsInRegion(parentLocalPoint.GetX(), parentLocalPoint.GetY()) ||
        endHandleRegion_.ContainsInRegion(parentLocalPoint.GetX(), parentLocalPoint.GetY())) {
        const auto coordinateOffset = globalPoint - localPoint;
        globalPoint_ = globalPoint;
        OnTouchTestHit(coordinateOffset, touchRestrict, result);
        return true;
    }
    if (globalPoint.GetSourceType() == SourceType::MOUSE) {
        PopOverlay();
        return true;
    }
    return false;
}

void RenderTextOverlay::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    clickDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickDetector_);
    dragDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(dragDetector_);
    touchDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchDetector_);
}

bool RenderTextOverlay::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button != MouseButton::LEFT_BUTTON && event.action == MouseAction::PRESS) {
        PopOverlay();
        return true;
    }
    return false;
}

void RenderTextOverlay::HandleClick(const Offset& clickOffset)
{
    if (isSingleHandle_ && startHandleRegion_.ContainsInRegion(clickOffset.GetX(), clickOffset.GetY())) {
        childRightBoundary_ = 0.0;
        showOption_.showMenu = true;
        auto textField = weakTextField_.Upgrade();
        if (textField) {
            textField->SetIsOverlayShowed(true, false);
        }
        if (onRebuild_) {
            OnFocusChange(RenderStatus::FOCUS);
            onRebuild_(true, true, hasMenu_, true, false);
        }
    }
}

void RenderTextOverlay::HandleDragStart(const Offset& startOffset)
{
    childRightBoundary_ = 0.0;
    showOption_.showMenu = true;
    auto textField = weakTextField_.Upgrade();
    if (!textField) {
        LOGE("TextField is nullptr");
        return;
    }

    // Mark start and end index
    startIndex_ = textField->GetEditingValue().selection.GetStart();
    endIndex_ = textField->GetEditingValue().selection.GetEnd();

    // Mark start or end flag and mark the index
    if (startHandleRegion_.ContainsInRegion(startOffset.GetX(), startOffset.GetY())) {
        isStartDrag_ = true;
        isEndDrag_ = false;
        textField->SetInitIndex(endIndex_);
    } else {
        isStartDrag_ = false;
        isEndDrag_ = endHandleRegion_.ContainsInRegion(startOffset.GetX(), startOffset.GetY());
        textField->SetInitIndex(startIndex_);
    }
}

void RenderTextOverlay::HandleDragUpdateAndEnd(const Offset& offset)
{
    childRightBoundary_ = 0.0;
    if (isStartDrag_) {
        auto startCallback = [weak = WeakClaim(this)](const Offset& startHandleOffset) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->startHandleOffset_ = startHandleOffset;
                if (overlay->isSingleHandle_) {
                    overlay->endHandleOffset_ = startHandleOffset;
                }
                overlay->MarkNeedLayout();
            }
        };
        onStartHandleMove_(endIndex_, offset - Offset(0.0, lineHeight_), startCallback, isSingleHandle_);
        isDragging_ = true;
    } else if (isEndDrag_) {
        auto endCallback = [weak = WeakClaim(this)](const Offset& endHandleOffset) {
            auto overlay = weak.Upgrade();
            if (overlay) {
                overlay->endHandleOffset_ = endHandleOffset;
                overlay->MarkNeedLayout();
            }
        };
        onEndHandleMove_(startIndex_, offset - Offset(0.0, lineHeight_), endCallback);
        isDragging_ = true;
    }
}

void RenderTextOverlay::HandleCut()
{
    needCloseKeyboard_ = false;
    if (onCut_) {
        onCut_();
    }
    PopOverlay();
}

void RenderTextOverlay::HandleCopy()
{
    needCloseKeyboard_ = false;
    if (onCopy_) {
        onCopy_();
    }
    PopOverlay();
}

void RenderTextOverlay::HandlePaste()
{
    needCloseKeyboard_ = false;
    if (onPaste_) {
        onPaste_();
    }
    PopOverlay();
}

void RenderTextOverlay::HandleCopyAll()
{
    needCloseKeyboard_ = false;
    isSingleHandle_ = false;
    childRightBoundary_ = 0.0;
    auto callback = [weak = WeakClaim(this)](const Offset& startHandleOffset, const Offset& endHandleOffset) {
        auto overlay = weak.Upgrade();
        if (overlay) {
            overlay->startHandleOffset_ = startHandleOffset;
            overlay->endHandleOffset_ = endHandleOffset;
            overlay->isSingleHandle_ = false;
            if (startHandleOffset == endHandleOffset) {
                overlay->isSingleHandle_ = true;
            }
            if (overlay->onRebuild_) {
                overlay->onRebuild_(overlay->isSingleHandle_, true, overlay->hasMenu_, true, false);
            }
        }
    };
    if (onCopyAll_) {
        onCopyAll_(callback);
    }
}

void RenderTextOverlay::HandleMoreButtonClick()
{
    needCloseKeyboard_ = false;
    // Is animation is not stopped, do not handle click to start a new animation.
    if (!isAnimationStopped_) {
        return;
    }

    hasMenu_ = !hasMenu_;
    isAnimationStarted_ = false;
    isAnimationStopped_ = false;
    if (onRebuild_) {
        animateUntilPaint_ = hasMenu_;
        onRebuild_(isSingleHandle_, true, true, true, true);
    }

    if (!animateUntilPaint_) {
        startAnimation_(tweenOptionIn_, innerTweenOptionIn_, isSingleHandle_, true);
        StartMoreAnimation(reverse_);
    }
}

void RenderTextOverlay::OnPaintFinish()
{
    if (animateUntilPaint_) {
        animateUntilPaint_ = false;
        startAnimation_(tweenOptionOut_, innerTweenOptionOut_, isSingleHandle_, false);
        StartMoreAnimation(reverse_);
    }
}

void RenderTextOverlay::RestoreMoreButtonStyle()
{
    if (!controller_) {
        return;
    }
    if (reverse_) {
        BuildAndStartMoreButtonAnimation();
        controller_->Finish();
    } else if (controller_->IsRunning()) {
        controller_->Finish();
    }
}

void RenderTextOverlay::StartMoreAnimation(bool reverse)
{
    if (controller_ && controller_->IsRunning()) {
        reverse_ = !reverse_;
        controller_->Reverse();
        return;
    }
    BuildAndStartMoreButtonAnimation();
}

void RenderTextOverlay::BuildStrokeWidthAnimation(const RefPtr<KeyframeAnimation<Dimension>>& widthAnimation,
    const Dimension& from, const Dimension& to, bool reverse)
{
    auto widthFrameStart = AceType::MakeRefPtr<Keyframe<Dimension>>(KEYFRAME_BEGINNING, from);
    auto widthFrameEnd = AceType::MakeRefPtr<Keyframe<Dimension>>(KEYFRAME_ENDING, to);
    widthAnimation->AddKeyframe(widthFrameStart);
    if (reverse) {
        widthFrameEnd->SetCurve(Curves::FRICTION);
    } else {
        auto widthFrameMid = AceType::MakeRefPtr<Keyframe<Dimension>>(KEYFRAME_PERCENT_THIRTY, to);
        widthFrameMid->SetCurve(Curves::FRICTION);
        widthFrameEnd->SetCurve(Curves::LINEAR);
        widthAnimation->AddKeyframe(widthFrameMid);
    }
    widthAnimation->AddKeyframe(widthFrameEnd);
    widthAnimation->AddListener([weakText = AceType::WeakClaim(this)](const Dimension& value) {
        auto overlay = weakText.Upgrade();
        if (overlay) {
            overlay->strokeWidth_ = value;
            overlay->clipWidth_ = std::clamp((CLIP_WIDTH - (value - STROKE_MIN_WIDTH).Value()), 0.0, CLIP_WIDTH);
            overlay->MarkNeedRender(true);
        }
    });
}

void RenderTextOverlay::BuildEndPointOffsetAnimation(
    const RefPtr<KeyframeAnimation<double>>& offsetAnimation, double from, double to, bool reverse)
{
    auto offsetFrameStart = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_BEGINNING, from);
    auto offsetFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_ENDING, to);

    offsetAnimation->AddKeyframe(offsetFrameStart);
    if (reverse) {
        offsetFrameEnd->SetCurve(Curves::FRICTION);
    } else {
        auto offsetFrameMid = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_PERCENT_THIRTY, from);
        offsetFrameMid->SetCurve(Curves::LINEAR);
        offsetFrameEnd->SetCurve(Curves::FRICTION);
        offsetAnimation->AddKeyframe(offsetFrameMid);
    }
    offsetAnimation->AddKeyframe(offsetFrameEnd);
    offsetAnimation->AddListener([weakText = AceType::WeakClaim(this)](double value) {
        auto overlay = weakText.Upgrade();
        if (overlay) {
            overlay->ProcessEndPointAnimation(value);
            overlay->MarkNeedRender(true);
        }
    });
}

void RenderTextOverlay::BuildFrictionAnimation(
    const RefPtr<KeyframeAnimation<double>>& animation, double from, double to)
{
    auto frameStart = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_BEGINNING, from);
    auto frameEnd = AceType::MakeRefPtr<Keyframe<double>>(KEYFRAME_ENDING, to);
    frameEnd->SetCurve(Curves::FRICTION);

    animation->AddKeyframe(frameStart);
    animation->AddKeyframe(frameEnd);
    animation->AddListener([weakText = AceType::WeakClaim(this)](double value) {
        auto overlay = weakText.Upgrade();
        if (overlay) {
            overlay->ProcessFrictionAnimation(value);
            overlay->MarkNeedRender(true);
        }
    });
}

void RenderTextOverlay::ProcessFrictionAnimation(double value)
{
    // calculate start point offset of dots
    dot1StartOffset_ = DOT1_OFFSET * value;
    dot2StartOffset_ = DOT2_OFFSET * value;
    dot3StartOffset_ = DOT3_OFFSET * value;
    dot4StartOffset_ = DOT4_OFFSET * value;

    // calculate rotate degree
    rotateDegree_ = ROTATE_DEGREE * value;
}

void RenderTextOverlay::ProcessEndPointAnimation(double value)
{
    dot2Offset_ = (END_POINT - DOT2_POSITION - DOT2_OFFSET) * value;
    dot3Offset_ = (END_POINT - DOT3_POSITION - DOT3_OFFSET) * value;
    dot4Offset_ = (END_POINT - DOT4_POSITION - DOT4_OFFSET) * value;
}

void RenderTextOverlay::BuildAndStartMoreButtonAnimation()
{
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetContext());
    }
    controller_->ClearInterpolators();
    controller_->ClearAllListeners();

    RefPtr<KeyframeAnimation<Dimension>> strokeWidthAnimation = AceType::MakeRefPtr<KeyframeAnimation<Dimension>>();
    RefPtr<KeyframeAnimation<double>> startPointAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    RefPtr<KeyframeAnimation<double>> endPointAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    if (reverse_) {
        BuildStrokeWidthAnimation(strokeWidthAnimation, STROKE_MIN_WIDTH, STROKE_MAX_WIDTH, true);
        BuildFrictionAnimation(startPointAnimation, KEYFRAME_ENDING, KEYFRAME_BEGINNING);
        BuildEndPointOffsetAnimation(endPointAnimation, KEYFRAME_ENDING, KEYFRAME_BEGINNING, true);
    } else {
        BuildStrokeWidthAnimation(strokeWidthAnimation, STROKE_MAX_WIDTH, STROKE_MIN_WIDTH, false);
        BuildFrictionAnimation(startPointAnimation, KEYFRAME_BEGINNING, KEYFRAME_ENDING);
        BuildEndPointOffsetAnimation(endPointAnimation, KEYFRAME_BEGINNING, KEYFRAME_ENDING, false);
    }

    controller_->SetDuration(MORE_ANIMATION_DURATION);
    controller_->AddStopListener([more = AceType::WeakClaim(this)]() {
        auto textMore = more.Upgrade();
        if (textMore) {
            textMore->reverse_ = (!textMore->reverse_);
        }
    });
    controller_->AddInterpolator(strokeWidthAnimation);
    controller_->AddInterpolator(startPointAnimation);
    controller_->AddInterpolator(endPointAnimation);
    controller_->Forward();
}

void RenderTextOverlay::PopOverlay()
{
    if (hasPoped_) {
        return;
    }
    auto textField = weakTextField_.Upgrade();
    if (!textField) {
        return;
    }
    auto stack = textField->GetStackElement().Upgrade();
    if (stack) {
        hasPoped_ = true;
        stack->PopTextOverlay();
    }
    textField->SetIsOverlayShowed(false, needStartTwinkling_);
    textField->SetTextOverlayPushed(false);
}

void RenderTextOverlay::OnFocusChange(RenderStatus renderStatus)
{
    if (onFocusChange_) {
        onFocusChange_(renderStatus == RenderStatus::FOCUS, needCloseKeyboard_);
    }
}

void RenderTextOverlay::InitRenderChild(const RefPtr<RenderNode>& render)
{
    if (!render) {
        return;
    }

    if (AceType::InstanceOf<RenderBox>(render)) {
        if (!renderBox_) {
            renderBox_ = AceType::DynamicCast<RenderBox>(render);
        }
    } else if (AceType::InstanceOf<RenderClip>(render)) {
        if (!renderClip_) {
            renderClip_ = AceType::DynamicCast<RenderClip>(render);
        }
    } else if (AceType::InstanceOf<RenderSelectPopup>(render)) {
        if (!renderMenu_) {
            renderMenu_ = AceType::DynamicCast<RenderSelectPopup>(render);
        }
    }

    for (const auto& child : render->GetChildren()) {
        InitRenderChild(child);
    }
}

void RenderTextOverlay::ResetRenderChild()
{
    renderBox_.Reset();
    renderClip_.Reset();
    renderMenu_.Reset();
}

bool RenderTextOverlay::HasToolBarOnly() const
{
    // Child of render overlay is focus collaboration.
    auto focusCollaboration = AceType::DynamicCast<RenderFocusCollaboration>(GetChildren().front());
    if (!focusCollaboration) {
        return false;
    }
    // Child of render focus collaboration is column.
    auto column = AceType::DynamicCast<RenderFlex>(focusCollaboration->GetChildren().front());
    if (!column) {
        return false;
    }
    // Column has two children at most, tool bar and menu, if there is only one, it must be tool bar.
    return column->GetChildren().size() == 1;
}

void RenderTextOverlay::SetOnRebuild(const std::function<void(bool, bool, bool, bool, bool)>& onRebuild)
{
    onRebuild_ = onRebuild;
}

void RenderTextOverlay::SetStartAnimationCallback(const StartAnimationCallback& callback)
{
    startAnimation_ = callback;
}

void RenderTextOverlay::SetIsAnimationStarted(bool isAnimationStarted)
{
    isAnimationStarted_ = isAnimationStarted;
}

bool RenderTextOverlay::IsAnimationStarted() const
{
    return isAnimationStarted_;
}

void RenderTextOverlay::SetIsAnimationStopped(bool isAnimationStopped)
{
    isAnimationStopped_ = isAnimationStopped;
}

double RenderTextOverlay::GetHorizonOffsetForAnimation() const
{
    return horizonOffsetForAnimation_;
}

} // namespace OHOS::Ace
