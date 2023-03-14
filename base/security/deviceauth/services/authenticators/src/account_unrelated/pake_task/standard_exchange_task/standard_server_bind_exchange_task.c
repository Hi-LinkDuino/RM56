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

#include "standard_server_bind_exchange_task.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "standard_exchange_message_util.h"

enum {
    TASK_STATUS_SERVER_BIND_EXCHANGE_BEGIN = 0,
    TASK_STATUS_SERVER_BIND_EXCHANGE_START,
    TASK_STATUS_SERVER_BIND_EXCHANGE_RESPONSE,
};

static CurTaskType GetTaskType(void)
{
    return TASK_TYPE_BIND_STANDARD_EXCHANGE;
}

static int ExchangeStart(AsyBaseCurTask *task, PakeParams *params, CJson *out, int *status)
{
    int res = HC_SUCCESS;
    if (task->taskStatus != TASK_STATUS_SERVER_BIND_EXCHANGE_BEGIN) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }

    if (params->baseParams.challengeSelf.val == NULL) {
        res = GenerateSelfChallenge(params);
        if (res != HC_SUCCESS) {
            LOGE("GenerateSelfChallenge failed");
            return res;
        }
        CJson *payload = GetObjFromJson(out, FIELD_PAYLOAD);
        if (payload == NULL) {
            LOGE("Get payload failed.");
            return HC_ERR_JSON_GET;
        }
        RETURN_IF_ERR(AddByteToJson(payload, FIELD_CHALLENGE,
            params->baseParams.challengeSelf.val, params->baseParams.challengeSelf.length));
    }

    task->taskStatus = TASK_STATUS_SERVER_BIND_EXCHANGE_START;
    *status = CONTINUE;
    return res;
}

static int ExchangeResponse(AsyBaseCurTask *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res = HC_SUCCESS;
    CJson *data = NULL;
    CJson *sendToPeer = NULL;

    if (task->taskStatus < TASK_STATUS_SERVER_BIND_EXCHANGE_START) {
        return HC_ERR_BAD_MESSAGE;
    }
    if (task->taskStatus > TASK_STATUS_SERVER_BIND_EXCHANGE_START) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }

    StandardBindExchangeServerTask *realTask = (StandardBindExchangeServerTask *)task;

    // parse message
    /*
     * If failing to get userTypePeer, use the default value(DEVICE_TYPE_ACCESSORY),
     * which was assigned at initialization.
     */
    (void)GetIntFromJson(in, FIELD_PEER_USER_TYPE, &(params->userTypePeer));
    if (params->baseParams.challengePeer.val == NULL) {
        GOTO_ERR_AND_SET_RET(GetPeerChallenge(params, in), res);
    }
    GOTO_ERR_AND_SET_RET(ParseNonceAndCipherFromJson(&(realTask->params.nonce), &(realTask->params.exInfoCipher),
        in, FIELD_EX_AUTH_INFO), res);

    // execute
    res = ServerResponseStandardBindExchange(params, &(realTask->params));
    if (res != HC_SUCCESS) {
        LOGE("ServerResponseStandardBindExchange failed");
        return res;
    }

    sendToPeer = CreateJson();
    if (sendToPeer == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToPeer, FIELD_MESSAGE, PAKE_BIND_EXCHANGE_RESPONSE), res);
    data = CreateJson();
    if (data == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    GOTO_ERR_AND_SET_RET(PackageNonceAndCipherToJson(&(realTask->params.nonce), &(realTask->params.exInfoCipher),
        data, FIELD_EX_AUTH_INFO), res);
    GOTO_ERR_AND_SET_RET(AddIntToJson(data, FIELD_PEER_USER_TYPE, params->userType), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(sendToPeer, "payload", data), res);
    GOTO_ERR_AND_SET_RET(AddObjToJson(out, FIELD_SEND_TO_PEER, sendToPeer), res);

    task->taskStatus = TASK_STATUS_SERVER_BIND_EXCHANGE_RESPONSE;
    *status = FINISH;
ERR:
    FreeJson(data);
    FreeJson(sendToPeer);
    return res;
}

static int Process(struct AsyBaseCurTaskT *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    if (task->taskStatus == TASK_STATUS_SERVER_BIND_EXCHANGE_BEGIN) {
        res = ExchangeStart(task, params, out, status);
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
        case PAKE_BIND_EXCHANGE_REQUEST:
            res = ExchangeResponse(task, params, in, out, status);
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

static void DestroyStandardBindExchangeServerTask(struct AsyBaseCurTaskT *task)
{
    StandardBindExchangeServerTask *innerTask = (StandardBindExchangeServerTask *)task;
    if (innerTask == NULL) {
        return;
    }

    DestroyStandardBindExchangeParams(&(innerTask->params));
    HcFree(innerTask);
}

AsyBaseCurTask *CreateStandardBindExchangeServerTask()
{
    StandardBindExchangeServerTask *task =
        (StandardBindExchangeServerTask *)HcMalloc(sizeof(StandardBindExchangeServerTask), 0);
    if (task == NULL) {
        return NULL;
    }
    task->taskBase.destroyTask = DestroyStandardBindExchangeServerTask;
    task->taskBase.process = Process;
    task->taskBase.taskStatus = TASK_STATUS_SERVER_BIND_EXCHANGE_BEGIN;
    task->taskBase.getCurTaskType = GetTaskType;

    int res = InitStandardBindExchangeParams(&(task->params));
    if (res != 0) {
        DestroyStandardBindExchangeServerTask((struct AsyBaseCurTaskT *)task);
        return NULL;
    }

    return (AsyBaseCurTask *)task;
}

