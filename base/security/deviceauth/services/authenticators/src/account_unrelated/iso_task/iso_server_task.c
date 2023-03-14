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

#include "iso_server_task.h"
#include "hc_log.h"
#include "hc_types.h"
#include "iso_server_bind_exchange_task.h"
#include "iso_server_protocol_task.h"
#include "iso_server_unbind_exchange_task.h"
#include "iso_task_common.h"

static int GetIsoServerTaskType(const struct SubTaskBaseT *task)
{
    IsoServerTask *realTask = (IsoServerTask *)task;
    if (realTask->curTask == NULL) {
        LOGE("CurTask is null.");
        return TASK_TYPE_NONE;
    }
    return realTask->curTask->getCurTaskType();
}

static void DestroyIsoServerTask(struct SubTaskBaseT *task)
{
    IsoServerTask *innerTask = (IsoServerTask *)task;
    if (innerTask == NULL) {
        return;
    }
    DestroyIsoParams(&(innerTask->params));
    if (innerTask->curTask != NULL) {
        innerTask->curTask->destroyTask(innerTask->curTask);
    }
    HcFree(innerTask);
}

static int CreateNextTask(IsoServerTask *realTask, const CJson *in, CJson *out, int32_t *status)
{
    int32_t message = 0;
    if (GetIntFromJson(in, FIELD_MESSAGE, &message) != 0) {
        LOGE("Get message code failed.");
        return HC_ERR_JSON_GET;
    }
    int res = HC_SUCCESS;
    switch (realTask->params.opCode) {
        case OP_BIND:
            if (message != ISO_CLIENT_BIND_EXCHANGE_CMD) {
                LOGI("The message is repeated, ignore it message: %d.", message);
                *status = IGNORE_MSG;
                break;
            }
            realTask->curTask = CreateServerBindExchangeTask(&(realTask->params), in, out, status);
            if (realTask->curTask == NULL) {
                LOGE("CreateBindExchangeTask failed");
                return HC_ERROR;
            }
            break;
        case OP_UNBIND:
            if (message != ISO_CLIENT_UNBIND_EXCHANGE_CMD) {
                LOGI("The message is repeated, ignore it message: %d.", message);
                *status = IGNORE_MSG;
                break;
            }
            realTask->curTask = CreateServerUnbindExchangeTask(&(realTask->params), in, out, status);
            if (realTask->curTask == NULL) {
                LOGE("CreateBindExchangeTask failed");
                return HC_ERROR;
            }
            break;
        case AUTHENTICATE:
            if ((res = CheckEncResult(&(realTask->params), in, RESULT_AAD)) != 0) {
                LOGE("CheckEncResult failed, res: %d.", res);
                break;
            }
            if ((res = SendResultToFinalSelf(&(realTask->params), out, true)) != 0) {
                LOGE("SendResultToFinalSelf failed, res: %d.", res);
                break;
            }
            LOGD("Authenticate task end.");
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
    IsoServerTask *realTask = (IsoServerTask *)task;
    if (realTask->curTask != NULL) {
        int res = realTask->curTask->process(realTask->curTask, &(realTask->params), in, out, status);
        if (res != HC_SUCCESS) {
            LOGE("CurTask processes failed, res: %x.", res);
        }
        if (*status == FINISH && (realTask->curTask->getCurTaskType() == TASK_TYPE_ISO_PROTOCOL)) {
            realTask->curTask->destroyTask(realTask->curTask);
            realTask->curTask = NULL;
            *status = CONTINUE;
        }
        return res;
    } else {
        return CreateNextTask(realTask, in, out, status);
    }
}

SubTaskBase *CreateIsoServerTask(const CJson *in)
{
    IsoServerTask *task = (IsoServerTask *)HcMalloc(sizeof(IsoServerTask), 0);
    if (task == NULL) {
        LOGE("Malloc for IsoServerTask failed.");
        return NULL;
    }

    task->taskBase.getTaskType = GetIsoServerTaskType;
    task->taskBase.destroyTask = DestroyIsoServerTask;
    task->taskBase.process = Process;

    int res = InitIsoParams(&(task->params), in);
    if (res != 0) {
        LOGE("InitIsoParams failed, res: %x.", res);
        DestroyIsoServerTask((struct SubTaskBaseT *)task);
        return NULL;
    }

    task->curTask = CreateProtocolServerTask();
    if (task->curTask == NULL) {
        LOGE("CreateProtocolServerTask failed.");
        DestroyIsoServerTask((struct SubTaskBaseT *)task);
        return NULL;
    }
    return (SubTaskBase *)task;
}
