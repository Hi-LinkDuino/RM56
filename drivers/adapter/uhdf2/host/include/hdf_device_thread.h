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

#ifndef DEVICE_THREAD_H
#define DEVICE_THREAD_H

#include "hdf_thread_ex.h"
#include "hdf_message_task.h"
#include "hdf_message_looper.h"
#include "hdf_device_info.h"
#include "hdf_device_node.h"

struct DeviceThread {
    struct HdfThread super;
    struct HdfMessageTask task;
    struct HdfDeviceInfo *attribute;
    struct HdfMessageLooper looper;
};

enum {
    DEVICE_SERVICE_MESSAGE_LAUNCH,
    DEVICE_SERVICE_MESSAGE_SUSPEND,
    DEVICE_SERVICE_MESSAGE_RESUME
};

struct DeviceThread *DeviceThreadNewInstance(void);
void DeviceThreadFreeInstance(struct DeviceThread *thread);
void DeviceThreadMain(void *args);
int DeviceThreadAttach(struct DeviceThread *inst, struct IHdfDevice *device, struct HdfDeviceNode *service);

#endif /* DEVICE_THREAD_H */
