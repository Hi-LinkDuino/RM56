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
#ifndef TOUCH_IF_H
#define TOUCH_IF_H

#include "platform_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* event */
#define TOUCH_EVENT_UP (0x01)
#define TOUCH_EVENT_DOWN (0x02)
#define TOUCH_EVENT_MOVE (0x03)
#define TOUCH_EVENT_EXIST (0x04)
#define TOUCH_EVENT_NONE (0x80)

#define TOUCH_DEV_MAX 1
#define TOUCH_MSG_MAX 8

struct touch_msg {
    uint16_t x;
    uint16_t y;
    uint8_t event;
};

DevHandle TouchOpen(int id);

void TouchClose(DevHandle handle);

int TouchRead(DevHandle handle, struct touch_msg *msg, uint32_t timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
