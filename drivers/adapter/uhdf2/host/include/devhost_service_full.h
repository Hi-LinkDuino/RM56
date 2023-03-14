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

#ifndef DEVHOST_SERVICE_FULL_H
#define DEVHOST_SERVICE_FULL_H
#include "devhost_service.h"
#include "hdf_message_task.h"
#include "hdf_message_looper.h"
#include "hdf_device_info.h"
#include "osal_sysevent.h"

struct DevHostServiceFull {
    struct DevHostService super;
    struct HdfMessageTask task;
    struct HdfMessageLooper looper;
    struct HdfSysEventNotifyNode sysEventNotifyNode;
};

enum {
    DEVHOST_MESSAGE_ADD_DEVICE = 1,
    DEVHOST_MESSAGE_DEL_DEVICE,
};

void DevHostServiceFullConstruct(struct DevHostServiceFull *inst);
void DevHostServiceFullDestruct(struct DevHostServiceFull *inst);

#endif /* DEVHOST_SERVICE_FULL_H */
