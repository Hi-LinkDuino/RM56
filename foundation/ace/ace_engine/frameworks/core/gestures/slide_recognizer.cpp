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

#include "core/gestures/slide_recognizer.h"

#include "base/geometry/offset.h"
#include "base/log/log.h"
#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t MAX_SLIDE_FINGERS = 10;

double ChangeValueRange(double value)
{
    double result = 0.0;
    if (LessOrEqual(value, -180.0)) {
        result = value + 360.0;
    } else if (GreatNotEqual(value, 180.0)) {
        result = value - 360.0;
    } else {
        result = value;
    }

    return result;
}

} // namespace

void SlideRecognizer::OnAccepted()
{
    if (slidingEnd_) {
        LOGD("slide gesture recognizer has sliding end event when waiting to be accepted");
        Reset();
    } else if (slidingCancel_) {
        SendCancelMsg();
        Reset();
    }
}

void SlideRecognizer::OnRejected()
{
    LOGD("slide gesture has been rejected!");
    Reset();
}

void SlideRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    LOGD("slide recognizer receives touch down event, begin to detect slide event");
    fingers_ = newFingers_;
    speed_ = newSpeed_;
    direction_ = newDirection_;

    if (fingers_ > MAX_SLIDE_FINGERS) {
        return;
    }

    if (direction_.type == SwipeDirection::NONE) {
        return;
    }

    touchPoints_[event.id] = event;
    fingersDistance_[event.id] = Offset();
    touchDownTime_ = event.time;

    if (state_ == DetectState::READY) {
        AddToReferee(event.id, AceType::Claim(this));
        if (static_cast<int32_t>(refereePointers_.size()) == fingers_) {
            initialAngle_ = ComputeAngle();
            state_ = DetectState::DETECTING;
        }
    }
}

void SlideRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }
    auto itf = fingersDistance_.find(event.id);
    if (itf == fingersDistance_.end()) {
        return;
    }

    globalPoint_ = Point(event.x, event.y);
    touchPoints_.erase(itr);
    auto distanceData = fingersDistance_;
    fingersDistance_.erase(itf);
    if (state_ == DetectState::READY) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (state_ == DetectState::DETECTING) {
        size_t inRefereeNum = refereePointers_.size();
        bool inReferee = IsInReferee(static_cast<size_t>(event.id));
        if (inReferee) {
            inRefereeNum--;
        }

        if (static_cast<int32_t>(touchPoints_.size()) < fingers_ || inRefereeNum < 1) {
            LOGD("this gesture is not slide, try to reject it");
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
            return;
        }

        if (inReferee) {
            DelFromReferee(event.id, AceType::Claim(this));
        }
        return;
    }

    if (static_cast<int32_t>(touchPoints_.size()) <= fingers_) {
        if (refereeState_ == RefereeState::SUCCEED) {
            double averageSpeed = 0.0;
            bool isAvalible = true;
            for (const auto& element : distanceData) {
                Offset offset = element.second;
                double distance = 0.0;
                if ((direction_.type & SwipeDirection::ALL) == SwipeDirection::ALL) {
                    distance = offset.GetDistance();
                } else {
                    if ((direction_.type & SwipeDirection::HORIZONTAL) != 0) {
                        distance = std::abs(offset.GetX());
                    } else if ((direction_.type & SwipeDirection::VERTICAL) != 0) {
                        distance = std::abs(offset.GetY());
                    }
                }
                auto slidingTime = event.time - touchDownTime_;
                auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double,
                                    std::ratio<1, 1000>>>(slidingTime);
                double slidingSpeed = (distance / duration_ms.count()) * 1000;
                if (speed_ >= slidingSpeed) {
                    isAvalible = false;
                    break;
                }
                averageSpeed += slidingSpeed;
            }
            if (isAvalible) {
                resultSpeed_ = averageSpeed / distanceData.size();
                SendCallbackMsg(onAction_);
            }
            Reset();
        } else {
            slidingEnd_ = true;
        }
    }
}

void SlideRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("slide recognizer receives touch move event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }
    auto itf = fingersDistance_.find(event.id);
    if (itf == fingersDistance_.end()) {
        return;
    }
    globalPoint_ = Point(event.x, event.y);
    if (state_ == DetectState::READY) {
        touchPoints_[event.id] = event;
        return;
    }

    Offset moveDistance = event.GetOffset() - touchPoints_[event.id].GetOffset();
    fingersDistance_[event.id] = itf->second + moveDistance;
    touchPoints_[event.id] = event;
    currentAngle_ = ComputeAngle();
    time_ = event.time;

    if (state_ == DetectState::DETECTING) {
        double diffAngle = fabs((currentAngle_ - initialAngle_));
        if (GreatOrEqual(diffAngle, angle_)) {
            resultAngle_ = ChangeValueRange(currentAngle_ - initialAngle_);
        }
        auto result = ParseFingersOffset();
        if (result == GestureAcceptResult::ACCEPT) {
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        } else if (result == GestureAcceptResult::REJECT) {
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        }
    } else if (state_ == DetectState::DETECTED && refereeState_ == RefereeState::SUCCEED) {
        resultAngle_ = ChangeValueRange(currentAngle_ - initialAngle_);
    }
}

void SlideRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("slide recognizer receives touch cancel event");
    if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
        LOGD("cancel slide gesture detect, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCancelMsg();
        Reset();
    } else {
        slidingCancel_ = true;
    }
}

SlideRecognizer::GestureAcceptResult SlideRecognizer::ParseFingersOffset() const
{
    if ((direction_.type & SwipeDirection::ALL) == SwipeDirection::ALL) {
        for (const auto& element : fingersDistance_) {
            Offset offset = element.second;
            double distance = offset.GetDistance();
            if (fabs(distance) < DEFAULT_SLIDE_DISTANCE) {
                return GestureAcceptResult::DETECTING;
            }
        }
        return GestureAcceptResult::ACCEPT;
    }

    for (const auto& element : fingersDistance_) {
        Offset offset = element.second;
        if (fabs(offset.GetX()) > fabs(offset.GetY())) {
            if ((direction_.type & SwipeDirection::HORIZONTAL) != 0) {
                double offsetX = offset.GetX();
                if (fabs(offsetX) < DEFAULT_SLIDE_DISTANCE) {
                    return GestureAcceptResult::DETECTING;
                }
            } else {
                return GestureAcceptResult::REJECT;
            }
        } else {
            if ((direction_.type & SwipeDirection::VERTICAL) != 0) {
                double offsetY = offset.GetY();
                if (fabs(offsetY) < DEFAULT_SLIDE_DISTANCE) {
                    return GestureAcceptResult::DETECTING;
                }
            } else {
                return GestureAcceptResult::REJECT;
            }
        }
    }

    return GestureAcceptResult::ACCEPT;
}

void SlideRecognizer::Reset()
{
    touchPoints_.clear();
    fingersDistance_.clear();
    resultSpeed_ = 0.0;
    state_ = DetectState::READY;
    slidingEnd_ = false;
    slidingCancel_ = false;
}

void SlideRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback)
{
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetGlobalPoint(globalPoint_);
        info.SetAngle(resultAngle_);
        info.SetSpeed(resultSpeed_);
        info.SetSourceDevice(deviceType_);
        info.SetDeviceId(deviceId_);
        (*callback)(info);
    }
}

bool SlideRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<SlideRecognizer> curr = AceType::DynamicCast<SlideRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->fingers_ != fingers_ || !NearEqual(curr->angle_, angle_) ||
        curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    direction_.type = curr->direction_.type;
    newDirection_.type = curr->newDirection_.type;
    speed_ = curr->speed_;
    newSpeed_ = curr->newSpeed_;

    onAction_ = std::move(curr->onAction_);

    return true;
}

void SlideRecognizer::ChangeFingers(int32_t fingers)
{
    if (fingers_ != fingers) {
        newFingers_ = fingers;
    }
}

void SlideRecognizer::ChangeDirection(const SwipeDirection& direction)
{
    if (direction_.type != direction.type) {
        direction_.type = direction.type;
        newDirection_.type = direction.type;
    }
}

void SlideRecognizer::ChangeSpeed(double speed)
{
    if (speed_ != speed) {
        if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
            speed_ = speed;
        }
        newSpeed_ = speed;
    }
}

double SlideRecognizer::ComputeAngle()
{
    double fx = touchPoints_[0].x;
    double fy = touchPoints_[0].y;
    double sx = touchPoints_[1].x;
    double sy = touchPoints_[1].y;
    return atan2(fy - sy, fx - sx) * 180.0 / M_PI;
}

} // namespace OHOS::Ace
