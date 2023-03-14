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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PINCH_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PINCH_RECOGNIZER_H

#include <cmath>
#include <functional>
#include <map>

#include "core/gestures/multi_fingers_recognizer.h"

namespace OHOS::Ace {

class PinchRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(PinchRecognizer, MultiFingersRecognizer);

public:
    PinchRecognizer(int32_t fingers, double distance) : distance_(distance)
    {
        fingers_ = fingers;
    }
    ~PinchRecognizer() override = default;

    void OnAccepted() override;
    void OnRejected() override;

private:
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;
    double ComputeAverageDeviation();
    void Reset();
    void SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback);
    Offset ComputePinchCenter();

    double distance_ = 0.0;
    double initialDev_ = 0.0;
    double currentDev_ = 0.0;
    double scale_ = 1.0;
    Offset pinchCenter_;
    TimeStamp time_;
    std::map<int32_t, TouchEvent> touchPoints_;
    bool pendingEnd_ = false;
    bool pendingCancel_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PINCH_RECOGNIZER_H
