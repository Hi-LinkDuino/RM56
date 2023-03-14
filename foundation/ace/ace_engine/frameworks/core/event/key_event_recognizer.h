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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_KEY_EVENT_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_KEY_EVENT_RECOGNIZER_H

#include <unordered_map>
#include <vector>

#include "core/event/key_event.h"

namespace OHOS::Ace {

class KeyEventRecognizer {
public:
    // This method recognizes events by recording raw key events. If the raw event can be identified as a special
    // event, it returns the raw event and the identified special event, otherwise it returns only the raw event.
    std::vector<KeyEvent> GetKeyEvents(int32_t keyCode, int32_t keyAction, int32_t repeatTime, int64_t timeStamp = 0,
        int64_t timeStampStart = 0, int32_t metaKey = 0, int32_t keySource = 0, int64_t deviceId = 0);
private:
    std::unordered_map<int32_t, bool> keyMap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_KEY_EVENT_RECOGNIZER_H
