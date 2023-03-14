/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup UI_Events
 * @{
 *
 * @brief Defines UI events, such as press, click and drag events.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file long_press_event.h
 *
 * @brief Declares a long-press event, which indicates that the finger is not raised or moved within 1 second
 *        after a view is pressed.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_LONG_PRESS_EVENT_H
#define GRAPHIC_LITE_LONG_PRESS_EVENT_H

#include "event.h"

namespace OHOS {
/**
 * @brief Defines a long-press event, which indicates that the finger is not raised or moved within 1 second
 *        after a view is pressed.
 *
 * @since 1.0
 * @version 1.0
 */
class LongPressEvent : public Event {
public:
    /**
     * @brief A constructor used to create a <b>LongPressEvent</b> instance.
     * @param pos Indicates the position where the long-press event occurs.
     * @param timeStamp Indicates the time stamp when the long-press event occurs.
     * @since 1.0
     * @version 1.0
     */
    LongPressEvent(const Point& pos, const TimeType& timeStamp) : Event(pos)
    {
        timeStamp_ = timeStamp;
    }

    /**
     * @brief A destructor used to delete the <b>LongPressEvent</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~LongPressEvent() {}
};
} // namespace OHOS
#endif // GRAPHIC_LITE_LONG_PRESS_EVENT_H
