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

#include "standard_client_unbind_exchange_task.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "standard_exchange_message_util.h"

enum {
    TASK_STATUS_CLIENT_UNBIND_EXCHANGE_BEGIN = 0,
    TASK_STATUS_CLIENT_UNBIND_EXCHANGE_REQUEST,
    TASK_STATUS_CLIENT_UNBIND_EXCHANGE_CONFIRM,
};

static CurTaskType GetTaskType(void)
{
    return TASK_TYPE_UNBIND_STANDARD_EXCHANGE;
}

static int ExchangeRequest(AsyBaseCurTask *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    if (task->taskStatus != TASK_STATUS_CLIENT_UNBIND_EXCHANGE_BEGIN) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }
    StandardUnbindExchangeClientTask *realTask = (StandardUnbindExchangeClientTask *)task;
    CJson *data = CreateJson();
    if (data == NULL) {
        return HC_ERR_ALLOC_MEMORY;
    }
    CJson *sendToPeer = CreateJson();
    if (sendToPeer == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    // parse message
    if (params->baseParams.challengePeer.val == NULL) {
        GOTO_ERR_AND_SET_RET(GetPeerChallenge(params, in), res);
        GOTO_ERR_AND_SET_RET(GenerateSelfChallenge(params), res);
        GOTO_ERR_AND_SET_RET(AddByteToJson(data, FIELD_CHALLENGE, params->baseParams.challengeSelf.val,
            params->baseParams.challengeSelf.length), res);
    }

    // execute
    res = ClientRequestStandardUnbindExchange(params, &(realTask->params));
    if (res != HC_SUCCESS) {
        LOGE("ClientRequestStandardUnbindExchange failed");
        goto ERR;
    }

    // package message
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToPeer, FIELD_MESSAGE, PAKE_UNBIND_EXCHANGE_REQUEST), res);
    GOTO_ERR_AND_SET_RET(PackageNonceAndCipherToJson(&(realTask->params.nonce), &(realTask->params.exRmvCipher),
        data, FIELD_RMV_AUTH_INFO), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(sendToPeer, FIELD_PAYLOAD, data), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(out, FIELD_SEND_TO_PEER, sendToPeer), res);

    task->taskStatus = TASK_STATUS_CLIENT_UNBIND_EXCHANGE_REQUEST;
    *status = CONTINUE;
ERR:
    FreeJson(data);
    FreeJson(sendToPeer);
    return res;
}

static int ExchangeConfirm(AsyBaseCurTask *task, PakeParams *params, const CJson *in, int *status)
{
    int res;
    if (task->taskStatus < TASK_STATUS_CLIENT_UNBIND_EXCHANGE_REQUEST) {
        LOGE("task status failed");
        return HC_ERR_BAD_MESSAGE;
    }

    if (task->taskStatus > TASK_STATUS_CLIENT_UNBIND_EXCHANGE_REQUEST) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }

    StandardUnbindExchangeClientTask *realTask = (StandardUnbindExchangeClientTask *)task;

    // parse message
    RETURN_IF_ERR(ParseNonceAndCipherFromJson(&(realTask->params.nonce), &(realTask->params.resultCipher),
        in, FIELD_RMV_RETURN));

    // execute
    res = ClientConfirmStandardUnbindExchange(params, &(realTask->params));
    if (res != HC_SUCCESS) {
        LOGE("ClientRequestStandardUnbindExchange failed");
        return res;
    }

    task->taskStatus = TASK_STATUS_CLIENT_UNBIND_EXCHANGE_CONFIRM;
    *status = FINISH;
    return res;
}

static int Process(struct AsyBaseCurTaskT *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    if (task->taskStatus == TASK_STATUS_CLIENT_UNBIND_EXCHANGE_BEGIN) {
        res = ExchangeRequest(task, params, in, out, status);
        if (res != HC_SUCCESS) {
            goto ERR;
        }
        return res;
    }

    int message = 0;
    res = GetIntFromJson(in, "message", &message);
    if (res != HC_SUCCESS) {
        goto ERR;
    }

    switch (message) {
        case PAKE_UNBIND_EXCHANGE_RESPONSE:
            res = ExchangeConfirm(task, params, in, status);
            break;
        default:
            res = HC_ERR_INVALID_PARAMS;
            break;
    }
    if (res != HC_SUCCESS) {
        goto ERR;
    }
    return res;
ERR:
    FreeAndCleanKey(&(params->baseParams.sessionKey));
    return res;
}

static void DestroyStandardUnbindExchangeClientTask(struct AsyBaseCurTaskT *task)
{
    StandardUnbindExchangeClientTask *innerTask = (StandardUnbindExchangeClientTask *)task;
    if (innerTask == NULL) {
        return;
    }

    DestroyStandardUnbindExchangeParams(&(innerTask->params));
    HcFree(innerTask);
}

AsyBaseCurTask *CreateStandardUnbindExchangeClientTask()
{
    StandardUnbindExchangeClientTask *task =
        (StandardUnbindExchangeClientTask *)HcMalloc(sizeof(StandardUnbindExchangeClientTask), 0);
    if (task == NULL) {
        return NULL;
    }
    task->taskBase.destroyTask = DestroyStandardUnbindExchangeClientTask;
    task->taskBase.process = Process;
    task->taskBase.taskStatus = TASK_STATUS_CLIENT_UNBIND_EXCHANGE_BEGIN;
    task->taskBase.getCurTaskType = GetTaskType;

    int res = InitStandardUnbindExchangeParams(&(task->params));
    if (res != HC_SUCCESS) {
        DestroyStandardUnbindExchangeClientTask((struct AsyBaseCurTaskT *)task);
        return NULL;
    }

    return (AsyBaseCurTask *)task;
}