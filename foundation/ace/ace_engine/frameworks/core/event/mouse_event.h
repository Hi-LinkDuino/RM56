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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_EVENT_H

#include "core/event/touch_event.h"

namespace OHOS::Ace {

static const int32_t MOUSE_BASE_ID = 1000;

enum class MouseAction : int32_t {
    NONE = 0,
    PRESS = 1,
    RELEASE = 2,
    MOVE = 3,
    HOVER = 4,
    HOVER_ENTER,
    HOVER_MOVE,
    HOVER_EXIT,
};

enum class MouseState : int32_t {
    NONE = 0,
    HOVER = 1,
};

enum class MouseButton : int32_t {
    NONE_BUTTON = 0,
    LEFT_BUTTON = 1,
    RIGHT_BUTTON = 2,
    MIDDLE_BUTTON = 4,
    BACK_BUTTON = 8,
    FORWARD_BUTTON = 16,
};

struct MouseEvent final {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    float deltaZ = 0.0f;
    float scrollX = 0.0f;
    float scrollY = 0.0f;
    float scrollZ = 0.0f;
    float screenX = 0.0f;
    float screenY = 0.0f;
    MouseAction action = MouseAction::NONE;
    MouseButton button = MouseButton::NONE_BUTTON;
    int32_t pressedButtons = 0; // combined by MouseButtons
    TimeStamp time;
    int64_t deviceId = 0;
    SourceType sourceType = SourceType::NONE;

    Offset GetOffset() const
    {
        return Offset(x, y);
    }

    Offset GetScreenOffset() const
    {
        return Offset(screenX, screenY);
    }

    int32_t GetId() const
    {
        if (pressedButtons > 0) {
            return pressedButtons + MOUSE_BASE_ID;
        } else {
            return (int32_t)button + MOUSE_BASE_ID;
        }
    }

    MouseEvent CreateScaleEvent(float scale) const
    {
        if (NearZero(scale)) {
            return { .x = x,
                .y = y,
                .z = z,
                .deltaX = deltaX,
                .deltaY = deltaY,
                .deltaZ = deltaZ,
                .scrollX = scrollX,
                .scrollY = scrollY,
                .scrollZ = scrollZ,
                .screenX = screenX,
                .screenY = screenY,
                .action = action,
                .button = button,
                .pressedButtons = pressedButtons,
                .time = time,
                .deviceId = deviceId,
                .sourceType = sourceType };
        }

        return { .x = x / scale,
            .y = y / scale,
            .z = z / scale,
            .deltaX = deltaX / scale,
            .deltaY = deltaY / scale,
            .deltaZ = deltaZ / scale,
            .scrollX = scrollX / scale,
            .scrollY = scrollY / scale,
            .scrollZ = scrollZ / scale,
            .screenX = screenX / scale,
            .screenY = screenY / scale,
            .action = action,
            .button = button,
            .pressedButtons = pressedButtons,
            .time = time,
            .deviceId = deviceId,
            .sourceType = sourceType };
    }

    TouchEvent CreateTouchPoint() const
    {
        TouchType type = TouchType::UNKNOWN;
        if (action == MouseAction::PRESS) {
            type = TouchType::DOWN;
        } else if (action == MouseAction::RELEASE) {
            type = TouchType::UP;
        } else if (action == MouseAction::MOVE) {
            type = TouchType::MOVE;
        } else {
            type = TouchType::UNKNOWN;
        }
        int32_t id = GetId();
        TouchPoint point { .id = id,
            .x = x,
            .y = y,
            .screenX = screenX,
            .screenY = screenY,
            .downTime = time,
            .size = 0.0,
            .isPressed = (type == TouchType::DOWN) };
        TouchEvent event { .id = id,
            .x = x,
            .y = y,
            .screenX = screenX,
            .screenY = screenY,
            .type = type,
            .time = time,
            .size = 0.0,
            .deviceId = deviceId,
            .sourceType = sourceType };
        event.pointers.emplace_back(std::move(point));
        return event;
    }

    MouseEvent operator-(const Offset& offset) const
    {
        return { .x = x - offset.GetX(),
            .y = y - offset.GetY(),
            .z = z,
            .deltaX = deltaX,
            .deltaY = deltaY,
            .deltaZ = deltaZ,
            .scrollX = scrollX,
            .scrollY = scrollY,
            .scrollZ = scrollZ,
            .screenX = screenX - offset.GetX(),
            .screenY = screenY - offset.GetY(),
            .action = action,
            .button = button,
            .pressedButtons = pressedButtons,
            .time = time,
            .deviceId = deviceId,
            .sourceType = sourceType };
    }
};

class MouseInfo : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(MouseInfo, BaseEventInfo);

public:
    MouseInfo() : BaseEventInfo("onMouse") {}
    ~MouseInfo() override = default;

    void SetButton(MouseButton button)
    {
        button_ = button;
    }

    MouseButton GetButton() const
    {
        return button_;
    }

    void SetAction(MouseAction action)
    {
        action_ = action;
    }

    MouseAction GetAction() const
    {
        return action_;
    }

    MouseInfo& SetGlobalLocation(const Offset& globalLocation)
    {
        globalLocation_ = globalLocation;
        return *this;
    }
    MouseInfo& SetLocalLocation(const Offset& localLocation)
    {
        localLocation_ = localLocation;
        return *this;
    }

    MouseInfo& SetScreenLocation(const Offset& screenLocation)
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

private:
    MouseButton button_ = MouseButton::NONE_BUTTON;
    MouseAction action_ = MouseAction::NONE;
    // global position at which the touch point contacts the screen.
    Offset globalLocation_;
    // Different from global location, The local location refers to the location of the contact point relative to the
    // current node which has the recognizer.
    Offset localLocation_;
    Offset screenLocation_;
};

class MouseEventTarget : public virtual AceType {
    DECLARE_ACE_TYPE(MouseEventTarget, AceType);

public:
    virtual void HandleEvent(const MouseEvent& event) = 0;
};

using MouseTestResult = std::list<RefPtr<MouseEventTarget>>;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_EVENT_H
