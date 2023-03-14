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

#include "iso_server_unbind_exchange_task.h"
#include "das_module_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "iso_task_common.h"
#include "string_util.h"

enum {
    TASK_TYPE_BEGIN = 1,
    TASK_TYPE_FINAL,
};

static CurTaskType GetTaskType(void)
{
    return TASK_TYPE_UNBIND_LITE_EXCHANGE;
}

static void DestroyServerUnbindExchangeTask(struct SymBaseCurTaskT *task)
{
    HcFree(task);
}

static int Process(struct SymBaseCurTaskT *task, IsoParams *params, const CJson *in, CJson *out, int32_t *status)
{
    (void)task;
    (void)params;
    (void)in;
    (void)out;
    (void)status;
    return HC_ERROR;
}

static int CheckRemoveInfo(const Uint8Buff *removeInfoBuf, const IsoParams *params)
{
    CJson *removeInfoJson = CreateJsonFromString((const char *)(removeInfoBuf->val));
    if (removeInfoJson == NULL) {
        LOGE("Get remove info json failed");
        return HC_ERR_JSON_GET;
    }
    int32_t userType = 0;
    uint8_t *peerAuthId = NULL;
    int res = GetIntFromJson(removeInfoJson, FIELD_RMV_TYPE, &userType);
    if (res != HC_SUCCESS) {
        LOGE("Get user type failed");
        goto ERR;
    }
    if (userType != params->peerUserType) {
        LOGE("User type not match :%d", userType);
        res = HC_ERR_JSON_GET;
        goto ERR;
    }
    peerAuthId = (uint8_t *)HcMalloc(params->baseParams.authIdPeer.length, 0);
    if (peerAuthId == NULL) {
        LOGE("Malloc failed");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    res = GetByteFromJson(removeInfoJson, FIELD_RMV_ID, peerAuthId, params->baseParams.authIdPeer.length);
    if (res != HC_SUCCESS) {
        LOGE("Get remove id failed, res:%d", res);
        goto ERR;
    }
    if (memcmp(peerAuthId, params->baseParams.authIdPeer.val, params->baseParams.authIdPeer.length) != 0) {
        LOGE("Compare peerAuthId failed");
        res = HC_ERROR;
    }
ERR:
    FreeJson(removeInfoJson);
    HcFree(peerAuthId);
    return res;
}

static char *GetAndCheckEncDataStr(const CJson *in, uint32_t *removeInfoFromJsonLen)
{
    char *removeInfoFromJson = (char *)GetStringFromJson(in, FIELD_ENC_DATA);
    if (removeInfoFromJson == NULL) {
        LOGE("No encData in unbind json");
        return NULL;
    }

    *removeInfoFromJsonLen = strlen(removeInfoFromJson);
    if (*removeInfoFromJsonLen <= TAG_LEN || *removeInfoFromJsonLen > MAX_BUFFER_LEN) {
        LOGE("The length of removeInfoFromJson is invalid.");
        return NULL;
    }
    return removeInfoFromJson;
}

static int DecryptRemoveInfo(const IsoParams *params, const CJson *in)
{
    int res;
    uint8_t *nonce = NULL;
    uint8_t *removeInfo = NULL;
    Uint8Buff encDataBuf = { NULL, 0 };

    nonce = (uint8_t *)HcMalloc(NONCE_SIZE, 0);
    if (nonce == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    GOTO_ERR_AND_SET_RET(GetByteFromJson(in, FIELD_NONCE, nonce, NONCE_SIZE), res);
    uint32_t removeInfoFromJsonLen = 0;
    char *removeInfoFromJson = GetAndCheckEncDataStr(in, &removeInfoFromJsonLen);
    if (removeInfoFromJson == NULL) {
        res = HC_ERR_JSON_GET;
        goto ERR;
    }

    uint32_t removeInfoLen = removeInfoFromJsonLen - TAG_LEN;
    removeInfo = (uint8_t *)HcMalloc(removeInfoLen, 0);
    if (removeInfo == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    encDataBuf.length = removeInfoFromJsonLen / BYTE_TO_HEX_OPER_LENGTH;
    encDataBuf.val = (uint8_t *)HcMalloc(encDataBuf.length, 0);
    if (encDataBuf.val == NULL) {
        LOGE("Malloc encDataBuf.val failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    res = HexStringToByte(removeInfoFromJson, encDataBuf.val, encDataBuf.length);
    if (res != HC_SUCCESS) {
        LOGE("HexStringToByte for encData failed.");
        goto ERR;
    }
    Uint8Buff removeInfoBuf = { removeInfo, removeInfoLen };
    GcmParam gcmParam = { nonce, NONCE_SIZE, (uint8_t *)UNBIND_ADD_REQUEST, HcStrlen(UNBIND_ADD_REQUEST) };
    res = params->baseParams.loader->aesGcmDecrypt(&params->baseParams.sessionKey, &encDataBuf, &gcmParam, false,
        &removeInfoBuf);
    if (res != 0) {
        LOGE("decrypt removeInfo failed, res:%d", res);
        goto ERR;
    }
    res = CheckRemoveInfo(&removeInfoBuf, params);
ERR:
    HcFree(nonce);
    HcFree(removeInfo);
    HcFree(encDataBuf.val);
    return res;
}

static int ServerUnbindExchangeStart(IsoParams *param, IsoServerUnbindExchangeTask *task,
    const CJson *in, CJson *out, int32_t *status)
{
    int res = DecryptRemoveInfo(param, in);
    if (res != 0) {
        return res;
    }

    res = GenEncResult(param, ISO_SERVER_UNBIND_EXCHANGE_RET, out, UNBIND_ADD_RESPONSE, false);
    if (res != 0) {
        LOGE("unbind exchange gen enc result failed, res:%d", res);
        goto ERR;
    }

    task->taskBase.taskStatus = TASK_TYPE_BEGIN;
    *status = FINISH;
ERR:
    return res;
}

SymBaseCurTask *CreateServerUnbindExchangeTask(IsoParams *params, const CJson *in, CJson *out, int32_t *status)
{
    (void)in;
    IsoServerUnbindExchangeTask *task = (IsoServerUnbindExchangeTask *)HcMalloc(sizeof(IsoServerUnbindExchangeTask), 0);
    if (task == NULL) {
        return NULL;
    }
    task->taskBase.destroyTask = DestroyServerUnbindExchangeTask;
    task->taskBase.process = Process;
    task->taskBase.getCurTaskType = GetTaskType;
    int res = ServerUnbindExchangeStart(params, task, in, out, status);
    if (res != 0) {
        DestroyServerUnbindExchangeTask((struct SymBaseCurTaskT *)task);
        return NULL;
    }
    return (SymBaseCurTask *)task;
}
