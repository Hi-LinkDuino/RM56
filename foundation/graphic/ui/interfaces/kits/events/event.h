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
 * @file event.h
 *
 * @brief Declares the base class of a user input event and provides functions to record information such as
 *        the event position and timestamp.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_EVENT_H
#define GRAPHIC_LITE_EVENT_H

#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_types.h"

namespace OHOS {
class UIView;
/**
 * @brief Defines the base class of a user input event and provides functions to record information such as
 *        the event position and timestamp.
 *
 * @since 1.0
 * @version 1.0
 */
class Event : public HeapBase {
public:
    /**
     * @brief A default constructor used to create an <b>Event</b> instance.
     * @since 1.0
     * @version 1.0
     */
    Event();

    /**
     * @brief A constructor used to create an <b>Event</b> instance.
     * @param pos Indicates the position where the event occurs.
     * @since 1.0
     * @version 1.0
     */
    Event(const Point& curPos);

    /**
     * @brief A destructor used to delete the <b>Event</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~Event() {}

    /**
     * @brief Obtains the position where an event occurs.
     * @return Returns the position.
     * @since 1.0
     * @version 1.0
     */
    const Point& GetCurrentPos() const
    {
        return curPos_;
    }

    /**
    * @brief Obtains the timestamp when an event occurs.
    * @return Returns the timestamp.
    * @since 1.0
    * @version 1.0
    */
    const TimeType& GetTimeStamp() const
    {
        return timeStamp_;
    }

    /**
     * @brief Sets the timestamp when an event occurs.
     * @param timeStamp Indicates the timestamp of the event to set.
     * @since 1.0
     * @version 1.0
     */
    void SetTimeStamp(const TimeType& timeStamp)
    {
        timeStamp_ = timeStamp;
    }

protected:
    TimeType timeStamp_;
    Point curPos_;
    UIView* targetView_ = nullptr;
    UIView* currentView_ = nullptr;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_EVENT_H
