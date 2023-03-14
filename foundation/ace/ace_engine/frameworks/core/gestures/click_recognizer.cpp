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

#include "core/gestures/click_recognizer.h"

#include "base/geometry/offset.h"
#include "base/log/log.h"
#include "base/ressched/ressched_report.h"

namespace OHOS::Ace {
namespace {

int32_t MULTI_FINGER_TIMEOUT = 300;
constexpr int32_t MULTI_FINGER_TIMEOUT_TOUCH = 300;
constexpr int32_t MULTI_FINGER_TIMEOUT_MOUSE = 300;
int32_t MULTI_TAP_TIMEOUT = 300;
constexpr int32_t MULTI_TAP_TIMEOUT_TOUCH = 300;
constexpr int32_t MULTI_TAP_TIMEOUT_MOUSE = 500;
int32_t MULTI_TAP_SLOP = 100;
constexpr int32_t MULTI_TAP_SLOP_TOUCH = 100;
constexpr int32_t MULTI_TAP_SLOP_MOUSE = 15;
#ifndef WEARABLE_PRODUCT
double MAX_THRESHOLD = 20.0;
constexpr double MAX_THRESHOLD_TOUCH = 20.0;
#else
constexpr double MAX_THRESHOLD = 12.0;
constexpr double MAX_THRESHOLD_TOUCH = 12.0;
#endif
constexpr int32_t MAX_THRESHOLD_MOUSE = 15;
constexpr int32_t MAX_TAP_FINGERS = 10;

} // namespace

void ClickRecognizer::InitGlobalValue(SourceType sourceType)
{
    switch (sourceType) {
        case SourceType::TOUCH:
            MULTI_FINGER_TIMEOUT = MULTI_FINGER_TIMEOUT_TOUCH;
            MULTI_TAP_TIMEOUT = MULTI_TAP_TIMEOUT_TOUCH;
            MULTI_TAP_SLOP = MULTI_TAP_SLOP_TOUCH;
            MAX_THRESHOLD = MAX_THRESHOLD_TOUCH;
            break;
        case SourceType::MOUSE:
        case SourceType::TOUCH_PAD:
            MULTI_FINGER_TIMEOUT = MULTI_FINGER_TIMEOUT_MOUSE;
            MULTI_TAP_TIMEOUT = MULTI_TAP_TIMEOUT_MOUSE;
            MULTI_TAP_SLOP = MULTI_TAP_SLOP_MOUSE;
            MAX_THRESHOLD = MAX_THRESHOLD_MOUSE;
            break;
        default:
            LOGI("Unrecognized input source type: %{public}d", sourceType);
    }
}

void ClickRecognizer::OnAccepted()
{
    LOGI("Click gesture has been accepted!");
    ResSchedReport::GetInstance().ResSchedDataReport("click");
    if (onClick_) {
        TouchEvent touchPoint = {};
        if (!touchPoints_.empty()) {
            touchPoint = touchPoints_.begin()->second;
        }

        ClickInfo info(touchPoint.id);
        info.SetTimeStamp(touchPoint.time);
        info.SetGlobalLocation(touchPoint.GetOffset()).SetLocalLocation(touchPoint.GetOffset() - coordinateOffset_);
        info.SetSourceDevice(deviceType_);
        info.SetDeviceId(deviceId_);
        onClick_(info);
    }

    if (remoteMessage_) {
        TouchEvent touchPoint = {};
        if (!touchPoints_.empty()) {
            touchPoint = touchPoints_.begin()->second;
        }

        ClickInfo info(touchPoint.id);
        info.SetTimeStamp(touchPoint.time);
        info.SetGlobalLocation(touchPoint.GetOffset()).SetLocalLocation(touchPoint.GetOffset() - coordinateOffset_);
        remoteMessage_(info);
    }
    SetFingerList(touchPoints_, coordinateOffset_, fingerList_);
    SendCallbackMsg(onAction_);
    Reset();
}

void ClickRecognizer::OnRejected()
{
    LOGD("click gesture has been rejected!");
    Reset();
}

void ClickRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    InitGlobalValue(event.sourceType);
    LOGD("click recognizer receives touch down event, begin to detect click event");
    if (fingers_ > MAX_TAP_FINGERS) {
        return;
    }

    // If number of fingers which put on the screen more than fingers_,
    // the gesture will be rejected when one of them touch up.
    if (pointsCount_ == fingers_) {
        pointsCount_++;
        LOGE("number of fingers put on the screen more than %{public}d, current fingers is %{public}d",
            fingers_, pointsCount_);
        return;
    }

    if (state_ == DetectState::READY) {
        if (tappedCount_ > 0 && pointsCount_ == 0) {
            tapDeadlineTimer_.Cancel();
        }
        AddToReferee(event.id, AceType::Claim(this));
        touchPoints_[event.id] = event;
        pointsCount_++;
        if (fingers_ > 1 && pointsCount_ == 1) {
            // waiting for multi-finger press
            DeadlineTimer(fingerDeadlineTimer_, MULTI_FINGER_TIMEOUT);
        }
    } else {
        LOGE("the state of click recognizer is not ready to receive touch down event, "
             "state is %{public}d, id is %{public}d",
            state_, event.id);
    }

    if (pointsCount_ == fingers_) {
        // Turn off the multi-finger press deadline timer
        fingerDeadlineTimer_.Cancel();
        state_ = DetectState::DETECTING;
        equalsToFingers_ = true;
        if (ExceedSlop()) {
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        }
    }
}

void ClickRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    InitGlobalValue(event.sourceType);
    LOGD("click recognizer receives touch up event");
    if (pointsCount_ > fingers_) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    if (state_ == DetectState::READY) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (state_ == DetectState::DETECTED) {
        return;
    }

    touchPoints_[event.id] = event;
    pointsCount_--;

    // Check whether multi-finger taps are completed in count_ times
    if (pointsCount_ == 0 && equalsToFingers_) {
        // Turn off the multi-finger lift deadline timer
        fingerDeadlineTimer_.Cancel();
        focusPoint_ = ComputeFocusPoint();
        tappedCount_++;

        if (tappedCount_ == count_) {
            LOGI("this gesture is click, try to accept it");
            time_ = event.time;
            state_ = DetectState::DETECTED;
            if (useCatchMode_) {
                Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
            } else {
                OnAccepted();
            }
            return;
        }
        equalsToFingers_ = false;
        state_ = DetectState::READY;
        // waiting for multi-finger lift
        DeadlineTimer(tapDeadlineTimer_, MULTI_TAP_TIMEOUT);
    }

    Adjudicate(AceType::Claim(this), GestureDisposal::PENDING);
    if (pointsCount_ < fingers_ && equalsToFingers_) {
        DeadlineTimer(fingerDeadlineTimer_, MULTI_FINGER_TIMEOUT);
    }
}

void ClickRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    InitGlobalValue(event.sourceType);
    LOGD("click recognizer receives touch move event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    Offset offset = event.GetOffset() - touchPoints_[event.id].GetOffset();
    if (offset.GetDistance() > MAX_THRESHOLD) {
        LOGD("this gesture is not click, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
    }
}

void ClickRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    InitGlobalValue(event.sourceType);
    LOGD("click recognizer receives touch cancel event");
    Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
}

void ClickRecognizer::HandleOverdueDeadline()
{
    if (pointsCount_ < fingers_) {
        LOGD("the state is not detecting for accept multi-finger tap gesture");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
    }
}

void ClickRecognizer::DeadlineTimer(CancelableCallback<void()>& deadlineTimer, int32_t time)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to detect tap gesture due to context is nullptr");
        return;
    }
    auto&& callback = [weakPtr = AceType::WeakClaim(this)]() {
        auto refPtr = weakPtr.Upgrade();
        if (refPtr) {
            refPtr->HandleOverdueDeadline();
        } else {
            LOGE("fail to handle overdue deadline due to context is nullptr");
        }
    };

    deadlineTimer.Reset(callback);
    auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    taskExecutor.PostDelayedTask(deadlineTimer, time);
}

void ClickRecognizer::Reset()
{
    touchPoints_.clear();
    fingerList_.clear();
    pointsCount_ = 0;
    equalsToFingers_ = false;
    tappedCount_ = 0;
    state_ = DetectState::READY;
}

Offset ClickRecognizer::ComputeFocusPoint()
{
    Offset sumOfPoints;
    for (auto& element : touchPoints_) {
        sumOfPoints = sumOfPoints + element.second.GetOffset();
    }
    Offset focusPoint = sumOfPoints / touchPoints_.size();
    return focusPoint;
}

bool ClickRecognizer::ExceedSlop()
{
    if (tappedCount_ > 0 && tappedCount_ < count_) {
        Offset currentFocusPoint = ComputeFocusPoint();
        Offset slop = currentFocusPoint - focusPoint_;
        if (GreatOrEqual(slop.GetDistance(), MULTI_TAP_SLOP)) {
            return true;
        }
    }
    return false;
}

void ClickRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& onAction)
{
    if (onAction && *onAction) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetFingerList(fingerList_);
        TouchEvent touchPoint = {};
        if (!touchPoints_.empty()) {
            touchPoint = touchPoints_.begin()->second;
        }
        info.SetGlobalLocation(touchPoint.GetOffset()).SetLocalLocation(touchPoint.GetOffset() - coordinateOffset_);
        info.SetSourceDevice(deviceType_);
        info.SetDeviceId(deviceId_);
        (*onAction)(info);
    }
}

bool ClickRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<ClickRecognizer> curr = AceType::DynamicCast<ClickRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->count_ != count_ || curr->fingers_ != fingers_ || curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    context_ = curr->context_;
    onAction_ = std::move(curr->onAction_);

    return true;
}

} // namespace OHOS::Ace
