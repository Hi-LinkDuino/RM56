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

#include "core/components/slider/render_slider.h"

#include "base/json/json_util.h"
#include "base/log/event_report.h"
#include "core/components/box/render_box.h"
#include "core/components/bubble/render_bubble.h"
#include "core/components/tip/render_tip.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr double DOUBLE_TO_PERCENT = 100.0;
constexpr double CHANGE_RATIO = 0.2;
constexpr double DEFAULT_NORMAL_RADIUS_SCALE = 1.0;
constexpr double DEFAULT_LARGE_RADIUS_SCALE = 1.1;
constexpr Dimension DEFAULT_OUTSET_TRACK_THICKNESS = 4.0_vp;
constexpr Dimension DEFAULT_INSET_TRACK_THICKNESS = 20.0_vp;
constexpr int32_t DEFAULT_SLIDER_ANIMATION_DURATION = 150;
constexpr int32_t SLIDER_MOVE_DURATION = 100;
constexpr Dimension DEFAULT_SLIDER_WIDTH_DP = 260.0_vp;
constexpr Dimension DEFAULT_SLIDER_HEIGHT_DP = 40.0_vp;

} // namespace

RenderSlider::RenderSlider() : RenderNode(true) {}

void RenderSlider::Update(const RefPtr<Component>& component)
{
    auto slider = AceType::DynamicCast<SliderComponent>(component);
    if (!slider) {
        LOGE("Update error, slider component is null");
        return;
    }
    if (slider->GetOnChange()) {
        onChange_ = *slider->GetOnChange();
    }
    sliderComponent_ = slider;
    if (!blockActive_) {
        Initialize();
        if (!slider) {
            LOGE("RenderSlider update with nullptr");
            EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
            return;
        }
        showSteps_ = slider->NeedShowSteps();
        showTips_ = slider->NeedShowTips();
        mode_ = slider->GetSliderMode();
        min_ = slider->GetMinValue();
        max_ = slider->GetMaxValue();
        step_ = slider->GetStep();
        disable_ = slider->GetDisable();
        SetTextDirection(slider->GetTextDirection());
        direction_ = slider->GetDirection();
        isReverse_ = slider->IsReverse();
        isError_ = false;
        isValueError_ = false;
        if (slider->GetThickness().IsValid()) {
            thickness_ = NormalizeToPx(slider->GetThickness());
        } else {
            thickness_ = mode_ == SliderMode::INSET ? NormalizeToPx(DEFAULT_INSET_TRACK_THICKNESS) :
               NormalizeToPx(DEFAULT_OUTSET_TRACK_THICKNESS);
        }
        scaleValue_ = mode_ == SliderMode::INSET ? thickness_ / NormalizeToPx(DEFAULT_INSET_TRACK_THICKNESS) :
            thickness_ / NormalizeToPx(DEFAULT_OUTSET_TRACK_THICKNESS);
        SyncValueToComponent(std::clamp(slider->GetValue(), min_, max_));
        if (min_ >= max_ || step_ > (max_ - min_) || step_ <= 0.0) {
            isValueError_ = true;
            LOGE("RenderSlider update min, max, value, step error");
            MarkNeedLayout();
            return;
        }
        LOGD("Slider::RenderSlider::Update Min: %{public}lf max: %{public}lf step: %{public}lf value: %{public}lf",
            min_, max_, step_, value_);
        totalRatio_ = (value_ - min_) / (max_ - min_);

        // Event update
        if (!slider->GetOnMoveEndEventId().IsEmpty()) {
            onMoveEnd_ = AceAsyncEvent<void(const std::string&)>::Create(slider->GetOnMoveEndEventId(), context_);
        }
        if (!slider->GetOnMovingEventId().IsEmpty()) {
            onMoving_ = AceAsyncEvent<void(const std::string&)>::Create(slider->GetOnMovingEventId(), context_);
        }
        InitAccessibilityEventListener();

        // animation control
        if (!controller_) {
            controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        }

        const auto& rotationController = slider->GetRotationController();
        if (rotationController) {
            auto weak = AceType::WeakClaim(this);
            rotationController->SetRequestRotationImpl(weak, context_);
        }

        MarkNeedLayout();
    }
}

void RenderSlider::PerformLayout()
{
    Size size = Measure();

    // Update layout size.
    SetLayoutSize(size);

    // The size of child will be set in flutter slider
    UpdateTouchRegion();
}

void RenderSlider::OnPaintFinish()
{
    UpdateAccessibilityAttr();
}

void RenderSlider::UpdateAccessibilityAttr()
{
    // Update text with slider value
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetText(std::to_string(value_));
    accessibilityNode->SetAccessibilityValue(value_, min_, max_);
    auto context = context_.Upgrade();
    if (context) {
        AccessibilityEvent sliderEvent;
        sliderEvent.nodeId = accessibilityNode->GetNodeId();
        sliderEvent.eventType = "selected";
        sliderEvent.componentType = "slider";
        sliderEvent.currentItemIndex = value_;
        sliderEvent.itemCount = max_ - min_;
        context->SendEventToAccessibility(sliderEvent);
    }
}

void RenderSlider::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
    accessibilityNode->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);

    accessibilityNode->SetActionScrollBackward([weakPtr = WeakClaim(this)]() {
        const auto& slider = weakPtr.Upgrade();
        if (slider) {
            slider->HandleScrollUpdate(-1);
            return true;
        }
        return false;
    });

    accessibilityNode->SetActionScrollForward([weakPtr = WeakClaim(this)]() {
        const auto& slider = weakPtr.Upgrade();
        if (slider) {
            slider->HandleScrollUpdate(1);
            return true;
        }
        return false;
    });
}

void RenderSlider::HandleScrollUpdate(double delta)
{
    value_ = value_ + (max_ - min_) * CHANGE_RATIO * delta;
    if (value_ > max_) {
        value_ = max_;
    }
    if (value_ < min_) {
        value_ = min_;
    }
    SyncValueToComponent(value_);
    if (min_ >= max_) {
        return;
    }
    totalRatio_ = (value_ - min_) / (max_ - min_);
    UpdateTouchRegion();
    MarkNeedLayout();
    FireMovingEvent(SliderEvent::ACCESSIBILITY);
}

Size RenderSlider::Measure()
{
    if (direction_ == Axis::VERTICAL) {
        Size layoutConstrainMax = GetLayoutParam().GetMaxSize();
        LayoutParam childrenLayoutConstrain;
        if (layoutConstrainMax.Height() == Size::INFINITE_SIZE) {
            trackLength_ = NormalizeToPx(DEFAULT_SLIDER_WIDTH_DP) - 2 * NormalizeToPx(SLIDER_PADDING_DP);
            childrenLayoutConstrain.SetMaxSize(
                Size(NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP), NormalizeToPx(DEFAULT_SLIDER_WIDTH_DP)));
        } else {
            trackLength_ = layoutConstrainMax.Height() - 2 * NormalizeToPx(SLIDER_PADDING_DP);
            childrenLayoutConstrain.SetMaxSize(Size(NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP), trackLength_));
        }
        for (const auto& item : GetChildren()) {
            item->Layout(childrenLayoutConstrain);
        }
        if (trackLength_ < 0.0) {
            trackLength_ = 0.0;
        }
        return Size(NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP), layoutConstrainMax.Height());
    } else {
        Size layoutConstrainMax = GetLayoutParam().GetMaxSize();
        LayoutParam childrenLayoutConstrain;
        if (layoutConstrainMax.Width() == Size::INFINITE_SIZE) {
            // set the default size to (260dp, 40dp) and length to 160dp
            trackLength_ = NormalizeToPx(DEFAULT_SLIDER_WIDTH_DP) - 2 * NormalizeToPx(SLIDER_PADDING_DP);
            childrenLayoutConstrain.SetMaxSize(
                Size(NormalizeToPx(DEFAULT_SLIDER_WIDTH_DP), NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP)));
        } else {
            trackLength_ = layoutConstrainMax.Width() - 2 * NormalizeToPx(SLIDER_PADDING_DP);
            childrenLayoutConstrain.SetMaxSize(Size(trackLength_, NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP)));
        }
        for (const auto& item : GetChildren()) {
            item->Layout(childrenLayoutConstrain);
        }
        if (trackLength_ < 0.0) {
            trackLength_ = 0.0;
        }
        return Size(layoutConstrainMax.Width(), NormalizeToPx(DEFAULT_SLIDER_HEIGHT_DP));
    }
}

void RenderSlider::Initialize()
{
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<HorizontalDragRecognizer>();
        dragDetector_->SetOnDragStart([weakSlider = AceType::WeakClaim(this)](const DragStartInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleDragStart(info.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragUpdate([weakSlider = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleDragUpdate(info.GetLocalLocation());
            }
        });
        dragDetector_->SetOnDragEnd([weakSlider = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleDragEnd();
            }
        });
    }
    if (!clickDetector_) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickDetector_->SetOnClick([weakSlider = AceType::WeakClaim(this)](const ClickInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleClick(info.GetLocalLocation());
            }
        });
    }
}

bool RenderSlider::MouseHoverTest(const Point& parentLocalPoint)
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }
    if (blockTouchRegion_.ContainsInRegion(
            parentLocalPoint.GetX() - GetPosition().GetX(), parentLocalPoint.GetY() - GetPosition().GetY())) {
        if (mouseState_ == MouseState::NONE) {
            OnMouseHoverEnterTest();
            mouseState_ = MouseState::HOVER;
        }
        context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
        return true;
    } else {
        if (mouseState_ == MouseState::HOVER) {
            OnMouseHoverExitTest();
            mouseState_ = MouseState::NONE;
        }
        return false;
    }
}

void RenderSlider::FireMoveEndEvent()
{
    if (onMoveEnd_) {
        std::string param = std::string(R"("changed",{"progress":)")
                                .append(std::to_string(value_))
                                .append(R"(},{"value":)")
                                .append(std::to_string(value_))
                                .append("}");
        onMoveEnd_(param);
    }
}

void RenderSlider::FireMovingEvent(SliderEvent mode)
{
    if (onMoving_ || onChange_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("progress", std::to_string(value_).c_str());
        switch (mode) {
            case SliderEvent::MOVE_START:
                jsonResult->Put("isEnd", "false");
                jsonResult->Put("mode", "start");
                if (onChange_) {
                    onChange_(value_, static_cast<int>(SliderEvent::MOVE_START));
                }
                break;
            case SliderEvent::MOVE_MOVING:
                jsonResult->Put("isEnd", "false");
                jsonResult->Put("mode", "move");
                if (onChange_ && !NearEqual(value_, preMovingValue_)) {
                    onChange_(value_, static_cast<int>(SliderEvent::MOVE_MOVING));
                    preMovingValue_ = value_;
                }
                break;
            case SliderEvent::MOVE_END:
                jsonResult->Put("isEnd", "true");
                jsonResult->Put("mode", "end");
                if (onChange_) {
                    onChange_(value_, static_cast<int>(SliderEvent::MOVE_END));
                }
                break;
            case SliderEvent::CLICK:
                jsonResult->Put("isEnd", "true");
                jsonResult->Put("mode", "click");
                if (onChange_) {
                    onChange_(value_, static_cast<int>(SliderEvent::CLICK));
                }
                break;
            case SliderEvent::ACCESSIBILITY:
                jsonResult->Put("isEnd", "false");
                jsonResult->Put("mode", "accessibility");
                if (onChange_) {
                    onChange_(value_, static_cast<int>(SliderEvent::ACCESSIBILITY));
                }
                break;
            case SliderEvent::FOCUS:
                jsonResult->Put("isEnd", "true");
                jsonResult->Put("mode", "keyevent");
                if (onChange_) {
                    onChange_(value_, static_cast<int>(SliderEvent::FOCUS));
                }
                break;
            default:
                break;
        }
        jsonResult->Put("value", value_);
        if (onMoving_) {
            onMoving_(std::string(R"("change",)").append(jsonResult->ToString()));
        }
    }
}

void RenderSlider::HandleClick(const Offset& clickPosition)
{
    LOGD("Slider::Handle click position x:%{public}f y:%{public}f", clickPosition.GetX(), clickPosition.GetY());
    if (NearZero(trackLength_)) {
        totalRatio_ = 0.0;
        return;
    }
    std::string accessibilityEventType = "click";
    SendAccessibilityEvent(accessibilityEventType);
    if (NeedSmoothMoving()) {
        UpdateBlockPosition(clickPosition, true);
    } else {
        RenderBlockPosition(clickPosition);
        UpdateTouchRegion();
    }
    insideBlockRegion_ = false;
    FireMovingEvent(SliderEvent::CLICK);
    FireMoveEndEvent();
}

void RenderSlider::HandleDragStart(const Offset& startPoint)
{
    if (showTips_ && tip_) {
        tip_->SetVisible(true);
    }
    if (NearZero(trackLength_)) {
        totalRatio_ = 0.0;
        return;
    }
    if (blockTouchRegion_.ContainsInRegion(startPoint.GetX(), startPoint.GetY())) {
        insideBlockRegion_ = true;
        blockActive_ = true;
        UpdateTouchRegion();
        if (!controller_->IsStopped()) {
            controller_->Stop();
        }
        UpdateAnimation();
        controller_->Play();
        isDraging_ = true;
        FireMovingEvent(SliderEvent::MOVE_START);
    }
}

void RenderSlider::HandleDragUpdate(const Offset& updatePoint)
{
    if (NearZero(trackLength_)) {
        totalRatio_ = 0.0;
        return;
    }
    if (insideBlockRegion_) {
        RenderBlockPosition(updatePoint);
        FireMovingEvent(SliderEvent::MOVE_MOVING);
    }
}

void RenderSlider::HandleDragEnd()
{
    if (isDraging_) {
        isDraging_ = false;
    }
    if (tip_) {
        tip_->SetVisible(false);
    }
    if (NearZero(trackLength_)) {
        totalRatio_ = 0.0;
        return;
    }
    if (insideBlockRegion_) {
        MarkNeedLayout();
        UpdateTouchRegion();
    }
    FireMovingEvent(SliderEvent::MOVE_END);
    FireMoveEndEvent();

    insideBlockRegion_ = false;
    blockActive_ = false;

    if (!controller_->IsStopped()) {
        controller_->Stop();
    }
    UpdateAnimation();
    controller_->Play();
}

// Render the block position after clicking or dragging
void RenderSlider::RenderBlockPosition(const Offset& touchPosition)
{
    double diff = 0.0;
    if (direction_ == Axis::VERTICAL) {
        diff = isReverse_ ?
            GetLayoutSize().Height() - touchPosition.GetY() : touchPosition.GetY() - NormalizeToPx(SLIDER_PADDING_DP);
    } else {
        if ((GetTextDirection() == TextDirection::LTR &&
            !isReverse_) || (GetTextDirection() == TextDirection::RTL && isReverse_)) {
            diff = touchPosition.GetX() - NormalizeToPx(SLIDER_PADDING_DP);
        } else if ((GetTextDirection() == TextDirection::RTL &&
            !isReverse_) || (GetTextDirection() == TextDirection::LTR && isReverse_)) {
            diff = GetLayoutSize().Width() - touchPosition.GetX();
        }
    }
    if (diff < 0.0) {
        SyncValueToComponent(min_);
        SetTotalRatio(0.0);
        LOGD("Slider::RenderSlider RenderBlockPosition value: %{public}lf", value_);
        MarkNeedLayout();
        return;
    }
    totalRatio_ = diff / trackLength_;
    LOGD("Slider::RenderSlider RenderBlockPosition totalRatio: %{public}lf", totalRatio_);
    if (totalRatio_ > 1.0) {
        value_ = max_;
        SetTotalRatio(1.0);
    } else {
        if (NearEqual(step_, 0.0)) {
            // continuous slider
            value_ = (max_ - min_) * totalRatio_ + min_;
        } else {
            // The following line is used to find value which is the multiple of step.
            // The example shows below
            // "value < x < value + 0.5 * step   -->  x = value"
            // "value + 0.5 * step < x < value + step  --> x = value + step"
            double stepRatio = step_ / (max_ - min_);
            SetTotalRatio(stepRatio * std::floor((totalRatio_ + HALF * stepRatio) / stepRatio));
            value_ = (max_ - min_) * totalRatio_ + min_;
        }
    }
    SyncValueToComponent(value_);
    LOGD("Slider::RenderSlider RenderBlockPosition value: %{public}lf", value_);
    MarkNeedLayout();
}

void RenderSlider::UpdateBlockPosition(const Offset& touchPosition, bool isClick)
{
    if (LessOrEqual(trackLength_, 0.0)) {
        LOGE("slider parameter trackLength_ invalid");
        return;
    }
    double diff = 0.0;
    if (direction_ == Axis::VERTICAL) {
        diff = isReverse_ ?
            GetLayoutSize().Height() - touchPosition.GetY() : touchPosition.GetY() - NormalizeToPx(SLIDER_PADDING_DP);
    } else {
        if ((GetTextDirection() == TextDirection::LTR &&
            !isReverse_) || (GetTextDirection() == TextDirection::RTL && isReverse_)) {
            diff = touchPosition.GetX() - NormalizeToPx(SLIDER_PADDING_DP);
        } else if ((GetTextDirection() == TextDirection::RTL &&
            !isReverse_) || (GetTextDirection() == TextDirection::LTR && isReverse_)) {
            diff = GetLayoutSize().Width() - touchPosition.GetX();
        }
    }
    double totalRatio = diff / trackLength_;
    if (LessOrEqual(diff, 0.0)) {
        value_ = min_;
    } else if (GreatOrEqual(totalRatio, 1.0)) {
        value_ = max_;
    } else {
        double stepRatio = step_ / (max_ - min_);
        double endRatio = stepRatio * std::floor((totalRatio + HALF * stepRatio) / stepRatio);
        value_ = (max_ - min_) * endRatio + min_;
        if (GreatOrEqual(value_, max_)) {
            value_ = max_;
        }
    }
    RestartMoveAnimation(value_, isClick);
}

void RenderSlider::UpdateTipText(double value)
{
    int32_t percent = std::round(value * DOUBLE_TO_PERCENT);
    std::string valueText = std::to_string(percent).append("%");
    if (tipText_ && renderText_) {
        tipText_->SetData(valueText);
        renderText_->Update(tipText_);
        renderText_->PerformLayout();
    }
}

void RenderSlider::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!isValueError_ && !disable_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        clickDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
        result.emplace_back(clickDetector_);
    }
}

Vertex RenderSlider::FindCenterVertex(double x, double y, double objectWidth, double objectHeight)
{
    // 0.5 is used to find the center position.
    return Vertex(x + objectWidth * HALF, y + objectHeight * HALF);
}

TouchRegionPoint RenderSlider::GetTopTouchRegion(const Vertex& center, double width, double height)
{
    // 0.5 is used to find the top left point of the touch region
    return TouchRegionPoint(center.GetX() - width * HALF, center.GetY() - height * HALF);
}

TouchRegionPoint RenderSlider::GetBotTouchRegion(const Vertex& center, double width, double height)
{
    // 0.5 is used to find the bot right point of the touch region
    return TouchRegionPoint(center.GetX() + width * HALF, center.GetY() + height * HALF);
}

void RenderSlider::UpdateTouchRegion()
{
    if (direction_ == Axis::VERTICAL) {
        double dxOffset = GetLayoutSize().Width() * HALF;
        double dyOffset = trackLength_ * totalRatio_ + NormalizeToPx(SLIDER_PADDING_DP);
        Vertex blockCenter = isReverse_ ?
            TouchRegionPoint(dxOffset, GetLayoutSize().Height() - dyOffset) : TouchRegionPoint(dxOffset, dyOffset);
        TouchRegionPoint blockTopPoint =
            GetTopTouchRegion(blockCenter, NormalizeToPx(blockHotWidth_), NormalizeToPx(blockHotHeight_));
        TouchRegionPoint blockBottomPoint =
            GetBotTouchRegion(blockCenter, NormalizeToPx(blockHotWidth_), NormalizeToPx(blockHotHeight_));
        blockTouchRegion_ = TouchRegion(blockTopPoint, blockBottomPoint);
    } else {
        double dxOffset = trackLength_ * totalRatio_ + NormalizeToPx(SLIDER_PADDING_DP);
        double dyOffset = GetLayoutSize().Height() * HALF;
        Vertex blockCenter = TouchRegionPoint();
        if ((GetTextDirection() == TextDirection::LTR &&
            !isReverse_) || (GetTextDirection() == TextDirection::RTL && isReverse_)) {
            blockCenter = TouchRegionPoint(dxOffset, dyOffset);
        } else if ((GetTextDirection() == TextDirection::RTL &&
            !isReverse_) || (GetTextDirection() == TextDirection::LTR && isReverse_)) {
            blockCenter = TouchRegionPoint(GetLayoutSize().Width() - dxOffset, dyOffset);
        }
        TouchRegionPoint blockTopPoint =
            GetTopTouchRegion(blockCenter, NormalizeToPx(blockHotWidth_), NormalizeToPx(blockHotHeight_));
        TouchRegionPoint blockBottomPoint =
            GetBotTouchRegion(blockCenter, NormalizeToPx(blockHotWidth_), NormalizeToPx(blockHotHeight_));
        blockTouchRegion_ = TouchRegion(blockTopPoint, blockBottomPoint);
    }
}

bool RenderSlider::HandleFocusEvent(const KeyEvent& keyEvent)
{
    bool updateEvent = false;
    if (NearZero(trackLength_)) {
        totalRatio_ = 0.0;
        return updateEvent;
    }
    switch (keyEvent.code) {
        case KeyCode::TV_CONTROL_LEFT:
            totalRatio_ -= step_ / (max_ - min_);
            if (totalRatio_ < 0.0) {
                totalRatio_ = 0.0;
            }
            SyncValueToComponent((max_ - min_) * totalRatio_ + min_);
            MarkNeedLayout();
            updateEvent = true;
            break;
        case KeyCode::TV_CONTROL_RIGHT:
            totalRatio_ += step_ / (max_ - min_);
            if (totalRatio_ > 1.0) {
                totalRatio_ = 1.0;
            }
            SyncValueToComponent((max_ - min_) * totalRatio_ + min_);
            MarkNeedLayout();
            updateEvent = true;
            break;
        default:
            updateEvent = false;
            break;
    }
    if (updateEvent) {
        FireMoveEndEvent();
        FireMovingEvent(SliderEvent::FOCUS);
        std::string accessibilityEventType = "focus";
        SendAccessibilityEvent(accessibilityEventType);
    }
    return updateEvent;
}

void RenderSlider::StartMoveAnimation(double from, double to, bool isClick)
{
    if (NearEqual(from, to)) {
        return;
    }
    if (!moveController_) {
        moveController_ = AceType::MakeRefPtr<Animator>(GetContext());
    } else if (moveController_->IsRunning()) {
        moveController_->Finish();
    }
    moveController_->ClearInterpolators();
    moveController_->ClearAllListeners();

    moveController_->AddStartListener([weak = AceType::WeakClaim(this), to]() {
        auto slider = weak.Upgrade();
        if (slider) {
            slider->animationEnd_ = to;
        }
    });

    moveController_->AddStopListener([weak = AceType::WeakClaim(this), isClick]() {
        auto slider = weak.Upgrade();
        if (slider) {
            slider->SyncValueToComponent(slider->totalRatio_ * (slider->max_ - slider->min_) + slider->min_);
            slider->SetTotalRatio(slider->totalRatio_);
            slider->UpdateTouchRegion();
        }
    });

    ResetMoveAnimation(from, to);
    moveController_->SetDuration(SLIDER_MOVE_DURATION);
    moveController_->AddInterpolator(moveAnimation_);
    moveController_->Play();
}

void RenderSlider::CalculateTotalRadio()
{
    auto ratio = (value_ - min_) / (max_ - min_);
    totalRatio_ = std::clamp(ratio, 0.0, 1.0);
}

void RenderSlider::ResetMoveAnimation(double from, double to)
{
    moveAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::MAGNETIC);
    auto weak = AceType::WeakClaim(this);
    moveAnimation_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto slider = weak.Upgrade();
        if (slider) {
            slider->value_ = value;
            slider->CalculateTotalRadio();
            slider->MarkNeedLayout();
        }
    }));
}

void RenderSlider::RestartMoveAnimation(double value, bool isClick)
{
    if (moveController_ && moveController_->IsRunning()) {
        if (!NearEqual(value, animationEnd_)) {
            moveController_->Stop();
            StartMoveAnimation(value_, value, isClick);
        }
    } else {
        StartMoveAnimation(value_, value, isClick);
    }
}

void RenderSlider::UpdateAnimation()
{
    double from = DEFAULT_NORMAL_RADIUS_SCALE;
    double to = DEFAULT_LARGE_RADIUS_SCALE;
    if (!blockActive_) {
        from = DEFAULT_LARGE_RADIUS_SCALE;
        to = DEFAULT_NORMAL_RADIUS_SCALE;
    }

    if (translate_) {
        controller_->RemoveInterpolator(translate_);
    }
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto sliderComp = weak.Upgrade();
        if (sliderComp) {
            sliderComp->radiusScale_ = value;
            sliderComp->MarkNeedLayout();
        }
    }));
    controller_->SetDuration(DEFAULT_SLIDER_ANIMATION_DURATION);
    controller_->AddInterpolator(translate_);
}

} // namespace OHOS::Ace
