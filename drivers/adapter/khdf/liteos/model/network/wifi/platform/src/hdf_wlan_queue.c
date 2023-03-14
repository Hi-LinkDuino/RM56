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

#include "hdf_wlan_queue.h"
#include "los_queue.h"
#include "osal/osal_mem.h"
#include "utils/hdf_log.h"

typedef struct HdfWlanQueueImpl {
    HdfWlanQueue queue;
    uint32_t queueID;
} HdfWlanQueueImpl;

HdfWlanQueue *CreateQueue(uint16_t maxQueueSize)
{
    HdfWlanQueueImpl *queue = (HdfWlanQueueImpl *)OsalMemCalloc(sizeof(HdfWlanQueueImpl));
    if (queue == NULL) {
        HDF_LOGE("%s:oom", __func__);
        return NULL;
    }

    uint32_t ret = LOS_QueueCreate("", maxQueueSize, &(queue->queueID), 0, sizeof(void *));
    if (ret != LOS_OK) {
        OsalMemFree(queue);
        return NULL;
    }
    return (HdfWlanQueue *)queue;
}
void DestroyQueue(HdfWlanQueue *queue)
{
    if (queue == NULL) {
        return;
    }
    HdfWlanQueueImpl *queueImpl = (HdfWlanQueueImpl *)queue;
    uint32_t status = LOS_QueueDelete(queueImpl->queueID);
    if (status != LOS_OK) {
        HDF_LOGE("Destroy message queue failed!err=%u", status);
    }
    OsalMemFree(queue);
}

void *PopQueue(HdfWlanQueue *queue)
{
    if (queue == NULL) {
        return NULL;
    }
    void *context = NULL;
    HdfWlanQueueImpl *queueImpl = (HdfWlanQueueImpl *)queue;

    uint32_t bufferSize = sizeof(void *);
    uint32_t status = LOS_QueueReadCopy(queueImpl->queueID, &context, &bufferSize, 0);
    if (status == LOS_OK && bufferSize == sizeof(void *)) {
        return context;
    } else if (bufferSize != sizeof(void *)) {
        HDF_LOGE("Bad pointer size detected! size=%d", bufferSize);
    }
    return NULL;
}
int32_t PushQueue(HdfWlanQueue *queue, void *context)
{
    if (queue == NULL) {
        HDF_LOGE("%s:oom", __func__);
        return HDF_FAILURE;
    }
    HdfWlanQueueImpl *queueImpl = (HdfWlanQueueImpl *)queue;

    uint32_t ret = LOS_QueueWriteCopy(queueImpl->queueID, &context, sizeof(void *), LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        HDF_LOGE("%s:Write queue failed!ret=%u", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}