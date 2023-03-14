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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_CONVERTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_CONVERTOR_H

#include <vector>

#include "core/event/mouse_event.h"
#include "core/event/touch_event.h"

namespace OHOS::Ace {

constexpr int32_t ROTATION_DIVISOR = 64;

struct alignas(8) AceActionData {
    enum class ActionType : int64_t {
        UNKNOWN = -1,
        CANCEL = 0,
        ADD,
        REMOVE,
        HOVER,
        DOWN,
        MOVE,
        UP,
    };

    int64_t timeStamp = 0;
    ActionType actionType = ActionType::UNKNOWN;
    int64_t actionId = 0;
    double physicalX = 0.0;
    double physicalY = 0.0;

    double pressure = 0.0;
    double maxPressure = 0.0;
    double size = 0.0;
    int64_t sourceDevice = 0;
    int64_t sourceDeviceId = 0;
};

struct alignas(8) AceMouseData {
    enum class Action : int64_t {
        NONE,
        PRESS,
        RELEASE,
        MOVE,
        HOVER_ENTER,
        HOVER_MOVE,
        HOVER_EXIT,
    };

    enum class ActionButton : int64_t {
        NONE_BUTTON = 0,
        LEFT_BUTTON = 1,
        RIGHT_BUTTON = 2,
        MIDDLE_BUTTON = 4,
        BACK_BUTTON = 8,
        FORWARD_BUTTON = 16,
    };

    double physicalX;
    double physicalY;
    double physicalZ;
    double deltaX;
    double deltaY;
    double deltaZ;
    double scrollDeltaX;
    double scrollDeltaY;
    double scrollDeltaZ;
    Action action;
    ActionButton actionButton;
    int64_t pressedButtons;
    int64_t timeStamp;
    int64_t deviceId;
};

void ConvertTouchEvent(const std::vector<uint8_t>& data, std::vector<TouchEvent>& events);

void ConvertMouseEvent(const std::vector<uint8_t>& data, MouseEvent& events);

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_CONVERTOR_H
