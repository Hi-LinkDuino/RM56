/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_TASK_QUEUE_H
#define HDF_TASK_QUEUE_H

#include "hdf_dlist.h"
#include "osal_sem.h"
#include "osal_mutex.h"
#include "osal_thread.h"

struct HdfTaskType;
typedef int32_t (*HdfTaskFunc)(struct HdfTaskType *para);

struct HdfTaskType {
    struct DListHead node;
    HdfTaskFunc func;
};

struct HdfTaskQueue {
    struct OsalSem sem;
    struct OsalMutex mutex;
    struct DListHead head;
    struct OsalThread thread;
    bool threadRunFlag;
    HdfTaskFunc queueFunc;
    const char *queueName;
};

void HdfTaskEnqueue(struct HdfTaskQueue *queue, struct HdfTaskType *task);
struct HdfTaskQueue *HdfTaskQueueCreate(HdfTaskFunc queueFunc, const char *name);
void HdfTaskQueueDestroy(struct HdfTaskQueue *queue);

#endif /* HDF_TASK_QUEUE_H */