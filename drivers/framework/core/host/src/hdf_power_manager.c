/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_power_manager.h"
#include "hdf_log.h"
#include "hdf_pm_reg.h"
#include "osal_mem.h"
#include "power_state_token.h"

#define HDF_LOG_TAG hdf_power_manager

struct PmTaskQueue {
    struct HdfTaskQueue *taskQueue;
};

static struct PmTaskQueue *HdfPmTaskQueueInstance(void)
{
    static struct PmTaskQueue pmTaskQueue = {NULL};
    return &pmTaskQueue;
}

struct PmTaskQueue *HdfPmTaskQueueInit(HdfTaskFunc func)
{
    struct PmTaskQueue *pmTaskQueue = HdfPmTaskQueueInstance();

    if (pmTaskQueue->taskQueue == NULL) {
        pmTaskQueue->taskQueue = HdfTaskQueueCreate(func, "pm_queue");
        if (pmTaskQueue->taskQueue != NULL) {
            HDF_LOGI("%s HdfTaskQueueCreate success", __func__);
        }
    }

    return pmTaskQueue;
}

void HdfPmTaskQueueDestroy(void)
{
    struct PmTaskQueue *pmTaskQueue = HdfPmTaskQueueInstance();
    HdfTaskQueueDestroy(pmTaskQueue->taskQueue);
    pmTaskQueue->taskQueue = NULL;
}

static int32_t PmTaskFunc(struct HdfTaskType *para)
{
    struct HdfPmRequest *pmRequest = NULL;
    struct IPowerStateToken *tokenIf = NULL;

    if (para == NULL) {
        return HDF_FAILURE;
    }

    pmRequest = CONTAINER_OF(para, struct HdfPmRequest, task);
    tokenIf = (struct IPowerStateToken *)pmRequest->token;
    if (pmRequest->pmType == HDF_PM_REQUEST_ACQUIRE) {
        if ((tokenIf != NULL) && (tokenIf->AcquireWakeLock != NULL)) {
            tokenIf->AcquireWakeLock(tokenIf);
        }
    } else if (pmRequest->pmType == HDF_PM_REQUEST_RELEASE) {
        if ((tokenIf != NULL) && (tokenIf->ReleaseWakeLock != NULL)) {
            tokenIf->ReleaseWakeLock(tokenIf);
        }
    }
    OsalMemFree(pmRequest);
    return HDF_SUCCESS;
}

void HdfPmTaskPut(struct PowerStateToken *powerToken, HDF_PM_REQUEST_TYPE type)
{
    struct HdfPmRequest *pmRequest = NULL;
    struct PmTaskQueue *pmTaskQueue = NULL;

    if (powerToken == NULL) {
        return;
    }

    pmTaskQueue = HdfPmTaskQueueInstance();
    pmRequest = (struct HdfPmRequest *)OsalMemCalloc(sizeof(*pmRequest));
    if (pmRequest == NULL) {
        HDF_LOGI("%s OsalMemCalloc fail", __func__);
        return;
    }

    pmRequest->token = powerToken;
    pmRequest->pmType = type;
    pmRequest->task.func = PmTaskFunc;
    HdfTaskEnqueue(pmTaskQueue->taskQueue, &pmRequest->task);
}

int32_t HdfPowerManagerInit(void)
{
    DevMgrPmRegister();
    HdfPmTaskQueueInit(NULL);
    return HDF_SUCCESS;
}

void HdfPowerManagerExit(void)
{
    HdfPmTaskQueueDestroy();
}