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

#ifndef DEVMGR_SERVICE_FULL_H
#define DEVMGR_SERVICE_FULL_H

#include "devmgr_service.h"
#include "hdf_message_task.h"
#include "hdf_message_looper.h"

struct DevmgrServiceFull {
    struct DevmgrService super;
    struct HdfMessageLooper looper;
    struct HdfMessageTask task;
};

enum {
    DEVMGR_MESSAGE_DEVHOST_DIED,
};

void DevmgrServiceFullConstruct(struct DevmgrServiceFull *inst);
struct HdfObject *DevmgrServiceFullCreate(void);
struct HdfMessageTask *DevmgrServiceFullGetMessageTask(void);

#endif /* DEVMGR_SERVICE_FULL_H */
