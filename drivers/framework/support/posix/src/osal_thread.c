/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_thread.h"
#include <pthread.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#ifndef PTHREAD_STACK_MIN
#define OSAL_PTHREAD_STACK_MIN 4096
#else
#define OSAL_PTHREAD_STACK_MIN PTHREAD_STACK_MIN
#endif

#define HDF_LOG_TAG osal_thread

typedef void *(*PosixEntry)(void *data);

struct ThreadWrapper {
    OsalThreadEntry threadEntry;
    void *entryPara;
    pthread_t id;
};
#ifdef _LINUX_USER_
enum {
    OSAL_PRIORITY_LOW      = 10,
    OSAL_PRIORITY_MIDDLE   = 50,
    OSAL_PRIORITY_HIGH     = 90,
    OSAL_PRIORITY_HIGHEST  = 99,
};
#else
enum {
    OSAL_PRIORITY_HIGHEST  = 5,
    OSAL_PRIORITY_HIGH     = 15,
    OSAL_PRIORITY_MIDDLE   = 25,
    OSAL_PRIORITY_LOW      = 30,
};
#endif

static void OsalThreadRemapSched(int priority, struct sched_param *param)
{
    if (priority == OSAL_THREAD_PRI_HIGHEST) {
        param->sched_priority = OSAL_PRIORITY_HIGHEST;
    } else if (priority == OSAL_THREAD_PRI_HIGH) {
        param->sched_priority = OSAL_PRIORITY_HIGH;
    } else if (priority == OSAL_THREAD_PRI_DEFAULT) {
        param->sched_priority = OSAL_PRIORITY_MIDDLE;
    } else {
        param->sched_priority = OSAL_PRIORITY_LOW;
    }
}

int32_t OsalThreadCreate(struct OsalThread *thread, OsalThreadEntry threadEntry, void *entryPara)
{
    struct ThreadWrapper *para = NULL;

    if (thread == NULL || threadEntry == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    thread->realThread = NULL;
    para = (struct ThreadWrapper *)OsalMemCalloc(sizeof(*para));
    if (para == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        return HDF_FAILURE;
    }

    para->entryPara = entryPara;
    para->threadEntry = threadEntry;
    thread->realThread = para;

    return HDF_SUCCESS;
}

int32_t OsalThreadDestroy(struct OsalThread *thread)
{
    if (thread != NULL && thread->realThread != NULL) {
        OsalMemFree(thread->realThread);
        thread->realThread = NULL;
    }
    return HDF_SUCCESS;
}

static int OsalCreatePthread(pthread_t *threadId, pthread_attr_t *attribute, struct ThreadWrapper *para)
{
    int resultCode = pthread_create(threadId, attribute, (PosixEntry)para->threadEntry, para->entryPara);
    if (resultCode != 0) {
        HDF_LOGE("pthread_create errorno: %d", resultCode);
        return resultCode;
    }
    resultCode = pthread_detach(*threadId);
    if (resultCode != 0) {
        HDF_LOGE("pthread_detach errorno: %d", resultCode);
        return resultCode;
    }
    resultCode = pthread_attr_destroy(attribute);
    if (resultCode != 0) {
        HDF_LOGE("pthread_attr_destroy errorno: %d", resultCode);
        return resultCode;
    }
    return HDF_SUCCESS;
}

int32_t OsalThreadStart(struct OsalThread *thread, const struct OsalThreadParam *param)
{
    pthread_attr_t attribute;
    struct sched_param priorityHolder;

    if (thread == NULL || thread->realThread == NULL || param == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct ThreadWrapper *para = (struct ThreadWrapper *)thread->realThread;

    int resultCode = pthread_attr_init(&attribute);
    if (resultCode != 0) {
        HDF_LOGE("pthread_attr_init errorno: %d", resultCode);
        goto DEAL_FAIL;
    }

    size_t stackSize = param->stackSize;
    if (stackSize > 0) {
        stackSize = (stackSize < OSAL_PTHREAD_STACK_MIN) ? OSAL_PTHREAD_STACK_MIN : stackSize;
        resultCode = pthread_attr_setstacksize(&attribute, stackSize);
        if (resultCode != 0) {
            HDF_LOGE("pthread_attr_setstacksize errorno: %d", resultCode);
            goto DEAL_FAIL;
        }
    }

    resultCode = pthread_attr_getschedparam(&attribute, &priorityHolder);
    if (resultCode != 0) {
        HDF_LOGE("pthread_attr_getschedparam errorno: %d", resultCode);
        goto DEAL_FAIL;
    }

    OsalThreadRemapSched(param->priority, &priorityHolder);

    resultCode = pthread_attr_setschedparam(&attribute, &priorityHolder);
    if (resultCode != 0) {
        HDF_LOGE("pthread_attr_setschedparam errorno: %d", resultCode);
        goto DEAL_FAIL;
    }

    resultCode = OsalCreatePthread(&para->id, &attribute, thread->realThread);
    if (resultCode != 0) {
        HDF_LOGE("OsalCreatePthread errorno: %d", resultCode);
        goto DEAL_FAIL;
    }
    return HDF_SUCCESS;

DEAL_FAIL:
    (void)OsalThreadDestroy(thread);
    return HDF_FAILURE;
}

