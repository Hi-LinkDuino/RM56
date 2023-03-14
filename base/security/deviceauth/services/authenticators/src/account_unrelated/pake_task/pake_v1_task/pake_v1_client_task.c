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

#include "pake_v1_client_task.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "pake_v1_client_protocol_task.h"
#include "pake_v1_protocol_task_common.h"
#include "pake_task_common.h"
#include "standard_client_bind_exchange_task.h"
#include "standard_client_unbind_exchange_task.h"

static int GetPakeV1ClientTaskType(const struct SubTaskBaseT *task)
{
    PakeV1ClientTask *realTask = (PakeV1ClientTask *)task;
    if (realTask->curTask == NULL) {
        LOGE("CurTask is null.");
        return TASK_TYPE_NONE;
    }
    return realTask->curTask->getCurTaskType();
}

static void DestroyPakeV1ClientTask(struct SubTaskBaseT *task)
{
    PakeV1ClientTask *innerTask = (PakeV1ClientTask *)task;
    if (innerTask == NULL) {
        return;
    }

    DestroyDasPakeV1Params(&(innerTask->params));
    if (innerTask->curTask != NULL) {
        innerTask->curTask->destroyTask(innerTask->curTask);
    }
    HcFree(innerTask);
}

static int CreateAndProcessNextBindTask(PakeV1ClientTask *realTask, const CJson *in, CJson *out, int *status)
{
    realTask->curTask->destroyTask(realTask->curTask);
    realTask->curTask = CreateStandardBindExchangeClientTask();
    if (realTask->curTask == NULL) {
        LOGE("CreateStandardBindExchangeClientTask failed.");
        return HC_ERROR;
    }
    int res = realTask->curTask->process(realTask->curTask, &(realTask->params), in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("Process StandardBindExchangeClientTask failed.");
    }
    return res;
}

static int CreateAndProcessNextUnbindTask(PakeV1ClientTask *realTask, const CJson *in, CJson *out, int *status)
{
    realTask->curTask->destroyTask(realTask->curTask);
    realTask->curTask = CreateStandardUnbindExchangeClientTask();
    if (realTask->curTask == NULL) {
        LOGE("CreateStandardUnbindExchangeClientTask failed.");
        return HC_ERROR;
    }
    int res = realTask->curTask->process(realTask->curTask, &(realTask->params), in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("Process StandardUnbindExchangeClientTask failed.");
    }
    return res;
}

static int CreateNextTask(PakeV1ClientTask *realTask, const CJson *in, CJson *out, int *status)
{
    int res = HC_SUCCESS;
    switch (realTask->params.opCode) {
        case OP_BIND:
            if (realTask->curTask->getCurTaskType() == TASK_TYPE_BIND_STANDARD_EXCHANGE) {
                break;
            }
            *status = CONTINUE;
            res = CreateAndProcessNextBindTask(realTask, in, out, status);
            break;
        case OP_UNBIND:
            if (realTask->curTask->getCurTaskType() == TASK_TYPE_UNBIND_STANDARD_EXCHANGE) {
                break;
            }
            *status = CONTINUE;
            res = CreateAndProcessNextUnbindTask(realTask, in, out, status);
            break;
        case AUTH_KEY_AGREEMENT:
        case AUTHENTICATE:
            break;
        default:
            LOGE("Unsupported opCode: %d.", realTask->params.opCode);
            res = HC_ERR_NOT_SUPPORT;
    }
    if (res != HC_SUCCESS) {
        LOGE("Create and process next task failed, opcode: %d, res: %d.", realTask->params.opCode, res);
        return res;
    }
    if (*status != FINISH) {
        return res;
    }
    res = SendResultToSelf(&realTask->params, out);
    if (res != HC_SUCCESS) {
        LOGE("SendResultToSelf failed, res: %d", res);
        return res;
    }
    LOGI("End client task successfully, opcode: %d.", realTask->params.opCode);
    return res;
}

static int Process(struct SubTaskBaseT *task, const CJson *in, CJson *out, int *status)
{
    PakeV1ClientTask *realTask = (PakeV1ClientTask *)task;
    if (realTask->curTask == NULL) {
        LOGE("CurTask is null.");
        return HC_ERR_NULL_PTR;
    }

    realTask->params.baseParams.supportedPakeAlg = GetSupportedPakeAlg(&(realTask->taskBase.curVersion), PAKE_V1);
    realTask->params.isPskSupported = IsSupportedPsk(&(realTask->taskBase.curVersion));
    int res = realTask->curTask->process(realTask->curTask, &(realTask->params), in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("CurTask processes failed, res: %x.", res);
        return res;
    }
    if (*status != FINISH) {
        return res;
    }
    return CreateNextTask(realTask, in, out, status);
}

SubTaskBase *CreatePakeV1ClientTask(const CJson *in)
{
    PakeV1ClientTask *task = (PakeV1ClientTask *)HcMalloc(sizeof(PakeV1ClientTask), 0);
    if (task == NULL) {
        LOGE("Malloc for PakeV1ClientTask failed.");
        return NULL;
    }

    task->taskBase.getTaskType = GetPakeV1ClientTaskType;
    task->taskBase.destroyTask = DestroyPakeV1ClientTask;
    task->taskBase.process = Process;

    int res = InitDasPakeV1Params(&(task->params), in);
    if (res != HC_SUCCESS) {
        LOGE("Init das pake params failed, res: %d.", res);
        DestroyPakeV1ClientTask((struct SubTaskBaseT *)task);
        return NULL;
    }
    task->curTask = CreatePakeV1ProtocolClientTask();
    if (task->curTask == NULL) {
        LOGE("Create pake protocol client task failed.");
        DestroyPakeV1ClientTask((struct SubTaskBaseT *)task);
        return NULL;
    }
    return (SubTaskBase *)task;
}
