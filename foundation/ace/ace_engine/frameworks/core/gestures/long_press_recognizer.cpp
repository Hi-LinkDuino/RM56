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

#include "core/gestures/long_press_recognizer.h"

#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {
namespace {

constexpr double MAX_THRESHOLD = 15.0;
constexpr int32_t MAX_FINGERS = 10;
} // namespace

void LongPressRecognizer::OnAccepted()
{
    LOGI("long press gesture has been accepted!");

    if (onLongPress_ && !touchMap_.empty()) {
        TouchEvent trackPoint = touchMap_.begin()->second;
        LongPressInfo info(trackPoint.id);
        info.SetTimeStamp(time_);
        info.SetScreenLocation(trackPoint.GetScreenOffset());
        info.SetGlobalLocation(trackPoint.GetOffset()).SetLocalLocation(trackPoint.GetOffset() - coordinateOffset_);
        onLongPress_(info);
    }

    SetFingerList(touchMap_, coordinateOffset_, fingerList_);
    SendCallbackMsg(onActionUpdate_, false);
    SendCallbackMsg(onAction_, false);
    if (repeat_) {
        StartRepeatTimer();
    }

    if (pendingEnd_) {
        SendCallbackMsg(onActionEnd_, false);
        Reset();
    } else if (pendingCancel_) {
        SendCancelMsg();
        Reset();
    }
}

void LongPressRecognizer::OnRejected()
{
    LOGI("long press gesture has been rejected!");
    Reset();
}

void LongPressRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    if (fingers_ > MAX_FINGERS) {
        return;
    }
    LOGD("long press recognizer receives touch down event, begin to detect long press event");
    if ((touchRestrict_.forbiddenType & TouchRestrict::LONG_PRESS) == TouchRestrict::LONG_PRESS) {
        LOGI("the long press is forbidden");
        return;
    }
    if (state_ == DetectState::READY) {
        touchMap_[event.id] = event;
        pointsCount_++;
        AddToReferee(event.id, AceType::Claim(this));
        if (pointsCount_ == fingers_) {
            state_ = DetectState::DETECTING;
            DeadlineTimer(duration_);
        }
    } else {
        LOGW("the state is not ready for detecting long press gesture, state id %{public}d, id is %{public}d",
            state_, event.id);
    }
}

void LongPressRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    LOGD("long press recognizer receives touch up event");
    auto it = touchMap_.find(event.id);
    if (it == touchMap_.end()) {
        return;
    }

    if (state_ != DetectState::DETECTED) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionUpdate_, false);
        SendCallbackMsg(onActionEnd_, false);
        Reset();
    } else {
        pendingEnd_ = true;
    }
}

void LongPressRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("long press recognizer receives touch move event");
    auto it = touchMap_.find(event.id);
    if (it == touchMap_.end()) {
        return;
    }

    Offset offset = event.GetOffset() - touchMap_[event.id].GetOffset();
    if (offset.GetDistance() > MAX_THRESHOLD) {
        if (state_ == DetectState::DETECTED && refereeState_ == RefereeState::SUCCEED) {
            SendCallbackMsg(onActionEnd_, false);
            Reset();
            return;
        }

        LOGD("this gesture is not long press, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    time_ = event.time;
}

void LongPressRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("long press recognizer receives touch cancel event");
    if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
        LOGD("cancel long press gesture detect, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        SendCancelMsg();
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCancelMsg();
        Reset();
    } else {
        pendingCancel_ = true;
    }
}

void LongPressRecognizer::HandleOverdueDeadline()
{
    if (state_ == DetectState::DETECTING) {
        LOGI("this gesture is long press, try to accept it");
        state_ = DetectState::DETECTED;
        Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        SendCallbackMsg(onActionUpdate_, false);
    } else {
        LOGW("the state is not detecting for accept long press gesture");
    }
}

void LongPressRecognizer::DeadlineTimer(int32_t time)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGI("fail to detect long press gesture due to context is nullptr");
        return;
    }
    auto&& callback = [weakPtr = AceType::WeakClaim(this)]() {
        auto refPtr = weakPtr.Upgrade();
        if (refPtr) {
            refPtr->HandleOverdueDeadline();
        } else {
            LOGI("fail to handle overdue deadline due to context is nullptr");
        }
    };
    deadlineTimer_.Reset(callback);
    auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    taskExecutor.PostDelayedTask(deadlineTimer_, time);
}

void LongPressRecognizer::DoRepeat()
{
    SendCallbackMsg(onAction_, true);
    StartRepeatTimer();
}

void LongPressRecognizer::StartRepeatTimer()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("fail to detect long press gesture due to context is nullptr");
        return;
    }
    auto&& callback = [weakPtr = AceType::WeakClaim(this)]() {
        auto refPtr = weakPtr.Upgrade();
        if (refPtr) {
            refPtr->DoRepeat();
        } else {
            LOGW("fail to handle overdue deadline due to context is nullptr");
        }
    };
    timer_.Reset(callback);
    auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    taskExecutor.PostDelayedTask(timer_, duration_);
}

double LongPressRecognizer::ConvertPxToVp(double offset) const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to detect tap gesture due to context is nullptr");
        return offset;
    }
    double vpOffset = context->ConvertPxToVp(Dimension(offset, DimensionUnit::PX));
    return vpOffset;
}

void LongPressRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback, bool isRepeat)
{
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetRepeat(isRepeat);
        info.SetFingerList(fingerList_);
        TouchEvent trackPoint = {};
        if (!touchMap_.empty()) {
            trackPoint = touchMap_.begin()->second;
        }
        info.SetSourceDevice(deviceType_);
        info.SetDeviceId(deviceId_);
        info.SetScreenLocation(trackPoint.GetScreenOffset());
        info.SetGlobalLocation(trackPoint.GetOffset()).SetLocalLocation(trackPoint.GetOffset() - coordinateOffset_);
        (*callback)(info);
    }
}

void LongPressRecognizer::Reset()
{
    touchMap_.clear();
    fingerList_.clear();
    pointsCount_ = 0;
    timer_.Cancel();
    deadlineTimer_.Cancel();
    state_ = DetectState::READY;
    pendingEnd_ = false;
    pendingCancel_ = false;
}

bool LongPressRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<LongPressRecognizer> curr = AceType::DynamicCast<LongPressRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->duration_ != duration_ || curr->fingers_ != fingers_ || curr->repeat_ != repeat_ ||
        curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    context_ = curr->context_;
    onAction_ = std::move(curr->onAction_);
    onActionEnd_ = std::move(curr->onActionEnd_);
    onActionCancel_ = std::move(curr->onActionCancel_);

    return true;
}
} // namespace OHOS::Ace
