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

#ifndef GRAPHIC_LITE_PRESS_EVENT_H
#define GRAPHIC_LITE_PRESS_EVENT_H

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
 * @file press_event.h
 *
 * @brief Declares a touch event, which indicates that the touchscreen or mouse is pressed.
 *
 * @since 1.0
 * @version 1.0
 */

#include "event.h"

namespace OHOS {
/**
 * @brief Defines a touch event, which indicates that the touchscreen or mouse is pressed.
 *
 * @since 1.0
 * @version 1.0
 */
class PressEvent : public Event {
public:
    /**
     * @brief A constructor used to create a <b>PressEvent</b> instance.
     * @param pos Indicates the position where the press event occurs.
     * @since 1.0
     * @version 1.0
     */
    PressEvent(const Point& pos) : Event(pos) {}

    /**
     * @brief A destructor used to delete the <b>PressEvent</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~PressEvent() {}
};
} // namespace OHOS
#endif // GRAPHIC_LITE_PRESS_EVENT_H
