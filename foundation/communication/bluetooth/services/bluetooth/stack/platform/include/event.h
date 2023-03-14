/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MS_PER_SECOND 1000
#define NS_PER_MS 1000000

#define EVENT_WAIT_TIMEOUT_ERR -1
#define EVENT_WAIT_OTHER_ERR -2

typedef struct Event Event;

/**
 * @brief Perform instantiation of the Event, set Event Attributes isAutoClear.
 *
 * @param isAutoClear Event isAutoClear or not. If Event is not autoClear,
 *            should perform EventClear after EventSet Manually.
 * @return Event pointer.
 * @since 6
 */
Event *EventCreate(bool isAutoClear);

/**
 * @brief Delete an Event object.
 *
 * @param event Event pointer.
 * @since 6
 */
void EventDelete(Event *event);

/**
 * @brief Set an Event.
 *
 * @param event Event pointer.
 * @since 6
 */
void EventSet(Event *event);

/**
 * @brief Clear an Event.
 *
 * @param event Event pointer.
 * @since 6
 */
void EventClear(Event *event);

/**
 * @brief Wait an Event object, wait for Fixed time or block waiting.
 *
 * @param event Event pointer.
 * @param ms Countdown time. If countdown time less than 0, block waiting.
 * @return Success return 0, timeout return -1, failed return -2.
 * @since 6
 */
int32_t EventWait(Event *event, int64_t ms);

#ifdef __cplusplus
}
#endif

#endif  // EVENT_H