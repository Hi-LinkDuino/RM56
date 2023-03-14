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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_TOUCH_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_TOUCH_EVENT_H

#include <list>

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "core/event/ace_events.h"

namespace OHOS::Ace {

enum class TouchType : size_t {
    DOWN = 0,
    UP,
    MOVE,
    CANCEL,
    UNKNOWN,
};

struct TouchRestrict final {
    static constexpr uint32_t NONE = 0x00000000;
    static constexpr uint32_t CLICK = 0x00000001;
    static constexpr uint32_t LONG_PRESS = 0x00000010;
    static constexpr uint32_t SWIPE_LEFT = 0x00000100;
    static constexpr uint32_t SWIPE_RIGHT = 0x00000200;
    static constexpr uint32_t SWIPE_UP = 0x00000400;
    static constexpr uint32_t SWIPE_DOWN = 0x00000800;
    static constexpr uint32_t SWIPE = 0x00000F00;
    static constexpr uint32_t SWIPE_VERTICAL = 0x0000C00;   // Vertical
    static constexpr uint32_t SWIPE_HORIZONTAL = 0x0000300; // Horizontal
    static constexpr uint32_t TOUCH = 0xFFFFFFFF;

    uint32_t forbiddenType = NONE;

    void UpdateForbiddenType(uint32_t gestureType)
    {
        forbiddenType |= gestureType;
    }
};

struct TouchPoint final {
    int32_t id = 0;
    float x = 0.0f;
    float y = 0.0f;
    float screenX = 0.0f;
    float screenY = 0.0f;
    TimeStamp downTime;
    double size = 0.0;
    float force = 0.0f;
    bool isPressed = false;
};

/**
 * @brief TouchEvent contains the active change point and a list of all touch points.
 */
struct TouchEvent final {
    // the active changed point info
    // The ID is used to identify the point of contact between the finger and the screen. Different fingers have
    // different ids.
    int32_t id = 0;
    float x = 0.0f;
    float y = 0.0f;
    float screenX = 0.0f;
    float screenY = 0.0f;
    TouchType type = TouchType::UNKNOWN;
    // nanosecond time stamp.
    TimeStamp time;
    double size = 0.0;
    float force = 0.0f;
    int64_t deviceId = 0;
    SourceType sourceType = SourceType::NONE;

    // all points on the touch screen.
    std::vector<TouchPoint> pointers;

    Offset GetOffset() const
    {
        return Offset(x, y);
    }

    Offset GetScreenOffset() const
    {
        return Offset(screenX, screenY);
    }

    TouchEvent CreateScalePoint(float scale) const
    {
        if (NearZero(scale)) {
            return { id, x, y, screenX, screenY, type, time, size, force, deviceId, sourceType, pointers };
        }
        auto temp = pointers;
        std::for_each(temp.begin(), temp.end(), [scale](auto&& point) {
            point.x = point.x / scale;
            point.y = point.y / scale;
            point.screenX = point.screenX / scale;
            point.screenY = point.screenY / scale;
        });
        return { id, x / scale, y / scale, screenX / scale, screenY / scale, type, time, size, force, deviceId,
            sourceType, temp };
    }

    TouchEvent UpdateScalePoint(float scale, float offsetX, float offsetY, int32_t pointId) const
    {
        auto temp = pointers;
        if (NearZero(scale)) {
            std::for_each(temp.begin(), temp.end(), [offsetX, offsetY](auto&& point) {
                point.x = point.x - offsetX;
                point.y = point.y - offsetY;
                point.screenX = point.screenX - offsetX;
                point.screenY = point.screenY - offsetY;
            });
            return { pointId, x - offsetX, y - offsetY, screenX - offsetX, screenY - offsetY, type, time, size, force,
                deviceId, sourceType, temp };
        }

        std::for_each(temp.begin(), temp.end(), [scale, offsetX, offsetY](auto&& point) {
            point.x = (point.x - offsetX) / scale;
            point.y = (point.y - offsetY) / scale;
            point.screenX = (point.screenX - offsetX) / scale;
            point.screenY = (point.screenY - offsetY) / scale;
        });
        return { pointId, (x - offsetX) / scale, (y - offsetY) / scale, (screenX - offsetX) / scale,
            (screenY - offsetY) / scale, type, time, size, force, deviceId, sourceType, temp };
    }

    TouchEvent UpdatePointers() const
    {
        TouchPoint point { .id = id,
            .x = x,
            .y = y,
            .screenX = screenX,
            .screenY = screenY,
            .downTime = time,
            .size = size,
            .force = force,
            .isPressed = (type == TouchType::DOWN) };
        TouchEvent event { .id = id,
            .x = x,
            .y = y,
            .screenX = screenX,
            .screenY = screenY,
            .type = type,
            .time = time,
            .size = size,
            .force = force,
            .deviceId = deviceId,
            .sourceType = sourceType };
        event.pointers.emplace_back(std::move(point));
        return event;
    }
};

class TouchCallBackInfo : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(TouchCallBackInfo, BaseEventInfo);

public:
    explicit TouchCallBackInfo(TouchType type) : BaseEventInfo("onTouchEvent"), touchType_(type) {}
    ~TouchCallBackInfo() override = default;

    void SetScreenX(float screenX)
    {
        screenX_ = screenX;
    }
    float GetScreenX() const
    {
        return screenX_;
    }
    void SetScreenY(float screenY)
    {
        screenY_ = screenY;
    }
    float GetScreenY() const
    {
        return screenY_;
    }
    void SetLocalX(float localX)
    {
        localX_ = localX;
    }
    float GetLocalX() const
    {
        return localX_;
    }
    void SetLocalY(float localY)
    {
        localY_ = localY;
    }
    float GetLocalY() const
    {
        return localY_;
    }
    void SetTouchType(TouchType type)
    {
        touchType_ = type;
    }
    TouchType GetTouchType() const
    {
        return touchType_;
    }
    void SetTimeStamp(const TimeStamp& time)
    {
        time_ = time;
    }
    TimeStamp GetTimeStamp() const
    {
        return time_;
    }
    void SetForce(float force)
    {
        force_ = force;
    }
    float GetForce() const
    {
        return force_;
    }

private:
    float screenX_ = 0.0f;
    float screenY_ = 0.0f;
    float localX_ = 0.0f;
    float localY_ = 0.0f;
    float force_ = 0.0f;
    TouchType touchType_ = TouchType::UNKNOWN;
    TimeStamp time_;
};

class TouchLocationInfo : public virtual TypeInfoBase {
    DECLARE_RELATIONSHIP_OF_CLASSES(TouchLocationInfo, TypeInfoBase);

public:
    explicit TouchLocationInfo(int32_t fingerId) : fingerId_(fingerId) {}
    ~TouchLocationInfo() override = default;

    TouchLocationInfo& SetGlobalLocation(const Offset& globalLocation)
    {
        globalLocation_ = globalLocation;
        return *this;
    }
    TouchLocationInfo& SetLocalLocation(const Offset& localLocation)
    {
        localLocation_ = localLocation;
        return *this;
    }

    TouchLocationInfo& SetScreenLocation(const Offset& screenLocation)
    {
        screenLocation_ = screenLocation;
        return *this;
    }

    const Offset& GetScreenLocation() const
    {
        return screenLocation_;
    }

    const Offset& GetLocalLocation() const
    {
        return localLocation_;
    }
    const Offset& GetGlobalLocation() const
    {
        return globalLocation_;
    }
    int32_t GetFingerId() const
    {
        return fingerId_;
    }

    void SetSize(double size)
    {
        size_ = size;
    }

    double GetSize() const
    {
        return size_;
    }

    void SetTouchDeviceId(int64_t deviceId)
    {
        touchDeviceId_ = deviceId;
    }

    int64_t GetTouchDeviceId() const
    {
        return touchDeviceId_;
    }

    void SetForce(float force)
    {
        force_ = force;
    }
    float GetForce() const
    {
        return force_;
    }
    TouchType GetTouchType() const
    {
        return touchType_;
    }
    void SetTouchType(TouchType type)
    {
        touchType_ = type;
    }

private:
    // The finger id is used to identify the point of contact between the finger and the screen. Different fingers have
    // different ids.
    int32_t fingerId_ = -1;

    // global position at which the touch point contacts the screen.
    Offset globalLocation_;
    // Different from global location, The local location refers to the location of the contact point relative to the
    // current node which has the recognizer.
    Offset localLocation_;

    Offset screenLocation_;

    // finger touch size
    double size_ = 0.0;

    // input device id
    int64_t touchDeviceId_ = 0;

    // touch pressure
    float force_ = 0.0f;

    // touch type
    TouchType touchType_ = TouchType::UNKNOWN;
};

class ACE_EXPORT TouchEventTarget : public virtual AceType {
    DECLARE_ACE_TYPE(TouchEventTarget, AceType);

public:
    virtual bool DispatchEvent(const TouchEvent& point) = 0;
    virtual bool HandleEvent(const TouchEvent& point) = 0;

    void SetTouchRestrict(const TouchRestrict& touchRestrict)
    {
        touchRestrict_ = touchRestrict;
    }

protected:
    TouchRestrict touchRestrict_ { TouchRestrict::NONE };
};

using TouchTestResult = std::list<RefPtr<TouchEventTarget>>;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_TOUCH_EVENT_H
