/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef GROUP_AUTH_COMMON_DEFINES_H
#define GROUP_AUTH_COMMON_DEFINES_H

#include <stdint.h>
#include "device_auth.h"
#include "hc_task_thread.h"
#include "json_utils.h"

#define MAX_UDID_LEN 64

typedef struct {
    HcTaskBase base;
    int64_t authReqId;
    CJson *authParams;
    const DeviceAuthCallback *callback;
} AuthDeviceTask;

#endif
