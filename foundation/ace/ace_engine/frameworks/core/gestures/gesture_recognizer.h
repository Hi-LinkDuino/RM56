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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_RECOGNIZER_H

#include <map>
#include <memory>
#include <set>

#include "gesture_referee.h"

#include "core/event/touch_event.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace {

enum class DetectState { READY, DETECTING, DETECTED };

enum class RefereeState { DETECTING, PENDING, BLOCKED, SUCCEED, FAIL };

class ACE_EXPORT GestureRecognizer : public TouchEventTarget {
    DECLARE_ACE_TYPE(GestureRecognizer, TouchEventTarget)

public:
    // Called when request of handling gesture sequence is accepted by gesture referee.
    virtual void OnAccepted(size_t touchId) = 0;

    // Called when request of handling gesture sequence is rejected by gesture referee.
    virtual void OnRejected(size_t touchId) = 0;

    // Called when request of handling gesture sequence is rejected by gesture referee.
    virtual void OnPending(size_t touchId) {}

    // Reconiciles the state from the given recognizer into this. The
    // implementation must check that the given recognizer type matches the
    // current one. The return value should be false if the reconciliation fails
    // and true if it succeeds
    virtual bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
    {
        return true;
    }

    bool DispatchEvent(const TouchEvent& point) override
    {
        return true;
    }
    bool HandleEvent(const TouchEvent& point) override;

    // Coordinate offset is used to calculate the local location of the touch point in the render node.
    void SetCoordinateOffset(const Offset& coordinateOffset)
    {
        coordinateOffset_ = coordinateOffset;
    }

    // Gets the coordinate offset to calculate the local location of the touch point by manually.
    const Offset& GetCoordinateOffset() const
    {
        return coordinateOffset_;
    }

    GesturePriority GetPriority() const
    {
        return priority_;
    }

    void SetPriority(GesturePriority priority)
    {
        priority_ = priority;
    }

    GestureMask GetPriorityMask() const
    {
        return priorityMask_;
    }

    void SetPriorityMask(GestureMask priorityMask)
    {
        priorityMask_ = priorityMask;
    }

    RefereeState GetRefereeState() const
    {
        return refereeState_;
    }

    void SetRefereeState(RefereeState refereeState)
    {
        refereeState_ = refereeState;
    }

    DetectState GetDetectState() const
    {
        return state_;
    }

    void SetGestureGroup(const WeakPtr<GestureRecognizer>& gestureGroup)
    {
        gestureGroup_ = gestureGroup;
    }

    void SetOnAction(const GestureEventFunc& onAction)
    {
        onAction_ = std::make_unique<GestureEventFunc>(onAction);
    }

    void SetOnActionStart(const GestureEventFunc& onActionStart)
    {
        onActionStart_ = std::make_unique<GestureEventFunc>(onActionStart);
    }

    void SetOnActionUpdate(const GestureEventFunc& onActionUpdate)
    {
        onActionUpdate_ = std::make_unique<GestureEventFunc>(onActionUpdate);
    }

    void SetOnActionEnd(const GestureEventFunc& onActionEnd)
    {
        onActionEnd_ = std::make_unique<GestureEventFunc>(onActionEnd);
    }

    void SetOnActionCancel(const GestureEventNoParameter& onActionCancel)
    {
        onActionCancel_ = std::make_unique<GestureEventNoParameter>(onActionCancel);
    }

    inline void SendCancelMsg()
    {
        if (onActionCancel_ && *onActionCancel_) {
            (*onActionCancel_)();
        }
    }

    void SetIsExternalGesture(bool isExternalGesture)
    {
        isExternalGesture_ = isExternalGesture;
    }

    bool GetIsExternalGesture() const
    {
        return isExternalGesture_;
    }

protected:
    virtual void HandleTouchDownEvent(const TouchEvent& event) = 0;
    virtual void HandleTouchUpEvent(const TouchEvent& event) = 0;
    virtual void HandleTouchMoveEvent(const TouchEvent& event) = 0;
    virtual void HandleTouchCancelEvent(const TouchEvent& event) = 0;

    virtual void AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer);
    virtual void DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer);
    virtual void BatchAdjudicate(
        const std::set<size_t>& touchIds, const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal);

    Offset coordinateOffset_;
    DetectState state_ { DetectState::READY };
    RefereeState refereeState_ { RefereeState::DETECTING };
    GesturePriority priority_ = GesturePriority::Low;
    GestureMask priorityMask_ = GestureMask::Normal;

    int32_t fingers_ = 1;
    std::list<FingerInfo> fingerList_;
    bool isExternalGesture_ = false;

    std::unique_ptr<GestureEventFunc> onAction_;
    std::unique_ptr<GestureEventFunc> onActionStart_;
    std::unique_ptr<GestureEventFunc> onActionUpdate_;
    std::unique_ptr<GestureEventFunc> onActionEnd_;
    std::unique_ptr<GestureEventNoParameter> onActionCancel_;

    int64_t deviceId_ = 0;
    SourceType deviceType_ = SourceType::NONE;

private:
    WeakPtr<GestureRecognizer> gestureGroup_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_RECOGNIZER_H
