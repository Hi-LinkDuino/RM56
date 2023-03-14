/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BASE_GROUP_MANAGER_TASK_H
#define BASE_GROUP_MANAGER_TASK_H

#include "device_auth.h"
#include "hc_task_thread.h"
#include "json_utils.h"

typedef struct {
    HcTaskBase base;
    int64_t reqId;
    int32_t osAccountId;
    int32_t opCode;
    CJson *params;
    const DeviceAuthCallback *cb;
} GroupManagerTask;

typedef struct {
    int64_t reqId;
    int32_t osAccountId;
    int32_t opCode;
    CJson *params;
    const DeviceAuthCallback *cb;
} GMTaskParams;

#endif
