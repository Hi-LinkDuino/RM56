/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_message_looper.h"
#include "hdf_message_task.h"
#include "osal_message.h"

void HdfMessageLooperStart(struct HdfMessageLooper *looper)
{
    struct HdfMessage *message = NULL;
    while (looper != NULL) {
        message = HdfMessageQueueNext(&looper->messageQueue);
        if (message != NULL) {
            if (message->messageId == MESSAGE_STOP_LOOP) {
                HdfMessageRecycle(message);
                OsalMessageQueueDestroy(&looper->messageQueue);
                break;
            } else if (message->target != NULL) {
                struct HdfMessageTask *task = message->target;
                task->DispatchMessage(task, message);
            }
            HdfMessageRecycle(message);
        }
    }
}

void HdfMessageLooperStop(struct HdfMessageLooper *looper)
{
    if (looper == NULL) {
        return;
    }

    struct HdfMessage *message = HdfMessageObtain(0);
    if (message != NULL) {
        message->messageId = MESSAGE_STOP_LOOP;
        HdfMessageQueueEnqueue(&looper->messageQueue, message, 0);
    }
}

void HdfMessageLooperConstruct(struct HdfMessageLooper *looper)
{
    if (looper != NULL) {
        OsalMessageQueueInit(&looper->messageQueue);
        looper->Start = HdfMessageLooperStart;
        looper->Stop = HdfMessageLooperStop;
    }
}

