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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_GESTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_GESTURE_H

namespace OHOS::Ace {

enum class GestureType {
    CLICK = 0,
    LONG_PRESS,
    TOUCH_START,
    TOUCH_MOVE,
    TOUCH_SLIDE,
    TOUCH_CANCEL,
    TOUCH_END,
    REMOTE_MESSAGE,
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_GESTURE_H
