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

#include "pake_message_util.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "json_utils.h"
#include "pake_base_cur_task.h"
#include "protocol_common.h"
#include "string_util.h"

int32_t PackagePakeRequestData(const PakeParams *params, CJson *payload)
{
    // The value of is256ModSupported is true, when only 256 mode is supported
    bool is256ModSupported =
        (((uint32_t)params->baseParams.supportedDlPrimeMod | DL_PRIME_MOD_256) == DL_PRIME_MOD_256);
    if (AddBoolToJson(payload, FIELD_SUPPORT_256_MOD, is256ModSupported) != HC_SUCCESS) {
        LOGE("Add is256ModSupported failed.");
        return HC_ERR_JSON_ADD;
    }
    if (AddIntToJson(payload, FIELD_OPERATION_CODE, params->opCode) != HC_SUCCESS) {
        LOGE("Add opCode failed.");
        return HC_ERR_JSON_ADD;
    }
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        if (AddStringToJson(payload, FIELD_PKG_NAME, params->packageName) != HC_SUCCESS) {
            LOGE("Add packageName failed.");
            return HC_ERR_JSON_ADD;
        }
        if (AddStringToJson(payload, FIELD_SERVICE_TYPE, params->serviceType) != HC_SUCCESS) {
            LOGE("Add serviceType failed.");
            return HC_ERR_JSON_ADD;
        }
        if (AddIntToJson(payload, FIELD_PEER_USER_TYPE, params->userType) != HC_SUCCESS) {
            LOGE("Add userType failed.");
            return HC_ERR_JSON_ADD;
        }
    }
    return HC_SUCCESS;
}

int32_t ParsePakeRequestMessage(PakeParams *params, const CJson *in)
{
    bool is256ModSupported = true;
    if (GetBoolFromJson(in, FIELD_SUPPORT_256_MOD, &is256ModSupported) != HC_SUCCESS) {
        LOGE("Get is256ModSupported failed.");
        return HC_ERR_JSON_GET;
    }
    params->baseParams.supportedDlPrimeMod = is256ModSupported ?
        (params->baseParams.supportedDlPrimeMod & DL_PRIME_MOD_256) :
        (params->baseParams.supportedDlPrimeMod & DL_PRIME_MOD_384);

    /*
     * opCode: OP_UNBIND, pake v1: don't need returnKey, use default 0 length
     * opCode: OP_UNBIND, pake v2: don't need returnKey, use default 0 length
     * opCode: OP_BIND, pake v1: use default 32 length
     * opCode: OP_BIND, pake v2: don't need returnKey, use default 0 length
     * Therefore, only params->opCode == AUTHENTICATE or AUTH_KEY_AGREEMENT, need to get keyLen from message
     */
    if (params->opCode == AUTHENTICATE || params->opCode == AUTH_KEY_AGREEMENT) {
        int32_t res = GetAndCheckKeyLenOnServer(in, &(params->returnKey.length));
        if (res != HC_SUCCESS) {
            LOGE("GetAndCheckKeyLenOnServer failed, res: %d.", res);
            return res;
        }
    }

    return HC_SUCCESS;
}

int32_t PackagePakeResponseData(const PakeParams *params, CJson *payload)
{
    int32_t res = AddByteToJson(payload, FIELD_SALT, params->baseParams.salt.val, params->baseParams.salt.length);
    if (res != HC_SUCCESS) {
        LOGE("Add salt failed, res: %d.", res);
        return res;
    }
    res = AddByteToJson(payload, FIELD_EPK, params->baseParams.epkSelf.val, params->baseParams.epkSelf.length);
    if (res != HC_SUCCESS) {
        LOGE("Add epkSelf failed, res: %d.", res);
        return res;
    }
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        res = AddByteToJson(payload, FIELD_NONCE, params->nonce.val, params->nonce.length);
        if (res != HC_SUCCESS) {
            LOGE("Add nonce failed, res: %d.", res);
            return res;
        }
        res = AddIntToJson(payload, FIELD_PEER_USER_TYPE, params->userType);
        if (res != HC_SUCCESS) {
            LOGE("Add userType failed, res: %d.", res);
            return res;
        }
    }
    return res;
}

static int32_t GetDasEpkPeerFromJson(PakeParams *params, const CJson *in)
{
    const char *epkPeerHex = GetStringFromJson(in, FIELD_EPK);
    if (epkPeerHex == NULL) {
        LOGE("Get epkPeerHex failed.");
        return HC_ERR_JSON_GET;
    }
    int res = InitSingleParam(&(params->baseParams.epkPeer), strlen(epkPeerHex) / BYTE_TO_HEX_OPER_LENGTH);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for epkPeer failed, res: %d.", res);
        return res;
    }
    res = HexStringToByte(epkPeerHex, params->baseParams.epkPeer.val, params->baseParams.epkPeer.length);
    if (res != HC_SUCCESS) {
        LOGE("Convert epkPeer from hex string to byte failed, res: %d.", res);
        return res;
    }
    return res;
}

int32_t ParsePakeResponseMessage(PakeParams *params, const CJson *in)
{
    int32_t res = GetByteFromJson(in, FIELD_SALT, params->baseParams.salt.val, params->baseParams.salt.length);
    if (res != HC_SUCCESS) {
        LOGE("Get salt failed, res: %d.", res);
        return res;
    }
    res = GetDasEpkPeerFromJson(params, in);
    if (res != HC_SUCCESS) {
        LOGE("GetDasEpkPeerFromJson failed, res: %d.", res);
        return res;
    }
    if (params->opCode == AUTHENTICATE || params->opCode == OP_UNBIND) {
        res = GetByteFromJson(in, FIELD_NONCE, params->nonce.val, params->nonce.length);
        if (res != HC_SUCCESS) {
            LOGE("Get nonce failed, res: %d.", res);
            return res;
        }
    }

    return res;
}

int32_t PackagePakeClientConfirmData(const PakeParams *params, CJson *payload)
{
    int32_t res = AddByteToJson(payload, FIELD_EPK, params->baseParams.epkSelf.val, params->baseParams.epkSelf.length);
    if (res != HC_SUCCESS) {
        LOGE("Add epkSelf failed, res: %d.", res);
        return res;
    }
    res = AddByteToJson(payload, FIELD_KCF_DATA, params->baseParams.kcfData.val, params->baseParams.kcfData.length);
    if (res != HC_SUCCESS) {
        LOGE("Add kcfData failed, res: %d.", res);
        return res;
    }

    return res;
}

int32_t ParsePakeClientConfirmMessage(PakeParams *params, const CJson *in)
{
    int32_t res = GetByteFromJson(in, FIELD_KCF_DATA, params->baseParams.kcfDataPeer.val,
        params->baseParams.kcfDataPeer.length);
    if (res != HC_SUCCESS) {
        LOGE("Get kcfDataPeer failed, res: %d.", res);
        return res;
    }
    res = GetDasEpkPeerFromJson(params, in);
    if (res != HC_SUCCESS) {
        LOGE("GetDasEpkPeerFromJson failed, res: %d.", res);
        return res;
    }
    return res;
}

int32_t PackagePakeServerConfirmData(const PakeParams *params, CJson *payload)
{
    int32_t res = AddByteToJson(payload, FIELD_KCF_DATA,
        params->baseParams.kcfData.val, params->baseParams.kcfData.length);
    if (res != HC_SUCCESS) {
        LOGE("Add kcfData failed, res: %d.", res);
        return res;
    }
    return res;
}

int32_t ParsePakeServerConfirmMessage(PakeParams *params, const CJson *in)
{
    int32_t res = GetByteFromJson(in, FIELD_KCF_DATA, params->baseParams.kcfDataPeer.val,
        params->baseParams.kcfDataPeer.length);
    if (res != HC_SUCCESS) {
        LOGE("Get kcfDataPeer failed, res: %d.", res);
        return res;
    }
    return res;
}