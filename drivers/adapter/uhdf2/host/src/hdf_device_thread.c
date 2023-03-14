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

#include "hdf_device_thread.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_message.h"

#define HDF_LOG_TAG hdf_device_thread

int32_t DeviceThreadMessageHandler(struct HdfMessageTask *task, struct HdfMessage *msg)
{
    (void)task;
    struct HdfDevice *device = (struct HdfDevice *)msg->data[0];
    switch (msg->messageId) {
        case DEVICE_SERVICE_MESSAGE_LAUNCH: {
            (void)device;
            struct HdfDeviceNode *devService = (struct HdfDeviceNode *)msg->data[1];
            if (devService != NULL && devService->super.LaunchNode != NULL) {
                devService->super.LaunchNode(devService);
            }
            break;
        }
        case DEVICE_SERVICE_MESSAGE_SUSPEND: {
            break;
        }
        case DEVICE_SERVICE_MESSAGE_RESUME: {
            break;
        }
        default: {
            HDF_LOGE("Unsupported message, message id is %{public}u", msg->messageId);
            break;
        }
    }
    return HDF_SUCCESS;
}

int DeviceThreadAttach(struct DeviceThread *inst, struct IHdfDevice *device, struct HdfDeviceNode *service)
{
    if (inst != NULL || device == NULL || service == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    // Thread is already running
    struct HdfMessageTask *task = &inst->task;
    struct HdfMessage *message = HdfMessageObtain(sizeof(struct HdfDeviceNode *));
    if (message == NULL) {
        HDF_LOGE("DeviceThreadAttach:obtain message error");
        return HDF_ERR_MALLOC_FAIL;
    }
    message->messageId = DEVICE_SERVICE_MESSAGE_LAUNCH;
    message->data[0] = (void *)device;
    message->data[1] = (void *)service;
    return task->SendMessage(task, message, true);
}

void DeviceThreadMain(void *args)
{
    struct DeviceThread *currentThread = (struct DeviceThread *)args;
    if (currentThread != NULL) {
        struct HdfMessageLooper *looper = &currentThread->looper;
        if (looper->Start != NULL) {
            looper->Start(looper);
        }
    }
}

void DeviceThreadConstruct(struct DeviceThread *inst)
{
    static struct IHdfMessageHandler handler = {
        .Dispatch = DeviceThreadMessageHandler
    };

    HdfMessageLooperConstruct(&inst->looper);
    HdfMessageTaskConstruct(&inst->task, &inst->looper, &handler);
    inst->super.ThreadEntry = DeviceThreadMain;
    HdfThreadConstruct(&inst->super);
}

struct DeviceThread *DeviceThreadNewInstance()
{
    struct DeviceThread *thread =
        (struct DeviceThread *)OsalMemCalloc(sizeof(struct DeviceThread));
    if (thread != NULL) {
        DeviceThreadConstruct(thread);
    }
    return thread;
}

void DeviceThreadFreeInstance(struct DeviceThread *thread)
{
    if (thread != NULL) {
        HdfThreadDestruct(&thread->super);
        OsalMemFree(thread);
    }
}

