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

#include "common_standard_unbind_exchange.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "securec.h"

int32_t InitStandardUnbindExchangeParams(StandardUnbindExchangeParams *params)
{
    if (params == NULL) {
        return HC_ERR_INVALID_PARAMS;
    }

    int32_t res;

    params->nonce.length = STANDARD_UNBIND_EXCHANGE_NONCE_LEN;
    params->nonce.val = (uint8_t *)HcMalloc(params->nonce.length, 0);
    if (params->nonce.val == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    params->rmvInfo.length = 0;
    params->rmvInfo.val = NULL;
    params->exRmvCipher.length = 0;
    params->exRmvCipher.val = NULL;
    params->resultCipher.length = 0;
    params->resultCipher.val = NULL;

    return HC_SUCCESS;
ERR:
    DestroyStandardUnbindExchangeParams(params);
    return res;
}

void DestroyStandardUnbindExchangeParams(StandardUnbindExchangeParams *params)
{
    if (params == NULL) {
        return;
    }
    if (params->rmvInfo.val != NULL) {
        HcFree(params->rmvInfo.val);
        params->rmvInfo.val = NULL;
    }
    if (params->exRmvCipher.val != NULL) {
        HcFree(params->exRmvCipher.val);
        params->exRmvCipher.val = NULL;
    }
    if (params->resultCipher.val != NULL) {
        HcFree(params->resultCipher.val);
        params->resultCipher.val = NULL;
    }
    if (params->nonce.val != NULL) {
        HcFree(params->nonce.val);
        params->nonce.val = NULL;
    }
}

static int32_t PackageRmvInfo(const PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = HC_SUCCESS;
    CJson *rmvInfoJson = CreateJson();
    char *rmvInfoStr = NULL;
    GOTO_ERR_AND_SET_RET(AddByteToJson(rmvInfoJson, FIELD_RMV_ID, pakeParams->baseParams.idSelf.val,
        pakeParams->baseParams.idSelf.length), res);
    GOTO_ERR_AND_SET_RET(AddIntToJson(rmvInfoJson, FIELD_RMV_TYPE, pakeParams->userType), res);
    rmvInfoStr = PackJsonToString(rmvInfoJson);
    if (rmvInfoStr == NULL) {
        LOGE("rmvInfoStr PackJsonToString failed");
        res = HC_ERR_PACKAGE_JSON_TO_STRING_FAIL;
        goto ERR;
    }

    res = InitSingleParam(&exchangeParams->rmvInfo, strlen(rmvInfoStr));
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam rmvInfo failed.");
        goto ERR;
    }

    if (memcpy_s(exchangeParams->rmvInfo.val, exchangeParams->rmvInfo.length, rmvInfoStr, strlen(rmvInfoStr)) != EOK) {
        LOGE("Memcpy rmvInfo failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
ERR:
    FreeJson(rmvInfoJson);
    FreeJsonString(rmvInfoStr);
    return res;
}

static int32_t EncryptRmvInfo(const PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = InitSingleParam(&(exchangeParams->exRmvCipher), exchangeParams->rmvInfo.length + AE_TAG_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitRmvInfoCipher failed");
        return res;
    }

    // encrypt
    res = pakeParams->baseParams.loader->generateRandom(&(exchangeParams->nonce));
    if (res != HC_SUCCESS) {
        LOGE("generateRandom failed");
        return res;
    }

    GcmParam encryptInfo = {
        .aad = (uint8_t *)HICHAIN_REMOVE_INFO_REQUEST,
        .aadLen = strlen(HICHAIN_REMOVE_INFO_REQUEST),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };
    res = pakeParams->baseParams.loader->aesGcmEncrypt(&(pakeParams->baseParams.sessionKey), &(exchangeParams->rmvInfo),
        &encryptInfo, false, &(exchangeParams->exRmvCipher));
    if (res != HC_SUCCESS) {
        LOGE("aesGcmEncrypt failed");
        return res;
    }

    return HC_SUCCESS;
}

static int32_t DecryptRmvInfo(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = InitSingleParam(&exchangeParams->rmvInfo, exchangeParams->exRmvCipher.length - AE_TAG_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam rmvInfo failed.");
        return res;
    }

    GcmParam decryptInfo = {
        .aad = (uint8_t *)HICHAIN_REMOVE_INFO_REQUEST,
        .aadLen = strlen(HICHAIN_REMOVE_INFO_REQUEST),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };

    res = pakeParams->baseParams.loader->aesGcmDecrypt(&(pakeParams->baseParams.sessionKey),
        &(exchangeParams->exRmvCipher), &decryptInfo, false, &(exchangeParams->rmvInfo));
    if (res != HC_SUCCESS) {
        LOGE("aesGcmDecrypt failed");
        return res;
    }

    return res;
}

static int32_t ParseRmvInfo(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res;
    CJson *rmvInfoJson = CreateJsonFromString((char *)exchangeParams->rmvInfo.val);
    if (rmvInfoJson == NULL) {
        LOGE("Create rmvInfoJson failed.");
        return HC_ERR_JSON_CREATE;
    }
    GOTO_ERR_AND_SET_RET(GetIntFromJson(rmvInfoJson, FIELD_RMV_TYPE, &(pakeParams->userTypePeer)), res);
    res = GetIdPeer(rmvInfoJson, FIELD_RMV_ID, &pakeParams->baseParams.idSelf, &pakeParams->baseParams.idPeer);
    if (res != HC_SUCCESS) {
        LOGE("GetIdPeer failed, res: %d.", res);
        goto ERR;
    }
ERR:
    FreeJson(rmvInfoJson);
    return res;
}

static int32_t DeleteAuthInfo(PakeParams *pakeParams)
{
    uint8_t keyAliasVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff keyAlias = { keyAliasVal, PAKE_KEY_ALIAS_LEN };
    KeyAliasType keyType = pakeParams->userTypePeer;
    Uint8Buff packageName = { (uint8_t *)pakeParams->packageName, strlen(pakeParams->packageName) };
    Uint8Buff serviceType = { (uint8_t *)pakeParams->serviceType, strlen(pakeParams->serviceType) };
    int32_t res = GenerateKeyAlias(&packageName, &serviceType, keyType, &(pakeParams->baseParams.idPeer), &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("generate pubKey alias failed");
        return res;
    }
    LOGI("PubKey alias: %x%x%x%x****.", keyAliasVal[0], keyAliasVal[1], keyAliasVal[2], keyAliasVal[3]);
    res = pakeParams->baseParams.loader->deleteKey(&keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("deleteKey failed");
        return res;
    }
    LOGI("delete peer pubKey success.");
    res = GenerateKeyAlias(&packageName, &serviceType, KEY_ALIAS_PSK, &(pakeParams->baseParams.idPeer),
        &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("generate pskKey alias failed");
        return res;
    }
    LOGI("Psk alias: %x%x%x%x****.", keyAliasVal[0], keyAliasVal[1], keyAliasVal[2], keyAliasVal[3]);
    res = pakeParams->baseParams.loader->deleteKey(&keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("delete pskKey failed");
        return res;
    }
    LOGI("delete pskKey success.");
    return HC_SUCCESS;
}

static int32_t BuildRmvResult(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    uint8_t resultVal[sizeof(int32_t)] = { 0 };
    Uint8Buff result = { resultVal, sizeof(int32_t) };
    int32_t res = InitSingleParam(&(exchangeParams->resultCipher), result.length + AE_TAG_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitResultInfoCipher failed");
        return res;
    }

    // encrypt
    res = pakeParams->baseParams.loader->generateRandom(&(exchangeParams->nonce));
    if (res != HC_SUCCESS) {
        LOGE("generateRandom failed");
        return res;
    }

    GcmParam encryptInfo = {
        .aad = (uint8_t *)HICHAIN_REMOVE_INFO_RESPONSE,
        .aadLen = strlen(HICHAIN_REMOVE_INFO_RESPONSE),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };

    res = pakeParams->baseParams.loader->aesGcmEncrypt(&(pakeParams->baseParams.sessionKey),
        &result, &encryptInfo, false, &(exchangeParams->resultCipher));
    if (res != HC_SUCCESS) {
        LOGE("aesGcmEncrypt failed");
        return res;
    }

    return HC_SUCCESS;
}

static int32_t ConfirmRmvResult(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    uint8_t resultVal[sizeof(int32_t)] = { 0 };
    Uint8Buff result = { resultVal, sizeof(int32_t) };

    // decrypt
    GcmParam decryptInfo = {
        .aad = (uint8_t *)HICHAIN_REMOVE_INFO_RESPONSE,
        .aadLen = strlen(HICHAIN_REMOVE_INFO_RESPONSE),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };

    int32_t res = pakeParams->baseParams.loader->aesGcmDecrypt(&(pakeParams->baseParams.sessionKey),
        &(exchangeParams->resultCipher), &decryptInfo, false, &result);
    if (res != HC_SUCCESS) {
        LOGE("aesGcmDecrypt failed");
        return res;
    }

    uint8_t resultSuccess[sizeof(int32_t)] = { 0 };
    if (memcmp(result.val, resultSuccess, sizeof(int32_t)) != 0) {
        LOGE("RemoveAuthStartRequest failed, ret: %d", res);
        int32_t *resultTmp = (int32_t *)result.val;
        return *resultTmp;
    }

    return HC_SUCCESS;
}

int32_t ClientRequestStandardUnbindExchange(const PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = PackageRmvInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("PackageRmvInfo failed");
        return res;
    }

    res = EncryptRmvInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("EncryptRmvInfo failed");
        return res;
    }

    return res;
}

int32_t ServerResponseStandardUnbindExchange(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = DecryptRmvInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("DecryptRmvInfo failed");
        return res;
    }

    res = ParseRmvInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("ParseRmvInfo failed");
        return res;
    }

    res = DeleteAuthInfo(pakeParams);
    if (res != HC_SUCCESS) {
        LOGE("DeleteAuthInfo failed");
        return res;
    }

    res = BuildRmvResult(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("BuildRmvResult failed");
        return res;
    }

    return HC_SUCCESS;
}

int32_t ClientConfirmStandardUnbindExchange(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams)
{
    int32_t res = ConfirmRmvResult(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("ConfirmRmvResult failed");
        return res;
    }

    res = DeleteAuthInfo(pakeParams);
    if (res != HC_SUCCESS) {
        LOGE("DeleteAuthInfo failed");
        return res;
    }

    return HC_SUCCESS;
}