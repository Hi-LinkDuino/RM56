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

#ifndef GRAPHIC_LITE_ROTATE_EVENT_H
#define GRAPHIC_LITE_ROTATE_EVENT_H
/**
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file rotate_event.h
 *
 * @brief Declares a <b>RotateEvent</b>, which usually occurs on a knob.
 *
 * @since 5.0
 * @version 3.0
 */

#include "event.h"

namespace OHOS {
/**
 * @brief Defines a <b>RotateEvent</b>, which usually occurs on a knob.
 *
 * @since 5.0
 * @version 3.0
 */
class RotateEvent : public Event {
public:
    RotateEvent() = delete;

    /**
     * @brief A constructor used to create a <b>RotateEvent</b> instance.
     * @param rotate Indicates the short data representing the number reported by a <b>RotateEvent</b>.
     *
     * @since 5.0
     * @version 3.0
     */
    RotateEvent(int16_t rotate) : rotate_(static_cast<int16_t>(rotate * ROTATE_SENSITIVITY)) {} // Rotation direction

    ~RotateEvent() {}

    /**
     * @brief Obtains the number reported by a <b>RotateEvent</b>.
     * @return Returns the number reported by a <b>RotateEvent</b>.
     *
     * @since 5.0
     * @version 3.0
     */
    int16_t GetRotate() const
    {
        return rotate_;
    }

private:
    int16_t rotate_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_ROTATE_EVENT_H
