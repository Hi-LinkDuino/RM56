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

#include "pake_v1_protocol_task_common.h"
#include "das_standard_token_manager.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "pake_v1_protocol_common.h"
#include "pake_task_common.h"

#define ASCII_CASE_DIFFERENCE_VALUE 32

void DestroyDasPakeV1Params(PakeParams *params)
{
    if (params == NULL) {
        return;
    }

    DestroyPakeV1BaseParams(&(params->baseParams));

    if (params->returnKey.val != NULL) {
        (void)memset_s(params->returnKey.val, params->returnKey.length, 0, params->returnKey.length);
        HcFree(params->returnKey.val);
        params->returnKey.val = NULL;
    }

    HcFree(params->packageName);
    params->packageName = NULL;

    HcFree(params->serviceType);
    params->serviceType = NULL;

    HcFree(params->nonce.val);
    params->nonce.val = NULL;
}

static int32_t AllocReturnKey(PakeParams *params, const CJson *in)
{
    if (params->opCode == OP_UNBIND) {
        params->returnKey.val = NULL;
        params->returnKey.length = 0;
        return HC_SUCCESS;
    }
    int32_t res = GetIntFromJson(in, FIELD_KEY_LENGTH, (int *)&(params->returnKey.length));
    if (res != HC_SUCCESS) {
        LOGD("Get key length failed, use default, res: %d", res);
        params->returnKey.length = DEFAULT_RETURN_KEY_LENGTH;
    }
    if (params->returnKey.length < MIN_OUTPUT_KEY_LEN || params->returnKey.length > MAX_OUTPUT_KEY_LEN) {
        LOGE("Output key length is invalid.");
        return HC_ERR_INVALID_LEN;
    }
    res = InitSingleParam(&params->returnKey, params->returnKey.length);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for returnKey failed, res: %d.", res);
    }
    return res;
}

int32_t InitDasPakeV1Params(PakeParams *params, const CJson *in)
{
    int32_t res = InitPakeV1BaseParams(&(params->baseParams));
    if (res != HC_SUCCESS) {
        LOGE("InitPakeV1BaseParams failed, res: %d.", res);
        goto ERR;
    }

    res = FillDasPakeParams(params, in);
    if (res != HC_SUCCESS) {
        LOGE("FillDasPakeParams failed, res: %d.", res);
        goto ERR;
    }

    res = AllocReturnKey(params, in);
    if (res != HC_SUCCESS) {
        LOGE("AllocReturnKey failed, res: %d.", res);
        goto ERR;
    }

    return HC_SUCCESS;
ERR:
    DestroyDasPakeV1Params(params);
    return res;
}

static void UpperToLowercase(Uint8Buff *hex)
{
    for (uint32_t i = 0; i < hex->length; i++) {
        if (hex->val[i] >= 'A' && hex->val[i] <= 'F') {
            hex->val[i] += ASCII_CASE_DIFFERENCE_VALUE;
        }
    }
}

static int32_t ConvertPsk(const Uint8Buff *srcPsk, PakeParams *params)
{
    int res = InitSingleParam(&(params->baseParams.psk), PAKE_PSK_LEN * BYTE_TO_HEX_OPER_LENGTH + 1);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for psk failed, res: %d.", res);
        return res;
    }

    if (ByteToHexString(srcPsk->val, srcPsk->length, (char *)params->baseParams.psk.val,
        params->baseParams.psk.length) != HC_SUCCESS) {
        LOGE("Convert psk from byte to hex string failed.");
        return HC_ERR_CONVERT_FAILED;
    }
    params->baseParams.psk.length = params->baseParams.psk.length - 1; // do not need include '\0' when using psk
    (void)UpperToLowercase(&(params->baseParams.psk));
    return res;
}

int32_t FillPskWithDerivedKeyHex(PakeParams *params)
{
    int32_t res;
    if (!(params->baseParams.isClient)) {
        res = params->baseParams.loader->generateRandom(&(params->nonce));
        if (res != HC_SUCCESS) {
            LOGE("Generate nonce failed, res: %d.", res);
            return res;
        }
    }
    uint8_t pskKeyAliasVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff pskKeyAlias = { pskKeyAliasVal, PAKE_KEY_ALIAS_LEN };
    Uint8Buff packageName = { (uint8_t *)params->packageName, strlen(params->packageName) };
    Uint8Buff serviceType = { (uint8_t *)params->serviceType, strlen(params->serviceType) };
    res = GenerateKeyAlias(&packageName, &serviceType, KEY_ALIAS_PSK, &(params->baseParams.idPeer), &pskKeyAlias);
    if (res != HC_SUCCESS) {
        LOGE("GenerateKeyAlias for psk failed, res: %d.", res);
        return res;
    }

    LOGI("Psk alias: %x%x%x%x****.", pskKeyAliasVal[0], pskKeyAliasVal[1], pskKeyAliasVal[2], pskKeyAliasVal[3]);
    if (params->baseParams.loader->checkKeyExist(&pskKeyAlias) != HC_SUCCESS) {
        res = GetStandardTokenManagerInstance()->computeAndSavePsk(params);
        if (res != HC_SUCCESS) {
            LOGE("ComputeAndSavePsk failed, res: %d.", res);
            return res;
        }
    }

    Uint8Buff pskByte = { NULL, PAKE_PSK_LEN };
    pskByte.val = (uint8_t *)HcMalloc(PAKE_PSK_LEN, 0);
    if (pskByte.val == NULL) {
        LOGE("Malloc for pskByte failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff keyInfo = { (uint8_t *)TMP_AUTH_KEY_FACTOR, strlen(TMP_AUTH_KEY_FACTOR) };
    res = params->baseParams.loader->computeHkdf(&pskKeyAlias, &(params->nonce), &keyInfo, &pskByte, true);
    if (res != HC_SUCCESS) {
        LOGE("ComputeHkdf for psk failed, res: %d.", res);
        goto ERR;
    }

    res = ConvertPsk(&pskByte, params);
    if (res != HC_SUCCESS) {
        LOGE("ConvertPsk failed, res: %d.", res);
        goto ERR;
    }
    goto OUT;
ERR:
    FreeAndCleanKey(&(params->baseParams.psk));
OUT:
    FreeAndCleanKey(&pskByte);
    return res;
}