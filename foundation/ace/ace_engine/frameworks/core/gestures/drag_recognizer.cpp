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

#include "core/gestures/drag_recognizer.h"

#include <cmath>

#include "base/log/log.h"
#include "base/log/frame_report.h"
#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {
namespace {

#ifndef WEARABLE_PRODUCT
constexpr double DELTA_DURATION = 3.0;
#else
constexpr double DELTA_DURATION = 15.0;
#endif

} // namespace

void DragRecognizer::OnAccepted(size_t touchId)
{
    LOGD("drag gesture has been accepted! the touch id is %{public}zu", touchId);
    auto iter = dragFingers_.find(touchId);
    if (iter == dragFingers_.end()) {
        LOGE("the dragFingers_ is not ready to receive accepted, id is %{public}zu", touchId);
        return;
    }

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().BeginListFling();
    }

    auto& dragInfo = iter->second;
    dragInfo.states_ = DetectState::DETECTED;
    if (onDragStart_) {
        const auto& firstPoint = dragInfo.velocityTracker_.GetFirstTrackPoint();
        DragStartInfo startInfo(firstPoint.id);
        startInfo.SetGlobalLocation(firstPoint.GetOffset())
            .SetLocalLocation(firstPoint.GetOffset() - coordinateOffset_);
        startInfo.SetTimeStamp(firstPoint.time);
        AsyncCallback(onDragStart_, startInfo);
    }
    if (onDragUpdate_) {
        const auto& currentPoint = dragInfo.velocityTracker_.GetCurrentTrackPoint();
        const auto dragOffsetInMainAxis =
            axis_ == Axis::VERTICAL ? dragInfo.dragOffset_.GetY() : dragInfo.dragOffset_.GetX();
        DragUpdateInfo updateInfo(currentPoint.id);
        updateInfo.SetDelta(dragInfo.dragOffset_)
            .SetMainDelta(dragOffsetInMainAxis)
            .SetGlobalLocation(currentPoint.GetOffset())
            .SetLocalLocation(currentPoint.GetOffset() - coordinateOffset_);
        updateInfo.SetTimeStamp(currentPoint.time);
        AsyncCallback(onDragUpdate_, updateInfo);
    }
}

void DragRecognizer::OnRejected(size_t touchId)
{
    LOGD("drag gesture has been rejected! the touch id is %{public}zu", touchId);
    auto iter = dragFingers_.find(touchId);
    if (iter == dragFingers_.end()) {
        LOGE("the dragFingers_ is not ready to receive rejected, id is %{public}zu", touchId);
        return;
    }
    // Resets drag state to ready.
    iter->second.states_ = DetectState::READY;
}

void DragRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    LOGD("drag recognizer receives touch down event, detecting drag event");
    if ((touchRestrict_.forbiddenType & TouchRestrict::SWIPE) == TouchRestrict::SWIPE) {
        LOGD("drag recognizer forbid swipe");
        return;
    }
    if (((touchRestrict_.forbiddenType & TouchRestrict::SWIPE_HORIZONTAL) == TouchRestrict::SWIPE_HORIZONTAL) &&
        axis_ == Axis::HORIZONTAL) {
        LOGD("horizontal drag recognizer forbid swipe");
        return;
    }
    if (((touchRestrict_.forbiddenType & TouchRestrict::SWIPE_VERTICAL) == TouchRestrict::SWIPE_VERTICAL) &&
        axis_ == Axis::VERTICAL) {
        LOGD("vertical drag recognizer forbid swipe");
        return;
    }
    DragFingersInfo dragFingerInfo(axis_);
    auto result = dragFingers_.insert_or_assign(event.id, dragFingerInfo);

    auto& dragInfo = result.first->second;
    if (dragInfo.states_ == DetectState::READY) {
        AddToReferee(event.id, Claim(this));
        dragInfo.dragOffset_.Reset();
        dragInfo.velocityTracker_.Reset();
        dragInfo.velocityTracker_.UpdateTouchPoint(event);
        dragInfo.states_ = DetectState::DETECTING;
    } else {
        LOGE("the state is not ready to receive touch down event, state is %{public}d, id is %{public}d",
            dragInfo.states_, event.id);
    }
}

void DragRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("drag recognizer receives touch move event");
    auto iter = dragFingers_.find(event.id);
    if (iter == dragFingers_.end()) {
        LOGE("the dragFingers_ is not ready to receive touch move event, id is %{public}d", event.id);
        return;
    }

    auto& dragInfo = iter->second;
    dragInfo.velocityTracker_.UpdateTouchPoint(event);
    if (dragInfo.states_ == DetectState::DETECTED) {
        if (onDragUpdate_) {
            DragUpdateInfo info(event.id);

            info.SetDelta(dragInfo.velocityTracker_.GetDelta())
                .SetMainDelta(dragInfo.velocityTracker_.GetMainAxisDeltaPos())
                .SetGlobalLocation(event.GetOffset())
                .SetLocalLocation(event.GetOffset() - coordinateOffset_);

            info.SetTimeStamp(event.time);
            onDragUpdate_(info);
            if (onDragUpdateNotifyCall_) {
                onDragUpdateNotifyCall_(event.GetOffset().GetX(), event.GetOffset().GetY(), info);
            }
        }
    } else if (dragInfo.states_ == DetectState::DETECTING) {
        dragInfo.dragOffset_ += dragInfo.velocityTracker_.GetDelta();
        double dragOffsetInMainAxis = 0.0;
        if (axis_ == Axis::FREE) {
            dragOffsetInMainAxis = dragInfo.dragOffset_.GetDistance();
        } else if (axis_ == Axis::VERTICAL) {
            dragOffsetInMainAxis = dragInfo.dragOffset_.GetY();
        } else {
            dragOffsetInMainAxis = dragInfo.dragOffset_.GetX();
        }
        LOGD("handle move event, the drag offset is %{public}lf, axis is %{public}d", dragOffsetInMainAxis, axis_);
        if (IsDragGestureAccept(dragOffsetInMainAxis)) {
            LOGD("this gesture is drag, try to accept it");
            Accept(event.id);
        }
    } else {
        LOGD("state is ready, need to use touch down event to trigger, state is %{public}d", dragInfo.states_);
    }
}

void DragRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    LOGD("drag recognizer receives touch up event");
    auto iter = dragFingers_.find(event.id);
    if (iter == dragFingers_.end()) {
        LOGE("the dragFingers_ is not ready to receive touch up event, id is %{public}d", event.id);
        return;
    }

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().EndListFling();
    }

    auto& dragInfo = iter->second;
    dragInfo.velocityTracker_.UpdateTouchPoint(event, true);
    if (dragInfo.states_ == DetectState::DETECTED) {
        bool upSuccess = true;
        for (auto entry = dragFingers_.begin(); entry != dragFingers_.end(); ++entry) {
            if (entry == iter) {
                continue;
            }
            auto& otherDragInfo = entry->second;
            if (otherDragInfo.states_ == DetectState::DETECTED) {
                upSuccess = false;
            }
        }
        if (upSuccess) {
            LOGD("use animation to end drag");
            if (onDragEnd_) {
                DragEndInfo endInfo(event.id);
                endInfo.SetVelocity(dragInfo.velocityTracker_.GetVelocity())
                    .SetMainVelocity(dragInfo.velocityTracker_.GetMainAxisVelocity())
                    .SetGlobalLocation(event.GetOffset())
                    .SetLocalLocation(event.GetOffset() - coordinateOffset_);
                endInfo.SetTimeStamp(event.time);
                onDragEnd_(endInfo);
            }
        }
        if (onDragEndNotifyCall_) {
            DragEndInfo endInfo(event.id);

            endInfo.SetVelocity(dragInfo.velocityTracker_.GetVelocity())
                .SetMainVelocity(dragInfo.velocityTracker_.GetMainAxisVelocity())
                .SetGlobalLocation(event.GetOffset())
                .SetLocalLocation(event.GetOffset() - coordinateOffset_);

            endInfo.SetTimeStamp(event.time);
            onDragEndNotifyCall_(event.GetOffset().GetX(), event.GetOffset().GetY(), endInfo);
            AsyncCallback(onDragEnd_, endInfo);
        }
    } else if (dragInfo.states_ == DetectState::DETECTING) {
        LOGD("this gesture is not drag, try to reject it");
        Reject(event.id);
    }
    dragInfo.states_ = DetectState::READY;
}

void DragRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("drag recognizer receives touch cancel event");
    auto iter = dragFingers_.find(event.id);
    if (iter == dragFingers_.end()) {
        LOGE("the dragFingers_ is not ready to receive touch cancel event, id is %{public}d", event.id);
        return;
    }

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().EndListFling();
    }

    auto& dragInfo = iter->second;
    if (dragInfo.states_ == DetectState::DETECTED) {
        if (onDragCancel_) {
            AsyncCallback(onDragCancel_);
        }
    } else if (dragInfo.states_ == DetectState::DETECTING) {
        LOGD("cancel drag gesture detect, try to reject it");
        Reject(event.id);
    }
    dragInfo.states_ = DetectState::READY;
}

bool DragRecognizer::IsDragGestureAccept(double offset) const
{
    if (std::abs(offset) > DELTA_DURATION) {
        if (axis_ == Axis::HORIZONTAL) {
            uint32_t flag = offset > 0 ? TouchRestrict::SWIPE_RIGHT : TouchRestrict::SWIPE_LEFT;
            if ((touchRestrict_.forbiddenType & flag) != flag) {
                return true;
            }
        } else if (axis_ == Axis::VERTICAL) {
            uint32_t flag = offset > 0 ? TouchRestrict::SWIPE_DOWN : TouchRestrict::SWIPE_UP;
            if ((touchRestrict_.forbiddenType & flag) != flag) {
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}

void DragRecognizer::Accept(size_t touchId)
{
    std::set<size_t> ids;
    ids.insert(touchId);
    BatchAdjudicate(ids, Claim(this), GestureDisposal::ACCEPT);
}

void DragRecognizer::Reject(size_t touchId)
{
    std::set<size_t> ids;
    ids.insert(touchId);
    BatchAdjudicate(ids, Claim(this), GestureDisposal::REJECT);
}
} // namespace OHOS::Ace
