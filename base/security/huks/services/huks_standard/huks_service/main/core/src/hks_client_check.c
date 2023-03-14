/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_client_check.h"

#include "hks_common_check.h"
#include "hks_log.h"

#ifndef _CUT_AUTHENTICATE_
static int32_t CheckProcessNameAndKeyAliasSize(uint32_t processNameSize, uint32_t keyAliasSize)
{
    if (processNameSize > HKS_MAX_PROCESS_NAME_LEN) {
        HKS_LOG_E("processName size too long, size %u", processNameSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (keyAliasSize > HKS_MAX_KEY_ALIAS_LEN) {
        HKS_LOG_E("keyAlias size too long, size %u", keyAliasSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckProcessNameAndKeyAlias(const struct HksBlob *processName, const struct HksBlob *keyAlias)
{
    if (HksCheckBlob2(processName, keyAlias) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
}

int32_t HksCheckGenAndImportKeyParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, const struct HksBlob *key)
{
    int32_t ret = HksCheckBlob3AndParamSet(processName, keyAlias, key, paramSetIn);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
}

int32_t HksCheckAllParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *data1, const struct HksBlob *data2)
{
    int32_t ret = HksCheckBlob4AndParamSet(processName, keyAlias, data1, data2, paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
}

int32_t HksCheckServiceInitParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet)
{
    int32_t ret = HksCheckBlob2AndParamSet(processName, keyAlias, paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
}

int32_t HksCheckGetKeyParamSetParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet)
{
    if (HksCheckProcessNameAndKeyAlias(processName, keyAlias) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((paramSet == NULL) || (paramSet->paramSetSize == 0)) {
        HKS_LOG_E("invalid paramSet");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckExportPublicKeyParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksBlob *key)
{
    if (HksCheckBlob3(processName, keyAlias, key) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
}

int32_t HksCheckDeriveKeyParams(const struct HksBlob *processName, const struct HksParamSet *paramSet,
    const struct HksBlob *mainKey, const struct HksBlob *derivedKey)
{
    return HksCheckGenAndImportKeyParams(processName, mainKey, paramSet, derivedKey);
}

int32_t HksCheckGetKeyInfoListParams(const struct HksBlob *processName, const struct HksKeyInfo *keyInfoList,
    const uint32_t *listCount)
{
    if (CheckBlob(processName) != HKS_SUCCESS) {
        HKS_LOG_E("invalid processName");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (processName->size > HKS_MAX_PROCESS_NAME_LEN) {
        HKS_LOG_E("processName size too long, size %u", processName->size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((keyInfoList == NULL) || (listCount == NULL)) {
        HKS_LOG_E("keyInfoList or listCount null.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    for (uint32_t i = 0; i < *listCount; ++i) {
        if ((CheckBlob(&(keyInfoList[i].alias)) != HKS_SUCCESS) ||
            (keyInfoList[i].paramSet == NULL) || (keyInfoList[i].paramSet->paramSetSize == 0)) {
            return HKS_ERROR_INVALID_ARGUMENT;
        }
    }

    return HKS_SUCCESS;
}

int32_t HksCheckInitParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const uint64_t *operationHandle)
{
    int32_t ret = HksCheckBlob2AndParamSet(processName, keyAlias, paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = CheckProcessNameAndKeyAliasSize(processName->size, keyAlias->size);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (operationHandle == NULL) {
        HKS_LOG_E("operation handle invalid.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}
#endif /* _CUT_AUTHENTICATE_ */

int32_t HksCheckGenerateRandomParams(const struct HksBlob *processName, const struct HksBlob *random)
{
    if (HksCheckBlob2(processName, random) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (processName->size > HKS_MAX_PROCESS_NAME_LEN) {
        HKS_LOG_E("processName size too long, size %u.", processName->size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (random->size > HKS_MAX_RANDOM_LEN) {
        HKS_LOG_E("random size too long, size %u.", random->size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

#ifdef HKS_SUPPORT_API_ATTEST_KEY
int32_t HksCheckAttestKeyParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *certChain)
{
    return HksCheckGenAndImportKeyParams(processName, keyAlias, paramSet, certChain);
}
#endif

#ifdef HKS_SUPPORT_API_GET_CERTIFICATE_CHAIN
int32_t HksCheckGetCertificateChainParams(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *certChain)
{
    return HksCheckGenAndImportKeyParams(processName, keyAlias, paramSet, certChain);
}
#endif