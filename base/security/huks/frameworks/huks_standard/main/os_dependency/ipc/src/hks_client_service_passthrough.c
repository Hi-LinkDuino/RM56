/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "hks_client_ipc.h"
#include "hks_client_service.h"
#include "hks_get_process_info.h"
#include "hks_log.h"

#define HKS_PROCESS_INFO_LEN    128
#define HKS_MAX_DIRENT_FILE_LEN 128

#ifndef _CUT_AUTHENTICATE_
int32_t HksClientInitialize(void)
{
    return HksServiceInitialize();
}

int32_t HksClientRefreshKeyInfo(void)
{
    char *processName = NULL;
    if (HksGetProcessName(&processName) != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };
    return HksServiceRefreshKeyInfo(&processNameBlob);
}

static int32_t GetProcessInfo(char **processName, char **userId)
{
    if (HksGetProcessName(processName) != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    if (HksGetUserId(userId) != HKS_SUCCESS) {
        HKS_LOG_E("get user id failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    return HKS_SUCCESS;
}

int32_t HksClientGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut)
{
    (void)paramSetOut;
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceGenerateKey(&processInfo, keyAlias, paramSetIn, NULL);
}

int32_t HksClientImportKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceImportKey(&processInfo, keyAlias, paramSet, key);
}

int32_t HksClientExportPublicKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *key)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceExportPublicKey(&processInfo, keyAlias, key);
}

int32_t HksClientDeleteKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceDeleteKey(&processInfo, keyAlias);
}

int32_t HksClientGetKeyParamSet(const struct HksBlob *keyAlias, struct HksParamSet *paramSetOut)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceGetKeyParamSet(&processInfo, keyAlias, paramSetOut);
}

int32_t HksClientKeyExist(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceKeyExist(&processInfo, keyAlias);
}

int32_t HksClientSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceSign(&processInfo, key, paramSet, srcData, signature);
}

int32_t HksClientVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceVerify(&processInfo, key, paramSet, srcData, signature);
}

int32_t HksClientEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceEncrypt(&processInfo, key, paramSet, plainText, cipherText);
}

int32_t HksClientDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceDecrypt(&processInfo, key, paramSet, cipherText, plainText);
}

int32_t HksClientAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceAgreeKey(&processInfo, paramSet, privateKey, peerPublicKey, agreedKey);
}

int32_t HksClientDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceDeriveKey(&processInfo, paramSet, mainKey, derivedKey);
}

int32_t HksClientMac(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *mac)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceMac(&processInfo, key, paramSet, srcData, mac);
}

int32_t HksClientGetKeyInfoList(struct HksKeyInfo *keyInfoList, uint32_t *listCount)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceGetKeyInfoList(&processInfo, keyInfoList, listCount);
}

int32_t HksClientAttestKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksClientGetCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksClientWrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData)
{
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksClientUnwrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksBlob *wrappedData, const struct HksParamSet *paramSet)
{
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksClientInit(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *handle)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceInit(&processInfo, keyAlias, paramSet, handle);
}

int32_t HksClientUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceUpdate(handle, &processInfo, paramSet, inData, outData);
}

int32_t HksClientFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceFinish(handle, &processInfo, paramSet, inData, outData);
}

int32_t HksClientAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    char *processName = NULL;
    char *userId = NULL;
    if (GetProcessInfo(&processName, &userId) != HKS_SUCCESS) {
        HKS_LOG_E("get process info failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }

    struct HksProcessInfo processInfo = {
        { strlen(userId), (uint8_t *)userId },
        { strlen(processName), (uint8_t *)processName }
    };
    return HksServiceAbort(handle, &processInfo, paramSet);
}
#endif

int32_t HksClientGenerateRandom(struct HksBlob *random, const struct HksParamSet *paramSet)
{
    (void)paramSet;
    char *processName = NULL;
    if (HksGetProcessName(&processName) != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };
    return HksServiceGenerateRandom(&processNameBlob, random);
}
