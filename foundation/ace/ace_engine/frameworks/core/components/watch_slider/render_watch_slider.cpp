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

#include "core/components/watch_slider/render_watch_slider.h"

#include "base/json/json_util.h"
#include "core/components/image/image_component.h"
#include "core/components/image/render_image.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr double CHANGE_RATIO = 0.05;
constexpr double INTERMITTENT_RATIO = 0.25;
constexpr double SLIDER_CONTINUOUS_ROTATION_SENSITIVITY_NORMAL = 1.0;
// UX variation corresponding to each watch rotation base event.
constexpr double ROTATION_BASE_DELTA_DP = 0.834;
constexpr Dimension HOT_REGION = 52.0_vp;
constexpr Dimension ICON_SIZE = 28.0_vp;
constexpr Dimension PADDING_SIZE = 3.0_vp;
constexpr Dimension PADDING_ICON = 12.0_vp;
constexpr double THRESHOLD = 90.0;
constexpr double SCROLL_DELTA = 10.0;

} // namespace

void RenderWatchSlider::InitAccessibilityEventListener()
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
            slider->HandleDragUpdate(SCROLL_DELTA * CHANGE_RATIO);
            slider->HandleDragEnd(SCROLL_DELTA);
            return true;
        }
        return false;
    });

    accessibilityNode->SetActionScrollForward([weakPtr = WeakClaim(this)]() {
        const auto& slider = weakPtr.Upgrade();
        if (slider) {
            slider->HandleDragUpdate(-1 * SCROLL_DELTA * CHANGE_RATIO);
            slider->HandleDragEnd(-1 * SCROLL_DELTA * CHANGE_RATIO);
            return true;
        }
        return false;
    });
}

void RenderWatchSlider::Update(const RefPtr<Component>& component)
{
    auto sliderComponent = AceType::DynamicCast<WatchSliderComponent>(component);
    if (!sliderComponent) {
        LOGE("component type error");
        return;
    }
    step_ = sliderComponent->GetStep();
    value_ = sliderComponent->GetValue();
    max_ = sliderComponent->GetMaxValue();
    min_ = sliderComponent->GetMinValue();
    disable_ = sliderComponent->GetDisable();
    minIconUrl_ = sliderComponent->GetMinIconUrl();
    maxIconUrl_ = sliderComponent->GetMaxIconUrl();
    backgroundColor_ = sliderComponent->GetBackgroundColor();
    selectColor_ = sliderComponent->GetSelectColor();
    isContinuous_ = sliderComponent->GetContinuous();

    if (LessOrEqual(max_, min_)) {
        max_ = 100.0;
        min_ = 0.0;
    }

    if (!isContinuous_) {
        value_ = std::round(value_ / ((max_ - min_) / 4.0)) * ((max_ - min_) / 4.0);
    }

    // Event update
    if (!sliderComponent->GetOnMoveEndEventId().IsEmpty()) {
        onMoveEnd_ = AceAsyncEvent<void(const std::string&)>::Create(sliderComponent->GetOnMoveEndEventId(), context_);
    }
    if (!sliderComponent->GetOnMovingEventId().IsEmpty()) {
        onMoving_ = AceAsyncEvent<void(const std::string&)>::Create(sliderComponent->GetOnMovingEventId(), context_);
    }

    // initialize event handler
    Initialize();
    InitAccessibilityEventListener();

    RefPtr<ImageComponent> minIconComponent = AceType::MakeRefPtr<ImageComponent>(minIconUrl_);
    RefPtr<ImageComponent> maxIconComponent = AceType::MakeRefPtr<ImageComponent>(maxIconUrl_);
    minIconComponent->SetWidth(ICON_SIZE);
    minIconComponent->SetHeight(ICON_SIZE);
    maxIconComponent->SetHeight(ICON_SIZE);
    maxIconComponent->SetWidth(ICON_SIZE);
    if (!topIcon_) {
        topIcon_ = AceType::DynamicCast<RenderImage>(maxIconComponent->CreateRenderNode());
        topIcon_->Attach(GetContext());
        AddChild(topIcon_);
    }

    if (!bottomIcon_) {
        bottomIcon_ = AceType::DynamicCast<RenderImage>(minIconComponent->CreateRenderNode());
        bottomIcon_->Attach(GetContext());
        AddChild(bottomIcon_);
    }

    topIcon_->Update(maxIconComponent);
    bottomIcon_->Update(minIconComponent);

    const auto& rotationController = sliderComponent->GetRotationController();
    if (rotationController) {
        auto weak = AceType::WeakClaim(this);
        rotationController->SetRequestRotationImpl(weak, context_);
    }
}

void RenderWatchSlider::PerformLayout()
{
    SetLayoutSize(GetLayoutParam().GetMaxSize());

    double iconSizeInPx = NormalizeToPx(ICON_SIZE);
    LayoutParam iconParam;
    iconParam.SetFixedSize(Size(iconSizeInPx, iconSizeInPx));
    topIcon_->Layout(iconParam);
    bottomIcon_->Layout(iconParam);
}

void RenderWatchSlider::Initialize()
{
    if (!dragDetector_) {
        dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
        dragDetector_->SetOnDragUpdate([weakSlider = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleDragUpdate(info.GetMainDelta());
            }
        });
        dragDetector_->SetOnDragEnd([weakSlider = AceType::WeakClaim(this)](const DragEndInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleDragEnd(info.GetMainVelocity());
            }
        });
    }
    if (!clickDetector_) {
        clickDetector_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickDetector_->SetOnClick([weakSlider = AceType::WeakClaim(this)](const ClickInfo& info) {
            auto slider = weakSlider.Upgrade();
            if (slider) {
                slider->HandleClick(info.GetGlobalLocation());
            }
        });
    }
}

void RenderWatchSlider::HandleClick(const OHOS::Ace::Offset& clickPosition)
{
    if (topIconRect_.IsInRegion(Point(clickPosition.GetX(), clickPosition.GetY()))) {
        if (isContinuous_) {
            value_ = value_ + CHANGE_RATIO * (max_ - min_);
        } else {
            value_ = value_ + INTERMITTENT_RATIO * (max_ - min_);
        }
        if (value_ > max_) {
            value_ = max_;
        }
    } else if (bottomIconRect_.IsInRegion(Point(clickPosition.GetX(), clickPosition.GetY()))) {
        if (isContinuous_) {
            value_ = value_ - CHANGE_RATIO * (max_ - min_);
        } else {
            value_ = value_ - INTERMITTENT_RATIO * (max_ - min_);
        }
        if (value_ < min_) {
            value_ = min_;
        }
    }
    if (onMoveEnd_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("progress", std::to_string(value_).c_str());
        jsonResult->Put("isEnd", "true");
        onMoveEnd_(std::string(R"("change",)").append(jsonResult->ToString()));
    }
    MarkNeedRender();
}

void RenderWatchSlider::HandleDragUpdate(double delta)
{
    if (NearZero(GetLayoutSize().Height())) {
        return;
    }
    double diff = (delta / GetLayoutSize().Height()) * (max_ - min_);
    if (!isContinuous_) {
        accumulatedValue_ += diff;
        double blockValue = (max_ - min_) * INTERMITTENT_RATIO;
        diff = std::round(accumulatedValue_ / blockValue) * blockValue;
        // delete the consumed value
        accumulatedValue_ -= diff;
    }
    value_ = value_ - diff;
    if (value_ > max_) {
        value_ = max_;
    }
    if (value_ < min_) {
        value_ = min_;
    }
    if (onMoveEnd_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("progress", std::to_string(value_).c_str());
        jsonResult->Put("isEnd", "false");
        onMoveEnd_(std::string(R"("change",)").append(jsonResult->ToString()));
    }
    MarkNeedRender();
}

void RenderWatchSlider::HandleDragEnd(double velocity)
{
    if (onMoveEnd_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("progress", std::to_string(value_).c_str());
        jsonResult->Put("isEnd", "true");
        onMoveEnd_(std::string(R"("change",)").append(jsonResult->ToString()));
    }
    accumulatedValue_ = 0.0;
    MarkNeedRender();
}

void RenderWatchSlider::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!disable_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        clickDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
        result.emplace_back(clickDetector_);
    }
}

bool RenderWatchSlider::OnRotation(const RotationEvent& event)
{
    double eventValue = -1 * event.value;
    accumulatedValue_ += eventValue;
    if (!isContinuous_) {
        if (accumulatedValue_ > THRESHOLD) {
            value_ += INTERMITTENT_RATIO * (max_ - min_);
            accumulatedValue_ = 0.0;
        } else if (accumulatedValue_ < -THRESHOLD) {
            value_ -= INTERMITTENT_RATIO * (max_ - min_);
            accumulatedValue_ = 0.0;
        }
    } else {
        auto context = GetContext().Upgrade();
        if (!context) {
            LOGE("get context failed!");
            return false;
        }
        if (NearZero(GetLayoutSize().Height())) {
            LOGE("slider layout height is near zero");
            return false;
        }
        double deltaPx = context->NormalizeToPx(Dimension(ROTATION_BASE_DELTA_DP, DimensionUnit::VP));
        double step = (max_ - min_) * deltaPx / GetLayoutSize().Height();
        double count = round(accumulatedValue_ / ROTATION_BASE_DELTA_DP);
        value_ += count * step * SLIDER_CONTINUOUS_ROTATION_SENSITIVITY_NORMAL;
        accumulatedValue_ = 0.0;
    }
    if (value_ > max_) {
        value_ = max_;
    }
    if (value_ < min_) {
        value_ = min_;
    }
    if (onMoveEnd_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("progress", std::to_string(value_).c_str());
        jsonResult->Put("isEnd", "false");
        onMoveEnd_(std::string(R"("change",)").append(jsonResult->ToString()));
    }
    MarkNeedRender();
    return true;
}

void RenderWatchSlider::OnPaintFinish()
{
    UpdateAccessibilityAttr();
}

void RenderWatchSlider::UpdateAccessibilityAttr()
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetAccessibilityValue(value_, min_, max_);
}

void RenderWatchSlider::UpdatePosition(const Offset& offset)
{
    double iconSizeInPx = NormalizeToPx(ICON_SIZE);
    double iconCenterX = offset.GetX() + GetLayoutSize().Width() / 2.0;
    topIconPosition_ =
        Offset(iconCenterX - (iconSizeInPx / 2.0), offset.GetY() + NormalizeToPx(PADDING_SIZE + PADDING_ICON));
    bottomIconPosition_ = Offset(iconCenterX - (iconSizeInPx / 2.0),
        offset.GetY() + GetLayoutSize().Height() - NormalizeToPx(PADDING_SIZE + PADDING_ICON + ICON_SIZE));

    topIconRect_.SetOffset(
        Offset(iconCenterX - NormalizeToPx(HOT_REGION) / 2.0, offset.GetY() + NormalizeToPx(PADDING_SIZE)));
    topIconRect_.SetSize(Size(NormalizeToPx(HOT_REGION), NormalizeToPx(HOT_REGION)));
    bottomIconRect_.SetOffset(Offset(iconCenterX - NormalizeToPx(HOT_REGION) / 2.0,
        offset.GetY() + GetLayoutSize().Height() - NormalizeToPx(PADDING_SIZE + HOT_REGION)));
    bottomIconRect_.SetSize(Size(NormalizeToPx(HOT_REGION), NormalizeToPx(HOT_REGION)));
}

} // namespace OHOS::Ace
