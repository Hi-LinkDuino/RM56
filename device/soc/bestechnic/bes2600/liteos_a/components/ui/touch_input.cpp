/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "touch_input.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"

namespace OHOS
{
TouchInput *TouchInput::GetInstance()
{
    static TouchInput instance;
    if (!instance.init) {
        instance.handle = TouchOpen(0);
        if (!instance.handle) {
            GRAPHIC_LOGE("TouchOpen failed");
            return nullptr;
        }
        instance.init = true;
    }
    return &instance;
}

bool TouchInput::IsValidTouchMsg(struct touch_msg *msg)
{
    if (msg->x >= HORIZONTAL_RESOLUTION || msg->y >= VERTICAL_RESOLUTION)
        return false;

    if (msg->event != TOUCH_EVENT_UP && msg->event != TOUCH_EVENT_DOWN && msg->event != TOUCH_EVENT_MOVE)
        return false;

    return true;
}

bool TouchInput::Read(DeviceData &data)
{
    // merge msg with the same event
    struct touch_msg tmp[TOUCH_MSG_MAX] = {0};
    int i = 0;
    while (i < TOUCH_MSG_MAX && TouchRead(this->handle, &tmp[i], 0) == 0) {
        if (!IsValidTouchMsg(&tmp[i]))
            break;

        if (tmp[i].event == TOUCH_EVENT_MOVE) {
            tmp[i].event = TOUCH_EVENT_DOWN;
        }
        i++;
        if (IsValidTouchMsg(&this->msg)) {
            if (tmp[i - 1].event != this->msg.event) {
                break;
            }
        } else {
            if (i > 1 && tmp[i - 1].event != tmp[0].event)
                break;
        }
    }
    if (i <= 1) {
        data.point.x = this->msg.x;
        data.point.y = this->msg.y;
        data.state = (this->msg.event == TOUCH_EVENT_DOWN) ? STATE_PRESS : STATE_RELEASE;
    } else if (i <= TOUCH_MSG_MAX) {
        data.point.x = tmp[i - 2].x;
        data.point.y = tmp[i - 2].y;
        data.state = (tmp[i - 2].event == TOUCH_EVENT_DOWN) ? STATE_PRESS : STATE_RELEASE;
    }
    // GRAPHIC_LOGD("Touch {%d, %d} %d", data.point.x, data.point.y, data.state);
    if (i >= 1 && i <= TOUCH_MSG_MAX && IsValidTouchMsg(&tmp[i - 1]))
        this->msg = tmp[i - 1];

    return false;
}
} // namespace OHOS
