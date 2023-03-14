/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OSAL_MSG_QUEUE_H
#define OSAL_MSG_QUEUE_H

#include "hdf_slist.h"
#include "osal_message.h"
#include "osal_mutex.h"
#include "osal_sem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfMessageQueue {
    struct OsalMutex mutex;
    struct OsalSem   semaphore;
    struct HdfSList  list;
};

void OsalMessageQueueInit(struct HdfMessageQueue *queue);
void OsalMessageQueueDestroy(struct HdfMessageQueue *queue);
void HdfMessageQueueEnqueue(
    struct HdfMessageQueue *queue, struct HdfMessage *message, long delayed);

struct HdfMessage *HdfMessageQueueNext(struct HdfMessageQueue *queue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_MSG_QUEUE_H */
