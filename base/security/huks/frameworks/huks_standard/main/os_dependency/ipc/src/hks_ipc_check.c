/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hks_ipc_check.h"
#include "hks_common_check.h"
#include "hks_ipc_serialization.h"
#include "hks_log.h"
#include "hks_param.h"

#define MIN_CERT_COUNT 3

int32_t HksCheckIpcGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn)
{
    int32_t ret = HksCheckBlobAndParamSet(keyAlias, paramSetIn);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check keyAlias or paramSetIn failed");
        return ret;
    }

    if ((keyAlias->size > MAX_PROCESS_SIZE) ||
        ((sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) + ALIGN_SIZE(paramSetIn->paramSetSize) +
        sizeof(uint32_t)) > MAX_PROCESS_SIZE)) {
        HKS_LOG_E("ipc generate key check size failed");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckIpcImportKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key)
{
    int32_t ret = HksCheckBlob2AndParamSet(keyAlias, key, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check keyAlias or key or paramSetIn failed");
        return ret;
    }

    if ((keyAlias->size > MAX_PROCESS_SIZE) || (key->size > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) + ALIGN_SIZE(paramSet->paramSetSize) +
        sizeof(key->size) + ALIGN_SIZE(key->size) > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcExportPublicKey(const struct HksBlob *keyAlias, const struct HksBlob *key)
{
    if (HksCheckBlob2(keyAlias, key) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (keyAlias->size > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) + sizeof(key->size)) > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcGetKeyParamSet(const struct HksBlob *keyAlias, struct HksParamSet *paramSet)
{
    if (CheckBlob(keyAlias) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (keyAlias->size > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (paramSet->paramSetSize == 0) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) + sizeof(paramSet->paramSetSize)) > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey)
{
    int32_t ret = HksCheckBlob3AndParamSet(privateKey, peerPublicKey, agreedKey, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check key or paramSetIn failed");
        return ret;
    }

    if ((privateKey->size > MAX_PROCESS_SIZE) || (peerPublicKey->size > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((ALIGN_SIZE(paramSet->paramSetSize) + sizeof(privateKey->size) + ALIGN_SIZE(privateKey->size) +
        sizeof(peerPublicKey->size) + ALIGN_SIZE(peerPublicKey->size) + sizeof(agreedKey->size) > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    const struct HksBlob *derivedKey)
{
    int32_t ret = HksCheckBlob2AndParamSet(mainKey, derivedKey, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check key or paramSetIn failed");
        return ret;
    }

    if (mainKey->size > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((ALIGN_SIZE(paramSet->paramSetSize) + sizeof(mainKey->size) + ALIGN_SIZE(mainKey->size) +
        sizeof(derivedKey->size)) > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcGetKeyInfoList(const struct HksKeyInfo *keyInfoList, uint32_t listCount)
{
    if ((listCount == 0) || (listCount > (MAX_PROCESS_SIZE / (sizeof(uint32_t) + sizeof(uint32_t))))) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    for (uint32_t i = 0; i < listCount; ++i) {
        if ((CheckBlob(&keyInfoList[i].alias) != HKS_SUCCESS) ||
            (keyInfoList[i].paramSet == NULL) || (keyInfoList[i].paramSet->paramSetSize == 0)) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
    }

    uint32_t keyInfoBufSize = sizeof(listCount);
    for (uint32_t i = 0; i < listCount; ++i) {
        if (IsAdditionOverflow(keyInfoBufSize, sizeof(keyInfoList[i].alias.size))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        keyInfoBufSize += sizeof(keyInfoList[i].alias.size);
        if ((IsAdditionOverflow(keyInfoList[i].alias.size, DEFAULT_ALIGN_MASK_SIZE)) ||
            (IsAdditionOverflow(keyInfoList[i].paramSet->paramSetSize, DEFAULT_ALIGN_MASK_SIZE))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        if (IsAdditionOverflow(keyInfoBufSize, ALIGN_SIZE(keyInfoList[i].alias.size))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        keyInfoBufSize += ALIGN_SIZE(keyInfoList[i].alias.size);
        if (IsAdditionOverflow(keyInfoBufSize, ALIGN_SIZE(keyInfoList[i].paramSet->paramSetSize))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        keyInfoBufSize += ALIGN_SIZE(keyInfoList[i].paramSet->paramSetSize);
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksCertChain *certChain)
{
    if ((certChain->certs == NULL) || (certChain->certsCount < MIN_CERT_COUNT)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (HksCheckParamSet(paramSet, paramSet->paramSetSize) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((keyAlias->size > MAX_PROCESS_SIZE) ||
        ((sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) +
        ALIGN_SIZE(paramSet->paramSetSize)) > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t certBufSize = sizeof(certChain->certsCount);
    for (uint32_t i = 0; i < certChain->certsCount; ++i) {
        if (IsAdditionOverflow(certBufSize, sizeof(certChain->certs[i].size))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        certBufSize += sizeof(certChain->certs[i].size);

        if (IsAdditionOverflow(certChain->certs[i].size, DEFAULT_ALIGN_MASK_SIZE)) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        if (IsAdditionOverflow(certBufSize, ALIGN_SIZE(certChain->certs[i].size))) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        certBufSize += ALIGN_SIZE(certChain->certs[i].size);
    }
    return HKS_SUCCESS;
}

int32_t HksCheckIpcWrapUnwrapKey(bool isWrapKey, const struct HksBlob *keyAlias,
    const struct HksBlob *targetKeyAlias, const struct HksParamSet *paramSet, const struct HksBlob *wrappedData)
{
    int32_t ret = HksCheckBlob3AndParamSet(keyAlias, targetKeyAlias, wrappedData, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check key or paramSetIn failed");
        return ret;
    }

    if ((keyAlias->size > MAX_PROCESS_SIZE) || (targetKeyAlias->size > MAX_PROCESS_SIZE)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (ALIGN_SIZE(keyAlias->size) + ALIGN_SIZE(paramSet->paramSetSize) +
        ALIGN_SIZE(targetKeyAlias->size) > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t totalBufSize = sizeof(keyAlias->size) + ALIGN_SIZE(keyAlias->size) + ALIGN_SIZE(paramSet->paramSetSize) +
        sizeof(targetKeyAlias->size) + ALIGN_SIZE(targetKeyAlias->size);
    if (isWrapKey) {
        totalBufSize += sizeof(wrappedData->size);
    } else {
        if (wrappedData->size > MAX_PROCESS_SIZE) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        totalBufSize += sizeof(wrappedData->size) + ALIGN_SIZE(wrappedData->size);
    }
    if (totalBufSize > MAX_PROCESS_SIZE) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}
