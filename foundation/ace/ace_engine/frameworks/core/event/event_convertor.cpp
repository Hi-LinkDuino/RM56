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

#include "core/event/event_convertor.h"

#include <chrono>

#include "base/log/log.h"

namespace OHOS::Ace {

void ConvertTouchEvent(const std::vector<uint8_t>& data, std::vector<TouchEvent>& events)
{
    const auto* origin = reinterpret_cast<const AceActionData*>(data.data());
    size_t size = data.size() / sizeof(AceActionData);
    auto current = const_cast<AceActionData*>(origin);
    auto end = current + size;
    while (current < end) {
        std::chrono::microseconds micros(current->timeStamp);
        TimeStamp time(micros);
        TouchEvent point { static_cast<int32_t>(current->actionId), static_cast<float>(current->physicalX),
            static_cast<float>(current->physicalY), static_cast<float>(current->physicalX),
            static_cast<float>(current->physicalY), TouchType::UNKNOWN, time, current->size,
            static_cast<float>(current->pressure), static_cast<int64_t>(current->sourceDeviceId) };
        switch (current->actionType) {
            case AceActionData::ActionType::CANCEL:
                point.type = TouchType::CANCEL;
                events.push_back(point);
                break;
            case AceActionData::ActionType::ADD:
            case AceActionData::ActionType::REMOVE:
            case AceActionData::ActionType::HOVER:
                break;
            case AceActionData::ActionType::DOWN:
                point.type = TouchType::DOWN;
                events.push_back(point);
                break;
            case AceActionData::ActionType::MOVE:
                point.type = TouchType::MOVE;
                events.push_back(point);
                break;
            case AceActionData::ActionType::UP:
                point.type = TouchType::UP;
                events.push_back(point);
                break;
            case AceActionData::ActionType::UNKNOWN:
                break;
            default:
                break;
        }
        current++;
    }
}

void ConvertMouseEvent(const std::vector<uint8_t>& data, MouseEvent& events)
{
    const auto* mouseActionData = reinterpret_cast<const AceMouseData*>(data.data());
    if (!mouseActionData) {
        return;
    }
    std::chrono::microseconds micros(mouseActionData->timeStamp);
    TimeStamp time(micros);
    events.x = mouseActionData->physicalX;
    events.y = mouseActionData->physicalY;
    events.z = mouseActionData->physicalZ;
    events.deltaX = mouseActionData->deltaX;
    events.deltaY = mouseActionData->deltaY;
    events.deltaZ = mouseActionData->deltaZ;
    events.scrollX = mouseActionData->scrollDeltaX;
    events.scrollY = mouseActionData->scrollDeltaY;
    events.scrollZ = mouseActionData->scrollDeltaZ;
    switch (mouseActionData->action) {
        case AceMouseData::Action::PRESS:
            events.action = MouseAction::PRESS;
            break;
        case AceMouseData::Action::RELEASE:
            events.action = MouseAction::RELEASE;
            break;
        case AceMouseData::Action::MOVE:
            events.action = MouseAction::MOVE;
            break;
        case AceMouseData::Action::HOVER_ENTER:
            events.action = MouseAction::HOVER_ENTER;
            break;
        case AceMouseData::Action::HOVER_MOVE:
            events.action = MouseAction::HOVER_MOVE;
            break;
        case AceMouseData::Action::HOVER_EXIT:
            events.action = MouseAction::HOVER_EXIT;
            break;
        default:
            events.action = MouseAction::NONE;
            break;
    }
    switch (mouseActionData->actionButton) {
        case AceMouseData::ActionButton::LEFT_BUTTON:
            events.button = MouseButton::LEFT_BUTTON;
            break;
        case AceMouseData::ActionButton::RIGHT_BUTTON:
            events.button = MouseButton::RIGHT_BUTTON;
            break;
        case AceMouseData::ActionButton::MIDDLE_BUTTON:
            events.button = MouseButton::MIDDLE_BUTTON;
            break;
        case AceMouseData::ActionButton::BACK_BUTTON:
            events.button = MouseButton::BACK_BUTTON;
            break;
        case AceMouseData::ActionButton::FORWARD_BUTTON:
            events.button = MouseButton::FORWARD_BUTTON;
            break;
        default:
            events.button = MouseButton::NONE_BUTTON;
            break;
    }
    events.pressedButtons = static_cast<int32_t>(mouseActionData->pressedButtons);
    events.time = time;
    events.deviceId = mouseActionData->deviceId;
}

} // namespace OHOS::Ace