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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SLIDE_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SLIDE_RECOGNIZER_H

#include <cmath>
#include <functional>
#include <map>

#include "core/gestures/multi_fingers_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class SlideRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(SlideRecognizer, MultiFingersRecognizer);

public:
    SlideRecognizer(
        const WeakPtr<PipelineContext>& context, int32_t fingers,
        const SwipeDirection& direction, double speed)
        : direction_(direction), speed_(speed), context_(context)
    {
        fingers_ = fingers;
        newSpeed_ = speed_;
        newFingers_ = fingers_;
        newDirection_ = direction_;
    }

    void OnAccepted() override;
    void OnRejected() override;
    ~SlideRecognizer() override
    {
    }

private:
    enum class GestureAcceptResult {
        ACCEPT,
        REJECT,
        DETECTING,
    };
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;
    GestureAcceptResult ParseFingersOffset() const;
    void Reset();
    void SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback);
    void ChangeFingers(int32_t fingers);
    void ChangeDirection(const SwipeDirection& direction);
    void ChangeSpeed(double speed);
    double ComputeAngle();

    const TouchRestrict& GetTouchRestrict() const
    {
        return touchRestrict_;
    }

    SwipeDirection direction_;
    double speed_ = 0.0;
    WeakPtr<PipelineContext> context_;
    std::map<int32_t, TouchEvent> touchPoints_;
    std::map<int32_t, Offset> fingersDistance_;
    TimeStamp time_;
    TimeStamp touchDownTime_;
    bool slidingEnd_ = false;
    bool slidingCancel_ = false;
    Point globalPoint_;
    OnSwipeFingersFunc onChangeFingers_;
    OnSwipeDirectionFunc onChangeDirection_;
    OnSwipeSpeedFunc onChangeSpeed_;
    int32_t newFingers_ = 1;

    double angle_ = 0.0;
    double initialAngle_ = 0.0;
    double currentAngle_ = 0.0;
    double resultAngle_ = 0.0;
    double resultSpeed_ = 0.0;

    double newSpeed_ = 0.0;
    SwipeDirection newDirection_;
};

} // namespace OHOS::Ace

#endif
