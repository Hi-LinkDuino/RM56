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

#ifndef LNN_EVENT_MONITOR_IMPL_H
#define LNN_EVENT_MONITOR_IMPL_H

#include <stdint.h>
#include "bus_center_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t (*LnnInitEventMonitorImpl)(void);

int32_t LnnInitNetlinkMonitorImpl(void);

int32_t LnnInitProductMonitorImpl(void);

int32_t LnnInitLwipMonitorImpl(void);

int32_t LnnInitWifiServiceMonitorImpl(void);

int32_t LnnInitDriverMonitorImpl(void);
#ifdef __cplusplus
}
#endif
#endif /* LNN_EVENT_MONITOR_IMPL_H */