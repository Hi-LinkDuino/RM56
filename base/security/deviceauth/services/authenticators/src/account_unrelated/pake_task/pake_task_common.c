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

#include "pake_task_common.h"
#include "das_module_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"

int32_t ConstructOutJson(const PakeParams *params, CJson *out)
{
    int32_t res;
    CJson *payload = NULL;
    CJson *sendToPeer = NULL;

    payload = CreateJson();
    if (payload == NULL) {
        LOGE("Create payload json failed.");
        res =  HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    sendToPeer = CreateJson();
    if (sendToPeer == NULL) {
        LOGE("Create sendToPeer json failed.");
        res =  HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    if (params->opCode == AUTHENTICATE) {
        res = AddIntToJson(sendToPeer, FIELD_AUTH_FORM, AUTH_FORM_ACCOUNT_UNRELATED);
        if (res != HC_SUCCESS) {
            LOGE("Add authForm failed, res: %d.", res);
            goto ERR;
        }
    }
    res = AddObjToJson(sendToPeer, FIELD_PAYLOAD, payload);
    if (res != HC_SUCCESS) {
        LOGE("Add payload to sendToPeer failed, res: %d.", res);
        goto ERR;
    }

    res = AddObjToJson(out, FIELD_SEND_TO_PEER, sendToPeer);
    if (res != HC_SUCCESS) {
        LOGE("Add sendToPeer to out failed, res: %d.", res);
        goto ERR;
    }
ERR:
    FreeJson(payload);
    FreeJson(sendToPeer);
    return res;
}

static int32_t GenerateOutputKey(PakeParams *params)
{
    Uint8Buff keyInfo = { (uint8_t *)HICHAIN_RETURN_KEY, strlen(HICHAIN_RETURN_KEY) };
    int32_t res = params->baseParams.loader->computeHkdf(&(params->baseParams.sessionKey), &(params->baseParams.salt),
        &keyInfo, &(params->returnKey), false);
    if (res != HC_SUCCESS) {
        LOGE("Generate returnKey failed.");
        FreeAndCleanKey(&(params->returnKey));
    }
    FreeAndCleanKey(&(params->baseParams.sessionKey));
    return res;
}

int32_t SendResultToSelf(PakeParams *params, CJson *out)
{
    int res;
    CJson *sendToSelf = CreateJson();
    if (sendToSelf == NULL) {
        LOGE("Create sendToSelf json failed.");
        res = HC_ERR_JSON_CREATE;
        goto ERR;
    }
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToSelf, FIELD_OPERATION_CODE, params->opCode), res);
    GOTO_ERR_AND_SET_RET(AddIntToJson(sendToSelf, FIELD_AUTH_FORM, AUTH_FORM_ACCOUNT_UNRELATED), res);

    if (params->returnKey.length != 0) { /* keyLen == 0 means that returnKey needn't to be generated. */
        res = GenerateOutputKey(params);
        if (res != HC_SUCCESS) {
            LOGE("GenerateOutputKey failed, res: %x.", res);
            goto ERR;
        }
        GOTO_ERR_AND_SET_RET(AddByteToJson(sendToSelf, FIELD_SESSION_KEY, params->returnKey.val,
            params->returnKey.length), res);
    }

    GOTO_ERR_AND_SET_RET(AddObjToJson(out, FIELD_SEND_TO_SELF, sendToSelf), res);
ERR:
    FreeAndCleanKey(&(params->returnKey));
    ClearSensitiveStringInJson(sendToSelf, FIELD_SESSION_KEY);
    FreeJson(sendToSelf);
    return res;
}

static int32_t FillPskWithPin(PakeParams *params, const CJson *in)
{
    const char *pinString = GetStringFromJson(in, FIELD_PIN_CODE);
    if (pinString == NULL) {
        LOGE("Get pin code failed.");
        return HC_ERR_JSON_GET;
    }
    if (strlen(pinString) < MIN_PIN_LEN || strlen(pinString) > MAX_PIN_LEN) {
        LOGE("Pin code len is invalid.");
        return HC_ERR_INVALID_LEN;
    }

    int res = InitSingleParam(&(params->baseParams.psk), strlen(pinString));
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for psk failed, res: %d.", res);
        return res;
    }
    if (memcpy_s(params->baseParams.psk.val, params->baseParams.psk.length,
        pinString, strlen(pinString)) != HC_SUCCESS) {
        LOGE("Memcpy for pin code failed.");
        FreeAndCleanKey(&params->baseParams.psk);
        return HC_ERR_MEMORY_COPY;
    }

    return HC_SUCCESS;
}

static int32_t FillAuthId(PakeParams *params, const CJson *in)
{
    const char *authId = GetStringFromJson(in, FIELD_SELF_AUTH_ID);
    if (authId == NULL) {
        LOGE("Get self authId failed.");
        return HC_ERR_JSON_GET;
    }
    uint32_t authIdLen = strlen(authId);
    if (authIdLen == 0 || authIdLen > MAX_AUTH_ID_LEN) {
        LOGE("Invalid self authId length: %d.", authIdLen);
        return HC_ERR_INVALID_LEN;
    }
    params->baseParams.idSelf.length = authIdLen;
    params->baseParams.idSelf.val = (uint8_t *)HcMalloc(params->baseParams.idSelf.length, 0);
    if (params->baseParams.idSelf.val == NULL) {
        LOGE("Malloc for idSelf failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(params->baseParams.idSelf.val, params->baseParams.idSelf.length, authId, strlen(authId)) != EOK) {
        LOGE("Memcpy for idSelf failed.");
        return HC_ERR_MEMORY_COPY;
    }

    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        authId = GetStringFromJson(in, FIELD_PEER_AUTH_ID);
        if (authId == NULL) {
            LOGE("Get peer authId failed.");
            return HC_ERR_JSON_GET;
        }
        authIdLen = strlen(authId);
        if (authIdLen == 0 || authIdLen > MAX_AUTH_ID_LEN) {
            LOGE("Invalid peer authId length: %d.", authIdLen);
            return HC_ERR_INVALID_LEN;
        }
        params->baseParams.idPeer.length = authIdLen;
        params->baseParams.idPeer.val = (uint8_t *)HcMalloc(params->baseParams.idPeer.length, 0);
        if (params->baseParams.idPeer.val == NULL) {
            LOGE("Malloc for idPeer failed.");
            return HC_ERR_ALLOC_MEMORY;
        }
        if (memcpy_s(params->baseParams.idPeer.val, params->baseParams.idPeer.length, authId, strlen(authId)) != EOK) {
            LOGE("Memcpy for idPeer failed.");
            return HC_ERR_MEMORY_COPY;
        }
    }

    return HC_SUCCESS;
}

static int32_t FillUserType(PakeParams *params, const CJson *in)
{
    if (GetIntFromJson(in, FIELD_SELF_TYPE, &(params->userType)) != HC_SUCCESS) {
        LOGE("Get self userType failed");
        return HC_ERR_JSON_GET;
    }

    if (GetIntFromJson(in, FIELD_PEER_USER_TYPE, &(params->userTypePeer)) != HC_SUCCESS) {
        LOGD("Get peer userType failed, use default.");
        params->userTypePeer = DEVICE_TYPE_ACCESSORY;
    }
    return HC_SUCCESS;
}

static int32_t FillPkgNameAndServiceType(PakeParams *params, const CJson *in)
{
    const char *packageName = GetStringFromJson(in, FIELD_PKG_NAME);
    if (packageName == NULL) {
        LOGE("Get packageName failed.");
        return HC_ERR_JSON_GET;
    }
    params->packageName = (char *)HcMalloc(strlen(packageName) + 1, 0);
    if (params->packageName == NULL) {
        LOGE("Malloc for packageName failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(params->packageName, strlen(packageName) + 1, packageName, strlen(packageName)) != EOK) {
        LOGE("Memcpy for packageName failed.");
        return HC_ERR_MEMORY_COPY;
    }

    const char *serviceType = GetStringFromJson(in, FIELD_SERVICE_TYPE);
    if (serviceType == NULL) {
        LOGE("Get serviceType failed.");
        return HC_ERR_JSON_GET;
    }
    params->serviceType = (char *)HcMalloc(strlen(serviceType) + 1, 0);
    if (params->serviceType == NULL) {
        LOGE("Malloc for serviceType failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(params->serviceType, strlen(serviceType) + 1, serviceType, strlen(serviceType)) != EOK) {
        LOGE("Memcpy for serviceType failed.");
        return HC_ERR_MEMORY_COPY;
    }

    return HC_SUCCESS;
}

static int32_t FillNonce(PakeParams *params)
{
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        params->nonce.length = PAKE_NONCE_LEN;
        params->nonce.val = (uint8_t *)HcMalloc(params->nonce.length, 0);
        if (params->nonce.val == NULL) {
            LOGE("Malloc for nonce failed.");
            return HC_ERR_ALLOC_MEMORY;
        }
    } else {
        params->nonce.length = 0;
        params->nonce.val = NULL;
    }
    return HC_SUCCESS;
}

int32_t FillDasPakeParams(PakeParams *params, const CJson *in)
{
    if (GetIntFromJson(in, FIELD_OPERATION_CODE, &(params->opCode)) != HC_SUCCESS) {
        LOGD("Get opCode failed, use default.");
        params->opCode = AUTHENTICATE;
    }
    if (params->opCode != OP_BIND && params->opCode != OP_UNBIND &&
        params->opCode != AUTHENTICATE && params->opCode != AUTH_KEY_AGREEMENT) {
        LOGE("Unsupported opCode: %d.", params->opCode);
        return HC_ERR_NOT_SUPPORT;
    }

    if (GetBoolFromJson(in, FIELD_IS_CLIENT, &(params->baseParams.isClient)) != HC_SUCCESS) {
        LOGE("Get isClient failed.");
        return HC_ERR_JSON_GET;
    }

    int res = FillNonce(params);
    if (res != HC_SUCCESS) {
        return res;
    }

    if (params->opCode != AUTH_KEY_AGREEMENT) {
        res = FillUserType(params, in);
        if (res != HC_SUCCESS) {
            return res;
        }

        res = FillPkgNameAndServiceType(params, in);
        if (res != HC_SUCCESS) {
            return res;
        }
    }

    res = FillAuthId(params, in);
    if (res != HC_SUCCESS) {
        return res;
    }

    if (params->opCode == OP_BIND || params->opCode == AUTH_KEY_AGREEMENT) {
        res = FillPskWithPin(params, in);
        if (res != HC_SUCCESS) {
            return res;
        }
    }

    params->baseParams.curveType = CURVE_25519;
#ifdef P2P_PAKE_DL_PRIME_LEN_384
    params->baseParams.supportedDlPrimeMod = (uint32_t)params->baseParams.supportedDlPrimeMod | DL_PRIME_MOD_384;
#endif
#ifdef P2P_PAKE_DL_PRIME_LEN_256
    params->baseParams.supportedDlPrimeMod = (uint32_t)params->baseParams.supportedDlPrimeMod | DL_PRIME_MOD_256;
#endif
    return HC_SUCCESS;
}
