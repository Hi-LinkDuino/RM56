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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_TRACKER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_TRACKER_H

#include "base/geometry/axis.h"
#include "base/geometry/least_square_impl.h"
#include "base/geometry/offset.h"
#include "core/event/touch_event.h"
#include "core/gestures/velocity.h"

namespace OHOS::Ace {

class VelocityTracker final {
public:
    VelocityTracker() = default;
    explicit VelocityTracker(Axis mainAxis) : mainAxis_(mainAxis) {}
    ~VelocityTracker() = default;

    void Reset()
    {
        lastPosition_.Reset();
        velocity_.Reset();
        delta_.Reset();
        isFirstPoint_ = true;
        xAxis_.Reset();
        yAxis_.Reset();
    }

    void UpdateTouchPoint(const TouchEvent& event, bool end = false);

    const TouchEvent& GetFirstTrackPoint() const
    {
        return firstTrackPoint_;
    }

    const TouchEvent& GetCurrentTrackPoint() const
    {
        return currentTrackPoint_;
    }

    const Offset& GetPosition() const
    {
        return lastPosition_;
    }

    const Offset& GetDelta() const
    {
        return delta_;
    }

    const Velocity& GetVelocity()
    {
        UpdateVelocity();
        return velocity_;
    }

    double GetMainAxisPos() const
    {
        switch (mainAxis_) {
            case Axis::FREE:
                return lastPosition_.GetDistance();
            case Axis::HORIZONTAL:
                return lastPosition_.GetX();
            case Axis::VERTICAL:
                return lastPosition_.GetY();
            default:
                return 0.0;
        }
    }

    double GetMainAxisDeltaPos() const
    {
        switch (mainAxis_) {
            case Axis::FREE:
                return delta_.GetDistance();
            case Axis::HORIZONTAL:
                return delta_.GetX();
            case Axis::VERTICAL:
                return delta_.GetY();
            default:
                return 0.0;
        }
    }

    double GetMainAxisVelocity()
    {
        UpdateVelocity();
        switch (mainAxis_) {
            case Axis::FREE:
                return velocity_.GetVelocityValue();
            case Axis::HORIZONTAL:
                return velocity_.GetVelocityX();
            case Axis::VERTICAL:
                return velocity_.GetVelocityY();
            default:
                return 0.0;
        }
    }

private:
    void UpdateVelocity();

    Axis mainAxis_ { Axis::FREE };
    TouchEvent firstTrackPoint_;
    TouchEvent currentTrackPoint_;
    Offset lastPosition_;
    Velocity velocity_;
    Offset delta_;
    Offset offset_;
    bool isFirstPoint_ = true;
    TimeStamp lastTimePoint_;
    LeastSquareImpl xAxis_ { 3, 5 };
    LeastSquareImpl yAxis_ { 3, 5 };
    bool isVelocityDone_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_VELOCITY_TRACKER_H
