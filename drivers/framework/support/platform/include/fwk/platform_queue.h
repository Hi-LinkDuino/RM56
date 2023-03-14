/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_QUEUE_H
#define PLATFORM_QUEUE_H

#include "hdf_dlist.h"
#include "osal_thread.h"
#include "osal_sem.h"
#include "osal_spinlock.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct PlatformMsg;
struct PlatformQueue;

struct PlatformMsg {
    struct DListHead node;
    int32_t code;
    int32_t error;
    void *data;
};

typedef int32_t (*PlatformMsgHandle)(struct PlatformQueue *queue, struct PlatformMsg *msg);

struct PlatformQueue {
    const char *name;
    OsalSpinlock spin;
    bool start;
    struct OsalSem sem;
    struct DListHead msgs;
    struct OsalThread thread; /* the worker thread of this queue */
    PlatformMsgHandle handle;
    void *data;
};

int32_t PlatformQueueAddMsg(struct PlatformQueue *queue, struct PlatformMsg *msg);
struct PlatformQueue *PlatformQueueCreate(PlatformMsgHandle handle, const char *name, void *data);
void PlatformQueueDestroy(struct PlatformQueue *queue);
int32_t PlatformQueueStart(struct PlatformQueue *queue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_QUEUE_H */
