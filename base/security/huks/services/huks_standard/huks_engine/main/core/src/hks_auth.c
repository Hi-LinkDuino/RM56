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

#include "hks_auth.h"
#include "hks_log.h"
#include "hks_param.h"
#include "hks_type_inner.h"

struct HksAuthPolicy {
    uint32_t authId;
    uint32_t policyCnt;
    uint32_t *policyTag;
};

#ifndef _CUT_AUTHENTICATE_
static uint32_t g_symCipherPolicyTag[] = { HKS_TAG_ALGORITHM, HKS_TAG_BLOCK_MODE, HKS_TAG_PADDING, HKS_TAG_PURPOSE };
static uint32_t g_asymCipherPolicyTag[] = { HKS_TAG_ALGORITHM, HKS_TAG_DIGEST, HKS_TAG_PADDING, HKS_TAG_PURPOSE };
static uint32_t g_signVerifyRsaPolicyTag[] = { HKS_TAG_ALGORITHM, HKS_TAG_DIGEST, HKS_TAG_PADDING, HKS_TAG_PURPOSE };
static uint32_t g_signVerifyEccPolicyTag[] = { HKS_TAG_ALGORITHM, HKS_TAG_DIGEST, HKS_TAG_PURPOSE };
static uint32_t g_macPolicyTag[] = { HKS_TAG_DIGEST, HKS_TAG_PURPOSE };
static uint32_t g_derivePolicyTag[] = { HKS_TAG_DIGEST, HKS_TAG_PURPOSE };

struct HksAuthPolicy g_authPolicyList[] = {
    { HKS_AUTH_ID_SYM_CIPHER, HKS_ARRAY_SIZE(g_symCipherPolicyTag), g_symCipherPolicyTag },
    { HKS_AUTH_ID_ASYM_CIPHER, HKS_ARRAY_SIZE(g_asymCipherPolicyTag), g_asymCipherPolicyTag },
    { HKS_AUTH_ID_SIGN_VERIFY_RSA, HKS_ARRAY_SIZE(g_signVerifyRsaPolicyTag), g_signVerifyRsaPolicyTag },
    { HKS_AUTH_ID_SIGN_VERIFY_ECC, HKS_ARRAY_SIZE(g_signVerifyEccPolicyTag), g_signVerifyEccPolicyTag },
    { HKS_AUTH_ID_MAC, HKS_ARRAY_SIZE(g_macPolicyTag), g_macPolicyTag },
    { HKS_AUTH_ID_DERIVE, HKS_ARRAY_SIZE(g_derivePolicyTag), g_derivePolicyTag }
};

static int32_t CheckPurpose(const struct HksParam *authParam, const struct HksParam *requestParam)
{
    if (requestParam->uint32Param == 0) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((requestParam->uint32Param & authParam->uint32Param) != requestParam->uint32Param) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t AuthPolicy(const struct HksAuthPolicy *policy, const struct HksKeyNode *keyNode,
    const struct HksParamSet *paramSet)
{
    int32_t ret;
    uint32_t authTag;
    struct HksParam *authParam = NULL;
    struct HksParam *requestParam = NULL;

    for (uint32_t i = 0; i < policy->policyCnt; i++) {
        authTag = policy->policyTag[i];
        ret = HksGetParam(keyNode->paramSet, authTag, &authParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get auth param[0x%x] failed!", authTag);
            return ret;
        }

        ret = HksGetParam(paramSet, authTag, &requestParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get request param[0x%x] failed!", authTag);
            return ret;
        }

        if (authTag != HKS_TAG_PURPOSE) {
            ret = HksCheckParamMatch((const struct HksParam *)authParam, (const struct HksParam *)requestParam);
        } else {
            ret = CheckPurpose((const struct HksParam *)authParam, (const struct HksParam *)requestParam);
        }
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("unmatch policy[0x%x], [0x%x] != [0x%x]!", authTag, requestParam->uint32Param,
                authParam->uint32Param);
            return ret;
        }
    }
    return HKS_SUCCESS;
}

static int32_t AuthThreeStagePolicy(const struct HksAuthPolicy *policy, const struct HuksKeyNode *keyNode)
{
    int32_t ret;
    uint32_t authTag;
    struct HksParam *authParam = NULL;
    struct HksParam *requestParam = NULL;

    for (uint32_t i = 0; i < policy->policyCnt; i++) {
        authTag = policy->policyTag[i];
        ret = HksGetParam(keyNode->keyBlobParamSet, authTag, &authParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get auth param[0x%x] failed!", authTag);
            return ret;
        }

        ret = HksGetParam(keyNode->runtimeParamSet, authTag, &requestParam);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get request param[0x%x] failed!", authTag);
            return ret;
        }

        if (authTag != HKS_TAG_PURPOSE) {
            ret = HksCheckParamMatch((const struct HksParam *)authParam, (const struct HksParam *)requestParam);
        } else {
            ret = CheckPurpose((const struct HksParam *)authParam, (const struct HksParam *)requestParam);
        }
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("unmatch policy[0x%x], [0x%x] != [0x%x]!", authTag, requestParam->uint32Param,
                authParam->uint32Param);
            return ret;
        }
    }
    return HKS_SUCCESS;
}

int32_t HksAuth(uint32_t authId, const struct HksKeyNode *keyNode, const struct HksParamSet *paramSet)
{
    for (uint32_t i = 0; i < HKS_ARRAY_SIZE(g_authPolicyList); i++) {
        if (authId == g_authPolicyList[i].authId) {
            return AuthPolicy(&g_authPolicyList[i], keyNode, paramSet);
        }
    }
    return HKS_ERROR_BAD_STATE;
}

int32_t HksThreeStageAuth(uint32_t authId, const struct HuksKeyNode *keyNode)
{
    for (uint32_t i = 0; i < HKS_ARRAY_SIZE(g_authPolicyList); i++) {
        if (authId == g_authPolicyList[i].authId) {
            return AuthThreeStagePolicy(&g_authPolicyList[i], keyNode);
        }
    }
    return HKS_ERROR_BAD_STATE;
}
#endif /* _CUT_AUTHENTICATE_ */
