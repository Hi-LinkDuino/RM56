/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef POWERMGR_POWER_MGR_TIMER_UTIL_H
#define POWERMGR_POWER_MGR_TIMER_UTIL_H

#include <stdint.h>

#include <ohos_types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef timer_t PowerTimer;
typedef void (*PowerTimerCallback)(void *privateData);

/*
 * Create timer. Must call PowerMgrDestroyTimer to release memory after timer is not needed
 */
PowerTimer *PowerMgrCreateTimer(int64_t whenMsec, int64_t intervalMsec, PowerTimerCallback cb);

/*
 * Reset timer attributes and stop timer
 */
BOOL PowerMgrResetTimer(PowerTimer *timer, int64_t whenMsec, int64_t intervalMsec);

/*
 * Start timer. The privateData will set to PowerTimerCallback function when timer timeout
 */
BOOL PowerMgrStartTimer(PowerTimer *timer, void *privateData);

/*
 * Restart timer. The privateData will set to PowerTimerCallback function when timer timeout
 */
BOOL PowerMgrRestartTimer(PowerTimer *timer, void *privateData);

/*
 * Stop timer
 */
BOOL PowerMgrStopTimer(PowerTimer *timer);

/*
 * Destory timer
 */
void PowerMgrDestroyTimer(PowerTimer *timer);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_POWER_MGR_TIMER_UTIL_H
