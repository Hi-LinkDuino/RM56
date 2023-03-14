/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iso_client_task.h"
#include "hc_log.h"
#include "hc_types.h"
#include "iso_client_bind_exchange_task.h"
#include "iso_client_protocol_task.h"
#include "iso_client_unbind_exchange_task.h"
#include "iso_task_common.h"

static int GetIsoClientTaskType(const struct SubTaskBaseT *task)
{
    IsoClientTask *realTask = (IsoClientTask *)task;
    if (realTask->curTask == NULL) {
        LOGE("CurTask is null.");
        return TASK_TYPE_NONE;
    }
    return realTask->curTask->getCurTaskType();
}

static void DestroyIsoClientTask(struct SubTaskBaseT *task)
{
    IsoClientTask *innerTask = (IsoClientTask *)task;
    if (innerTask == NULL) {
        return;
    }
    DestroyIsoParams(&(innerTask->params));
    if (innerTask->curTask != NULL) {
        innerTask->curTask->destroyTask(innerTask->curTask);
    }
    HcFree(innerTask);
}

static int CreateNextTask(IsoClientTask *realTask, const CJson *in, CJson *out, int32_t *status)
{
    int res = HC_SUCCESS;
    switch (realTask->params.opCode) {
        case OP_BIND:
            if (realTask->curTask->getCurTaskType() == TASK_TYPE_BIND_LITE_EXCHANGE) {
                LOGI("Bind exchange task end successfully.");
                *status = FINISH;
                return HC_SUCCESS;
            }
            realTask->curTask->destroyTask(realTask->curTask);
            realTask->curTask = CreateClientBindExchangeTask(&(realTask->params), in, out, status);
            if (realTask->curTask == NULL) {
                LOGE("CreateBindExchangeTask failed.");
                return HC_ERROR;
            }
            break;
        case OP_UNBIND:
            if (realTask->curTask->getCurTaskType() == TASK_TYPE_UNBIND_LITE_EXCHANGE) {
                LOGI("Unbind exchange task end successfully.");
                *status = FINISH;
                return HC_SUCCESS;
            }
            realTask->curTask->destroyTask(realTask->curTask);
            realTask->curTask = CreateClientUnbindExchangeTask(&(realTask->params), in, out, status);
            if (realTask->curTask == NULL) {
                LOGE("CreateBindExchangeTask failed");
                return HC_ERROR;
            }
            break;
        case AUTHENTICATE:
            res = GenEncResult(&(realTask->params), ISO_RESULT_CONFIRM_CMD, out, RESULT_AAD, true);
            if (res != HC_SUCCESS) {
                LOGE("GenEncResult failed, res:%d", res);
                return res;
            }
            LOGI("Authenticate task end successfully.");
            *status = FINISH;
            break;
        default:
            LOGE("Unsupported opCode: %d.", realTask->params.opCode);
            res = HC_ERR_NOT_SUPPORT;
    }
    return res;
}

static int Process(struct SubTaskBaseT *task, const CJson *in, CJson *out, int32_t *status)
{
    IsoClientTask *realTask = (IsoClientTask *)task;
    if (realTask->curTask == NULL) {
        LOGE("CurTask is null.");
        return HC_ERROR;
    }
    int res = realTask->curTask->process(realTask->curTask, &(realTask->params), in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("CurTask process failed, res: %x.", res);
        return res;
    }
    if (*status != FINISH) {
        return res;
    }
    return CreateNextTask(realTask, in, out, status);
}

SubTaskBase *CreateIsoClientTask(const CJson *in)
{
    IsoClientTask *task = (IsoClientTask *)HcMalloc(sizeof(IsoClientTask), 0);
    if (task == NULL) {
        LOGE("Malloc for IsoClientTask failed.");
        return NULL;
    }

    task->taskBase.getTaskType = GetIsoClientTaskType;
    task->taskBase.destroyTask = DestroyIsoClientTask;
    task->taskBase.process = Process;

    int res = InitIsoParams(&(task->params), in);
    if (res != HC_SUCCESS) {
        LOGE("InitIsoParams failed, res: %x.", res);
        DestroyIsoClientTask((struct SubTaskBaseT *)task);
        return NULL;
    }

    task->curTask = CreateProtocolClientTask();
    if (task->curTask == NULL) {
        LOGE("CreateProtocolClientTask failed.");
        DestroyIsoClientTask((struct SubTaskBaseT *)task);
        return NULL;
    }
    return (SubTaskBase *)task;
}
