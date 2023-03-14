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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_RAW_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_RAW_RECOGNIZER_H

#include <functional>

#include "core/gestures/gesture_recognizer.h"

namespace OHOS::Ace {

class TouchEventInfo : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(TouchEventInfo, BaseEventInfo);

public:
    explicit TouchEventInfo(const std::string& type) : BaseEventInfo(type) {}
    ~TouchEventInfo() override = default;

    void AddTouchLocationInfo(TouchLocationInfo&& info)
    {
        touches_.emplace_back(info);
    }
    void AddChangedTouchLocationInfo(TouchLocationInfo&& info)
    {
        changedTouches_.emplace_back(info);
    }

    const std::list<TouchLocationInfo>& GetTouches() const
    {
        return touches_;
    }
    const std::list<TouchLocationInfo>& GetChangedTouches() const
    {
        return changedTouches_;
    }

private:
    std::list<TouchLocationInfo> touches_;
    std::list<TouchLocationInfo> changedTouches_;
};

using OnTouchEventCallback = std::function<void(const TouchEventInfo&)>;
using CatchTouchEventCallback = std::function<void()>;

namespace EventAction {
constexpr uint32_t SIZE = 2;
constexpr uint32_t ON = 0;
constexpr uint32_t CATCH = 1;
} // namespace EventAction

namespace EventStage {
constexpr uint32_t SIZE = 2;
constexpr uint32_t CAPTURE = 0;
constexpr uint32_t BUBBLE = 1;
} // namespace EventStage

namespace EventType {
constexpr uint32_t SIZE = 4;
constexpr uint32_t TOUCH_DOWN = 0;
constexpr uint32_t TOUCH_MOVE = 1;
constexpr uint32_t TOUCH_UP = 2;
constexpr uint32_t TOUCH_CANCEL = 3;
} // namespace EventType

// Notice:
// The RawRecognizer does not participate in the gesture decision and is not affected by the gesture disambiguation.
// If there are other gesture recognizers that have accepted the gesture, the RawRecognizer can also accept the
// original event.
class RawRecognizer : public TouchEventTarget {
    DECLARE_ACE_TYPE(RawRecognizer, TouchEventTarget);

public:
    void HandleEvent(const TouchEvent& point, uint32_t stage);
    bool DispatchEvent(const TouchEvent& point) override;
    bool HandleEvent(const TouchEvent& point) override;

    void SetOnEventCallback(
        const OnTouchEventCallback& eventCallback, uint32_t stage, uint32_t eventType)
    {
        onEventCallbacks_[stage][eventType] = eventCallback;
    }

    void SetCatchEventCallback(
        const CatchTouchEventCallback& eventCallback, uint32_t stage, uint32_t eventType)
    {
        catcheventCallbacks_[stage][eventType] = eventCallback;
    }

    void SetOnTouchDown(const OnTouchEventCallback& onTouchDown)
    {
        onEventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_DOWN] = onTouchDown;
    }

    void SetOnTouchMove(const OnTouchEventCallback& onTouchMove)
    {
        onEventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_MOVE] = onTouchMove;
    }

    void SetOnTouchUp(const OnTouchEventCallback& onTouchUp)
    {
        onEventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_UP] = onTouchUp;
    }

    void SetOnTouchCancel(const OnTouchEventCallback& onTouchCancel)
    {
        onEventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_CANCEL] = onTouchCancel;
    }

    // Coordinate offset is used to calculate the local location of the touch point in the render node.
    void SetCoordinateOffset(const Offset& coordinateOffset)
    {
        coordinateOffset_ = coordinateOffset;
    }

private:
    TouchEventInfo CreateTouchEventInfo(
        const std::string& type, const TouchEvent& point, bool ignoreCurrent = false) const;

    OnTouchEventCallback onEventCallbacks_[EventStage::SIZE][EventType::SIZE];
    CatchTouchEventCallback catcheventCallbacks_[EventStage::SIZE][EventType::SIZE];
    TouchEvent lastPoint_;
    Offset coordinateOffset_;
    bool isFirstTrack_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_RAW_RECOGNIZER_H
