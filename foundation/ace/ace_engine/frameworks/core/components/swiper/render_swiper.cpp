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

#include "core/components/swiper/render_swiper.h"

#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "core/animation/curve_animation.h"
#include "core/animation/friction_motion.h"
#include "core/animation/keyframe.h"
#include "core/common/ace_application_info.h"
#include "core/common/frontend.h"
#include "core/components/align/render_align.h"
#include "core/components/display/render_display.h"
#include "core/components/swiper/swiper_component.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/multi_child.h"

namespace OHOS::Ace {
namespace {

constexpr double MAX_VIEW_PORT_WIDTH = 1080.0;
constexpr int32_t LEAST_SLIDE_ITEM_COUNT = 2;
constexpr uint8_t MAX_OPACITY = 255;
constexpr double CUR_START_TRANSLATE_TIME = 0.0;
constexpr double CUR_END_TRANSLATE_TIME = 1.0;
constexpr double CUR_START_OPACITY_TIME = 0.0;
constexpr uint8_t CUR_START_OPACITY_VALUE = 255;
constexpr uint8_t CUR_END_OPACITY_VALUE = 0;
constexpr double CUR_END_OPACITY_TIME = 0.5;
constexpr double TARGET_START_TRANSLATE_TIME = 0.0;
constexpr double TARGET_END_TRANSLATE_TIME = 1.0;
constexpr double TARGET_START_OPACITY_TIME = 0.3;
constexpr double TARGET_END_OPACITY_TIME = 1.0;
constexpr uint8_t TARGET_START_OPACITY_VALUE = 0;
constexpr uint8_t TARGET_END_OPACITY_VALUE = 255;
constexpr uint8_t TRANSLATE_RATIO = 10;
constexpr int32_t COMPONENT_CHANGE_END_LISTENER_KEY = 1001;
constexpr double MIN_SCROLL_OFFSET = 0.5;
constexpr int32_t DEFAULT_SHOWING_COUNT = 1;

// for watch rotation const param
constexpr double ROTATION_SENSITIVITY_NORMAL = 1.4;
constexpr uint64_t ROTATION_INTERVAL_MS = 200;

// for indicator animation const param
constexpr double SPRING_MASS = 1.0;
constexpr double SPRING_STIFF = 700.0;
constexpr double SPRING_DAMP = 22.0;
constexpr double SPRING_DAMP_INC = 5.0;
constexpr double DRAG_CALC_STRETCH_STEP = 0.01;
constexpr int32_t DRAG_CALC_STRETCH_STEP_INT = 1;   // 100*DRAG_CALC_STRETCH_STEP
constexpr int32_t DRAG_CALC_STRETCH_STEP_MAX = 100; // 100*DRAG_CALC_STRETCH_STEP_INT
constexpr double DRAG_OFFSET_START_DP = 4.0;
constexpr double DRAG_OFFSET_SWITCH_DP = 14.0;
constexpr double DRAG_STRETCH_LONGEST_DP = 80.0;
constexpr double DRAG_STRETCH_BASE_WIDTH = 1.0;
constexpr double DRAG_STRETCH_BASE_HIGH = 1.0;
constexpr double DRAG_STRETCH_MAX_WIDTH = 1.2;
constexpr double DRAG_STRETCH_MAX_HIGH = 0.8;
constexpr double DRAG_OFFSET_MIN = 0.0;
constexpr double DRAG_OFFSET_MAX = 1.0;
constexpr double ZOOM_MIN = 0.0;
constexpr double ZOOM_MAX = 1.0;
constexpr double OPACITY_MIN = 0.0;
constexpr double OPACITY_MAX = 0.1;
constexpr double ZOOM_DOT_MIN = 0.0;
constexpr double ZOOM_DOT_MAX = 1.0;
constexpr double ZOOM_HOTZONE_MAX_RATE = 1.33;
constexpr double INDICATOR_DIRECT_FORWARD = 1.0;
constexpr double INDICATOR_DIRECT_BACKWARD = -1.0;
constexpr int32_t VIBRATE_DURATION = 30;
constexpr int32_t ZOOM_IN_DURATION = 250;
constexpr int32_t ZOOM_OUT_DURATION = 250;
constexpr int32_t ZOOM_OUT_HOVER_DURATION = 250;
constexpr int32_t ZOOM_IN_DOT_DURATION = 100;
constexpr int32_t ZOOM_OUT_DOT_DURATION = 150;
constexpr int32_t DRAG_RETRETION_DURATION = 250;

// indicator animation curve
const RefPtr<CubicCurve> INDICATOR_FOCUS_HEAD = AceType::MakeRefPtr<CubicCurve>(0.2f, 0.0f, 1.0f, 1.0f);
const RefPtr<CubicCurve> INDICATOR_FOCUS_TAIL = AceType::MakeRefPtr<CubicCurve>(1.0f, 0.0f, 1.0f, 1.0f);
const RefPtr<CubicCurve> INDICATOR_NORMAL_POINT = AceType::MakeRefPtr<CubicCurve>(0.4f, 0.0f, 1.0f, 1.0f);
const RefPtr<CubicCurve> INDICATOR_ZONE_STRETCH = AceType::MakeRefPtr<CubicCurve>(0.1f, 0.2f, 0.48f, 1.0f);

// for indicator
constexpr double DELAY_TIME_DEFAULT = 250;
constexpr int32_t MICROSEC_TO_NANOSEC = 1000;
constexpr int32_t INDICATOR_INVALID_HOVER_INDEX = -1;
constexpr Dimension INDICATOR_PADDING_TOP_DEFAULT = 9.0_vp;
constexpr Dimension INDICATOR_DIGITAL_PADDING = 8.0_vp;
constexpr Dimension INDICATOR_FOCUS_DEL_OFFSET = 4.0_vp;
constexpr Dimension INDICATOR_FOCUS_DEL_SIZE = 8.0_vp;
constexpr Dimension INDICATOR_FOCUS_RADIUS_DEL_SIZE = 3.0_vp;
constexpr int32_t INDICATOR_FOCUS_COLOR = 0x0a59f7;

constexpr Dimension MIN_TURN_PAGE_VELOCITY = 400.0_vp;
constexpr Dimension MIN_DRAG_DISTANCE = 25.0_vp;

} // namespace

int64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * MICROSEC_TO_NANOSEC + ts.tv_nsec / (MICROSEC_TO_NANOSEC * MICROSEC_TO_NANOSEC));
}

RenderSwiper::~RenderSwiper()
{
    if (autoPlay_ && scheduler_ && scheduler_->IsActive()) {
        scheduler_->Stop();
    }
}

void RenderSwiper::Update(const RefPtr<Component>& component)
{
    const RefPtr<SwiperComponent> swiper = AceType::DynamicCast<SwiperComponent>(component);
    if (!swiper) {
        LOGW("swiper component is null");
        return;
    }
    auto context = context_.Upgrade();
    ACE_DCHECK(context);
    if (swiper->GetUpdateType() == UpdateType::STYLE) {
        // only update indicator when update style
        indicator_ = swiper->GetIndicator();
        MarkNeedRender();
        return;
    }

    const auto& swiperController = swiper->GetSwiperController();
    if (swiperController) {
        auto weak = AceType::WeakClaim(this);
        swiperController->SetSwipeToImpl([weak](int32_t index, bool reverse) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->SwipeTo(index, reverse);
            }
        });
        swiperController->SetShowPrevImpl([weak]() {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->ShowPrevious();
            }
        });
        swiperController->SetShowNextImpl([weak]() {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->ShowNext();
            }
        });
        swiperController->SetFinishImpl([weak]() {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->FinishAllSwipeAnimation(true);
            }
        });
    }

    const auto& rotationController = swiper->GetRotationController();
    if (rotationController) {
        auto weak = AceType::WeakClaim(this);
        rotationController->SetRequestRotationImpl(weak, context_);
    }

    changeEvent_ =
        AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(swiper->GetChangeEventId(), context_);
    animationFinishEvent_ = AceAsyncEvent<void()>::Create(swiper->GetAnimationFinishEventId(), context_);
    rotationEvent_ = AceAsyncEvent<void(const std::string&)>::Create(swiper->GetRotationEventId(), context_);
    auto clickId = swiper->GetClickEventId();
    catchMode_ = true;
    if (!clickId.IsEmpty()) {
        catchMode_ = clickId.GetCatchMode();
    }
    clickEvent_ = AceAsyncEvent<void(const std::shared_ptr<ClickInfo>&)>::Create(clickId, context_);
    remoteMessageEvent_ = AceAsyncEvent<void(const std::shared_ptr<ClickInfo>&)>::Create(
        swiper->GetRemoteMessageEventId(), context_);
    RegisterChangeEndListener(COMPONENT_CHANGE_END_LISTENER_KEY, swiper->GetChangeEndListener());
    if (swiper && swiper_ && (*swiper == *swiper_) && currentIndex_ == static_cast<int32_t>(swiper->GetIndex())) {
        LOGI("swiper not changed");
        swiper_ = swiper;
        return;
    }
    fadeColor_ = swiper->GetFadeColor();
    scale_ = context->GetDipScale();

    curve_ = swiper->GetCurve();
    if (curve_) {
        if (curve_ == Curves::EASE) {
            curveRender_ = "Curves.Ease";
        } else if (curve_ == Curves::EASE_IN) {
            curveRender_ = "Curves.EaseIn";
        } else if (curve_ == Curves::EASE_OUT) {
            curveRender_ = "Curves.EaseOut";
        } else if (curve_ == Curves::EASE_IN_OUT) {
            curveRender_ = "Curves.EaseInOut";
        } else if (curve_ == Curves::FAST_OUT_SLOW_IN) {
            curveRender_ = "Curves.FastOutSlowIn";
        } else if (curve_ == Curves::LINEAR_OUT_SLOW_IN) {
            curveRender_ = "Curves.LinearOutSlowIn";
        } else if (curve_ == Curves::FAST_OUT_LINEAR_IN) {
            curveRender_ = "Curves.FastOutLinearIn";
        } else if (curve_ == Curves::FRICTION) {
            curveRender_ = "Curves.Friction";
        } else if (curve_ == Curves::EXTREME_DECELERATION) {
            curveRender_ = "Curves.ExtremeDeceleration";
        } else if (curve_ == Curves::SHARP) {
            curveRender_ = "Curves.Sharp";
        } else if (curve_ == Curves::SMOOTH) {
            curveRender_ = "Curves.Smooth";
        } else if (curve_ == Curves::LINEAR) {
            curveRender_ = "Curves.Linear";
        }
    }

    // Get item count of swiper
    const auto& children = swiper->GetChildren();
    itemCount_ = static_cast<int32_t>(children.size());
    for (const auto& child : children) {
        auto multiChild = AceType::DynamicCast<MultiChild>(child);
        if (multiChild) {
            --itemCount_;
            itemCount_ += static_cast<int32_t>(multiChild->Count());
        }
    }

    indicator_ = swiper->GetIndicator();
    mainSwiperSize_ = swiper->GetMainSwiperSize();
    digitalIndicator_ = swiper->GetDigitalIndicator();
    show_ = swiper->IsShow();
    axis_ = swiper->GetAxis();
    needReverse_ = (swiper->GetTextDirection() == TextDirection::RTL) && (axis_ == Axis::HORIZONTAL);
    disableSwipe_ = swiper->GetDisableSwipe();
    disableRotation_ = swiper->GetDisableRotation();
    itemspace_ = swiper->GetItemSpace();
    autoPlay_ = !(context && context->IsJsCard()) && swiper->IsAutoPlay();
    autoPlayInterval_ = swiper->GetAutoPlayInterval();
    loop_ = swiper->IsLoop();
    animationOpacity_ = swiper->IsAnimationOpacity();
    duration_ = swiper->GetDuration();
    showIndicator_ = swiper->IsShowIndicator();

    lazyLoadCacheSize_ = swiper->GetCachedSize() * 2 + swiper->GetDisplayCount();
    auto lazyComponent = swiper->GetLazyForEachComponent();
    UpdateItemCount(lazyComponent ? static_cast<int32_t>(lazyComponent->TotalCount()) : itemCount_);
    ClearItems(lazyComponent, static_cast<int32_t>(swiper->GetIndex()));

    if (itemCount_ < LEAST_SLIDE_ITEM_COUNT) {
        LOGD("swiper item is less than least slide count");
        swiper_ = swiper;
        index_ = 0;
        return;
    }
    UpdateIndex(swiper->GetIndex());
    Initialize(GetContext(), catchMode_);
    swiper_ = swiper; // must after UpdateIndex
}

bool RenderSwiper::RefuseUpdatePosition(int32_t index)
{
    if ((isIndicatorAnimationStart_ && !quickTrunItem_) && (index == currentIndex_ || index == targetIndex_)) {
        return true;
    }
    return false;
}

void RenderSwiper::PerformLayout()
{
    LoadItems();
    // get the prevMargin_ and nextMargin_, and make sure that prevMargin_ + nextMargin_ <= maxLength
    prevMargin_ = swiper_ ? NormalizePercentToPx(swiper_->GetPreviousMargin(), axis_ == Axis::VERTICAL, true) : 0.0;
    nextMargin_ = swiper_ ? NormalizePercentToPx(swiper_->GetNextMargin(), axis_ == Axis::VERTICAL, true) : 0.0;
    Size swiperSize = GetLayoutSize();
    if (GreatNotEqual(swiperSize.Width(), 0) && GreatNotEqual(swiperSize.Height(), 0)) {
        double maxLength = (axis_ == Axis::HORIZONTAL ? swiperSize.Width() : swiperSize.Height()) - 1.0f;
        if (LessOrEqual(prevMargin_, 0.0)) {
            prevMargin_ = 0.0;
        }
        if (LessOrEqual(nextMargin_, 0.0)) {
            nextMargin_ = 0.0;
        }

        if (GreatOrEqual(prevMargin_, maxLength)) {
            prevMargin_ = maxLength;
        }
        if (GreatOrEqual(nextMargin_, maxLength - prevMargin_)) {
            nextMargin_ = maxLength - prevMargin_;
        }
    }

    LayoutParam innerLayout = GetLayoutParam();
    Size minSize = GetLayoutParam().GetMinSize();
    Size maxSize = GetLayoutParam().GetMaxSize();
    Size maxSizeClild = maxSize;
    auto showingCount = swiper_ ? swiper_->GetDisplayCount() : 1;
    double intervalSpace = swiper_ ? NormalizeToPx(swiper_->GetItemSpace()) : 0.0;
    if (axis_ == Axis::HORIZONTAL) {
        maxSizeClild.SetWidth(
            (maxSize.Width() - intervalSpace * (showingCount - 1)) / showingCount - prevMargin_ - nextMargin_);
    } else {
        maxSizeClild.SetHeight(
            (maxSize.Height() - intervalSpace * (showingCount - 1)) / showingCount - prevMargin_ - nextMargin_);
    }
    innerLayout.SetMaxSize(maxSizeClild);

    bool isLinearLayout = swiper_ ? swiper_->GetDisplayMode() == SwiperDisplayMode::AUTO_LINEAR : false;
    double maxWidth = minSize.Width();
    double maxHeight = minSize.Height();
    if (mainSwiperSize_ == MainSwiperSize::MAX) {
        maxWidth = (axis_ == Axis::HORIZONTAL && isLinearLayout) ? 0.0 : maxSize.Width();
        maxHeight = (axis_ == Axis::VERTICAL && isLinearLayout) ? 0.0 : maxSize.Height();
    } else if (mainSwiperSize_ == MainSwiperSize::MIN) {
        maxWidth = 0.0;
        maxHeight = 0.0;
    } else if (mainSwiperSize_ == MainSwiperSize::MAX_X) {
        maxWidth = (axis_ == Axis::HORIZONTAL && isLinearLayout) ? 0.0 : maxSize.Width();
        maxHeight = 0.0;
    } else if (mainSwiperSize_ == MainSwiperSize::MAX_Y) {
        maxWidth = 0.0;
        maxHeight = (axis_ == Axis::VERTICAL && isLinearLayout) ? 0.0 : maxSize.Height();
    } else if (mainSwiperSize_ == MainSwiperSize::AUTO) {
        LOGD("Use default MainSwiperSize");
    } else {
        LOGE("input wrong MainSwiperSize");
    }

    if (axis_ == Axis::HORIZONTAL) {
        maxWidth = (showingCount > DEFAULT_SHOWING_COUNT) ? innerLayout.GetMaxSize().Width() : maxWidth;
    } else {
        maxHeight = (showingCount > DEFAULT_SHOWING_COUNT) ? innerLayout.GetMaxSize().Height() : maxHeight;
    }

    for (auto iter = items_.begin(); iter != items_.end(); iter++) {
        const auto& childItem = iter->second;
        if (!childItem) {
            continue;
        }
        childItem->Layout(innerLayout);
        if (showingCount == DEFAULT_SHOWING_COUNT) {
            maxWidth = std::max(maxWidth, childItem->GetLayoutSize().Width());
            maxHeight = std::max(maxHeight, childItem->GetLayoutSize().Height());
        }
    }

    if (mainSwiperSize_ == MainSwiperSize::AUTO) {
        if (maxSize.IsInfinite()) {
            SetLayoutSize(Size(maxWidth, maxHeight));
        } else {
            SetLayoutSize(maxSize);
        }
    } else {
        SetLayoutSize((isLinearLayout || (showingCount > DEFAULT_SHOWING_COUNT)) ? maxSize : Size(maxWidth, maxHeight));
    }

    Size layoutSize = GetLayoutSize();
    double halfSpace = swiper_ ? NormalizeToPx(swiper_->GetItemSpace()) / 2.0 : 0.0;
    if (showingCount > DEFAULT_SHOWING_COUNT) {
        swiperWidth_ = (axis_ == Axis::HORIZONTAL) ? maxWidth + halfSpace : maxWidth;
        swiperHeight_ = (axis_ == Axis::HORIZONTAL) ? maxHeight : maxHeight + halfSpace;
    } else {
        swiperWidth_ = (isLinearLayout ? maxWidth : layoutSize.Width()) + 2.0 * halfSpace;
        swiperHeight_ = (isLinearLayout ? maxHeight : layoutSize.Height());
    }

    if (isLinearLayout) {
        prevItemOffset_ = axis_ == Axis::HORIZONTAL
                              ? (needReverse_ ? swiperWidth_ + halfSpace : -swiperWidth_ - halfSpace)
                              : -swiperHeight_;
    } else {
        prevItemOffset_ = axis_ == Axis::HORIZONTAL
                              ? (needReverse_ ? swiperWidth_ - prevMargin_ - nextMargin_
                                              : -swiperWidth_ + prevMargin_ + nextMargin_)
                              : -swiperHeight_ + prevMargin_ + nextMargin_;
    }
    nextItemOffset_ = -prevItemOffset_;
    UpdateChildPosition(std::fmod(scrollOffset_, nextItemOffset_), currentIndex_, true);
    quickTrunItem_ = false;

    // layout indicator, indicator style in tv is different.
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        LayoutIndicator(swiperIndicatorData_);
    } else {
        UpdateIndicator();
    }

    if (swipeToIndex_ != -1) {
        SwipeTo(swipeToIndex_, false);
        swipeToIndex_ = -1;
    }
}

bool RenderSwiper::IsUseOnly()
{
    return true;
}

void RenderSwiper::Initialize(const WeakPtr<PipelineContext>& context, bool catchMode)
{
    if (!disableSwipe_) {
        if (axis_ == Axis::VERTICAL) {
            dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
        } else {
            dragDetector_ = AceType::MakeRefPtr<HorizontalDragRecognizer>();
        }
    }
    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(context);
    } else {
        StopSwipeAnimation();
    }
    if (!swipeToController_) {
        swipeToController_ = AceType::MakeRefPtr<Animator>(context);
    }

    InitIndicatorAnimation(context);
    InitRecognizer(catchMode);
    InitAccessibilityEventListener();

    // for auto play
    auto weak = AceType::WeakClaim(this);
    if (!scheduler_) {
        auto&& callback = [weak](uint64_t duration) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->Tick(duration);
            } else {
                LOGW("empty swiper, skip tick callback.");
            }
        };
        scheduler_ = SchedulerBuilder::Build(callback, context);
    } else if (scheduler_->IsActive()) {
        LOGD("stop autoplay");
        scheduler_->Stop();
    }

    if (autoPlay_ && !scheduler_->IsActive() && show_ && !IsDisabled()) {
        LOGD("start autoplay");
        scheduler_->Start();
    }
}

void RenderSwiper::InitRecognizer(bool catchMode)
{
    if (!clickRecognizer_) {
        auto weak = AceType::WeakClaim(this);
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetOnClick([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->HandleClick(info);
            }
        });
        clickRecognizer_->SetRemoteMessage([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->HandleRemoteMessage(info);
            }
        });
        static const int32_t bubbleModeVersion = 6;
        auto pipeline = context_.Upgrade();
        if (!catchMode && pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            clickRecognizer_->SetUseCatchMode(false);
        } else {
            clickRecognizer_->SetUseCatchMode(true);
        }
    }
    auto context = context_.Upgrade();
    if (context && context->IsJsCard()) {
        return;
    }
    InitDragRecognizer();
    InitRawDragRecognizer();
}

void RenderSwiper::InitRawDragRecognizer()
{
    if (!rawRecognizer_) {
        rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        auto weak = AceType::WeakClaim(this);
        rawRecognizer_->SetOnTouchDown([weak](const TouchEventInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->HandleTouchDown(info);
            }
        });
        rawRecognizer_->SetOnTouchUp([weak](const TouchEventInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->HandleTouchUp(info);
            }
        });
        rawRecognizer_->SetOnTouchMove([weak](const TouchEventInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->HandleTouchMove(info);
            }
        });
    }
}

void RenderSwiper::InitDragRecognizer()
{
    auto weak = AceType::WeakClaim(this);
    if (!dragDetector_) {
        return;
    }
    dragDetector_->SetOnDragStart([weak](const DragStartInfo& info) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleDragStart(info);
        }
    });
    dragDetector_->SetOnDragUpdate([weak](const DragUpdateInfo& info) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleDragUpdate(info);
        }
    });
    dragDetector_->SetOnDragEnd([weak](const DragEndInfo& info) {
        auto client = weak.Upgrade();
        if (client) {
            client->HandleDragEnd(info);
        }
    });
}

void RenderSwiper::InitAccessibilityEventListener()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);

    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionScrollForward([weakPtr]() {
        auto swiper = weakPtr.Upgrade();
        if (swiper) {
            swiper->ShowPrevious();
            return true;
        }
        return false;
    });
    refNode->SetActionScrollBackward([weakPtr]() {
        auto swiper = weakPtr.Upgrade();
        if (swiper) {
            swiper->ShowNext();
            return true;
        }
        return false;
    });
}

void RenderSwiper::UpdateIndex(int32_t index)
{
    // can't change index when stretch indicator, as stretch direct is single.
    if (index >= 0 && NearEqual(stretchRate_, 0.0)) {
        if (index >= itemCount_) {
            index = itemCount_ - 1;
        }
        if (swiper_) {
            if (index_ != index) {
                swipeToIndex_ = index; // swipe to animation need to start after perform layout
                index_ = index;
            }
        } else {
            // render node first update index
            currentIndex_ = index;
        }
    }
}

void RenderSwiper::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (IsDisabled()) {
        return;
    }
    if (dragDetector_ && !disableSwipe_) {
        dragDetector_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(dragDetector_);
    }
    if (rawRecognizer_) {
        rawRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(rawRecognizer_);
    }
    if (clickRecognizer_) {
        clickRecognizer_->SetCoordinateOffset(coordinateOffset);
        result.emplace_back(clickRecognizer_);
    }
}

void RenderSwiper::HandleTouchDown(const TouchEventInfo& info)
{
    if (info.GetTouches().empty()) {
        return;
    }
    const auto& locationInfo = info.GetTouches().front();
    Point touchPoint = Point(locationInfo.GetGlobalLocation().GetX(), locationInfo.GetGlobalLocation().GetY());
    if (fingerId_ >= 0 && locationInfo.GetFingerId() != fingerId_) {
        return;
    }

    fingerId_ = locationInfo.GetFingerId();
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (indicatorRect_.IsInRegion(touchPoint)) {
        startTimeStamp_ = GetTickCount();
        if (isIndicatorAnimationStart_) {
            touchContentType_ = TouchContentType::TOUCH_NONE;
            return;
        }
        touchContentType_ = TouchContentType::TOUCH_INDICATOR;
    } else {
        touchContentType_ = TouchContentType::TOUCH_CONTENT;
        if (hasDragAction_ && swiper_->GetSlideContinue()) {
            controller_->Finish();
            return;
        }
        // when is in item moving animation, touch event will break animation and stop in current position
        StopSwipeAnimation();
        StopIndicatorAnimation();
        if (autoPlay_ && scheduler_) {
            scheduler_->Stop();
        }
    }
    StopIndicatorSpringAnimation();
}

// touch up event before than click event
void RenderSwiper::HandleTouchUp(const TouchEventInfo& info)
{
    // for indicator
    startTimeStamp_ = 0;
    int32_t fingerId = -1;
    if (!info.GetTouches().empty()) {
        fingerId = info.GetTouches().front().GetFingerId();
    } else if (!info.GetChangedTouches().empty()) {
        fingerId = info.GetChangedTouches().front().GetFingerId();
    }
    if ((fingerId_ >= 0 && fingerId != fingerId_) || fingerId_ == -1) {
        return;
    }
    
    fingerId_ = -1;
    // indicator zone
    if (touchContentType_ == TouchContentType::TOUCH_NONE) {
        LOGD(" touch content type is none");
        return;
    } else if (touchContentType_ == TouchContentType::TOUCH_INDICATOR) {
        if (swiperIndicatorData_.isPressed) {
            if (isDragStart_) {
                // reset flag of isPressed by function of HandleDragEnd.
                isDragStart_ = false;
                return;
            }
            if (IsZoomOutAnimationStopped()) {
                // reset flag of isPressed after zoom out animation
                StartZoomOutAnimation();
            }
        }
        return;
    }

    // content zone
    if (swiper_->GetSlideContinue()) {
        return;
    }
    if (hasDragAction_) {
        hasDragAction_ = false;
        return;
    }
    if (isIndicatorAnimationStart_) {
        return;
    }
    // restore the item position that slides to half stopped by a touch event during autoplay
    scrollOffset_ = fmod(scrollOffset_, nextItemOffset_);
    if (!NearZero(scrollOffset_)) {
        MoveItems(0.0);
    } else {
        // restore autoplay which break by a touch event
        RestoreAutoPlay();
    }
}

void RenderSwiper::HandleTouchMove(const TouchEventInfo& info)
{
    // for indicator
    if (!indicator_ || indicator_->GetIndicatorDisabled() || swiperIndicatorData_.isDigital) {
        return;
    }

    if (info.GetTouches().empty()) {
        return;
    }

    if (touchContentType_ != TouchContentType::TOUCH_INDICATOR) {
        return;
    }

    const auto& locationInfo = info.GetTouches().front();
    Point touchPoint = Point(locationInfo.GetGlobalLocation().GetX(), locationInfo.GetGlobalLocation().GetY());
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (indicatorRect_.IsInRegion(touchPoint)) {
        if (autoPlay_ && scheduler_ && scheduler_->IsActive()) {
            // forbid indicator operation on auto play period.
            return;
        }
        if (!swiperIndicatorData_.isHovered) {
            int64_t endStartTime = GetTickCount();
            if (startTimeStamp_ == 0) {
                // move into indicator rage
                startTimeStamp_ = endStartTime;
                return;
            }
            double delayTime = static_cast<double>(endStartTime - startTimeStamp_);
            if (!swiperIndicatorData_.isPressed && delayTime >= DELAY_TIME_DEFAULT) {
                swiperIndicatorData_.isPressed = true;
                StartZoomInAnimation();
            }
        }
    }
}

void RenderSwiper::HandleClick(const ClickInfo& clickInfo)
{
    if (clickEvent_) {
        clickEvent_(std::make_shared<ClickInfo>(clickInfo));
    }
    std::string accessibilityEventType = "click";
    SendAccessibilityEvent(accessibilityEventType);
    // for indicator
    if (!indicator_ || swiperIndicatorData_.isDigital) {
        return;
    }

    if (swiperIndicatorData_.isHovered || swiperIndicatorData_.isPressed) {
        if (currentHoverIndex_ != INDICATOR_INVALID_HOVER_INDEX) {
            auto toIndex = needReverse_ ? (itemCount_ - currentHoverIndex_ - 1) : currentHoverIndex_;
            StartIndicatorAnimation(currentIndex_, toIndex);
            return;
        }
        // refuse click event
        LOGD("drop click event on press and hover status, otherwise exist hover index.");
        return;
    }

    // handle operation not support when indicator disabled.
    if (indicator_->GetIndicatorDisabled()) {
        return;
    }
    Point clickPoint = Point(clickInfo.GetGlobalLocation().GetX(), clickInfo.GetGlobalLocation().GetY());
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (!indicatorRect_.IsInRegion(clickPoint)) {
        return;
    }
    if (autoPlay_ && scheduler_ && scheduler_->IsActive()) {
        // forbid indicator operation on auto play period.
        return;
    }
    if (fingerId_ >= 0 && clickInfo.GetFingerId() != fingerId_) {
        return;
    }

    Offset offset;
    Size size;
    Rect itemRect;
    if (axis_ == Axis::HORIZONTAL) {
        int currentIndex = needReverse_ ? (itemCount_ - currentIndex_ - 1) : currentIndex_;
        offset = indicatorRect_.GetOffset() + Offset(
            swiperIndicatorData_.indicatorItemData[currentIndex].position.GetX(), 0);
        size = Size(swiperIndicatorData_.indicatorItemData[currentIndex].width,
            swiperIndicatorData_.indicatorPaintData.height);
        itemRect = Rect(offset, size);
        if (clickPoint.GetX() < itemRect.GetOffset().GetX()) {
            if (needReverse_) {
                IndicatorSwipeNext();
            } else {
                IndicatorSwipePrev();
            }
        } else if (clickPoint.GetX() > itemRect.Right()) {
            if (needReverse_) {
                IndicatorSwipePrev();
            } else {
                IndicatorSwipeNext();
            }
        }
    } else {
        offset = indicatorRect_.GetOffset() + Offset(0,
            swiperIndicatorData_.indicatorItemData[currentIndex_].position.GetY());
        size = Size(swiperIndicatorData_.indicatorPaintData.width,
            swiperIndicatorData_.indicatorItemData[currentIndex_].height);
        itemRect = Rect(offset, size);
        if (clickPoint.GetY() < itemRect.GetOffset().GetY()) {
            IndicatorSwipePrev();
        } else if (clickPoint.GetY() > itemRect.Bottom()) {
            IndicatorSwipeNext();
        }
    }
}

void RenderSwiper::HandleRemoteMessage(const ClickInfo& clickInfo)
{
    if (remoteMessageEvent_) {
        remoteMessageEvent_(std::make_shared<ClickInfo>(clickInfo));
    }
}

void RenderSwiper::HandleDragStart(const DragStartInfo& info)
{
    Point dragStartPoint = Point(info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY());
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (indicatorRect_.IsInRegion(dragStartPoint)) {
        return;
    }
    if (fingerId_ >= 0 && info.GetFingerId() != fingerId_) {
        return;
    }
    // for swiper item
    hasDragAction_ = true;
    scrollOffset_ = fmod(scrollOffset_, nextItemOffset_);
    if (onFocus_) {
        auto context = GetContext().Upgrade();
        if (context) {
            context->CancelFocusAnimation();
        }
    }
    dragOffset_ = 0;
}

double RenderSwiper::CalculateFriction(double gamma)
{
    constexpr double SCROLL_RATIO = 0.72;
    if (GreatOrEqual(gamma, 1.0)) {
        gamma = 1.0;
    }
    return SCROLL_RATIO * std::pow(1.0 - gamma, SQUARE);
}

void RenderSwiper::HandleDragUpdate(const DragUpdateInfo& info)
{
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (swiperIndicatorData_.isPressed) {
        if (swiperIndicatorData_.isDigital) {
            return;
        }
        if (autoPlay_ && scheduler_ && scheduler_->IsActive()) {
            // forbid indicator operation on auto play period.
            return;
        }
        DragIndicator(std::clamp(info.GetMainDelta(), -MAX_VIEW_PORT_WIDTH, MAX_VIEW_PORT_WIDTH));
        return;
    }

    if (touchContentType_ == TouchContentType::TOUCH_CONTENT || rotationStatus_ == RotationStatus::ROTATION_UPDATE) {
        EdgeEffect edgeEffect = swiper_->GetEdgeEffect();
        if (edgeEffect == EdgeEffect::SPRING && (!loop_) && currentIndex_ == targetIndex_
            && (currentIndex_ == 0 || currentIndex_ == itemCount_ - 1)) {
            dragOffset_ += info.GetMainDelta();
            double friction = CalculateFriction(std::abs(dragOffset_)
                / (axis_ == Axis::VERTICAL ? swiperHeight_ : swiperWidth_));
            UpdateScrollPosition(friction * info.GetMainDelta());
        } else {
            UpdateScrollPosition(info.GetMainDelta());
        }
    }
}

bool RenderSwiper::SpringItems(const DragEndInfo& info)
{
    EdgeEffect edgeEffect = swiper_->GetEdgeEffect();
    if (edgeEffect == EdgeEffect::SPRING) {
        int32_t toIndex = 0;
        toIndex = GreatNotEqual(scrollOffset_, 0.0) ? GetPrevIndex() : GetNextIndex();

        if (currentIndex_ == toIndex) {
            double minLeading = 0.0;
            double maxTrainling = 0.0;
            if (axis_ == Axis::VERTICAL) {
                minLeading = needReverse_ ? 0.0 : (GreatNotEqual(scrollOffset_, 0.0) ?
                    (swiperHeight_ - std::abs(prevItemOffset_)) : -(swiperHeight_ - std::abs(prevItemOffset_)));
                maxTrainling = needReverse_ ? (GreatNotEqual(scrollOffset_, 0.0) ?
                    -(swiperHeight_ - std::abs(prevItemOffset_)) : (swiperHeight_ - std::abs(prevItemOffset_))) : 0.0;
            } else {
                minLeading = needReverse_ ? 0.0 : (GreatNotEqual(scrollOffset_, 0.0) ?
                    (swiperWidth_ - std::abs(prevItemOffset_)) : -(swiperWidth_ - std::abs(prevItemOffset_)));
                maxTrainling = needReverse_ ? (GreatNotEqual(scrollOffset_, 0.0) ?
                    -(swiperWidth_ - std::abs(prevItemOffset_)) : (swiperWidth_ - std::abs(prevItemOffset_))) : 0.0;
            }
            double friction = CalculateFriction(std::abs(dragOffset_) /
                 (axis_ == Axis::VERTICAL ? swiperHeight_ : swiperWidth_));
            StartSpringMotion(scrollOffset_, info.GetMainVelocity() * friction,
                GreatNotEqual(scrollOffset_, 0.0) ?
                ExtentPair(std::abs(minLeading), maxTrainling) : ExtentPair(0.0, 0.0),
                ExtentPair(std::abs(minLeading), maxTrainling));
            return true;
        }
    }

    return false;
}

void RenderSwiper::HandleDragEnd(const DragEndInfo& info)
{
    if (swiperIndicatorData_.isPressed) {
        DragIndicatorEnd();
        return;
    }

    if (touchContentType_ != TouchContentType::TOUCH_CONTENT && rotationStatus_ != RotationStatus::ROTATION_UPDATE) {
        return;
    }

    if (fingerId_ >= 0 && info.GetFingerId() != fingerId_) {
        return;
    }

    // for swiper item
    scrollOffset_ = fmod(scrollOffset_, nextItemOffset_);
    if (NearZero(scrollOffset_)) {
        // restore autoplay which break by a touch event
        RestoreAutoPlay();
        return;
    }
    if (isPaintedFade_) {
        auto translate = AceType::MakeRefPtr<CurveAnimation<double>>(dragDelta_, 0, Curves::LINEAR);
        auto weak = AceType::WeakClaim(this);
        translate->AddListener(Animation<double>::ValueCallback([weak](double value) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->dragDelta_ = value;
                swiper->MarkNeedRender();
            }
        }));

        fadeController_->ClearStopListeners();
        // trigger the event after the animation ends.
        fadeController_->AddStopListener([weak]() {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->isPaintedFade_ = false;
            }
        });
        fadeController_->SetDuration(FADE_DURATION);
        fadeController_->ClearInterpolators();
        fadeController_->AddInterpolator(translate);
        fadeController_->Play();
        MarkNeedRender();
        scrollOffset_ = 0.0;
        return;
    }

    if (SpringItems(info)) {
        return;
    }

    isIndicatorAnimationStart_ = false;
    MoveItems(info.GetMainVelocity());
}

void RenderSwiper::StartSpringMotion(double mainPosition, double mainVelocity,
    const ExtentPair& extent, const ExtentPair& initExtent)
{
    constexpr double SPRING_SCROLL_MASS = 0.5;
    constexpr double SPRING_SCROLL_STIFFNESS = 100.0;
    constexpr double SPRING_SCROLL_DAMPING = 15.55635;
    const RefPtr<SpringProperty> DEFAULT_OVER_SPRING_PROPERTY =
    AceType::MakeRefPtr<SpringProperty>(SPRING_SCROLL_MASS, SPRING_SCROLL_STIFFNESS, SPRING_SCROLL_DAMPING);

    if (isAnimationAlreadyAdded_) {
        controller_->RemoveInterpolator(translate_);
        isAnimationAlreadyAdded_ = false;
    }
    scrollMotion_ = AceType::MakeRefPtr<ScrollMotion>(mainPosition, mainVelocity, extent,
        initExtent, DEFAULT_OVER_SPRING_PROPERTY);
    scrollMotion_->AddListener([weakScroll = AceType::WeakClaim(this)](double position) {
        auto swiper = weakScroll.Upgrade();
        if (swiper) {
            swiper->UpdateChildPosition(position, swiper->currentIndex_);
        }
    });
    springController_->ClearStopListeners();
    springController_->PlayMotion(scrollMotion_);
    springController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->RestoreAutoPlay();
            swiper->ResetCachedChildren();
            swiper->UpdateOneItemOpacity(MAX_OPACITY, swiper->currentIndex_);
            swiper->UpdateOneItemOpacity(MAX_OPACITY, swiper->currentIndex_);
            swiper->ExecuteMoveCallback(swiper->currentIndex_);
            swiper->MarkNeedLayout();
        }
    });
}

void RenderSwiper::MoveItems(double dragVelocity)
{
    if (isIndicatorAnimationStart_) {
        LOGE("item and indicator animation is processing.");
        return;
    }

    if (isAnimationAlreadyAdded_) {
        controller_->RemoveInterpolator(translate_);
        isAnimationAlreadyAdded_ = false;
    }

    isIndicatorAnimationStart_ = true;
    double start = scrollOffset_;
    double end;

    bool needRestore = false;
    int32_t fromIndex = currentIndex_;
    int32_t toIndex = 0;
    // Adjust offset more than MIN_SCROLL_OFFSET at least
    double minOffset = 0.0;
    if (axis_ == Axis::VERTICAL) {
        minOffset = MIN_SCROLL_OFFSET * (swiperHeight_ - prevMargin_ - nextMargin_);
    } else {
        minOffset = MIN_SCROLL_OFFSET * (swiperWidth_ - prevMargin_ - nextMargin_);
    }
    if (std::abs(dragVelocity) > NormalizeToPx(MIN_TURN_PAGE_VELOCITY) &&
        std::abs(scrollOffset_) > NormalizeToPx(MIN_DRAG_DISTANCE)) {
        if (dragVelocity > 0.0) {
            toIndex = GetPrevIndex();
            end = currentIndex_ == toIndex ? 0.0 : scrollOffset_ < 0.0 ? 0.0 : nextItemOffset_;
            if (scrollOffset_ < 0.0) {
                fromIndex = GetNextIndex();
                start += nextItemOffset_;
                toIndex = currentIndex_;
                end += nextItemOffset_;
            }
        } else {
            toIndex = GetNextIndex();
            end = currentIndex_ == toIndex ? 0.0 : scrollOffset_ > 0.0 ? 0.0 : prevItemOffset_;
            if (scrollOffset_ > 0.0) {
                fromIndex = GetPrevIndex();
                start += prevItemOffset_;
                toIndex = currentIndex_;
                end += prevItemOffset_;
            }
        }
        targetIndex_ = toIndex;
        nextIndex_ = targetIndex_;
    } else if (std::abs(scrollOffset_) > minOffset) {
        if (scrollOffset_ > 0.0) {
            toIndex = GetPrevIndex();
            end = nextItemOffset_;
        } else {
            toIndex = GetNextIndex();
            end = prevItemOffset_;
        }
        targetIndex_ = toIndex;
        nextIndex_ = targetIndex_;
    } else {
        toIndex = scrollOffset_ > 0.0 ? GetPrevIndex() : GetNextIndex();
        end = 0.0;
        needRestore = true;
    }
    LOGD("translate animation, start=%{public}f, end=%{public}f", start, end);
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(start, end, curve_);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak, fromIndex, toIndex, start, end](double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            if (value != start && value != end && start != end) {
                double moveRate = Curves::EASE_OUT->MoveInternal((value - start) / (end - start));
                value = start + (end - start) * moveRate;
            }
            swiper->UpdateChildPosition(value, fromIndex);
            swiper->MoveIndicator(toIndex, value, true);
        }
    }));

    controller_->ClearStopListeners();
    // trigger the event after the animation ends.
    controller_->AddStopListener([weak, fromIndex, toIndex, needRestore]() {
        LOGI("slide animation stop");
        // moving animation end, one drag and item move is complete
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->isIndicatorAnimationStart_ = false;
            if (!needRestore) {
                swiper->LoadLazyItems((fromIndex + 1) % swiper->itemCount_ == toIndex);
                swiper->outItemIndex_ = fromIndex;
                swiper->currentIndex_ = toIndex;
            }
            swiper->RestoreAutoPlay();
            swiper->FireItemChangedEvent(!needRestore);
            swiper->ResetCachedChildren();
            swiper->UpdateOneItemOpacity(MAX_OPACITY, fromIndex);
            swiper->UpdateOneItemOpacity(MAX_OPACITY, toIndex);
            swiper->ExecuteMoveCallback(swiper->currentIndex_);
            swiper->MarkNeedLayout();
        }
    });

    controller_->SetDuration(duration_);
    controller_->AddInterpolator(translate_);
    controller_->Play();
    isAnimationAlreadyAdded_ = true;
    MarkNeedRender();
}

void RenderSwiper::FireItemChangedEvent(bool changed) const
{
    if (animationFinishEvent_) {
        LOGI("call animationFinishEvent_");
        animationFinishEvent_();
    }

    if (changeEvent_ && changed) {
        LOGI("call changeEvent_");
        changeEvent_(std::make_shared<SwiperChangeEvent>(currentIndex_));
    }

    for (const auto& [first, second] : changeEndListeners_) {
        if (second) {
            second(currentIndex_);
        }
    }
}

void RenderSwiper::SwipeTo(int32_t index, bool reverse)
{
    if (index >= itemCount_) {
        index = itemCount_ - 1;
    } else if (index < 0) {
        index = 0;
    }
    if (isIndicatorAnimationStart_) {
        RedoSwipeToAnimation(index, reverse);
    } else {
        StopIndicatorSpringAnimation();
        DoSwipeToAnimation(currentIndex_, index, reverse);
    }
}

void RenderSwiper::InitSwipeToAnimation(double start, double end)
{
    auto animationCurve = swiper_->GetAnimationCurve();
    auto curStartTranslateKeyframe = AceType::MakeRefPtr<Keyframe<double>>(CUR_START_TRANSLATE_TIME, start);
    auto curEndTranslateKeyframe = AceType::MakeRefPtr<Keyframe<double>>(CUR_END_TRANSLATE_TIME, end);
    curEndTranslateKeyframe->SetCurve(
        animationCurve == AnimationCurve::FRICTION ? Curves::FRICTION : Curves::FAST_OUT_SLOW_IN);
    curTranslateAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    curTranslateAnimation_->AddKeyframe(curStartTranslateKeyframe);
    curTranslateAnimation_->AddKeyframe(curEndTranslateKeyframe);

    auto targetStartTranslateKeyframe = AceType::MakeRefPtr<Keyframe<double>>(TARGET_START_TRANSLATE_TIME, -end);
    auto targetEndTranslateKeyframe = AceType::MakeRefPtr<Keyframe<double>>(TARGET_END_TRANSLATE_TIME, start);
    targetEndTranslateKeyframe->SetCurve(
        animationCurve == AnimationCurve::FRICTION ? Curves::FRICTION : Curves::FAST_OUT_SLOW_IN);
    targetTranslateAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    targetTranslateAnimation_->AddKeyframe(targetStartTranslateKeyframe);
    targetTranslateAnimation_->AddKeyframe(targetEndTranslateKeyframe);

    if (animationOpacity_) {
        auto curStartOpacityKeyframe =
            AceType::MakeRefPtr<Keyframe<uint8_t>>(CUR_START_OPACITY_TIME, CUR_START_OPACITY_VALUE);
        auto curEndOpacityKeyframe =
            AceType::MakeRefPtr<Keyframe<uint8_t>>(CUR_END_OPACITY_TIME, CUR_END_OPACITY_VALUE);
        curEndOpacityKeyframe->SetCurve(
            animationCurve == AnimationCurve::FRICTION ? Curves::FRICTION : Curves::FAST_OUT_SLOW_IN);
        curOpacityAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<uint8_t>>();
        curOpacityAnimation_->AddKeyframe(curStartOpacityKeyframe);
        curOpacityAnimation_->AddKeyframe(curEndOpacityKeyframe);

        auto targetStartOpacityKeyframe =
            AceType::MakeRefPtr<Keyframe<uint8_t>>(TARGET_START_OPACITY_TIME, TARGET_START_OPACITY_VALUE);
        auto targetEndOpacityKeyframe =
            AceType::MakeRefPtr<Keyframe<uint8_t>>(TARGET_END_OPACITY_TIME, TARGET_END_OPACITY_VALUE);
        targetEndOpacityKeyframe->SetCurve(
            animationCurve == AnimationCurve::FRICTION ? Curves::FRICTION : Curves::FAST_OUT_SLOW_IN);
        targetOpacityAnimation_ = AceType::MakeRefPtr<KeyframeAnimation<uint8_t>>();
        targetOpacityAnimation_->AddKeyframe(targetStartOpacityKeyframe);
        targetOpacityAnimation_->AddKeyframe(targetEndOpacityKeyframe);
    }
}

void RenderSwiper::AddSwipeToTranslateListener(int32_t fromIndex, int32_t toIndex)
{
    auto weak = AceType::WeakClaim(this);
    targetIndex_ = toIndex;
    nextIndex_ = toIndex;
    curTranslateAnimation_->AddListener([weak, fromIndex, toIndex](const double& value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateItemPosition(value, fromIndex, toIndex);
        }
    });

    targetTranslateAnimation_->AddListener([weak, fromIndex, toIndex](const double& value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateItemPosition(value, toIndex, fromIndex);
        }
    });
}

void RenderSwiper::AddSwipeToOpacityListener(int32_t fromIndex, int32_t toIndex)
{
    if (!animationOpacity_) {
        return;
    }
    auto weak = AceType::WeakClaim(this);
    curOpacityAnimation_->AddListener([weak, fromIndex, toIndex](const uint8_t& opacity) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateItemOpacity(opacity, fromIndex, toIndex);
        }
    });

    targetOpacityAnimation_->AddListener([weak, fromIndex, toIndex](const uint8_t& opacity) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateItemOpacity(opacity, toIndex, fromIndex);
        }
    });
}

void RenderSwiper::AddSwipeToIndicatorListener(int32_t fromIndex, int32_t toIndex)
{
    indicatorAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(
        CUR_START_TRANSLATE_TIME, CUR_END_TRANSLATE_TIME, curve_);
    indicatorAnimation_->AddListener(
        [weak = AceType::WeakClaim(this), fromIndex, toIndex](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateIndicatorOffset(fromIndex, toIndex, value);
        }
    });
    animationDirect_ = (fromIndex - toIndex <= 0) ? INDICATOR_DIRECT_FORWARD : INDICATOR_DIRECT_BACKWARD;
    if (needReverse_) {
        animationDirect_ *= INDICATOR_DIRECT_BACKWARD;
    }
    targetIndex_ = toIndex;
    nextIndex_ = toIndex;
}

double RenderSwiper::CalculateEndOffset(int32_t fromIndex, int32_t toIndex, bool reverse)
{
    double end = 0.0;
    auto context = GetContext().Upgrade();
    if (fromIndex > toIndex) {
        // default move to back position, if need reverse direction move to front position.
        end = reverse ? prevItemOffset_ / TRANSLATE_RATIO : nextItemOffset_ / TRANSLATE_RATIO;
    } else {
        // default move to front position, if need reverse direction move to back position.
        end = reverse ? nextItemOffset_ / TRANSLATE_RATIO : prevItemOffset_ / TRANSLATE_RATIO;
    }
    if (context && context->IsJsCard()) {
        if (loop_) {
            end = reverse ? nextItemOffset_ : prevItemOffset_;
        } else {
            if (fromIndex > toIndex) {
                end = reverse ? prevItemOffset_ : nextItemOffset_;
            } else {
                end = reverse ? nextItemOffset_ : prevItemOffset_;
            }
        }
    }
    return end;
}

void RenderSwiper::DoSwipeToAnimation(int32_t fromIndex, int32_t toIndex, bool reverse)
{
    if (!swipeToController_ || isIndicatorAnimationStart_ || fromIndex == toIndex) {
        return;
    }
    isIndicatorAnimationStart_ = true;
    double start = 0.0;
    moveStatus_ = true;
    if (onFocus_) {
        auto context = GetContext().Upgrade();
        if (context) {
            context->CancelFocusAnimation();
        }
    }
    double end = CalculateEndOffset(fromIndex, toIndex, reverse);
    swipeToController_->ClearStopListeners();
    if (isSwipeToAnimationAdded_) {
        swipeToController_->ClearInterpolators();
        isSwipeToAnimationAdded_ = false;
    }
    if (!swipeToController_->IsStopped()) {
        swipeToController_->Stop();
    }

    InitSwipeToAnimation(start, end);
    AddSwipeToTranslateListener(fromIndex, toIndex);
    AddSwipeToOpacityListener(fromIndex, toIndex);
    AddSwipeToIndicatorListener(fromIndex, toIndex);

    // trigger the event after the animation ends.
    auto weak = AceType::WeakClaim(this);
    swipeToController_->AddStopListener([weak, fromIndex, toIndex]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->LoadLazyItems((fromIndex + 1) % swiper->itemCount_ == toIndex);
            swiper->isIndicatorAnimationStart_ = false;
            swiper->outItemIndex_ = fromIndex;
            swiper->currentIndex_ = toIndex;
            swiper->moveStatus_ = false;
            swiper->UpdateIndicatorSpringStatus(SpringStatus::FOCUS_SWITCH);
            swiper->UpdateOneItemOpacity(MAX_OPACITY, fromIndex);
            swiper->UpdateOneItemOpacity(MAX_OPACITY, toIndex);
            swiper->RestoreAutoPlay();
            swiper->FireItemChangedEvent(true);
            swiper->ResetCachedChildren();
            swiper->MarkNeedLayout();
        }
    });
    swipeToController_->SetDuration(duration_);
    swipeToController_->AddInterpolator(curTranslateAnimation_);
    swipeToController_->AddInterpolator(targetTranslateAnimation_);
    swipeToController_->AddInterpolator(curOpacityAnimation_);
    swipeToController_->AddInterpolator(targetOpacityAnimation_);
    swipeToController_->AddInterpolator(indicatorAnimation_);
    swipeToController_->SetFillMode(FillMode::FORWARDS);
    swipeToController_->Play();
    isSwipeToAnimationAdded_ = true;
}

void RenderSwiper::RedoSwipeToAnimation(int32_t toIndex, bool reverse)
{
    if (toIndex == targetIndex_) {
        // continue move animation
        return;
    }
    // stop animation before update item position, otherwise the
    // animation callback will change the item position
    FinishAllSwipeAnimation();
    DoSwipeToAnimation(currentIndex_, toIndex, false);
}

void RenderSwiper::StopSwipeToAnimation()
{
    if (swipeToController_ && !swipeToController_->IsStopped()) {
        swipeToController_->ClearStopListeners();
        swipeToController_->Stop();
        UpdateOneItemOpacity(MAX_OPACITY, currentIndex_);
        UpdateOneItemOpacity(MAX_OPACITY, targetIndex_);
        isIndicatorAnimationStart_ = false;
    }
}

void RenderSwiper::UpdateItemOpacity(uint8_t opacity, int32_t index, int32_t otherIndex)
{
    UpdateOneItemOpacity(opacity, index);

    int32_t preIndex = GetPrevIndex(index);
    if (preIndex != index && preIndex != otherIndex) {
        UpdateOneItemOpacity(opacity, preIndex);
    }
    int32_t nextIndex = GetNextIndex(index);
    if (nextIndex != index && nextIndex != otherIndex) {
        UpdateOneItemOpacity(opacity, nextIndex);
    }
}

void RenderSwiper::UpdateOneItemOpacity(uint8_t opacity, int32_t index)
{
    if (!animationOpacity_) {
        return;
    }
    auto iter = items_.find(index);
    if (iter == items_.end()) {
        return;
    }
    auto display = AceType::DynamicCast<RenderDisplay>(iter->second);
    if (!display) {
        return;
    }
    display->UpdateOpacity(opacity);
}

void RenderSwiper::UpdateItemPosition(double offset, int32_t index, int32_t otherIndex)
{
    auto iter = items_.find(index);
    if (iter != items_.end()) {
        iter->second->SetPosition(GetMainAxisOffset(offset));
    }
    int32_t prevIndex = GetPrevIndex(index);
    if (prevIndex != index && prevIndex != otherIndex) {
        auto item = items_.find(prevIndex);
        if (item != items_.end()) {
            item->second->SetPosition(GetMainAxisOffset(offset + (needReverse_ ? nextItemOffset_ : prevItemOffset_)));
        }
    }
    int32_t nextIndex = GetNextIndex(index);
    if (nextIndex != index && nextIndex != otherIndex) {
        auto item = items_.find(nextIndex);
        if (item != items_.end()) {
            item->second->SetPosition(GetMainAxisOffset(offset + (needReverse_ ? prevItemOffset_ : nextItemOffset_)));
        }
    }
    MarkNeedRender();
}

int32_t RenderSwiper::GetPrevIndex() const
{
    return GetPrevIndex(currentIndex_);
}

int32_t RenderSwiper::GetPrevIndexOnAnimation() const
{
    return GetPrevIndex(targetIndex_);
}

int32_t RenderSwiper::GetPrevIndex(int32_t index) const
{
    return GetIndex(index, !needReverse_);
}

int32_t RenderSwiper::GetNextIndex() const
{
    return GetNextIndex(currentIndex_);
}

int32_t RenderSwiper::GetNextIndexOnAnimation() const
{
    return GetNextIndex(targetIndex_);
}

int32_t RenderSwiper::GetNextIndex(int32_t index) const
{
    return GetIndex(index, needReverse_);
}

int32_t RenderSwiper::GetIndex(int32_t index, bool leftOrTop) const
{
    if (leftOrTop) {
        if (--index < 0) {
            index = loop_ ? itemCount_ - 1 : 0;
        }
    } else {
        if (++index >= itemCount_) {
            index = loop_ ? 0 : itemCount_ - 1;
        }
    }
    return index;
}

void RenderSwiper::ShowPrevious()
{
    if (isIndicatorAnimationStart_) {
        int32_t index = GetPrevIndexOnAnimation();
        RedoSwipeToAnimation(index, false);
    } else {
        int32_t index = GetPrevIndex();
        StopIndicatorSpringAnimation();
        DoSwipeToAnimation(currentIndex_, index, false);
    }
}

void RenderSwiper::ShowNext()
{
    if (isIndicatorAnimationStart_) {
        int32_t index = GetNextIndexOnAnimation();
        RedoSwipeToAnimation(index, false);
    } else {
        int32_t index = GetNextIndex();
        StopIndicatorSpringAnimation();
        DoSwipeToAnimation(currentIndex_, index, false);
    }
}

void RenderSwiper::OnFocus()
{
    if (autoPlay_) {
        LOGD("stop autoplay cause by on focus");
        if (scheduler_) {
            scheduler_->Stop();
        }
        StopSwipeAnimation();
        StopSwipeToAnimation();
        StopIndicatorAnimation();
        StopIndicatorSpringAnimation();
        ResetIndicatorPosition();
    }
}

void RenderSwiper::OnBlur()
{
    RestoreAutoPlay();
}

void RenderSwiper::RegisterChangeEndListener(int32_t listenerId, const SwiperChangeEndListener& listener)
{
    if (listener) {
        changeEndListeners_[listenerId] = listener;
    }
}

void RenderSwiper::UnRegisterChangeEndListener(int32_t listenerId)
{
    changeEndListeners_.erase(listenerId);
}

void RenderSwiper::UpdateScrollPosition(double dragDelta)
{
    auto limitDelta = std::clamp(dragDelta, -MAX_VIEW_PORT_WIDTH, MAX_VIEW_PORT_WIDTH);
    double newDragOffset = scrollOffset_ + limitDelta;
    int32_t toIndex = newDragOffset > 0 ? GetPrevIndex() : GetNextIndex();
    if (toIndex < 0 || toIndex >= itemCount_) {
        LOGD("toIndex is error %{public}d", toIndex);
        return;
    }
    if (currentIndex_ == toIndex) {
        targetIndex_ = toIndex;
        nextIndex_ = toIndex;
        SetSwiperEffect(newDragOffset);
        return;
    }

    if (std::fabs(newDragOffset) >= std::fabs(nextItemOffset_)) {
        scrollOffset_ = (newDragOffset >= nextItemOffset_) ? newDragOffset - nextItemOffset_
                                                           : newDragOffset - prevItemOffset_;
        LoadLazyItems((currentIndex_ + 1) % itemCount_ == toIndex);
        outItemIndex_ = currentIndex_;
        currentIndex_ = toIndex;
        FireItemChangedEvent(true);
        ResetCachedChildren();
        UpdateOneItemOpacity(MAX_OPACITY, outItemIndex_);
        UpdateOneItemOpacity(MAX_OPACITY, currentIndex_);
        ExecuteMoveCallback(currentIndex_);
        ResetIndicatorPosition();
        MarkNeedLayout();
        // drag length is greater than swiper's width, don't need to move position
        LOGD("scroll to next page index[%{public}d] from index[%{public}d], scroll offset:%{public}lf",
            currentIndex_, outItemIndex_, scrollOffset_);
        return;
    }

    bool dragReverse = (newDragOffset * scrollOffset_) < 0.0;
    if (dragReverse) {
        int32_t lastToIndex = 0;
        double toItemPosValue = 0.0;
        lastToIndex = scrollOffset_ > 0 ? GetPrevIndex() : GetNextIndex();
        toItemPosValue = scrollOffset_ > 0 ? prevItemOffset_ : nextItemOffset_;

        auto iter = items_.find(lastToIndex);
        if (iter != items_.end()) {
            iter->second->SetPosition(GetMainAxisOffset(toItemPosValue));
        }
    }

    UpdateChildPosition(newDragOffset, currentIndex_);
    MoveIndicator(toIndex, newDragOffset);
}

void RenderSwiper::SetSwiperEffect(double dragOffset)
{
    EdgeEffect edgeEffect = swiper_->GetEdgeEffect();
    bool isFade = edgeEffect == EdgeEffect::FADE;
    bool isSpring = edgeEffect == EdgeEffect::SPRING;
    if (!isFade && !isSpring && !loop_) {
        return;
    }

    if ((isFade && currentIndex_ == 0 && GreatOrEqual(dragOffset, 0.0)) ||
        (isFade && (currentIndex_ == itemCount_ - 1) && LessOrEqual(dragOffset, 0.0))) {
        isPaintedFade_ = true;
        scrollOffset_ = dragOffset;
        dragDelta_ = dragOffset;
        MarkNeedRender();
    } else if (isSpring) {
        isPaintedFade_ = false;
        UpdateChildPosition(dragOffset, currentIndex_);
    }
}

void RenderSwiper::UpdateChildPosition(double offset, int32_t fromIndex, bool inLayout)
{
    if (std::abs(currentIndex_ - fromIndex) == 1) {
        scrollOffset_ = offset + (currentIndex_ - fromIndex) * nextItemOffset_;
    } else { // for loop reversal
        if (fromIndex > currentIndex_) {
            scrollOffset_ = offset + nextItemOffset_;
        } else if (fromIndex < currentIndex_) {
            scrollOffset_ = offset - nextItemOffset_;
        } else {
            scrollOffset_ = offset;
        }
    }
    // move current item
    auto item = items_.find(fromIndex);
    if (item != items_.end() && !(RefuseUpdatePosition(fromIndex) && inLayout)) {
        item->second->SetPosition(GetMainAxisOffset(offset));
    }

    // calculate the num of prev item
    int32_t prevItemCount = 0;
    if (!loop_) {
        prevItemCount = fromIndex;
    } else {
        prevItemCount = (offset + prevMargin_ + std::fabs(prevItemOffset_) - 1) / std::fabs(prevItemOffset_);
    }
    if (prevItemCount >= itemCount_ - 1) {
        prevItemCount = itemCount_ - 1;
    }

    // move prev item
    int32_t prevIndex = fromIndex;
    for (int32_t i = 0; i < prevItemCount; i++) {
        prevIndex = GetPrevIndex(prevIndex);
        if (RefuseUpdatePosition(prevIndex) && inLayout) {
            continue;
        }
        auto item = items_.find(prevIndex);
        if (item != items_.end()) {
            item->second->SetPosition(
                GetMainAxisOffset(offset + (needReverse_ ? (i + 1) * nextItemOffset_ : (i + 1) * prevItemOffset_)));
        }
    }

    // calculate the num of next item
    int32_t maxNextItemCount = itemCount_ - 1 - prevItemCount;
    int32_t nextItemCount = 0;
    if (inLayout) {
        nextItemCount = maxNextItemCount;
    } else {
        double maxLength = (axis_ == Axis::HORIZONTAL ? GetLayoutSize().Width() : GetLayoutSize().Height());

        nextItemCount =
            (maxLength - offset - prevMargin_ + std::fabs(prevItemOffset_) - 1) / std::fabs(prevItemOffset_);
        if (nextItemCount > maxNextItemCount) {
            nextItemCount = maxNextItemCount;
        }
    }

    // move next item
    int32_t nextIndex = fromIndex;
    for (int32_t i = 0; i < nextItemCount; i++) {
        nextIndex = GetNextIndex(nextIndex);
        if (RefuseUpdatePosition(nextIndex) && inLayout) {
            continue;
        }
        auto item = items_.find(nextIndex);
        if (item != items_.end()) {
            item->second->SetPosition(
                GetMainAxisOffset(offset + (needReverse_ ? (i + 1) * prevItemOffset_ : (i + 1) * nextItemOffset_)));
        }
    }

    MarkNeedRender();
}

void RenderSwiper::Tick(uint64_t duration)
{
    elapsedTime_ += duration;
    if (elapsedTime_ >= autoPlayInterval_) {
        if (currentIndex_ >= itemCount_ - 1 && !loop_) {
            LOGD("already last one, stop auto play because not loop");
            if (scheduler_) {
                scheduler_->Stop();
            }
        } else {
            if (swiperIndicatorData_.isPressed) {
                // end drag operations on auto play.
                DragIndicatorEnd();
            }
            if (swiperIndicatorData_.isHovered) {
                ResetHoverZoomDot();
                StartZoomOutAnimation(true);
            }
            int nextIndex = GetNextIndex();
            StartIndicatorAnimation(currentIndex_, nextIndex, currentIndex_ > nextIndex);
        }
        elapsedTime_ = 0;
    }
}

void RenderSwiper::OnHiddenChanged(bool hidden)
{
    if (hidden) {
        if (autoPlay_) {
            LOGD("stop autoplay cause by hidden");
            if (scheduler_) {
                scheduler_->Stop();
            }
            StopSwipeAnimation();
        }
    } else {
        RestoreAutoPlay();
    }
}

bool RenderSwiper::OnRotation(const RotationEvent& event)
{
    if (disableRotation_) {
        return false;
    }
    if (itemCount_ < LEAST_SLIDE_ITEM_COUNT) {
        LOGI("Rotation is not enabled when count is 1");
        return false;
    }
    // Clockwise rotation switches to the next one, counterclockwise rotation switches to the previous one.
    auto context = GetContext().Upgrade();
    if (!context) {
        LOGE("context is null!");
        return false;
    }
    double deltaPx = context->NormalizeToPx(Dimension(event.value, DimensionUnit::VP)) * ROTATION_SENSITIVITY_NORMAL;
    switch (rotationStatus_) {
        case RotationStatus::ROTATION_START:
        case RotationStatus::ROTATION_UPDATE:
            rotationStatus_ = RotationStatus::ROTATION_UPDATE;
            HandleRotationUpdate(deltaPx);
            break;
        case RotationStatus::ROTATION_END:
        default:
            rotationStatus_ = RotationStatus::ROTATION_START;
            HandleRotationStart();
    }
    ResetRotationEndListener();
    return true;
}

void RenderSwiper::OnStatusChanged(RenderStatus renderStatus)
{
    if (renderStatus == RenderStatus::FOCUS) {
        onFocus_ = true;
    } else if (renderStatus == RenderStatus::BLUR) {
        onFocus_ = false;
    }
}

double RenderSwiper::GetValidEdgeLength(double swiperLength, double indicatorLength, const Dimension& edge) const
{
    double edgeLength = edge.Unit() == DimensionUnit::PERCENT ? swiperLength * edge.Value() : NormalizeToPx(edge);
    if (!NearZero(edgeLength) && edgeLength > swiperLength - indicatorLength) {
        edgeLength = swiperLength - indicatorLength;
    }
    if (edgeLength < 0.0) {
        edgeLength = 0.0;
    }
    return edgeLength;
}

void RenderSwiper::GetIndicatorCurrentRect(SwiperIndicatorData& indicatorData)
{
    if (!indicator_) {
        indicatorRect_ = Rect();
        return;
    }
    Offset offset = indicatorPosition_ + GetGlobalOffset();
    Size size = Size(swiperIndicatorData_.indicatorPaintData.width, swiperIndicatorData_.indicatorPaintData.height);
    indicatorRect_ = Rect(offset, size);
}

double RenderSwiper::GetIndicatorWidth(SwiperIndicatorData& indicatorData)
{
    double indicatorWidth = 0.0;
    double lastItemEdge = 0.0;

    if (currentHoverIndex_ == itemCount_ - 1) {
        double deltaPadding = 0.0;
        if (axis_ == Axis::HORIZONTAL) {
            lastItemEdge = indicatorData.indicatorItemData[itemCount_ - 1].center.GetX() +
                           NormalizeToPx(indicator_->GetPressSize()) / 2;
        } else {
            lastItemEdge = indicatorData.indicatorItemData[itemCount_ - 1].center.GetY() +
                           NormalizeToPx(indicator_->GetPressSize()) / 2;
        }
        if (currentIndex_ == itemCount_ - 1) {
            deltaPadding = NormalizeToPx(indicator_->GetPressSize()) / 2;
        }
        indicatorWidth = lastItemEdge + indicatorData.startEndPadding + deltaPadding;
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            lastItemEdge = indicatorData.indicatorItemData[itemCount_ - 1].center.GetX() +
                           indicatorData.indicatorItemData[itemCount_ - 1].width / 2;
        } else {
            lastItemEdge = indicatorData.indicatorItemData[itemCount_ - 1].center.GetY() +
                           indicatorData.indicatorItemData[itemCount_ - 1].height / 2;
        }
        indicatorWidth = lastItemEdge + indicatorData.startEndPadding;
    }
    return indicatorWidth;
}

void RenderSwiper::LayoutIndicator(SwiperIndicatorData& indicatorData)
{
    if (!indicator_) {
        return;
    }

    // calc real hot zone size by zoom and stretch
    if (NearZero(hotZoneMaxSize_) || NearZero(hotZoneMinSize_)) {
        hotZoneMaxSize_ = NormalizeToPx((indicator_->GetHotZoneSize()));
        hotZoneMinSize_ = hotZoneMaxSize_ / ZOOM_HOTZONE_MAX_RATE;
    }
    hotZoneRealSize_ = hotZoneMinSize_ + (hotZoneMaxSize_ - hotZoneMinSize_) * zoomValue_;
    hotZoneRealSize_ *= heightStretchRate_;

    // update indicator item paint data;
    indicatorData.isDigital = digitalIndicator_;
    if (!digitalIndicator_) {
        UpdateIndicatorItem(indicatorData);
    }

    // update Indicator paint data
    if (digitalIndicator_) {
        LayoutDigitalIndicator(indicatorData);
        Size digitalIndicatorSize = indicatorData.textBoxRender->GetLayoutSize();
        indicatorData.indicatorPaintData.width = digitalIndicatorSize.Width();
        indicatorData.indicatorPaintData.height = digitalIndicatorSize.Height();
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            indicatorData.indicatorPaintData.width = GetIndicatorWidth(indicatorData);
            indicatorData.indicatorPaintData.height = hotZoneRealSize_; // influenced on zoom and stretch
        } else {
            indicatorData.indicatorPaintData.width = hotZoneRealSize_; // influenced on zoom and stretch
            indicatorData.indicatorPaintData.height = GetIndicatorWidth(indicatorData);
        }
    }
    indicatorData.indicatorPaintData.radius = hotZoneRealSize_ / 2; // influenced on zoom and stretch
    if (!digitalIndicator_ && (indicatorData.isHovered || indicatorData.isPressed)) {
        indicatorData.indicatorPaintData.color = indicator_->GetHotZoneColor();
    } else {
        indicatorData.indicatorPaintData.color = Color::WHITE;
    }

    // update position
    UpdateIndicatorPosition(indicatorData);
}

void RenderSwiper::InitDigitalIndicator(SwiperIndicatorData& indicatorData)
{
    auto textBoxComponent = AceType::MakeRefPtr<BoxComponent>();
    double padding = NormalizeToPx(INDICATOR_DIGITAL_PADDING);
    Edge margin = (axis_ == Axis::HORIZONTAL) ?
                  Edge(padding, 0, padding, 0, DimensionUnit::PX) :
                  Edge(0, padding, 0, padding, DimensionUnit::PX);
    textBoxComponent->SetPadding(margin);
    indicatorData.textBoxRender = AceType::DynamicCast<RenderBox>(textBoxComponent->CreateRenderNode());
    indicatorData.textBoxRender->Attach(GetContext());

    // add flex
    FlexDirection direction = axis_ == Axis::HORIZONTAL ? FlexDirection::ROW : FlexDirection::COLUMN;
    indicatorData.flexComponent = AceType::MakeRefPtr<FlexComponent>(direction,
        FlexAlign::FLEX_END, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    indicatorData.flexComponent->SetMainAxisSize(MainAxisSize::MIN);
    indicatorData.flexRender = AceType::DynamicCast<RenderFlex>(indicatorData.flexComponent->CreateRenderNode());
    indicatorData.textBoxRender->AddChild(indicatorData.flexRender);
    indicatorData.flexRender->Attach(GetContext());
    indicatorData.flexRender->Update(indicatorData.flexComponent);

    // add text
    indicatorData.textComponentPrev = AceType::MakeRefPtr<TextComponent>("");
    indicatorData.textRenderPrev = AceType::DynamicCast<RenderText>(
        indicatorData.textComponentPrev->CreateRenderNode());
    indicatorData.flexRender->AddChild(indicatorData.textRenderPrev);
    indicatorData.textRenderPrev->Attach(GetContext());
    indicatorData.textRenderPrev->Update(indicatorData.textComponentPrev);

    indicatorData.textComponentNext = AceType::MakeRefPtr<TextComponent>("");
    indicatorData.textRenderNext = AceType::DynamicCast<RenderText>(
        indicatorData.textComponentNext->CreateRenderNode());
    indicatorData.flexRender->AddChild(indicatorData.textRenderNext);
    indicatorData.textRenderNext->Attach(GetContext());
    indicatorData.textRenderNext->Update(indicatorData.textComponentNext);

    indicatorData.textBoxRender->Update(textBoxComponent);
}

void RenderSwiper::LayoutDigitalIndicator(SwiperIndicatorData& indicatorData)
{
    InitDigitalIndicator(indicatorData);

    auto textStyle = indicator_->GetDigitalIndicatorTextStyle();
    Color normalTextColor = textStyle.GetTextColor();
    // update text prev
    std::string indicatorTextPrev = std::to_string(currentIndex_ + 1);
    if (indicatorIsFocus_) {
        textStyle.SetTextColor(indicator_->GetIndicatorTextFocusColor());
    } else {
        textStyle.SetTextColor(normalTextColor);
    }
    indicatorData.textComponentPrev->SetTextStyle(textStyle);
    indicatorData.textComponentPrev->SetData(indicatorTextPrev);
    indicatorData.textRenderPrev->Update(indicatorData.textComponentPrev);

    // update text next
    std::string indicatorTextNext = (axis_ == Axis::HORIZONTAL) ? std::string("/").append(std::to_string(itemCount_))
        : std::string("/\n").append(std::to_string(itemCount_));
    textStyle.SetTextColor(normalTextColor);
    indicatorData.textComponentNext->SetTextStyle(textStyle);
    indicatorData.textComponentNext->SetData(indicatorTextNext);
    indicatorData.textRenderNext->Update(indicatorData.textComponentNext);

    // upadate text box
    auto decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetBackgroundColor(Color::TRANSPARENT);
    Border border;
    border.SetBorderRadius(Radius(indicator_->GetHotZoneSize() / 2.0));
    decoration->SetBorder(border);
    indicatorData.textBoxRender->SetBackDecoration(decoration);
    if (axis_ == Axis::HORIZONTAL) {
        indicatorData.textBoxRender->SetHeight(NormalizeToPx(indicator_->GetHotZoneSize()));
    } else {
        indicatorData.textBoxRender->SetWidth(NormalizeToPx(indicator_->GetHotZoneSize()));
    }

    LayoutParam innerLayout;
    innerLayout.SetMaxSize(Size(swiperWidth_, swiperHeight_));
    indicatorData.textBoxRender->Layout(innerLayout);
}

void RenderSwiper::UpdateIndicatorPosition(SwiperIndicatorData& indicatorData)
{
    Offset position;
    double indicatorWidth = indicatorData.indicatorPaintData.width;
    double indicatorHeight = indicatorData.indicatorPaintData.height;
    double stableOffset = NormalizeToPx(INDICATOR_PADDING_TOP_DEFAULT) + (hotZoneMaxSize_ + hotZoneRealSize_) * 0.5;

    if (indicator_->GetLeft().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t left = GetValidEdgeLength(swiperWidth_, indicatorWidth, indicator_->GetLeft());
        swiperLeft_ = indicator_->GetLeft();
        position.SetX(left);
    } else if (indicator_->GetRight().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t right = GetValidEdgeLength(swiperWidth_, indicatorWidth, indicator_->GetRight());
        swiperRight_ = indicator_->GetRight();
        position.SetX(swiperWidth_ - indicatorWidth - right);
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            position.SetX((swiperWidth_ - indicatorWidth) / 2.0);
        } else {
            // horizontal line of indicator zone is stable.
            double currentX = swiperWidth_ - stableOffset;
            position.SetX(currentX);
        }
    }

    if (indicator_->GetTop().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t top = GetValidEdgeLength(swiperHeight_, indicatorHeight, indicator_->GetTop());
        swiperTop_ = indicator_->GetTop();
        position.SetY(top);
    } else if (indicator_->GetBottom().Value() != SwiperIndicator::DEFAULT_POSITION) {
        int32_t bottom = GetValidEdgeLength(swiperHeight_, indicatorHeight, indicator_->GetBottom());
        swiperBottom_ = indicator_->GetBottom();
        position.SetY(swiperHeight_ - indicatorHeight - bottom);
    } else {
        if (axis_ == Axis::HORIZONTAL) {
            // horizontal line of indicator zone is stable.
            double currentY = swiperHeight_ - stableOffset;
            position.SetY(currentY);
        } else {
            position.SetY((swiperHeight_ - indicatorHeight) / 2.0);
        }
    }

    // update position on stretch or restract indicator zone
    UpdatePositionOnStretch(position, indicatorData);

    // update  position
    indicatorPosition_ = position;
    indicatorData.indicatorPaintData.position = position;
    indicatorData.indicatorPaintData.center = position + Offset(indicatorData.indicatorPaintData.width / 2,
        indicatorData.indicatorPaintData.height / 2);
}

void RenderSwiper::UpdateIndicatorItem(SwiperIndicatorData& indicatorData)
{
    // horizontal line of indicator zone is stable
    double hotZoneCenterPadding = hotZoneRealSize_ / 2.0;
    if (indicatorData.isHovered || indicatorData.isPressed) {
        indicatorData.startEndPadding = NormalizeToPx(indicator_->GetStartEndPadding() +
            (indicator_->GetPressPadding() - indicator_->GetStartEndPadding()) * zoomValue_);
    } else {
        indicatorData.startEndPadding = NormalizeToPx(indicator_->GetStartEndPadding());
    }
    Offset startCenterOffset = axis_ == Axis::HORIZONTAL ? Offset(indicatorData.startEndPadding, hotZoneCenterPadding) :
        Offset(hotZoneCenterPadding, indicatorData.startEndPadding);
    Offset centerOffset = startCenterOffset;

    double targetIndex = currentIndex_;
    double hoverIndex = currentHoverIndex_;
    if (needReverse_) {
        targetIndex = itemCount_ - currentIndex_ - 1;
    }

    double itemRadius = 0.0;
    for (int32_t i = 0; i < itemCount_; ++i) {
        swiperSize_ = indicator_->GetSize();
        bool isZoomInBackground = indicatorData.isHovered || indicatorData.isPressed;
        if (isZoomInBackground) {
            // indicator radius and point padding is dynamic changed on zoom and stretch
            itemRadius = NormalizeToPx(
                indicator_->GetSize() + (indicator_->GetPressSize() - indicator_->GetSize()) * zoomValue_) / 2.0;
            indicatorData.pointPadding = NormalizeToPx(indicator_->GetIndicatorPointPadding() +
                (indicator_->GetPressPointPadding() - indicator_->GetIndicatorPointPadding()) *
                zoomValue_) * widthStretchRate_;
        } else {
            itemRadius = NormalizeToPx(indicator_->GetSize()) / 2.0;
            indicatorData.pointPadding = NormalizeToPx(indicator_->GetIndicatorPointPadding());
        }

        double itemStartEndPadding = 0.0;
        if (i == targetIndex) {
            itemStartEndPadding = itemRadius * 2;
            indicatorData.indicatorItemData[i].color = indicator_->GetSelectedColor();
            selectedColors_ = indicator_->GetSelectedColor();
        } else {
            itemStartEndPadding = itemRadius;
            indicatorData.indicatorItemData[i].color = indicator_->GetColor();
            colors_ = indicator_->GetColor();
        }
        Offset paddingStartOffset;
        Offset paddingEndOffset;
        if (axis_ == Axis::HORIZONTAL) {
            paddingStartOffset = Offset(itemStartEndPadding, 0);
            paddingEndOffset = Offset(itemStartEndPadding + indicatorData.pointPadding, 0);
        } else {
            paddingStartOffset = Offset(0, itemStartEndPadding);
            paddingEndOffset = Offset(0, itemStartEndPadding + indicatorData.pointPadding);
        }

        // update mouse hover radius
        if (isZoomInBackground && i == hoverIndex) {
            // point radius is dynamic changed on mouse hover
            itemRadius = NormalizeToPx(indicator_->GetPressSize() +
                                       (indicator_->GetHoverSize() - indicator_->GetPressSize()) * zoomDotValue_) / 2.0;
        }
        if (axis_ == Axis::HORIZONTAL) {
            indicatorData.indicatorItemData[i].height = itemRadius * 2;
            indicatorData.indicatorItemData[i].width = (i == targetIndex ? itemRadius * 4 : itemRadius * 2);
        } else {
            indicatorData.indicatorItemData[i].width = itemRadius * 2;
            indicatorData.indicatorItemData[i].height = (i == targetIndex ? itemRadius * 4 : itemRadius * 2);
        }
        indicatorData.indicatorItemData[i].radius = itemRadius;

        centerOffset += paddingStartOffset;
        indicatorData.indicatorItemData[i].center = centerOffset;
        indicatorData.indicatorItemData[i].position = centerOffset -
            Offset(indicatorData.indicatorItemData[i].width / 2, indicatorData.indicatorItemData[i].height / 2);
        centerOffset += paddingEndOffset;
    }
}

void RenderSwiper::UpdatePositionOnStretch(Offset& position, const SwiperIndicatorData& indicatorData)
{
    double indicatorWidth = indicatorData.indicatorPaintData.width;
    double indicatorHeight = indicatorData.indicatorPaintData.height;
    if (widthStretchRate_ > DRAG_STRETCH_BASE_WIDTH) {
        if (zoomValue_ == ZOOM_MAX) {
            // stretch indicator and update start position
            if (axis_ == Axis::HORIZONTAL) {
                auto currentIndex = needReverse_ ? (itemCount_ - currentIndex_ - 1) : currentIndex_;
                if (currentIndex == itemCount_ - 1) {
                    position.SetX(indicatorZoomMaxPositionLT_.GetX());
                } else if (currentIndex == 0) {
                    position.SetX(indicatorZoomMaxPositionRB_.GetX() - indicatorData.indicatorPaintData.width);
                }
            } else {
                if (currentIndex_ == itemCount_ - 1) {
                    position.SetY(indicatorZoomMaxPositionLT_.GetY());
                } else if (currentIndex_ == 0) {
                    position.SetY(indicatorZoomMaxPositionRB_.GetY() - indicatorData.indicatorPaintData.height);
                }
            }
        } else if (zoomValue_ > ZOOM_MIN) {
            // restract indicator and update start position
            if (axis_ == Axis::HORIZONTAL) {
                auto currentIndex = needReverse_ ? (itemCount_ - currentIndex_ - 1) : currentIndex_;
                if (currentIndex == itemCount_ - 1) {
                    position.SetX(indicatorZoomMinPositionLT_.GetX() -
                        (indicatorZoomMinPositionLT_.GetX() - indicatorZoomMaxPositionLT_.GetX()) * zoomValue_);
                } else if (currentIndex == 0) {
                    position.SetX(indicatorZoomMinPositionRB_.GetX() - indicatorWidth +
                        (indicatorZoomMaxPositionRB_.GetX() - indicatorZoomMinPositionRB_.GetX()) * zoomValue_);
                }
            } else {
                if (currentIndex_ == itemCount_ - 1) {
                    position.SetY(indicatorZoomMinPositionLT_.GetY() -
                        (indicatorZoomMinPositionLT_.GetY() - indicatorZoomMaxPositionLT_.GetY()) * zoomValue_);
                } else if (currentIndex_ == 0) {
                    position.SetY(indicatorZoomMinPositionRB_.GetY() - indicatorHeight +
                        (indicatorZoomMaxPositionRB_.GetY() - indicatorZoomMinPositionRB_.GetY()) * zoomValue_);
                }
            }
        }
    }
}

void RenderSwiper::IndicatorSwipePrev()
{
    auto toIndex = GetPrevIndex();
    StartIndicatorAnimation(currentIndex_, toIndex, currentIndex_ == 0);
}

void RenderSwiper::IndicatorSwipeNext()
{
    auto toIndex = GetNextIndex();
    StartIndicatorAnimation(currentIndex_, toIndex, currentIndex_ == itemCount_ - 1);
}

bool RenderSwiper::MouseHoverTest(const Point& parentLocalPoint)
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }
    const auto localPoint = parentLocalPoint - GetPosition();
    const auto& children = GetChildren();
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        child->MouseHoverTest(localPoint);
    }
    bool isInRegion = InTouchRectList(parentLocalPoint, GetTouchRectList());
    if (isInRegion) {
        context->AddToHoverList(AceType::WeakClaim(this).Upgrade());
    }

    // swiper indicator mouse hover
    if (!indicator_) {
        return isInRegion;
    }
    // indicator zone zoom animation should wait for indicator moving animation finished
    if (isIndicatorAnimationStart_) {
        return isInRegion;
    }

    // get absolute position
    Point hoverPoint = parentLocalPoint + GetGlobalOffset();
    GetIndicatorCurrentRect(swiperIndicatorData_);
    if (indicatorRect_.IsInRegion(hoverPoint)) {
        if (autoPlay_ && scheduler_ && scheduler_->IsActive()) {
            // forbid indicator operation on auto play period.
            return isInRegion;
        }
        if ((!swiperIndicatorData_.isHovered && !swiperIndicatorData_.isPressed) || !IsZoomOutAnimationStopped()) {
            // hover animation
            swiperIndicatorData_.isHovered = true;
            StartZoomInAnimation(true);
            return isInRegion;
        }
        // point zoom after indicator zone zoom.
        if (!IsZoomAnimationStopped()) {
            return isInRegion;
        }
        for (int32_t i = 0; i < itemCount_; i++) {
            Offset offset = swiperIndicatorData_.indicatorItemData[i].position + indicatorRect_.GetOffset();
            Size size = Size(swiperIndicatorData_.indicatorItemData[i].width,
                swiperIndicatorData_.indicatorItemData[i].height);
            Rect itemRect = Rect(offset, size);
            if (itemRect.IsInRegion(hoverPoint)) {
                if (currentHoverIndex_ != i) {
                    StartZoomInDotAnimation(i);
                }
                return isInRegion;
            }
        }
        if (currentHoverIndex_ != INDICATOR_INVALID_HOVER_INDEX && IsZoomOutDotAnimationStopped()) {
            StartZoomOutDotAnimation();
        }
    } else {
        if (swiperIndicatorData_.isHovered) {
            ResetHoverZoomDot();
            StartZoomOutAnimation(true);
        }
    }
    return isInRegion;
}

void RenderSwiper::IndicatorShowFocus(bool isFocus)
{
    if (!indicator_) {
        return;
    }
    indicatorIsFocus_ = isFocus;
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    Offset offset = swiperIndicatorData_.indicatorPaintData.position;
    Size size = Size(swiperIndicatorData_.indicatorPaintData.width, swiperIndicatorData_.indicatorPaintData.height);
    Offset globalOffset = swiperIndicatorData_.indicatorPaintData.position + GetGlobalOffset();
    double radius = swiperIndicatorData_.indicatorPaintData.radius;
    if (isFocus) {
        offset += Offset(NormalizeToPx(INDICATOR_FOCUS_DEL_OFFSET), NormalizeToPx(INDICATOR_FOCUS_DEL_OFFSET));
        size -= Size(NormalizeToPx(INDICATOR_FOCUS_DEL_SIZE), NormalizeToPx(INDICATOR_FOCUS_DEL_SIZE));
        globalOffset += Offset(NormalizeToPx(INDICATOR_FOCUS_DEL_OFFSET), NormalizeToPx(INDICATOR_FOCUS_DEL_OFFSET));
        Radius focusRadius = Radius(radius) - Radius(NormalizeToPx(INDICATOR_FOCUS_RADIUS_DEL_SIZE));
        context->ShowFocusAnimation(RRect::MakeRRect(Rect(offset, size), focusRadius), Color(INDICATOR_FOCUS_COLOR),
            globalOffset);
    } else {
        context->CancelFocusAnimation();
    }
    MarkNeedLayout();
}

void RenderSwiper::UpdateIndicatorFocus(bool isFocus, bool reverse)
{
    if (reverse) {
        IndicatorSwipePrev();
    } else {
        IndicatorSwipeNext();
    }
    IndicatorShowFocus(isFocus);
}

void RenderSwiper::VibrateIndicator()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("GetVibrator fail, context is null");
        return;
    }

    if (!vibrator_) {
        vibrator_ = VibratorProxy::GetInstance().GetVibrator(context->GetTaskExecutor());
    }
    if (vibrator_) {
        vibrator_->Vibrate(VIBRATE_DURATION);
    } else {
        LOGW("GetVibrator fail");
    }
}

void RenderSwiper::UpdateIndicatorSpringStatus(SpringStatus status)
{
    if (indicatorSpringStatus_ == SpringStatus::SPRING_STOP &&
        status == SpringStatus::FOCUS_SWITCH) {
        return;
    }
    LOGD("UpdateIndicatorSpringStatus from[%{public}d] to[%{public}d]", indicatorSpringStatus_, status);
    indicatorSpringStatus_ = status;
}

SpringStatus RenderSwiper::GetIndicatorSpringStatus() const
{
    return indicatorSpringStatus_;
}

void RenderSwiper::ResetIndicatorSpringStatus()
{
    if (GetIndicatorSpringStatus() == SpringStatus::FOCUS_SWITCH) {
        UpdateIndicatorTailPosistion(DRAG_OFFSET_MIN, DRAG_OFFSET_MIN);
    }
    UpdateIndicatorSpringStatus(SpringStatus::SPRING_STOP);
}

void RenderSwiper::ResetIndicatorPosition()
{
    UpdateIndicatorHeadPosistion(DRAG_OFFSET_MIN);
    UpdateIndicatorTailPosistion(DRAG_OFFSET_MIN);
    UpdateIndicatorPointPosistion(DRAG_OFFSET_MIN);
}

void RenderSwiper::ResetHoverZoomDot()
{
    StopZoomDotAnimation();
    UpdateZoomDotValue(ZOOM_DOT_MIN);
    currentHoverIndex_ = INDICATOR_INVALID_HOVER_INDEX;
}

void RenderSwiper::MarkIndicatorPosition(bool isZoomMax)
{
    if (isZoomMax) {
        indicatorZoomMaxPositionLT_ = indicatorPosition_;
        indicatorZoomMaxPositionRB_ = indicatorZoomMaxPositionLT_ +
            Offset(swiperIndicatorData_.indicatorPaintData.width, swiperIndicatorData_.indicatorPaintData.height);
    } else {
        indicatorZoomMinPositionLT_ = indicatorPosition_;
        indicatorZoomMinPositionRB_ = indicatorZoomMinPositionLT_ +
            Offset(swiperIndicatorData_.indicatorPaintData.width, swiperIndicatorData_.indicatorPaintData.height);
    }
}

void RenderSwiper::StartIndicatorSpringAnimation(double start, double end)
{
    LOGD("StartIndicatorSpringAnimation(%{public}lf, %{public}lf)", start, end);
    UpdateIndicatorSpringStatus(SpringStatus::SPRING_START);
    double dampInc = std::fabs(currentIndex_ - targetIndex_) * SPRING_DAMP_INC;
    auto springDescription = AceType::MakeRefPtr<SpringProperty>(SPRING_MASS, SPRING_STIFF, SPRING_DAMP + dampInc);
    if (!indicatorSpringMotion_) {
        indicatorSpringMotion_ = AceType::MakeRefPtr<SpringMotion>(start, end, 0.0, springDescription);
    } else {
        indicatorSpringMotion_->Reset(start, end, 0.0, springDescription);
    }

    indicatorSpringMotion_->ClearListeners();
    indicatorSpringMotion_->AddListener([weak = AceType::WeakClaim(this), end](double position) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            double offset = position;
            double switchOffset = position - end;
            swiper->UpdateIndicatorTailPosistion(offset, switchOffset);
        }
    });

    springController_->PlayMotion(indicatorSpringMotion_);
    springController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->ResetIndicatorSpringStatus();
        }
    });
}

// spring animimation
void RenderSwiper::StopIndicatorSpringAnimation()
{
    if (springController_ && !springController_->IsStopped()) {
        // clear stop listener before stop
        springController_->ClearStopListeners();
        springController_->Stop();
    }
    ResetIndicatorSpringStatus();
    LOGD("StopIndicatorSpringAnimation");
}

void RenderSwiper::CalMaxStretch()
{
    if (focusStretchMaxTime_ == DRAG_OFFSET_MIN) {
        double stretch = DRAG_OFFSET_MIN;
        double maxStretch = DRAG_OFFSET_MIN;
        const int32_t step = DRAG_CALC_STRETCH_STEP_INT;
        for (int32_t i = step; i <= DRAG_CALC_STRETCH_STEP_MAX; i += step) {
            double actualStep = i * DRAG_CALC_STRETCH_STEP / static_cast<double>(DRAG_CALC_STRETCH_STEP_INT);
            stretch = INDICATOR_FOCUS_HEAD->Move(actualStep) - INDICATOR_FOCUS_TAIL->Move(actualStep);
            if (stretch > maxStretch) {
                maxStretch = stretch;
                focusStretchMaxTime_ = actualStep;
            }
        }
        LOGD("CalMaxStretch(%{public}lf), time(%{public}lf)", maxStretch, focusStretchMaxTime_);
    }
}

void RenderSwiper::MoveIndicator(int32_t toIndex, double offset, bool isAuto)
{
    if (toIndex == currentIndex_) {
        LOGW("MoveIndicator drop it for edge moving.");
        return;
    }

    double dragRange = ((axis_ == Axis::HORIZONTAL) ? swiperWidth_ : swiperHeight_) - prevMargin_ - nextMargin_;
    if (NearZero(dragRange)) {
        return;
    }
    double dragRate = offset / dragRange;
    animationDirect_ = (currentIndex_ <= toIndex) ? INDICATOR_DIRECT_FORWARD : INDICATOR_DIRECT_BACKWARD;
    if (needReverse_) {
        animationDirect_ *= INDICATOR_DIRECT_BACKWARD;
    }
    dragRate = std::fabs(dragRate);
    if (dragRate >= DRAG_OFFSET_MAX) {
        // move to end, and index change
        UpdateIndicatorPointPosistion(DRAG_OFFSET_MIN);
        UpdateIndicatorHeadPosistion(DRAG_OFFSET_MIN);
        UpdateIndicatorSpringStatus(SpringStatus::FOCUS_SWITCH);
        return;
    }

    targetIndex_ = toIndex;
    int32_t indicatorMoveNums = std::abs(currentIndex_ - toIndex);
    UpdateIndicatorPointPosistion(INDICATOR_NORMAL_POINT->MoveInternal(dragRate));
    UpdateIndicatorHeadPosistion(INDICATOR_FOCUS_HEAD->MoveInternal(dragRate) * indicatorMoveNums);
    if (!isAuto) {
        // move tails with hand
        UpdateIndicatorTailPosistion(INDICATOR_FOCUS_TAIL->MoveInternal(dragRate) * indicatorMoveNums);
        return;
    }

    // animation
    if (dragRate < focusStretchMaxTime_) {
        UpdateIndicatorTailPosistion(INDICATOR_FOCUS_TAIL->MoveInternal(dragRate) * indicatorMoveNums);
        return;
    }

    // curve sport into spring sport
    if (GetIndicatorSpringStatus() == SpringStatus::SPRING_STOP) {
        LOGD("indicator tail move end, start spring motion.");
        double springStart = INDICATOR_FOCUS_TAIL->MoveInternal(dragRate);
        UpdateIndicatorTailPosistion(springStart * indicatorMoveNums);
        StartIndicatorSpringAnimation(springStart * indicatorMoveNums, DRAG_OFFSET_MAX * indicatorMoveNums);
    }
}

void RenderSwiper::DragIndicator(double offset)
{
    // start drag after zoom in completed.
    if (!IsZoomAnimationStopped()) {
        LOGD("zoom in is not completed");
        return;
    }

    const double longPressDragStart = DRAG_OFFSET_START_DP * scale_;
    const double longPressDragSwitchFocus = DRAG_OFFSET_SWITCH_DP * scale_;
    const double longPressDragMaxDiff = longPressDragSwitchFocus - longPressDragStart;
    if (!isDragStart_) {
        isDragStart_ = true;
        dragBaseOffset_ = offset;
        dragMoveOffset_ = offset;
        return;
    }

    dragMoveOffset_ += offset;
    double diffOffset = dragMoveOffset_ - dragBaseOffset_;
    double fabsOffset = std::fabs(diffOffset);
    if (fabsOffset <= longPressDragStart) {
        // indicator move when drag offset large than 4 vp
        return;
    }

    animationDirect_ = diffOffset >= 0 ? INDICATOR_DIRECT_FORWARD : INDICATOR_DIRECT_BACKWARD;

    if ((!needReverse_ && (currentIndex_ + animationDirect_ >= itemCount_ || currentIndex_ + animationDirect_ < 0)) ||
        (needReverse_ && (currentIndex_ - animationDirect_ >= itemCount_ || currentIndex_ - animationDirect_ < 0))) {
        // drag end and stretch background
        return DragEdgeStretch(fabsOffset);
    }

    if (fabsOffset >= longPressDragSwitchFocus) {
        // focus switch and vibrate
        VibrateIndicator();
        outItemIndex_ = currentIndex_;
        if (needReverse_) {
            currentIndex_ -= animationDirect_;
        } else {
            currentIndex_ += animationDirect_;
        }
        nextIndex_ = currentIndex_;
        dragBaseOffset_ += longPressDragSwitchFocus * animationDirect_;
        ResetIndicatorPosition();
        MarkNeedLayout();
    } else {
        double dragRate = (fabsOffset - longPressDragStart) / longPressDragMaxDiff;
        UpdateIndicatorHeadPosistion(INDICATOR_FOCUS_HEAD->MoveInternal(dragRate));
        UpdateIndicatorTailPosistion(INDICATOR_FOCUS_TAIL->MoveInternal(dragRate));
        UpdateIndicatorPointPosistion(INDICATOR_NORMAL_POINT->MoveInternal(dragRate));
    }
}

void RenderSwiper::DragIndicatorEnd()
{
    if ((currentIndex_ + animationDirect_ >= itemCount_ || currentIndex_ + animationDirect_ < 0) &&
        std::fabs(dragMoveOffset_ - dragBaseOffset_) > 0) {
        // drag than 80dp, play reset and zoom out animation
        LOGD("drag end and start restrection animation");
        StartDragRetractionAnimation();
        StartZoomOutAnimation();
    } else {
        ResetIndicatorPosition();
        UpdateEdgeStretchRate(DRAG_OFFSET_MIN);
        // only play zoom out animation
        LOGD("drag end and start mask zoom out animation");
        StartZoomOutAnimation();
    }
    dragBaseOffset_ = DRAG_OFFSET_MIN;
    dragMoveOffset_ = DRAG_OFFSET_MIN;
}

void RenderSwiper::DragEdgeStretch(double offset)
{
    const double longPressDragStrechLongest = DRAG_STRETCH_LONGEST_DP * scale_;
    if (offset >= longPressDragStrechLongest) {
        UpdateEdgeStretchRate(DRAG_OFFSET_MAX);
    } else {
        UpdateEdgeStretchRate(offset / longPressDragStrechLongest);
    }
}

void RenderSwiper::StartZoomInAnimation(bool isMouseHover)
{
    StopZoomAnimation();
    if (zoomValue_ == ZOOM_MIN) {
        MarkIndicatorPosition(false);
    }
    LOGD("startZoomInAnimation zoom[%{public}lf,%{public}lf], opacity[%{public}lf,%{public}lf], duration[%{public}d]",
        zoomValue_, ZOOM_MAX, opacityValue_, OPACITY_MAX, ZOOM_IN_DURATION);
    zoomInAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(zoomValue_, ZOOM_MAX, Curves::SHARP);
    zoomInAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateZoomValue(value);
            if (value == ZOOM_MAX) {
                // record position zone of indicator zone for strecth when zoom in reach maximum value.
                swiper->MarkIndicatorPosition();
            }
        }
    });

    opacityInAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(opacityValue_, OPACITY_MAX, Curves::SHARP);
    opacityInAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateMaskOpacity(value);
        }
    });

    zoomInController_->ClearInterpolators();
    zoomInController_->AddInterpolator(zoomInAnimation_);
    zoomInController_->AddInterpolator(opacityInAnimation_);
    zoomInController_->SetDuration(ZOOM_IN_DURATION);
    zoomInController_->Play();
}

void RenderSwiper::StartZoomOutAnimation(bool isMouseHover)
{
    StopZoomAnimation();
    int duartion = isMouseHover ? ZOOM_OUT_HOVER_DURATION : ZOOM_OUT_DURATION;
    LOGD("StartZoomOutAnimation zoom[%{public}lf,%{public}lf], opacity[%{public}lf,%{public}lf], duration[%{public}d]",
        zoomValue_, ZOOM_MIN, opacityValue_, OPACITY_MIN, duartion);
    zoomOutAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(zoomValue_, ZOOM_MIN, Curves::SHARP);
    zoomOutAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateZoomValue(value);
        }
    });
    opacityOutAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(opacityValue_, OPACITY_MIN, Curves::SHARP);
    opacityOutAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateMaskOpacity(value);
        }
    });
    zoomOutController_->ClearInterpolators();
    zoomOutController_->AddInterpolator(zoomOutAnimation_);
    zoomOutController_->AddInterpolator(opacityOutAnimation_);
    zoomOutController_->SetDuration(duartion);
    zoomOutController_->AddStopListener([weak = AceType::WeakClaim(this), isMouseHover]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            isMouseHover ? swiper->UpdateHoverStatus(false) : swiper->UpdatePressStatus(false);
        }
    });
    zoomOutController_->Play();
}

void RenderSwiper::StartZoomInDotAnimation(int32_t index)
{
    StopZoomDotAnimation(); // function will reset currentHoverIndex_. set it after stop zoom out dot.
    currentHoverIndex_ = index;
    LOGD("StartZoomInDotAnimation zoom[%{public}lf, %{public}lf], duration[%{public}d]",
        ZOOM_DOT_MIN, ZOOM_DOT_MAX, ZOOM_IN_DOT_DURATION);
    if (!zoomInDotAnimation_) {
        zoomInDotAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(ZOOM_DOT_MIN, ZOOM_DOT_MAX, Curves::SHARP);
        zoomInDotAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->UpdateZoomDotValue(value);
            }
        });
    }

    zoomInDotController_->ClearInterpolators();
    zoomInDotController_->AddInterpolator(zoomInDotAnimation_);
    zoomInDotController_->SetDuration(ZOOM_IN_DOT_DURATION);
    zoomInDotController_->Play();
}

void RenderSwiper::StartZoomOutDotAnimation()
{
    StopZoomDotAnimation();
    LOGD("StartZoomOutDotAnimation zoom[%{public}lf, %{public}lf], duration[%{public}d]",
        ZOOM_DOT_MAX, ZOOM_DOT_MIN, ZOOM_OUT_DOT_DURATION);
    if (!zoomOutDotAnimation_) {
        zoomOutDotAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(ZOOM_DOT_MAX, ZOOM_DOT_MIN, Curves::SHARP);
        zoomOutDotAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->UpdateZoomDotValue(value);
            }
        });
    }
    zoomOutDotController_->ClearInterpolators();
    zoomOutDotController_->AddInterpolator(zoomOutDotAnimation_);
    zoomOutDotController_->SetDuration(ZOOM_OUT_DOT_DURATION);
    zoomOutDotController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->currentHoverIndex_ = INDICATOR_INVALID_HOVER_INDEX;
        }
    });
    zoomOutDotController_->Play();
}

void RenderSwiper::StopZoomAnimation()
{
    LOGD("stopZoomAnimation");
    if (!zoomInController_->IsStopped()) {
        zoomInController_->ClearStopListeners();
        zoomInController_->Stop();
    }
    if (!zoomOutController_->IsStopped()) {
        zoomOutController_->ClearStopListeners();
        zoomOutController_->Stop();
    }
}

void RenderSwiper::StopZoomDotAnimation()
{
    LOGD("StopZoomDotAnimation");
    if (!zoomInDotController_->IsStopped()) {
        zoomInDotController_->ClearStopListeners();
        zoomInDotController_->Stop();
    }
    if (!zoomOutDotController_->IsStopped()) {
        zoomOutDotController_->ClearStopListeners();
        zoomOutDotController_->Stop();
    }
}

void RenderSwiper::StartDragRetractionAnimation()
{
    StopDragRetractionAnimation();
    dragRetractionAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(
        stretchRate_, TARGET_START_TRANSLATE_TIME, INDICATOR_ZONE_STRETCH);
    dragRetractionAnimation_->AddListener([weak = AceType::WeakClaim(this)](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateEdgeStretchRate(value);
        }
    });

    dragRetractionController_->ClearInterpolators();
    dragRetractionController_->AddInterpolator(dragRetractionAnimation_);
    dragRetractionController_->SetDuration(DRAG_RETRETION_DURATION);
    dragRetractionController_->Play();
}

void RenderSwiper::StopDragRetractionAnimation()
{
    if (!dragRetractionController_->IsStopped()) {
        dragRetractionController_->ClearStopListeners();
        dragRetractionController_->Stop();
    }
}

void RenderSwiper::FinishAllSwipeAnimation(bool useFinish)
{
    if (useFinish && IsAnimatorStopped()) {
        FireSwiperControllerFinishEvent();
        return;
    }
    if (useFinish) {
        FinishSwipeAnimation();
    } else {
        StopSwipeAnimation();
    }

    StopSwipeToAnimation();
    StopIndicatorAnimation();
    StopIndicatorSpringAnimation();
    ResetIndicatorPosition();

    LoadLazyItems((currentIndex_ + 1) % itemCount_ == targetIndex_);
    UpdateOneItemOpacity(MAX_OPACITY, currentIndex_);
    UpdateOneItemOpacity(MAX_OPACITY, targetIndex_);
    currentIndex_ = targetIndex_;
    if (useFinish) {
        FireSwiperControllerFinishEvent();
    }
    quickTrunItem_ = true;
    MarkNeedLayout();
}

bool RenderSwiper::IsAnimatorStopped() const
{
    bool isControllerRunning = controller_ && controller_->IsRunning();
    bool isSwiperControllerRunning = swipeToController_ && swipeToController_->IsRunning();
    bool isIndicatorControllerRunning = indicatorController_ && indicatorController_->IsRunning();
    bool isSpringControllerRunning = springController_ && springController_->IsRunning();
    return !isControllerRunning && !isSwiperControllerRunning && !isIndicatorControllerRunning &&
           !isSpringControllerRunning;
}

void RenderSwiper::FireSwiperControllerFinishEvent()
{
    if (swiper_ && swiper_->GetSwiperController() && !swiper_->GetSwiperController()->GetFinishCallback().IsEmpty()) {
        auto finishEvent = AceAsyncEvent<void()>::Create(swiper_->GetSwiperController()->GetFinishCallback(), context_);
        finishEvent();
    }
}

bool RenderSwiper::IsZoomAnimationStopped()
{
    return zoomInController_->IsStopped() && zoomOutController_->IsStopped();
}

bool RenderSwiper::IsZoomOutAnimationStopped()
{
    return zoomOutController_->IsStopped();
}

bool RenderSwiper::IsZoomOutDotAnimationStopped()
{
    return zoomOutDotController_->IsStopped();
}

void RenderSwiper::UpdateIndicatorLayout()
{
    LayoutIndicator(swiperIndicatorData_);
    MarkNeedRender();
}

void RenderSwiper::UpdateIndicatorOffset(int32_t fromIndex, int32_t toIndex, double value)
{
    int32_t indicatorMoveNums = std::abs(fromIndex - toIndex);
    if (value >= 1.0) {
        // move to end, and index change
        UpdateIndicatorSpringStatus(SpringStatus::FOCUS_SWITCH);
        UpdateIndicatorPointPosistion(DRAG_OFFSET_MIN);
        UpdateIndicatorHeadPosistion(DRAG_OFFSET_MIN);
        return;
    }

    UpdateIndicatorPointPosistion(INDICATOR_NORMAL_POINT->MoveInternal(value));
    UpdateIndicatorHeadPosistion(INDICATOR_FOCUS_HEAD->MoveInternal(value) * indicatorMoveNums);
    if (value < focusStretchMaxTime_) {
        UpdateIndicatorTailPosistion(INDICATOR_FOCUS_TAIL->MoveInternal(value) * indicatorMoveNums);
        return;
    }

    // curive sport into spring sport
    if (GetIndicatorSpringStatus() == SpringStatus::SPRING_STOP) {
        LOGD("indicator tail move end, start spring sport.");
        double springStart = INDICATOR_FOCUS_TAIL->MoveInternal(value) * indicatorMoveNums;
        UpdateIndicatorTailPosistion(springStart);
        StartIndicatorSpringAnimation(springStart, indicatorMoveNums * DRAG_OFFSET_MAX);
    }
}

void RenderSwiper::UpdateIndicatorHeadPosistion(double offset)
{
    indicatorHeadOffset_ = offset * animationDirect_;
}

void RenderSwiper::UpdateIndicatorTailPosistion(double offset, double switchOffset)
{
    indicatorTailOffset_ = offset * animationDirect_;
    // if indicator switch to the next or last, tail offset will be different.
    indicatorSwitchTailOffset_ = switchOffset * animationDirect_;
    MarkNeedRender();
}

void RenderSwiper::UpdateIndicatorPointPosistion(double offset)
{
    indicatorPointOffset_ = offset * animationDirect_;
    MarkNeedRender();
}

void RenderSwiper::UpdateMaskOpacity(double value)
{
    opacityValue_ = value;
    MarkNeedRender();
}

void RenderSwiper::UpdateZoomValue(double value)
{
    zoomValue_ = value;
    LayoutIndicator(swiperIndicatorData_);
    MarkNeedRender();
}

void RenderSwiper::UpdateZoomDotValue(double value)
{
    zoomDotValue_ = value;
    LayoutIndicator(swiperIndicatorData_);
    MarkNeedRender();
}

void RenderSwiper::UpdateEdgeStretchRate(double value)
{
    stretchRate_ = value;
    widthStretchRate_ = DRAG_STRETCH_BASE_WIDTH + (DRAG_STRETCH_MAX_WIDTH - DRAG_STRETCH_BASE_WIDTH) * value;
    heightStretchRate_ = DRAG_STRETCH_BASE_HIGH + (DRAG_STRETCH_MAX_HIGH - DRAG_STRETCH_BASE_HIGH) * value;
    LayoutIndicator(swiperIndicatorData_);
    MarkNeedRender();
}

void RenderSwiper::UpdatePressStatus(bool isPress)
{
    swiperIndicatorData_.isPressed = isPress;
}

void RenderSwiper::UpdateHoverStatus(bool isHover)
{
    swiperIndicatorData_.isHovered = isHover;
}

void RenderSwiper::StartIndicatorAnimation(int32_t fromIndex, int32_t toIndex, bool isLoop)
{
    LOGD("StartIndicatorAnimation");
    if (fromIndex == toIndex) {
        LOGD("from index is same to next index.");
        return;
    }
    if (isIndicatorAnimationStart_) {
        LOGE("indicator animation is processing.");
        return;
    }

    CalMaxStretch();
    StopIndicatorSpringAnimation();
    StopIndicatorAnimation();
    ResetHoverZoomDot();
    targetIndex_ = toIndex;
    nextIndex_ = toIndex;
    isIndicatorAnimationStart_ = true;
    animationDirect_ = (fromIndex - toIndex <= 0) ? INDICATOR_DIRECT_FORWARD : INDICATOR_DIRECT_BACKWARD;

    // the start offset of swiper content zone.
    double contentOffset = (animationDirect_ == INDICATOR_DIRECT_FORWARD) ? (isLoop ? nextItemOffset_ : prevItemOffset_)
                                                                      : (isLoop ? prevItemOffset_ : nextItemOffset_);
    if (needReverse_) {
        animationDirect_ *= INDICATOR_DIRECT_BACKWARD;
    }
    indicatorAnimation_ = AceType::MakeRefPtr<CurveAnimation<double>>(
        CUR_START_TRANSLATE_TIME, CUR_END_TRANSLATE_TIME, curve_);
    indicatorAnimation_->AddListener(
        [weak = AceType::WeakClaim(this), fromIndex, toIndex, contentOffset](const double value) {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->UpdateIndicatorOffset(fromIndex, toIndex, value);
            double itemOffset = (value == CUR_END_TRANSLATE_TIME) ? value : Curves::EASE_OUT->MoveInternal(value);
            swiper->UpdateChildPosition(itemOffset * contentOffset, fromIndex);
        }
    });

    indicatorController_->ClearInterpolators();
    indicatorController_->AddInterpolator(indicatorAnimation_);
    indicatorController_->SetDuration(duration_);
    indicatorController_->ClearStopListeners();
    indicatorController_->AddStopListener([weak = AceType::WeakClaim(this), fromIndex, toIndex]() {
        auto swiper = weak.Upgrade();
        if (swiper) {
            swiper->LoadLazyItems((fromIndex + 1) % swiper->itemCount_ == toIndex);
            swiper->isIndicatorAnimationStart_ = false;
            swiper->outItemIndex_ = fromIndex;
            swiper->currentIndex_ = toIndex;
            swiper->UpdateIndicatorSpringStatus(SpringStatus::FOCUS_SWITCH);
            swiper->MarkNeedLayout();
        }
    });
    indicatorController_->Play();
}

void RenderSwiper::StopIndicatorAnimation()
{
    LOGD("stopZoomAnimation");
    if (indicatorController_ && !indicatorController_->IsStopped()) {
        indicatorController_->ClearStopListeners();
        indicatorController_->Stop();
        isIndicatorAnimationStart_ = false;
    }
}

void RenderSwiper::InitIndicatorAnimation(const WeakPtr<PipelineContext>& context)
{
    if (!springController_) {
        springController_ = AceType::MakeRefPtr<Animator>(context);
    } else {
        StopIndicatorSpringAnimation();
    }
    if (!fadeController_) {
        fadeController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!zoomInController_) {
        zoomInController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!zoomOutController_) {
        zoomOutController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!zoomInDotController_) {
        zoomInDotController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!zoomOutDotController_) {
        zoomOutDotController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!dragRetractionController_) {
        dragRetractionController_ = AceType::MakeRefPtr<Animator>(context);
    }
    if (!indicatorController_) {
        indicatorController_ = AceType::MakeRefPtr<Animator>(context);
    } else {
        StopIndicatorAnimation();
        ResetIndicatorPosition();
    }
    CalMaxStretch();
}

void RenderSwiper::HandleRotationStart()
{
    DragStartInfo info(0);
    info.SetGlobalLocation(Offset(0.0, 0.0));
    HandleDragStart(info);
}

void RenderSwiper::HandleRotationUpdate(double delta)
{
    DragUpdateInfo info(0);
    info.SetMainDelta(delta).SetGlobalLocation(Offset(0.0, 0.0));
    HandleDragUpdate(info);
    if (rotationEvent_) {
        std::string param =
            std::string(R"("rotation",{"value":)").append(std::to_string(delta).append("},null"));
        rotationEvent_(param);
    }
}

void RenderSwiper::HandleRotationEnd()
{
    DragEndInfo info(0);
    HandleDragEnd(info);
    rotationStatus_ = RotationStatus::ROTATION_END;
}

void RenderSwiper::ResetRotationEndListener()
{
    auto&& callback = [weakPtr = AceType::WeakClaim(this)]() {
        auto swiper = weakPtr.Upgrade();
        if (swiper) {
            swiper->HandleRotationEnd();
        } else {
            LOGE("fail to set rotation listener due to swiper weakPtr is nullptr");
        }
    };
    rotationTimer_.Reset(callback);
    auto context = GetContext().Upgrade();
    if (context) {
        auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        taskExecutor.PostDelayedTask(rotationTimer_, ROTATION_INTERVAL_MS);
    }
}

void RenderSwiper::UpdateItemCount(int32_t itemCount)
{
    if ((itemCount_ <= 1) && (itemCount > 1)) {
        Initialize(GetContext(), catchMode_);
    }
    itemCount_ = itemCount;
    if (currentIndex_ >= itemCount_) {
        currentIndex_ = itemCount_ > 0 ? itemCount_ - 1 : 0;
        if (changeEvent_) {
            changeEvent_(std::make_shared<SwiperChangeEvent>(currentIndex_));
        }
    }
}

void RenderSwiper::BuildLazyItems()
{
    if (itemCount_ <= lazyLoadCacheSize_) {
        cacheStart_ = 0;
        cacheEnd_ = itemCount_;
    } else {
        int32_t halfLazy = lazyLoadCacheSize_ / 2;
        if (loop_) {
            cacheStart_ = (itemCount_ + currentIndex_ - halfLazy) % itemCount_;
            cacheEnd_ = (cacheStart_ + lazyLoadCacheSize_ - 1) % itemCount_;
        } else {
            if (currentIndex_ <= halfLazy) {
                cacheStart_ = 0;
                cacheEnd_ = swiper_->GetCachedSize() + swiper_->GetDisplayCount() - 1;
            } else if (currentIndex_ >= itemCount_ - halfLazy - 1) {
                cacheEnd_ = itemCount_ - 1;
                cacheStart_ = cacheEnd_ - swiper_->GetCachedSize() - swiper_->GetDisplayCount();
            } else {
                cacheStart_ = currentIndex_ - halfLazy;
                cacheEnd_ = cacheStart_ + lazyLoadCacheSize_ - 1;
            }
        }
    }
    LOGI("currentIndex_ = %{public}d, init cached: %{public}d - %{public}d", currentIndex_, cacheStart_, cacheEnd_);
    int32_t index = cacheStart_;
    while ((index >= cacheStart_ && cacheStart_ > cacheEnd_) || index <= cacheEnd_) {
        buildChildByIndex_(index++);
        if (loop_) {
            index = index % itemCount_;
        }
    }
}

void RenderSwiper::LoadItems()
{
    if (!items_.empty()) {
        return;
    }
    if ((swiper_ && !swiper_->GetLazyForEachComponent()) || !swiper_) {
        auto children = GetChildren();
        int32_t index = 0;
        for (auto iter = children.begin(); iter != children.end(); ++iter, ++index) {
            items_.emplace(std::make_pair(index, *iter));
        }
    } else {
        BuildLazyItems(); // depend currentIndex_ value which init when swiper first update
    }
}

void RenderSwiper::LoadLazyItems(bool swipeToNext)
{
    if (!buildChildByIndex_) {
        // not lazy foreach case.
        return;
    }
    if (swipeToNext) {
        if (!loop_) {
            buildChildByIndex_(++cacheEnd_);
            if (cacheStart_ + cacheEnd_ >= lazyLoadCacheSize_) {
                deleteChildByIndex_(cacheStart_++);
            }
        } else {
            if (++cacheEnd_ == itemCount_) {
                cacheEnd_ = 0;
            }
            buildChildByIndex_(cacheEnd_);
            deleteChildByIndex_(cacheStart_);
            if (++cacheStart_ == itemCount_) {
                cacheStart_ = 0;
            }
        }
    } else {
        if (!loop_) {
            if (cacheStart_ + cacheEnd_ >= lazyLoadCacheSize_) {
                buildChildByIndex_(--cacheStart_);
            }
            deleteChildByIndex_(cacheEnd_--);
        } else {
            if (--cacheStart_ < 0) {
                cacheStart_ = itemCount_ - 1;
            }
            buildChildByIndex_(cacheStart_);
            deleteChildByIndex_(cacheEnd_);
            if (--cacheEnd_ < 0) {
                cacheEnd_ = itemCount_ - 1;
            }
        }
    }
    LOGI("load lazy cached: %{public}d - %{public}d, current = %{public}d", cacheStart_, cacheEnd_, currentIndex_);
}

void RenderSwiper::AddChildByIndex(int32_t index, const RefPtr<RenderNode>& renderNode)
{
    items_.try_emplace(index, renderNode);
}

void RenderSwiper::RemoveChildByIndex(int32_t index)
{
    auto item = items_.find(index);
    if (item != items_.end()) {
        items_.erase(item);
    }
}

void RenderSwiper::OnDataSourceUpdated(int32_t totalCount, int32_t startIndex)
{
    items_.clear();
    UpdateItemCount(totalCount);
    MarkNeedLayout();
}

void RenderSwiper::ClearItems(const RefPtr<Component>& lazyForEachComponent, int32_t index)
{
    if (lazyForEachComponent) {
        if (index != currentIndex_) {
            decltype(items_) items(std::move(items_));
            for (auto&& item : items) {
                deleteChildByIndex_(item.first);
            }
        }
    } else {
        items_.clear();
    }
}

void RenderSwiper::ResetCachedChildren()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("ResetCachedChildren fail, context is null");
        return;
    }
    if (context->GetIsDeclarative()) {
        return;
    }

    int32_t cachedSize = swiper_->GetCachedSize();
    int32_t childrenSize = itemCount_;
    int32_t forwardNum = 0;
    int32_t backNum = 0;
    if (cachedSize <= -1 || cachedSize >= childrenSize / 2) {
        for (const auto& item : items_) {
            const auto& childItem = item.second;
            if (!childItem) {
                continue;
            }
            childItem->SetHidden(false);
        }
        return;
    } else if (cachedSize == 0) {
        cachedSize = 1;
        forwardNum = cachedSize;
        backNum = cachedSize;
    }
    if (loop_) {
        forwardNum = cachedSize;
        backNum = cachedSize;
    } else {
        if (currentIndex_ == 0) {
            forwardNum = 0;
            if (cachedSize < childrenSize / 2) {
                backNum = 2 * cachedSize;
            }
        } else if (currentIndex_ != 0 && currentIndex_ < cachedSize) {
            forwardNum = currentIndex_;
            backNum = cachedSize + (cachedSize - forwardNum);
        } else if (currentIndex_ != 0 && currentIndex_ >= cachedSize) {
            forwardNum = cachedSize;
            if (cachedSize + currentIndex_ < childrenSize) { // small than size
                backNum = cachedSize;
            } else if (currentIndex_ + cachedSize >= childrenSize) {
                backNum = childrenSize - currentIndex_ - 1;
                int32_t des = cachedSize - backNum;
                forwardNum = des + cachedSize;
            }
        }
    }
    SetSwiperHidden(forwardNum, backNum);
}

void RenderSwiper::SetSwiperHidden(int32_t forwardNum, int32_t backNum)
{
    int32_t childrenSize = itemCount_;
    int32_t fromIndex = currentIndex_ - forwardNum;
    int32_t toIndex = currentIndex_ + backNum;
    if (fromIndex >= 0 && toIndex < childrenSize) { // normal
        for (const auto& item : items_) {
            const auto& childItem = item.second;
            if (childItem) {
                childItem->SetHidden(item.first >= fromIndex && item.first <= toIndex);
            }
        }
    }
    if (loop_) {
        if (fromIndex < 0) {
            for (const auto& item : items_) {
                const auto& childItem = item.second;
                if (childItem) {
                    childItem->SetHidden(item.first > toIndex && item.first < fromIndex + childrenSize);
                }
            }
        } else if (toIndex >= childrenSize) {
            for (const auto& item : items_) {
                const auto& childItem = item.second;
                if (childItem) {
                    childItem->SetHidden(item.first > toIndex - childrenSize && item.first < fromIndex);
                }
            }
        }
    }
}

bool RenderSwiper::IsChildrenTouchEnable()
{
    return !(controller_ && controller_->IsRunning());
}

void RenderSwiper::OnPaintFinish()
{
    if (!AceApplicationInfo::GetInstance().IsAccessibilityEnabled()) {
        return;
    }

    Rect itemRect;
    Rect viewPortRect(GetGlobalOffset(), GetChildViewPort());
    for (const auto& item : GetChildren()) {
        auto node = item->GetAccessibilityNode().Upgrade();
        if (!node) {
            continue;
        }
        bool visible = GetVisible();
        if (visible) {
            itemRect.SetSize(item->GetLayoutSize());
            itemRect.SetOffset(item->GetGlobalOffset());
            visible = itemRect.IsIntersectWith(viewPortRect);
        }
        item->SetAccessibilityVisible(visible);
        if (visible) {
            Rect clampRect = itemRect.Constrain(viewPortRect);
            if (clampRect != itemRect) {
                item->SetAccessibilityRect(clampRect);
            }
        } else {
            item->NotifyPaintFinish();
        }
    }
}

} // namespace OHOS::Ace
