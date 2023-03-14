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

#include "standard_exchange_message_util.h"
#include "common_defs.h"
#include "hc_log.h"
#include "hc_types.h"
#include "json_utils.h"
#include "protocol_common.h"
#include "pake_base_cur_task.h"
#include "string_util.h"

int32_t PackageNonceAndCipherToJson(const Uint8Buff *nonce, const Uint8Buff *cipher, CJson *data, const char *key)
{
    int32_t res = HC_SUCCESS;
    uint32_t exAuthInfoLen = nonce->length + cipher->length;
    uint8_t *exAuthInfoVal = (uint8_t *)HcMalloc(exAuthInfoLen, 0);
    if (exAuthInfoVal == NULL) {
        LOGE("malloc exAuthInfoVal failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    if (memcpy_s(exAuthInfoVal, exAuthInfoLen, nonce->val, nonce->length) != EOK) {
        LOGE("memcpy nonce failed");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    if (memcpy_s(exAuthInfoVal + nonce->length, exAuthInfoLen - nonce->length,
        cipher->val, cipher->length) != EOK) {
        LOGE("memcpy exInfoCipher failed");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    GOTO_ERR_AND_SET_RET(AddByteToJson(data, key, exAuthInfoVal, exAuthInfoLen), res);
ERR:
    HcFree(exAuthInfoVal);
    return res;
}

int32_t ParseNonceAndCipherFromJson(Uint8Buff *nonce, Uint8Buff *cipher, const CJson *in, const char *key)
{
    int32_t res = HC_SUCCESS;
    uint8_t *exAuthInfoVal = NULL;
    const char *exAuthInfoStr = GetStringFromJson(in, key);
    if (exAuthInfoStr == NULL) {
        LOGE("get exAuthInfoStr failed.");
        res = HC_ERR_JSON_GET;
        goto ERR;
    }
    int32_t exAuthInfoLen = strlen(exAuthInfoStr) / BYTE_TO_HEX_OPER_LENGTH;
    exAuthInfoVal = (uint8_t *)HcMalloc(exAuthInfoLen, 0);
    if (exAuthInfoVal == NULL) {
        LOGE("Malloc exAuthInfoVal failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }
    res = HexStringToByte(exAuthInfoStr, exAuthInfoVal, exAuthInfoLen);
    if (res != HC_SUCCESS) {
        LOGE("Convert exAuthInfo from hex string to byte failed.");
        goto ERR;
    }
    if (memcpy_s(nonce->val, nonce->length, exAuthInfoVal, nonce->length) != EOK) {
        LOGE("copy nonce failed!");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    res = InitSingleParam(cipher, exAuthInfoLen - nonce->length);
    if (res != HC_SUCCESS) {
        LOGE("init exInfoCipher failed");
        goto ERR;
    }
    if (memcpy_s(cipher->val, cipher->length, exAuthInfoVal + nonce->length,
        exAuthInfoLen - nonce->length) != EOK) {
        LOGE("copy exInfoCipher failed!");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
ERR:
    HcFree(exAuthInfoVal);
    return res;
}

int32_t GenerateSelfChallenge(PakeParams *params)
{
    int res = InitSingleParam(&(params->baseParams.challengeSelf), PAKE_CHALLENGE_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for challengeSelf failed, res: %d.", res);
        return res;
    }
    res = params->baseParams.loader->generateRandom(&(params->baseParams.challengeSelf));
    if (res != HC_SUCCESS) {
        LOGE("GenerateRandom for challengeSelf failed, res: %d.", res);
        return res;
    }
    return res;
}

int32_t GetPeerChallenge(PakeParams *params, const CJson *in)
{
    int res = InitSingleParam(&(params->baseParams.challengePeer), PAKE_CHALLENGE_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for challengePeer failed, res: %d.", res);
        return res;
    }
    res = GetByteFromJson(in, FIELD_CHALLENGE,
        params->baseParams.challengePeer.val, params->baseParams.challengePeer.length);
    if (res != HC_SUCCESS) {
        LOGE("Get challengePeer failed, res: %d.", res);
        return res;
    }
    return res;
}
