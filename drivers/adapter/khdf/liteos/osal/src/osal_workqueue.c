/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hdf_workqueue.h"
#include "linux/workqueue.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG hdf_workqueue

struct WorkWrapper {
    struct delayed_work work;
    HdfWorkFunc workFunc;
    void *para;
};

int32_t HdfWorkQueueInit(HdfWorkQueue *queue, char *name)
{
    if (queue == NULL || name == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    queue->realWorkQueue = create_singlethread_workqueue(name);
    if (queue->realWorkQueue == NULL) {
        HDF_LOGE("%s create queue fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void WorkEntry(struct work_struct *work)
{
    struct WorkWrapper *wrapper = NULL;
    if (work != NULL) {
        wrapper = (struct WorkWrapper *)work;
        if (wrapper->workFunc != NULL) {
            wrapper->workFunc(wrapper->para);
        } else {
            HDF_LOGE("%s routine null", __func__);
        }
    } else {
        HDF_LOGE("%s work null", __func__);
    }
}

int32_t HdfWorkInit(HdfWork *work, HdfWorkFunc func, void *para)
{
    struct work_struct *realWork = NULL;
    struct WorkWrapper *wrapper = NULL;

    if (work == NULL || func == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    work->realWork = NULL;

    wrapper = (struct WorkWrapper *)OsalMemCalloc(sizeof(*wrapper));
    if (wrapper == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    realWork = &(wrapper->work.work);
    wrapper->workFunc = func;
    wrapper->para = para;

    INIT_WORK(realWork, WorkEntry);
    work->realWork = wrapper;

    return HDF_SUCCESS;
}

int32_t HdfDelayedWorkInit(HdfWork *work, HdfWorkFunc func, void *para)
{
    struct delayed_work *realWork = NULL;
    struct WorkWrapper *wrapper = NULL;

    if (work == NULL || func == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    work->realWork = NULL;

    wrapper = (struct WorkWrapper *)OsalMemCalloc(sizeof(*wrapper));
    if (wrapper == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    realWork = &(wrapper->work);
    wrapper->workFunc = func;
    wrapper->para = para;

    INIT_DELAYED_WORK(realWork, WorkEntry);
    work->realWork = wrapper;

    return HDF_SUCCESS;
}

void HdfWorkDestroy(HdfWork *work)
{
    if (work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return;
    }

    OsalMemFree(work->realWork);
    work->realWork = NULL;

    return;
}

void HdfDelayedWorkDestroy(HdfWork *work)
{
    if (work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return;
    }

    return HdfWorkDestroy(work);
}

void HdfWorkQueueDestroy(HdfWorkQueue *queue)
{
    if (queue == NULL || queue->realWorkQueue == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return;
    }

    destroy_workqueue(queue->realWorkQueue);

    return;
}

bool HdfAddWork(HdfWorkQueue *queue, HdfWork *work)
{
    if (queue == NULL || queue->realWorkQueue == NULL || work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return false;
    }

    return queue_work(queue->realWorkQueue, &((struct WorkWrapper *)work->realWork)->work.work);
}

bool HdfAddDelayedWork(HdfWorkQueue *queue, HdfWork *work, uint32_t ms)
{
    if (queue == NULL || queue->realWorkQueue == NULL || work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return false;
    }

    return queue_delayed_work(queue->realWorkQueue, &((struct WorkWrapper *)work->realWork)->work, LOS_MS2Tick(ms));
}

unsigned int HdfWorkBusy(HdfWork *work)
{
    if (work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return 0;
    }

    return work_busy(&((struct WorkWrapper *)work->realWork)->work.work);
}

bool HdfCancelWorkSync(HdfWork *work)
{
    if (work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return false;
    }

    return cancel_work_sync(&((struct WorkWrapper *)work->realWork)->work.work);
}

bool HdfCancelDelayedWorkSync(HdfWork *work)
{
    if (work == NULL || work->realWork == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return false;
    }

    return cancel_delayed_work_sync(&((struct WorkWrapper *)work->realWork)->work);
}

