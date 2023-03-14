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

#include "pake_v1_server_protocol_task.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "hc_types.h"
#include "pake_message_util.h"
#include "pake_v1_protocol_common.h"
#include "pake_v1_protocol_task_common.h"
#include "pake_task_common.h"

enum {
    TASK_STATUS_SERVER_PAKE_BEGIN = 0,
    TASK_STATUS_SERVER_PAKE_RESPONSE,
    TASK_STATUS_SERVER_PAKE_CONFIRM
};

static CurTaskType GetTaskType(void)
{
    return TASK_TYPE_PAKE_V1_PROTOCOL;
}

static void DestroyPakeV1ProtocolServerTask(struct AsyBaseCurTaskT *task)
{
    HcFree(task);
}

static int PackageMsgForResponse(const PakeParams *params, CJson *out)
{
    int res = ConstructOutJson(params, out);
    if (res != HC_SUCCESS) {
        LOGE("ConstructOutJson failed, res: %d.", res);
        return res;
    }
    CJson *payload = GetObjFromJson(out, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Get payload from json failed.");
        return HC_ERR_JSON_GET;
    }
    res = PackagePakeResponseData(params, payload);
    if (res != HC_SUCCESS) {
        LOGE("PackagePakeResponseData failed, res: %d.", res);
        return res;
    }
    // package differentiated data
    res = AddByteToJson(payload, FIELD_CHALLENGE, params->baseParams.challengeSelf.val,
        params->baseParams.challengeSelf.length);
    if (res != HC_SUCCESS) {
        LOGE("Add challengeSelf failed, res: %d.", res);
        return res;
    }
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        res = AddByteToJson(payload, FIELD_PEER_AUTH_ID, params->baseParams.idSelf.val,
            params->baseParams.idSelf.length);
        if (res != HC_SUCCESS) {
            LOGE("Add idSelf failed, res: %d.", res);
            return res;
        }
    }
    return res;
}

static int PakeResponse(AsyBaseCurTask *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    if (task->taskStatus > TASK_STATUS_SERVER_PAKE_BEGIN) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }

    res = ParsePakeRequestMessage(params, in);
    if (res != HC_SUCCESS) {
        LOGE("ParsePakeRequestMessage failed, res: %d.", res);
        return res;
    }
    // parse differentiated data
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        res = GetAndCheckAuthIdPeer(in, &(params->baseParams.idSelf), &(params->baseParams.idPeer));
        if (res != HC_SUCCESS) {
            LOGE("GetAndCheckAuthIdPeer failed, res: %d.", res);
            return res;
        }
    }

    if (params->isPskSupported && (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND)) {
        res = FillPskWithDerivedKeyHex(params);
        if (res != HC_SUCCESS) {
            LOGE("FillPskWithDerivedKeyHex failed, res: %d.", res);
            return res;
        }
    }

    // execute
    res = ServerResponsePakeV1Protocol(&params->baseParams);
    if (res != HC_SUCCESS) {
        LOGE("ServerResponsePakeV1Protocol failed, res:%d", res);
        return res;
    }

    // package message
    res = PackageMsgForResponse(params, out);
    if (res != HC_SUCCESS) {
        LOGE("PackageMsgForResponse failed, res: %d.", res);
        return res;
    }

    task->taskStatus = TASK_STATUS_SERVER_PAKE_RESPONSE;
    *status = CONTINUE;
    return res;
}

static int PakeServerConfirm(AsyBaseCurTask *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    if (task->taskStatus < TASK_STATUS_SERVER_PAKE_RESPONSE) {
        LOGE("Invalid taskStatus: %d", task->taskStatus);
        return HC_ERR_BAD_MESSAGE;
    }
    if (task->taskStatus > TASK_STATUS_SERVER_PAKE_RESPONSE) {
        LOGI("The message is repeated, ignore it, status: %d", task->taskStatus);
        *status = IGNORE_MSG;
        return HC_SUCCESS;
    }

    // parse message
    res = ParsePakeClientConfirmMessage(params, in);
    if (res != HC_SUCCESS) {
        LOGE("ParsePakeClientConfirmMessage failed, res: %d", res);
        return res;
    }
    // differentiated data
    if (GetByteFromJson(in, FIELD_CHALLENGE, params->baseParams.challengePeer.val,
        params->baseParams.challengePeer.length) != HC_SUCCESS) {
        LOGE("Get challengePeer failed.");
        return HC_ERR_JSON_GET;
    }

    // execute
    res = ServerConfirmPakeV1Protocol(&params->baseParams);
    if (res != HC_SUCCESS) {
        LOGE("ServerConfirmPakeV1Protocol failed, res:%d", res);
        return res;
    }

    // package message
    res = ConstructOutJson(params, out);
    if (res != HC_SUCCESS) {
        LOGE("ConstructOutJson failed, res: %d.", res);
        return res;
    }
    CJson *payload = GetObjFromJson(out, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Get payload from json failed.");
        return HC_ERR_JSON_GET;
    }
    res = PackagePakeServerConfirmData(params, payload);
    if (res != HC_SUCCESS) {
        LOGE("PackagePakeServerConfirmData failed, res: %d.", res);
        return res;
    }

    task->taskStatus = TASK_STATUS_SERVER_PAKE_CONFIRM;
    *status = FINISH;
    return res;
}

static int Process(struct AsyBaseCurTaskT *task, PakeParams *params, const CJson *in, CJson *out, int *status)
{
    int res;
    uint32_t step = ProtocolMessageIn(in);
    if (step == INVALID_MESSAGE) {
        res = HC_ERR_BAD_MESSAGE;
        goto OUT;
    }

    switch (step) {
        case STEP_ONE:
            res = PakeResponse(task, params, in, out, status);
            break;
        case STEP_TWO:
            res = PakeServerConfirm(task, params, in, out, status);
            break;
        default:
            res = HC_ERR_BAD_MESSAGE;
            break;
    }
OUT:
    if (res != HC_SUCCESS) {
        LOGE("Process step:%d failed, res: %x.", step, res);
        return res;
    }
    res = ServerProtocolMessageOut(out, params->opCode, step);
    if (res != HC_SUCCESS) {
        LOGE("ServerProtocolMessageOut failed, res: %x.", res);
    }
    return res;
}

AsyBaseCurTask *CreatePakeV1ProtocolServerTask()
{
    PakeV1ProtocolServerTask *task = (PakeV1ProtocolServerTask *)HcMalloc(sizeof(PakeV1ProtocolServerTask), 0);
    if (task == NULL) {
        LOGE("Malloc for PakeV1ProtocolServerTask failed.");
        return NULL;
    }
    task->taskBase.destroyTask = DestroyPakeV1ProtocolServerTask;
    task->taskBase.process = Process;
    task->taskBase.taskStatus = TASK_STATUS_SERVER_PAKE_BEGIN;
    task->taskBase.getCurTaskType = GetTaskType;
    return (AsyBaseCurTask *)task;
}

