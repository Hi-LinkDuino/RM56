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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_RECOGNIZER_H

#include <cmath>
#include <functional>
#include <map>

#include "core/gestures/multi_fingers_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class PanRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(PanRecognizer, MultiFingersRecognizer);

public:
    PanRecognizer(
        const WeakPtr<PipelineContext>& context, int32_t fingers, const PanDirection& direction, double distance)
        : direction_(direction), distance_(distance), context_(context)
    {
        fingers_ = fingers;
        newFingers_ = fingers_;
        newDistance_ = distance_;
        newDirection_ = direction_;
    }
    PanRecognizer(
        const WeakPtr<PipelineContext>& context, RefPtr<PanGestureOption> panGestureOption)
        : panGestureOption_(panGestureOption)
    {
        auto newContext = context.Upgrade();
        if (!newContext) {
            LOGE("fail to create pan recognizer due to context is nullptr");
            return;
        }
        uint32_t directNum = panGestureOption->GetDirection().type;
        double distanceNumber = panGestureOption->GetDistance();
        int32_t fingersNumber = panGestureOption->GetFingers();

        double distance = LessNotEqual(distanceNumber, 0.0) ? DEFAULT_PAN_DISTANCE : distanceNumber;
        distance_ = newContext->NormalizeToPx(Dimension(distance, DimensionUnit::VP));
        fingers_ = fingersNumber <= DEFAULT_PAN_FINGER ? DEFAULT_PAN_FINGER : fingersNumber;

        if (directNum >= PanDirection::NONE && directNum <= PanDirection::ALL) {
            direction_.type = directNum;
        }

        newFingers_ = fingers_;
        newDistance_ = distance_;
        newDirection_ = direction_;

        PanFingersFuncType changeFingers = [weak = AceType::WeakClaim(this)](int32_t fingers) {
            auto panRecognizer = weak.Upgrade();
            if (!panRecognizer) {
                return;
            }
            panRecognizer->ChangeFingers(fingers);
        };
        onChangeFingers_ = OnPanFingersFunc(changeFingers);
        panGestureOption_->SetOnPanFingersId(onChangeFingers_);

        PanDirectionFuncType changeDirection = [weak = AceType::WeakClaim(this)](const PanDirection& direction) {
            auto panRecognizer = weak.Upgrade();
            if (!panRecognizer) {
                return;
            }
            panRecognizer->ChangeDirection(direction);
        };
        onChangeDirection_ = OnPanDirectionFunc(changeDirection);
        panGestureOption_->SetOnPanDirectionId(onChangeDirection_);

        PanDistanceFuncType changeDistance = [weak = AceType::WeakClaim(this)](double distance) {
            auto panRecognizer = weak.Upgrade();
            if (!panRecognizer) {
                return;
            }
            panRecognizer->ChangeDistance(distance);
        };
        onChangeDistance_ = OnPanDistanceFunc(changeDistance);
        panGestureOption_->SetOnPanDistanceId(onChangeDistance_);
    }
    ~PanRecognizer() override
    {
        if (panGestureOption_ == nullptr) {
            return;
        }
        panGestureOption_->GetOnPanFingersIds().erase(onChangeFingers_.GetId());
        panGestureOption_->GetOnPanDirectionIds().erase(onChangeDirection_.GetId());
        panGestureOption_->GetOnPanDistanceIds().erase(onChangeDistance_.GetId());
    }

    void OnAccepted() override;
    void OnRejected() override;

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
    GestureAcceptResult IsPanGestureAccept() const;
    void Reset();
    void SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback);
    void ChangeFingers(int32_t fingers);
    void ChangeDirection(const PanDirection& direction);
    void ChangeDistance(double distance);

    const TouchRestrict& GetTouchRestrict() const
    {
        return touchRestrict_;
    }

    PanDirection direction_;
    double distance_ = 0.0;
    WeakPtr<PipelineContext> context_;
    std::map<int32_t, TouchEvent> touchPoints_;
    Offset averageDistance_;
    TimeStamp time_;
    bool pendingEnd_ = false;
    bool pendingCancel_ = false;
    Point globalPoint_;
    RefPtr<PanGestureOption> panGestureOption_;
    OnPanFingersFunc onChangeFingers_;
    OnPanDirectionFunc onChangeDirection_;
    OnPanDistanceFunc onChangeDistance_;
    int32_t newFingers_ = 1;
    double newDistance_ = 0.0;
    PanDirection newDirection_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_RECOGNIZER_H
