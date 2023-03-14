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

#include "core/gestures/pinch_recognizer.h"

#include "base/geometry/offset.h"
#include "base/log/log.h"
#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t MAX_PINCH_FINGERS = 5;

} // namespace

void PinchRecognizer::OnAccepted()
{
    SendCallbackMsg(onActionStart_);

    if (pendingEnd_) {
        SendCallbackMsg(onActionEnd_);
        Reset();
    } else if (pendingCancel_) {
        SendCancelMsg();
        Reset();
    }
}

void PinchRecognizer::OnRejected()
{
    LOGD("pinch gesture has been rejected!");
    Reset();
}

void PinchRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    LOGD("pinch recognizer receives touch down event, begin to detect pinch event");
    if (fingers_ > MAX_PINCH_FINGERS) {
        return;
    }

    if (state_ == DetectState::READY) {
        AddToReferee(event.id, AceType::Claim(this));
        touchPoints_[event.id] = event;
        if (static_cast<int32_t>(touchPoints_.size()) == fingers_) {
            initialDev_ = ComputeAverageDeviation();
            pinchCenter_ = ComputePinchCenter();
            state_ = DetectState::DETECTING;
        }
    }
}

void PinchRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    LOGD("pinch recognizer receives touch up event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    touchPoints_.erase(itr);

    if (state_ != DetectState::DETECTED) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionEnd_);
        Reset();
    } else {
        pendingEnd_ = true;
    }
}

void PinchRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("pinch recognizer receives touch move event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    if (state_ == DetectState::READY) {
        touchPoints_[event.id] = event;
        return;
    }

    touchPoints_[event.id] = event;
    currentDev_ = ComputeAverageDeviation();
    time_ = event.time;

    if (state_ == DetectState::DETECTING) {
        if (GreatOrEqual(fabs(currentDev_ - initialDev_), distance_)) {
            scale_ = currentDev_ / initialDev_;
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        }
    } else if (state_ == DetectState::DETECTED && refereeState_ == RefereeState::SUCCEED) {
        scale_ = currentDev_ / initialDev_;
        SendCallbackMsg(onActionUpdate_);
    }
}

void PinchRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("pinch recognizer receives touch cancel event");
    if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
        LOGD("cancel pinch gesture detect, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCancelMsg();
        Reset();
    } else {
        pendingCancel_ = true;
    }
}

double PinchRecognizer::ComputeAverageDeviation()
{
    // compute the coordinate of focal point
    double sumOfX = 0.0;
    double sumOfY = 0.0;
    for (auto& element : touchPoints_) {
        sumOfX = sumOfX + element.second.x;
        sumOfY = sumOfY + element.second.y;
    }
    double focalX = sumOfX / touchPoints_.size();
    double focalY = sumOfY / touchPoints_.size();

    // compute average deviation
    double devX = 0.0;
    double devY = 0.0;
    for (auto& element : touchPoints_) {
        devX = devX + fabs(element.second.x - focalX);
        devY = devY + fabs(element.second.y - focalY);
    }
    double aveDevX = devX / touchPoints_.size();
    double aveDevY = devY / touchPoints_.size();

    // compute zoom distance
    double zoomDistance = sqrt(pow(aveDevX, 2) + pow(aveDevY, 2));

    return zoomDistance;
}

Offset PinchRecognizer::ComputePinchCenter()
{
    double sumOfX = 0.0;
    double sumOfY = 0.0;
    for (auto& element : touchPoints_) {
        sumOfX = sumOfX + element.second.x;
        sumOfY = sumOfY + element.second.y;
    }
    double focalX = sumOfX / touchPoints_.size();
    double focalY = sumOfY / touchPoints_.size();

    Offset pinchCenter = Offset(focalX, focalY);

    return pinchCenter;
}

void PinchRecognizer::Reset()
{
    touchPoints_.clear();
    state_ = DetectState::READY;
    pendingEnd_ = false;
    pendingCancel_ = false;
}

void PinchRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback)
{
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetScale(scale_);
        info.SetPinchCenter(pinchCenter_);
        info.SetDeviceId(deviceId_);
        info.SetSourceDevice(deviceType_);
        (*callback)(info);
    }
}

bool PinchRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<PinchRecognizer> curr = AceType::DynamicCast<PinchRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->fingers_ != fingers_ || curr->distance_ != distance_ || curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    onActionStart_ = std::move(curr->onActionStart_);
    onActionUpdate_ = std::move(curr->onActionUpdate_);
    onActionEnd_ = std::move(curr->onActionEnd_);
    onActionCancel_ = std::move(curr->onActionCancel_);

    return true;
}

} // namespace OHOS::Ace
