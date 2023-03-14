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

#include "hks_common_check.h"
#include "hks_log.h"
#include "hks_param.h"

int32_t HksCheckBlob4(const struct HksBlob *data1, const struct HksBlob *data2,
    const struct HksBlob *data3, const struct HksBlob *data4)
{
    if (CheckBlob(data1) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data1.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data2) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data2.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data3) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data3.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data4) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data4.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckBlob3(const struct HksBlob *data1, const struct HksBlob *data2, const struct HksBlob *data3)
{
    if (CheckBlob(data1) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data1.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data2) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data2.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data3) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data3.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckBlob2(const struct HksBlob *data1, const struct HksBlob *data2)
{
    if (CheckBlob(data1) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data1.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (CheckBlob(data2) != HKS_SUCCESS) {
        HKS_LOG_E("invalid data2.");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t HksCheckParamSetValidity(const struct HksParamSet *paramSet)
{
    if (paramSet == NULL) {
        HKS_LOG_E("paramSet NULL!");
        return HKS_ERROR_NULL_POINTER;
    }
    if (HksCheckParamSet(paramSet, paramSet->paramSetSize) != HKS_SUCCESS) {
        HKS_LOG_E("paramSet invalid!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HksCheckParamSetTag(paramSet);
}

int32_t HksCheckBlob4AndParamSet(const struct HksBlob *data1, const struct HksBlob *data2,
    const struct HksBlob *data3, const struct HksBlob *data4, const struct HksParamSet *paramSet)
{
    int32_t ret = HksCheckBlob4(data1, data2, data3, data4);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksCheckParamSetValidity(paramSet);
}

int32_t HksCheckBlob3AndParamSet(const struct HksBlob *data1, const struct HksBlob *data2,
    const struct HksBlob *data3, const struct HksParamSet *paramSet)
{
    int32_t ret = HksCheckBlob3(data1, data2, data3);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksCheckParamSetValidity(paramSet);
}

int32_t HksCheckBlob2AndParamSet(const struct HksBlob *data1, const struct HksBlob *data2,
    const struct HksParamSet *paramSet)
{
    int32_t ret = HksCheckBlob2(data1, data2);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksCheckParamSetValidity(paramSet);
}

int32_t HksCheckBlobAndParamSet(const struct HksBlob *data, const struct HksParamSet *paramSet)
{
    if (CheckBlob(data) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HksCheckParamSetValidity(paramSet);
}

int32_t HksGetDigestLen(uint32_t digest, uint32_t *digestLen)
{
    switch (digest) {
        case HKS_DIGEST_MD5:
            *digestLen = HKS_DIGEST_MD5_LEN;
            break;
        case HKS_DIGEST_SHA1:
            *digestLen = HKS_DIGEST_SHA1_LEN;
            break;
        case HKS_DIGEST_SHA224:
            *digestLen = HKS_DIGEST_SHA224_LEN;
            break;
        case HKS_DIGEST_SHA256:
            *digestLen = HKS_DIGEST_SHA256_LEN;
            break;
        case HKS_DIGEST_SHA384:
            *digestLen = HKS_DIGEST_SHA384_LEN;
            break;
        case HKS_DIGEST_SHA512:
            *digestLen = HKS_DIGEST_SHA512_LEN;
            break;
        default:
            return HKS_ERROR_INVALID_DIGEST;
    }
    return HKS_SUCCESS;
}

int32_t HksCheckAesAeMode(const struct HksParamSet *paramSet, bool *isAes, bool *isAeMode)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }
    *isAes = (algParam->uint32Param == HKS_ALG_AES);
    if (!(*isAes)) {
        return HKS_SUCCESS;
    }

    struct HksParam *modeParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_BLOCK_MODE, &modeParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get mode param failed!");
        return HKS_ERROR_CHECK_GET_MODE_FAIL;
    }

    *isAeMode = (modeParam->uint32Param == HKS_MODE_CCM) || (modeParam->uint32Param == HKS_MODE_GCM);
    return HKS_SUCCESS;
}
