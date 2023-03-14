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

#ifndef ALARM_H
#define ALARM_H

#include <stdint.h>
#include <stdbool.h>
#include "reactor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALARM_NAME_SIZE 16

#define MS_PER_SECOND 1000
#define NS_PER_MS 1000000

typedef struct Alarm Alarm;
typedef void (*AlarmCallback)(void *parameter);

/**
 * @brief Perform instantiation of the Alarm.
 *
 * @return Succeed return 0, failed return -1.
 * @since 6
 */
int32_t AlarmModuleInit();

/**
 * @brief Clean up the Alarm Module.
 *
 * @since 6
 */
void AlarmModuleCleanup();

/**
 * @brief Perform instantiation of the Alarm, set Alarm Attributes isPeriodic and name.
 *
 * @param name Alarm Name.
 * @param isPeriodic Alarm isPeriodic.
 * @return Succeed return Alarm pointer, failed return NULL.
 * @since 6
 */
Alarm *AlarmCreate(const char *name, const bool isPeriodic);

/**
 * @brief Delete an Alarm Object.
 *
 * @param alarm Alarm pointer.
 * @since 6
 */
void AlarmDelete(Alarm *alarm);

/**
 * @brief Set an Alarm. Set the time(ms) as well as callback function and its parameter.
 *        Execute AlarmSet, alarm start counting down.
 *
 * @param alarm Alarm pointer.
 * @param timeMs Countdown time, unit(ms).
 * @param callback Alarm timeout callback.
 * @param parameter Callback parameter.
 * @return Succeed return 0, failed return -1.
 * @since 6
 */
int32_t AlarmSet(Alarm *alarm, uint64_t timeMs, AlarmCallback callback, void *parameter);

/**
 * @brief Cancel an alarm.
 *        If alarm is running, this operation will stop it.
 *
 * @param alarm Alarm pointer.
 * @since 6
 */
void AlarmCancel(Alarm *alarm);

#ifdef __cplusplus
}
#endif

#endif  // ALARM_H