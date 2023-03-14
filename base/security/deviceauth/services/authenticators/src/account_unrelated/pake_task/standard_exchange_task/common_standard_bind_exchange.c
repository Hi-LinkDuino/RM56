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

#include "common_standard_bind_exchange.h"
#include "alg_defs.h"
#include "das_standard_token_manager.h"
#include "das_task_common.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "string_util.h"

int32_t InitStandardBindExchangeParams(StandardBindExchangeParams *params)
{
    int32_t res;
    if (params == NULL) {
        return HC_ERR_INVALID_PARAMS;
    }

    params->pubKeyPeer.length = PAKE_ED25519_KEY_PAIR_LEN;
    params->pubKeyPeer.val = (uint8_t *)HcMalloc(params->pubKeyPeer.length, 0);
    if (params->pubKeyPeer.val == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    params->pubKeySelf.length = PAKE_ED25519_KEY_PAIR_LEN;
    params->pubKeySelf.val = (uint8_t *)HcMalloc(params->pubKeySelf.length, 0);
    if (params->pubKeySelf.val == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    params->nonce.length = STANDARD_BIND_EXCHANGE_NONCE_LEN;
    params->nonce.val = (uint8_t *)HcMalloc(params->nonce.length, 0);
    if (params->nonce.val == NULL) {
        res = HC_ERR_ALLOC_MEMORY;
        goto ERR;
    }

    params->authInfo.length = 0;
    params->authInfo.val = NULL;
    params->exInfoCipher.length = 0;
    params->exInfoCipher.val = NULL;

    return HC_SUCCESS;
ERR:
    DestroyStandardBindExchangeParams(params);
    return res;
}

void DestroyStandardBindExchangeParams(StandardBindExchangeParams *params)
{
    if (params == NULL) {
        return;
    }
    if (params->pubKeySelf.val != NULL) {
        HcFree(params->pubKeySelf.val);
        params->pubKeySelf.val = NULL;
    }
    if (params->pubKeyPeer.val != NULL) {
        HcFree(params->pubKeyPeer.val);
        params->pubKeyPeer.val = NULL;
    }
    if (params->authInfo.val != NULL) {
        HcFree(params->authInfo.val);
        params->authInfo.val = NULL;
    }
    if (params->exInfoCipher.val != NULL) {
        HcFree(params->exInfoCipher.val);
        params->exInfoCipher.val = NULL;
    }
    if (params->nonce.val != NULL) {
        HcFree(params->nonce.val);
        params->nonce.val = NULL;
    }
}

static int32_t PackageAuthInfo(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams,
    const Uint8Buff *keyAlias)
{
    int32_t res = pakeParams->baseParams.loader->checkKeyExist(keyAlias);
    if (res != HC_SUCCESS) {
        LOGI("The local identity key pair does not exist, generate it.");
        Algorithm alg = (pakeParams->baseParams.curveType == CURVE_256) ? P256 : ED25519;
        /* UserType and pairType are not required when generating key. */
        ExtraInfo exInfo = { pakeParams->baseParams.idSelf, -1, -1 };
        res = pakeParams->baseParams.loader->generateKeyPairWithStorage(keyAlias, PAKE_ED25519_KEY_PAIR_LEN, alg,
            &exInfo);
        if (res != HC_SUCCESS) {
            LOGE("generate self auth keyPair failed.");
            return res;
        }
    }

    res = pakeParams->baseParams.loader->exportPublicKey(keyAlias, &(exchangeParams->pubKeySelf));
    if (res != HC_SUCCESS) {
        LOGE("exportPublicKey failed");
        return res;
    }

    CJson *authInfoJson = CreateJson();
    char *authInfoStr = NULL;
    GOTO_ERR_AND_SET_RET(AddByteToJson(authInfoJson, FIELD_AUTH_ID, pakeParams->baseParams.idSelf.val,
        pakeParams->baseParams.idSelf.length), res);
    GOTO_ERR_AND_SET_RET(AddByteToJson(authInfoJson, FIELD_AUTH_PK, exchangeParams->pubKeySelf.val,
        exchangeParams->pubKeySelf.length), res);
    authInfoStr = PackJsonToString(authInfoJson);
    if (authInfoStr == NULL) {
        LOGE("authInfoStr PackJsonToString failed");
        res = HC_ERR_PACKAGE_JSON_TO_STRING_FAIL;
        goto ERR;
    }

    res = InitSingleParam(&(exchangeParams->authInfo), strlen(authInfoStr));
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for authInfo failed.");
        goto ERR;
    }

    if (memcpy_s(exchangeParams->authInfo.val, exchangeParams->authInfo.length,
        authInfoStr, strlen(authInfoStr)) != EOK) {
        LOGE("Memcpy authInfo failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
ERR:
    FreeJson(authInfoJson);
    FreeJsonString(authInfoStr);
    return res;
}

static int32_t GenerateSignInfo(const PakeParams *pakeParams, const StandardBindExchangeParams *exchangeParams,
    const Uint8Buff *keyAlias, Uint8Buff *signInfo)
{
    // add challenge
    int32_t res = HC_SUCCESS;
    uint32_t msgInfoLen = pakeParams->baseParams.challengeSelf.length + pakeParams->baseParams.challengePeer.length +
        exchangeParams->authInfo.length;
    uint8_t *msgInfoVal = (uint8_t *)HcMalloc(msgInfoLen, 0);
    if (msgInfoVal == NULL) {
        LOGE("Malloc msgInfoVal failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff msgInfo = { msgInfoVal, msgInfoLen };
    if (memcpy_s(msgInfo.val, msgInfo.length, pakeParams->baseParams.challengeSelf.val,
        pakeParams->baseParams.challengeSelf.length) != EOK) {
        LOGE("Memcpy for challengeSelf failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    uint32_t usedLen = pakeParams->baseParams.challengeSelf.length;

    if (memcpy_s(msgInfo.val + usedLen, msgInfo.length - usedLen, pakeParams->baseParams.challengePeer.val,
        pakeParams->baseParams.challengePeer.length) != EOK) {
        LOGE("Memcpy for challengePeer failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    usedLen += pakeParams->baseParams.challengePeer.length;

    if (memcpy_s(msgInfo.val + usedLen, msgInfo.length - usedLen, exchangeParams->authInfo.val,
        exchangeParams->authInfo.length) != EOK) {
        LOGE("Memcpy for authInfo failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    Algorithm alg = (pakeParams->baseParams.curveType == CURVE_256) ? P256 : ED25519;
    res = pakeParams->baseParams.loader->sign(keyAlias, &msgInfo, alg, signInfo, true);
    if (res != HC_SUCCESS) {
        LOGE("sign failed");
        goto ERR;
    }
ERR:
    HcFree(msgInfo.val);
    return res;
}

static int32_t EncryptAuthAndSignInfo(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams,
    const Uint8Buff *signInfo, const char *aad)
{
    // contact auth and sign info
    int32_t res = HC_SUCCESS;
    uint32_t exchangeInfoLen = exchangeParams->authInfo.length + signInfo->length;
    uint8_t *exchangeInfoVal = (uint8_t *)HcMalloc(exchangeInfoLen, 0);
    if (exchangeInfoVal  == NULL) {
        LOGE("Malloc exchangeInfoVal failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff exchangeInfo = { exchangeInfoVal, exchangeInfoLen };
    if (memcpy_s(exchangeInfo.val, exchangeInfo.length, exchangeParams->authInfo.val,
        exchangeParams->authInfo.length) != EOK) {
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    if (memcpy_s(exchangeInfo.val + exchangeParams->authInfo.length,
        exchangeInfo.length - exchangeParams->authInfo.length, signInfo->val, signInfo->length) != EOK) {
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    res = InitSingleParam(&(exchangeParams->exInfoCipher), exchangeInfo.length + AE_TAG_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for failed.");
        goto ERR;
    }

    // encrypt
    res = pakeParams->baseParams.loader->generateRandom(&(exchangeParams->nonce));
    if (res != HC_SUCCESS) {
        LOGE("generateRandom failed");
        goto ERR;
    }

    GcmParam encryptInfo = {
        .aad = (uint8_t *)aad,
        .aadLen = strlen(aad),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };
    res = pakeParams->baseParams.loader->aesGcmEncrypt(&(pakeParams->baseParams.sessionKey), &exchangeInfo,
        &encryptInfo, false, &(exchangeParams->exInfoCipher));
    if (res != HC_SUCCESS) {
        LOGE("aesGcmEncrypt failed");
        goto ERR;
    }

ERR:
    HcFree(exchangeInfo.val);
    return res;
}

static int32_t DecryptAuthAndSignInfo(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams,
    Uint8Buff *signInfo, const char *aad)
{
    uint32_t exchangeInfoLen = exchangeParams->exInfoCipher.length - AE_TAG_LEN;
    uint8_t *exchangeInfoVal = (uint8_t *)HcMalloc(exchangeInfoLen, 0);
    if (exchangeInfoVal  == NULL) {
        LOGE("Malloc exchangeInfoVal failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff exchangeInfo = { exchangeInfoVal, exchangeInfoLen };

    GcmParam decryptInfo = {
        .aad = (uint8_t *)aad,
        .aadLen = strlen(aad),
        .nonce = exchangeParams->nonce.val,
        .nonceLen = exchangeParams->nonce.length
    };

    int32_t res = pakeParams->baseParams.loader->aesGcmDecrypt(&(pakeParams->baseParams.sessionKey),
        &(exchangeParams->exInfoCipher), &decryptInfo, false, &exchangeInfo);
    if (res != HC_SUCCESS) {
        LOGE("aesGcmDecrypt failed");
        goto ERR;
    }

    // get authInfo
    res = InitSingleParam(&(exchangeParams->authInfo), exchangeInfo.length - SIGNATURE_LEN);
    if (res != HC_SUCCESS) {
        LOGE("InitSingleParam for authInfo failed.");
        goto ERR;
    }

    if (memcpy_s(exchangeParams->authInfo.val, exchangeParams->authInfo.length, exchangeInfo.val,
        exchangeParams->authInfo.length) != EOK) {
        LOGE("memcpy authInfo failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    if (memcpy_s(signInfo->val, signInfo->length, exchangeInfo.val + exchangeParams->authInfo.length,
        SIGNATURE_LEN) != EOK) {
        LOGE("memcpy signInfo failed.");
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
ERR:
    HcFree(exchangeInfo.val);
    return res;
}

static int32_t ParseAuthInfo(PakeParams *pakeParams, const StandardBindExchangeParams *exchangeParams)
{
    int32_t res;
    CJson *authInfoJson = CreateJsonFromString((char *)exchangeParams->authInfo.val);
    if (authInfoJson == NULL) {
        LOGE("Create authInfoJson failed.");
        return HC_ERR_JSON_CREATE;
    }

    GOTO_ERR_AND_SET_RET(GetByteFromJson(authInfoJson, FIELD_AUTH_PK, exchangeParams->pubKeyPeer.val,
        exchangeParams->pubKeyPeer.length), res);
    res = GetIdPeer(authInfoJson, FIELD_AUTH_ID,
        &pakeParams->baseParams.idSelf, &pakeParams->baseParams.idPeer);
    if (res != HC_SUCCESS) {
        LOGE("GetIdPeer failed, res: %d.", res);
        goto ERR;
    }
ERR:
    FreeJson(authInfoJson);
    return res;
}

static int32_t VerifySignInfo(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams,
    Uint8Buff *signInfo)
{
    int32_t res = HC_SUCCESS;
    uint32_t verifyMsgLen = exchangeParams->authInfo.length + pakeParams->baseParams.challengePeer.length +
        pakeParams->baseParams.challengeSelf.length;
    uint8_t *verifyMsgVal = (uint8_t *)HcMalloc(verifyMsgLen, 0);
    if (verifyMsgVal == NULL) {
        LOGE("Malloc verifyMsgVal failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff verifyMsg = { verifyMsgVal, verifyMsgLen };
    if (memcpy_s(verifyMsg.val, verifyMsg.length, pakeParams->baseParams.challengePeer.val,
        pakeParams->baseParams.challengePeer.length) != EOK) {
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    uint32_t usedLen = pakeParams->baseParams.challengePeer.length;
    if (memcpy_s(verifyMsg.val + usedLen, verifyMsg.length - usedLen, pakeParams->baseParams.challengeSelf.val,
        pakeParams->baseParams.challengeSelf.length) != EOK) {
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }
    usedLen += pakeParams->baseParams.challengeSelf.length;
    if (memcpy_s(verifyMsg.val + usedLen, verifyMsg.length - usedLen, exchangeParams->authInfo.val,
        exchangeParams->authInfo.length) != EOK) {
        res = HC_ERR_MEMORY_COPY;
        goto ERR;
    }

    Algorithm alg = (pakeParams->baseParams.curveType == CURVE_256) ? P256 : ED25519;
    res = pakeParams->baseParams.loader->verify(&(exchangeParams->pubKeyPeer), &verifyMsg, alg, signInfo, false);
    if (res != HC_SUCCESS) {
        LOGE("verify failed");
        goto ERR;
    }
ERR:
    HcFree(verifyMsg.val);
    return res;
}

static int32_t SaveAuthInfo(const PakeParams *pakeParams, const StandardBindExchangeParams *exchangeParams)
{
    uint8_t keyAliasPeerVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff keyAliasPeer = { keyAliasPeerVal, PAKE_KEY_ALIAS_LEN };
    KeyAliasType keyType = pakeParams->userTypePeer;
    Uint8Buff packageName = { (uint8_t *)pakeParams->packageName, strlen(pakeParams->packageName) };
    Uint8Buff serviceType = { (uint8_t *)pakeParams->serviceType, strlen(pakeParams->serviceType) };
    int32_t res = GenerateKeyAlias(&packageName, &serviceType, keyType, &(pakeParams->baseParams.idPeer),
        &keyAliasPeer);
    if (res != HC_SUCCESS) {
        LOGE("generateKeyAlias failed");
        return res;
    }
    LOGI("PubKey alias: %x%x%x%x****.", keyAliasPeerVal[0], keyAliasPeerVal[1], keyAliasPeerVal[2], keyAliasPeerVal[3]);
    Algorithm alg = (pakeParams->baseParams.curveType == CURVE_256) ? P256 : ED25519;
    ExtraInfo exInfo = { pakeParams->baseParams.idPeer, pakeParams->userType, PAIR_TYPE_BIND };
    res = pakeParams->baseParams.loader->importPublicKey(&keyAliasPeer, &(exchangeParams->pubKeyPeer), alg, &exInfo);
    if (res != HC_SUCCESS) {
        LOGE("importPublicKey failed");
        return res;
    }
    res = GetStandardTokenManagerInstance()->computeAndSavePsk(pakeParams);
    if (res != HC_SUCCESS) {
        LOGE("ComputeAndSavePsk failed, res: %x.", res);
        return res;
    }
    LOGI("Save pubKey and psk success.");
    return HC_SUCCESS;
}

int32_t ClientRequestStandardBindExchange(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams)
{
    uint8_t keyAliasVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff keyAlias = { keyAliasVal, PAKE_KEY_ALIAS_LEN };
    KeyAliasType keyType = pakeParams->userType;
    Uint8Buff packageName = { (uint8_t *)pakeParams->packageName, strlen(pakeParams->packageName) };
    Uint8Buff serviceType = { (uint8_t *)pakeParams->serviceType, strlen(pakeParams->serviceType) };
    int32_t res = GenerateKeyAlias(&packageName, &serviceType, keyType, &(pakeParams->baseParams.idSelf), &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("generateKeyAlias failed");
        return res;
    }

    res = PackageAuthInfo(pakeParams, exchangeParams, &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("PackageAuthInfo failed");
        return res;
    }

    uint8_t signInfoVal[SIGNATURE_LEN] = { 0 };
    Uint8Buff signInfo = { signInfoVal, SIGNATURE_LEN };
    res = GenerateSignInfo(pakeParams, exchangeParams, &keyAlias, &signInfo);
    if (res != HC_SUCCESS) {
        LOGE("GenerateSignInfo failed");
        return res;
    }

    res = EncryptAuthAndSignInfo(pakeParams, exchangeParams, &signInfo, HICHAIN_EXCHANGE_REQUEST);
    if (res != HC_SUCCESS) {
        LOGE("EncryptAuthAndSignInfo failed");
        return res;
    }

    return res;
}

int32_t ServerResponseStandardBindExchange(PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams)
{
    uint8_t keyAliasVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff keyAlias = { keyAliasVal, PAKE_KEY_ALIAS_LEN };
    KeyAliasType keyType = pakeParams->userType;
    Uint8Buff packageName = { (uint8_t *)pakeParams->packageName, strlen(pakeParams->packageName) };
    Uint8Buff serviceType = { (uint8_t *)pakeParams->serviceType, strlen(pakeParams->serviceType) };
    int32_t res = GenerateKeyAlias(&packageName, &serviceType, keyType, &(pakeParams->baseParams.idSelf), &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("generateKeyAlias failed");
        return res;
    }

    uint8_t signInfoVal[SIGNATURE_LEN] = { 0 };
    Uint8Buff signInfo = { signInfoVal, SIGNATURE_LEN };
    res = DecryptAuthAndSignInfo(pakeParams, exchangeParams, &signInfo, HICHAIN_EXCHANGE_REQUEST);
    if (res != HC_SUCCESS) {
        LOGE("DecryptAuthAndSignInfo failed");
        return res;
    }

    res = ParseAuthInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("ParseAuthInfo failed");
        return res;
    }

    res = VerifySignInfo(pakeParams, exchangeParams, &signInfo);
    if (res != HC_SUCCESS) {
        LOGE("VerifySignInfo failed");
        return res;
    }

    res = SaveAuthInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("SaveAuthInfo failed");
        return res;
    }

    res = PackageAuthInfo(pakeParams, exchangeParams, &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("PackageAuthInfo failed");
        return res;
    }

    res = GenerateSignInfo(pakeParams, exchangeParams, &keyAlias, &signInfo);
    if (res != HC_SUCCESS) {
        LOGE("GenerateSignInfo failed");
        return res;
    }

    res = EncryptAuthAndSignInfo(pakeParams, exchangeParams, &signInfo, HICHAIN_EXCHANGE_RESPONSE);
    if (res != HC_SUCCESS) {
        LOGE("EncryptAuthAndSignInfo failed");
        return res;
    }

    return HC_SUCCESS;
}

int32_t ClientConfirmStandardBindExchange(PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams)
{
    uint8_t keyAliasVal[PAKE_KEY_ALIAS_LEN] = { 0 };
    Uint8Buff keyAlias = { keyAliasVal, PAKE_KEY_ALIAS_LEN };
    KeyAliasType keyType = pakeParams->userType;
    Uint8Buff packageName = { (uint8_t *)pakeParams->packageName, strlen(pakeParams->packageName) };
    Uint8Buff serviceType = { (uint8_t *)pakeParams->serviceType, strlen(pakeParams->serviceType) };
    int32_t res = GenerateKeyAlias(&packageName, &serviceType, keyType, &(pakeParams->baseParams.idSelf), &keyAlias);
    if (res != HC_SUCCESS) {
        LOGE("generateKeyAlias failed");
        return res;
    }

    uint8_t signInfoVal[SIGNATURE_LEN] = { 0 };
    Uint8Buff signInfo = { signInfoVal, SIGNATURE_LEN };
    res = DecryptAuthAndSignInfo(pakeParams, exchangeParams, &signInfo, HICHAIN_EXCHANGE_RESPONSE);
    if (res != HC_SUCCESS) {
        LOGE("DecryptAuthAndSignInfo failed");
        return res;
    }

    res = ParseAuthInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("ParseAuthInfo failed");
        return res;
    }

    res = VerifySignInfo(pakeParams, exchangeParams, &signInfo);
    if (res != HC_SUCCESS) {
        LOGE("VerifySignInfo failed");
        return res;
    }

    res = SaveAuthInfo(pakeParams, exchangeParams);
    if (res != HC_SUCCESS) {
        LOGE("SaveAuthInfo failed");
        return res;
    }

    return HC_SUCCESS;
}
