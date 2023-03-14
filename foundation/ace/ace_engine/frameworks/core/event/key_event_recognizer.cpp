/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/event/key_event_recognizer.h"

#include <ctime>

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t LONG_PRESS_DURATION = 1;

}

std::vector<KeyEvent> KeyEventRecognizer::GetKeyEvents(int32_t keyCode, int32_t keyAction, int32_t repeatTime,
    int64_t timeStamp, int64_t timeStampStart, int32_t metaKey, int32_t keySource, int64_t deviceId)
{
    if (timeStamp == 0) {
        timeStamp = clock();
        timeStampStart = timeStamp;
    }
    std::vector<KeyEvent> keyEvents;
    keyEvents.emplace_back(KeyEvent(static_cast<KeyCode>(keyCode), static_cast<KeyAction>(keyAction), repeatTime,
        timeStamp, deviceId, static_cast<SourceType>(keySource)));
    auto result = keyMap_.try_emplace(keyCode, false);
    auto iter = result.first;

    // Recognize long press event.
    if ((keyAction == static_cast<int32_t>(KeyAction::DOWN)) && (repeatTime >= LONG_PRESS_DURATION) &&
        (!iter->second)) {
        LOGD("this event is long press, key code is %{public}d", keyCode);
        iter->second = true;
        keyEvents.emplace_back(KeyEvent(static_cast<KeyCode>(keyCode), KeyAction::LONG_PRESS, repeatTime, timeStamp,
            deviceId, static_cast<SourceType>(keySource)));
    }
    // Recognize click event.
    if (keyAction == static_cast<int32_t>(KeyAction::UP)) {
        if (iter->second) {
            iter->second = false;
        } else {
            LOGD("this event is click, key code is %{public}d", keyCode);
            keyEvents.emplace_back(KeyEvent(static_cast<KeyCode>(keyCode), KeyAction::CLICK, repeatTime, timeStamp,
                deviceId, static_cast<SourceType>(keySource)));
        }
    }
    return keyEvents;
}

} // namespace OHOS::Ace
