/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_message_task.h"
#include "hdf_message_looper.h"
#include "osal_mem.h"
#include "osal_message.h"

int32_t HdfMessageTaskSendMessageLater(
    struct HdfMessageTask *task, struct HdfMessage *msg, bool sync, long delay)
{
    if (msg != NULL) {
        if (msg->target == NULL) {
            msg->target = task;
        }
        if (sync) {
            if (task->messageHandler != NULL && task->messageHandler->Dispatch != NULL) {
                int ret = task->messageHandler->Dispatch(task, msg);
                OsalMemFree(msg);
                return ret;
            }
        } else {
            HdfMessageQueueEnqueue(task->messageQueue, msg, delay);
            return HDF_SUCCESS;
        }
    }

    return HDF_ERR_INVALID_PARAM;
}

int32_t HdfMessageTaskSendMessage(struct HdfMessageTask *task, struct HdfMessage *msg, bool sync)
{
    return HdfMessageTaskSendMessageLater(task, msg, sync, 0);
}

void HdfMessageTaskDispatchMessage(struct HdfMessageTask *task, struct HdfMessage *msg)
{
    struct IHdfMessageHandler *handler = task->messageHandler;
    if ((handler != NULL) && (handler->Dispatch != NULL)) {
        handler->Dispatch(task, msg);
    }
}

void HdfMessageTaskConstruct(struct HdfMessageTask *inst,
    struct HdfMessageLooper *looper, struct IHdfMessageHandler *handler)
{
    if (inst != NULL && looper != NULL) {
        inst->SendMessage = HdfMessageTaskSendMessage;
        inst->messageHandler = handler;
        inst->messageQueue = &looper->messageQueue;
        inst->DispatchMessage = HdfMessageTaskDispatchMessage;
    }
}

