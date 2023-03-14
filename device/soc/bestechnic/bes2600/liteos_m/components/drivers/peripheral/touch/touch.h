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
#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cmsis_os2.h"
#include "touch_if.h"

struct touch_device {
    const char *name;
    osThreadId_t tid;
    osSemaphoreId_t sem;
    osMessageQueueId_t mq;
    int (*init)(struct touch_device *);
    void (*deinit)(void);
    void (*irq_enable)(bool);
    int (*read)(struct touch_device *, struct touch_msg *);
};

int32_t RegisterTouchDevice(struct touch_device *device);

#endif
