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

#include "auth_session_common_util.h"
#include "common_defs.h"
#include "string_util.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#define UID_HASH_HEX_STRING_LEN_MAX 64
#define UID_HASH_HEX_STRING_LEN_MIN 10

static bool IsPeerUidHashLenValid(uint32_t peerUidHashLen)
{
    if ((peerUidHashLen < UID_HASH_HEX_STRING_LEN_MIN) || (peerUidHashLen > UID_HASH_HEX_STRING_LEN_MAX)) {
        LOGE("The input uid hash len is invalid, input uid hash in hex string len = %d", peerUidHashLen);
        return false;
    }
    return true;
}

static int32_t AddPeerIdToReqParam(const CJson *receiveData, CJson *reqParam)
{
    const char *peerId = GetStringFromJson(receiveData, FIELD_PEER_AUTH_ID);
    if (peerId == NULL) {
        LOGE("Failed to get peerId from the data transmitted by the client!");
        return HC_ERR_JSON_GET;
    }
    uint32_t peerIdLen = HcStrlen(peerId);
    if ((peerIdLen == 0) || (peerIdLen > MAX_AUTH_ID_LEN) || ((peerIdLen % 2) != 0)) { /* 2: even numbers. */
        LOGE("Invalid len of peerId!");
        return HC_ERR_JSON_GET;
    }
    uint32_t peerIdToRequestLen = peerIdLen / BYTE_TO_HEX_OPER_LENGTH;
    uint8_t *peerIdToRequest = (uint8_t *)HcMalloc(peerIdToRequestLen + 1, 0);
    if (peerIdToRequest == NULL) {
        LOGE("Malloc failed for peerIdToRequest!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = HexStringToByte(peerId, peerIdToRequest, peerIdToRequestLen);
    if (res != HC_SUCCESS) {
        LOGE("Failed to convert peerId to byte, res: %d!", res);
        HcFree(peerIdToRequest);
        return res;
    }
    if (AddStringToJson(reqParam, FIELD_PEER_AUTH_ID, (const char *)peerIdToRequest) != HC_SUCCESS) {
        LOGE("Failed to add reqParam: peerId for onRequest!");
        HcFree(peerIdToRequest);
        return HC_ERR_JSON_FAIL;
    }
    HcFree(peerIdToRequest);
    return HC_SUCCESS;
}

char *GetServerConfirmation(const CJson *paramsFromClient, const CJson *reqParam,
    const DeviceAuthCallback *callback)
{
    int64_t requestId = 0;
    if (GetByteFromJson(paramsFromClient, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return NULL;
    }
    char *reqParamStr = PackJsonToString(reqParam);
    if (reqParamStr == NULL) {
        LOGE("Failed to pack reqParam!");
        return NULL;
    }
    char *serverInfo = NULL;
    do {
        int32_t authForm = AUTH_FORM_INVALID_TYPE;
        if (GetIntFromJson(paramsFromClient, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
            LOGE("Failed to get authForm in received data!");
            break;
        }
        if ((callback == NULL) || (callback->onRequest == NULL)) {
            LOGE("Failed to get request callback!");
            break;
        }
        LOGD("Begin to invoke onRequest for auth!");
        serverInfo = callback->onRequest(requestId, authForm, reqParamStr);
        LOGD("End to invoke onRequest for auth!");
        if (serverInfo == NULL) {
            LOGE("Failed to get server confirmation info!");
        }
    } while (0);
    FreeJsonString(reqParamStr);
    return serverInfo;
}


int32_t GetGeneralReqParams(const CJson *receiveData, CJson *reqParam)
{
    const char *pkgName = GetStringFromJson(receiveData, FIELD_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Failed to get pkgName from the data transmitted by the client!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(reqParam, FIELD_SERVICE_PKG_NAME, pkgName) != HC_SUCCESS) {
        LOGE("Failed to add reqParam: pkgName for onRequest!");
        return HC_ERR_JSON_FAIL;
    }
    const char *serviceType = GetStringFromJson(receiveData, FIELD_SERVICE_TYPE);
    if (serviceType == NULL) {
        LOGE("Failed to get serviceType from the data transmitted by the client!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(reqParam, FIELD_SERVICE_TYPE, serviceType) != HC_SUCCESS) {
        LOGE("Failed to add reqParam: serviceType for onRequest!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t res = AddPeerIdToReqParam(receiveData, reqParam);
    if (res != HC_SUCCESS) {
        LOGE("Failed to add peerId to reqParam!");
        return res;
    }
    int32_t peerType = 0;
    if (GetIntFromJson(receiveData, FIELD_PEER_USER_TYPE, &peerType) != HC_SUCCESS) {
        LOGE("Failed to get peerType from the data transmitted by the client!");
        return HC_ERR_JSON_GET;
    }
    if (AddIntToJson(reqParam, FIELD_PEER_USER_TYPE, peerType) != HC_SUCCESS) {
        LOGE("Failed to add reqParam: peerType for onRequest!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t keyLen = DEFAULT_RETURN_KEY_LENGTH;
    (void)GetIntFromJson(receiveData, FIELD_KEY_LENGTH, &keyLen);
    if (AddIntToJson(reqParam, FIELD_KEY_LENGTH, keyLen) != HC_SUCCESS) {
        LOGE("Failed to add reqParam: keyLen for onRequest!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

bool IsUidHashEqual(const char *uidHashInDb, const char *peerUidHash)
{
    if ((uidHashInDb == NULL) || (peerUidHash == NULL)) {
        LOGE("Input is null for uid hash!");
        return false;
    }
    char *peerUidHashToUpper = NULL;
    int32_t result = ToUpperCase(peerUidHash, &peerUidHashToUpper);
    if (result != HC_SUCCESS) {
        LOGE("Failed to convert the input uidHash to upper case!");
        return result;
    }
    uint32_t uidHashInDbLen = strlen(uidHashInDb);
    uint32_t peerUidHashLen = strlen(peerUidHash);
    if (!IsPeerUidHashLenValid(peerUidHashLen)) {
        HcFree(peerUidHashToUpper);
        peerUidHashToUpper = NULL;
        return false;
    }
    uint32_t cmpHashLen = (uidHashInDbLen > peerUidHashLen) ? peerUidHashLen : uidHashInDbLen;
    if (memcmp(uidHashInDb, peerUidHashToUpper, cmpHashLen) == EOK) {
        HcFree(peerUidHashToUpper);
        peerUidHashToUpper = NULL;
        return true;
    }
    HcFree(peerUidHashToUpper);
    peerUidHashToUpper = NULL;
    return false;
}