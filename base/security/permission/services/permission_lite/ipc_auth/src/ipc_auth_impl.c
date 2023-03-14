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

#include "ipc_auth.h"

#include <securec.h>
#include <stdbool.h>
#include <string.h>

#ifdef OHOS_APPFWK_ENABLE
#include "bundle_manager.h"
#endif
#include "log.h"

#include "ipc_auth_err.h"
#include "policy_preset.h"
#include "policy_preset_product.h"
#include "policy_registry.h"

static unsigned int g_systemSvcUids[] = {1, 2, 0, 7, 8, 9, 10};

static unsigned int g_systemSvcUidSize = sizeof(g_systemSvcUids) / sizeof(unsigned int);

static int IsUidValid(unsigned int uid)
{
    for (int i = 0; i < g_systemSvcUidSize; i++) {
        if (g_systemSvcUids[i] == uid) {
            return AUTH_ERRORCODE_SUCCESS;
        }
    }
    return AUTH_ERRORCODE_INVALID_UID;
}

#ifdef OHOS_APPFWK_ENABLE
static int GetUidByBundleName(const char *bundleName, unsigned int *uid)
{
    BundleInfo bundleInfo = {0};
    if (GetBundleInfo(bundleName, 0, &bundleInfo) != 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "Invalid bundleName, [name: %s][line: %d]", bundleName, __LINE__);
        return AUTH_ERRORCODE_INVALID_BUNDLENAME;
    }
    *uid = bundleInfo.uid;
    return AUTH_ERRORCODE_SUCCESS;
}
#endif

static int StrcmpWithNull(const char *s1, const char *s2)
{
    if ((s1 == NULL) && (s2 == NULL)) {
        return 0;
    }
    if ((s1 == NULL) || (s2 == NULL)) {
        return -1;
    }
    return strcmp(s1, s2);
}

static void SetPolicy(const IpcPolicy *policy, PolicyTrans *policyTrans)
{
    switch (policy->type) {
        case RANGE:
            policyTrans->type = RANGE;
            policyTrans->uidMax = policy->uidMax;
            policyTrans->uidMin = policy->uidMin;
            break;
        case FIXED:
            policyTrans->type = FIXED;
            for (int m = 0; m < UID_SIZE; m++) {
                policyTrans->fixedUid[m] = policy->fixedUid[m];
            }
            break;
#ifdef OHOS_APPFWK_ENABLE
        case BUNDLENAME: {
            policyTrans->type = BUNDLENAME;
            unsigned int uid = 0;
            int ret = GetUidByBundleName(policy->bundleName, &uid);
            if (ret != AUTH_ERRORCODE_SUCCESS) {
                break;
            }
            policyTrans->fixedUid[0] = uid;
            break;
        }
#endif
        default:
            break;
    }
}

static bool IsPolicyValid(enum PolicyType type)
{
    if ((type == RANGE) || (type == FIXED)) {
        return true;
    }
#ifdef OHOS_APPFWK_ENABLE
    if (type == BUNDLENAME) {
        return true;
    }
#endif
    return false;
}

static int SetPolicies(const FeaturePolicy *featurePolicy, PolicyTrans **policies, unsigned int *policyNum)
{
    int num = 0;
    for (int k = 0; k < POLICY_SIZE; k++) {
        if (IsPolicyValid(featurePolicy->policies[k].type)) {
            num++;
        }
    }
    int allocSize = sizeof(PolicyTrans) * num;
    if (allocSize == 0) {
        return AUTH_ERRORCODE_NO_POLICY_SET;
    }
    *policies = (PolicyTrans *)malloc(allocSize);
    if (*policies == NULL) {
        HILOG_ERROR(HILOG_MODULE_APP, "Malloc failed, [line: %d]", __LINE__);
        return AUTH_ERRORCODE_MALLOC_FAIL;
    }
    if (memset_s(*policies, allocSize, 0x0, allocSize) != EOK) {
        HILOG_ERROR(HILOG_MODULE_APP, "Memset failed, [line: %d]", __LINE__);
        free(*policies);
        *policies = NULL;
        return AUTH_ERRORCODE_MEMSET_FAIL;
    }
    int index = 0;
    for (int k = 0; k < POLICY_SIZE; k++) {
        IpcPolicy policy = featurePolicy->policies[k];
        if (IsPolicyValid(policy.type)) {
            SetPolicy(&policy, *policies + index);
            index++;
        }
    }
    *policyNum = num;
    return AUTH_ERRORCODE_SUCCESS;
}

static int SetPresetPolicies(const PolicySetting presetPolicy[], int policySize, RegParams params,
    PolicyTrans **policies, unsigned int *policyNum)
{
    for (int i = 0; i < policySize; i++) {
        if (strcmp(presetPolicy[i].service, params.service) != 0) {
            continue;
        }
        for (int j = 0; j < presetPolicy[i].featureNum; j++) {
            FeaturePolicy *featurePolicy = (FeaturePolicy *) presetPolicy[i].features + j;
            if (StrcmpWithNull(featurePolicy->feature, params.feature) != 0) {
                continue;
            }
            if (SetPolicies(featurePolicy, policies, policyNum) == AUTH_ERRORCODE_SUCCESS) {
                return AUTH_ERRORCODE_SUCCESS;
            }
            return AUTH_ERRORCODE_NO_POLICY_SET;
        }
        return AUTH_ERRORCODE_NO_POLICY_SET;
    }
    return AUTH_ERRORCODE_POLICY_NOT_FOUND;
}

int GetCommunicationStrategy(RegParams params, PolicyTrans **policies, unsigned int *policyNum)
{
    if (IsUidValid(params.uid) == AUTH_ERRORCODE_INVALID_UID) {
        HILOG_ERROR(HILOG_MODULE_APP, "Invalid uid, [svc: %s][ft: %s][uid: %d][pid: %d][line: %d]",
                    params.service, params.feature, params.uid, params.pid, __LINE__);
        return AUTH_ERRORCODE_INVALID_UID;
    }

    int res = SetPresetPolicies(g_presetPolicies, g_presetPolicySize, params, policies, policyNum);
    if (res != AUTH_ERRORCODE_POLICY_NOT_FOUND) {
        return res;
    }
#if  POLICY_PRODUCT
    res = SetPresetPolicies(g_productPolicies, g_productPolicySize, params, policies, policyNum);
    if (res != AUTH_ERRORCODE_POLICY_NOT_FOUND) {
        return res;
    }
#endif
    res = SetPresetPolicies(g_registryPolicies, g_regPoliciesSize, params, policies, policyNum);
    if (res != AUTH_ERRORCODE_POLICY_NOT_FOUND) {
        return res;
    }
    return AUTH_ERRORCODE_NO_POLICY_SET;
}

static int IsUidFixed(const int fixedUid[], unsigned int consumerUid)
{
    for (int m = 0; m < UID_SIZE; m++) {
        if (fixedUid[m] == consumerUid) {
            return AUTH_ERRORCODE_SUCCESS;
        }
    }
    return AUTH_ERRORCODE_ACCESS_DENIED;
}

static int CheckPolicy(const IpcPolicy *policy, unsigned int consumerUid)
{
    switch (policy->type) {
        case RANGE:
            if (consumerUid >= policy->uidMin && consumerUid <= policy->uidMax) {
                return AUTH_ERRORCODE_SUCCESS;
            }
            break;
        case FIXED:
            if (IsUidFixed(policy->fixedUid, consumerUid) == AUTH_ERRORCODE_SUCCESS) {
                return AUTH_ERRORCODE_SUCCESS;
            }
            break;
#ifdef OHOS_APPFWK_ENABLE
        case BUNDLENAME: {
            unsigned int uid = 0;
            int ret = GetUidByBundleName(policy->bundleName, &uid);
            if (ret != AUTH_ERRORCODE_SUCCESS) {
                return ret;
            }
            if (uid == consumerUid) {
                return AUTH_ERRORCODE_SUCCESS;
            }
            break;
        }
#endif
        default:
            break;
    }
    return AUTH_ERRORCODE_ACCESS_DENIED;
}

static int CheckFeaturePolicies(const FeaturePolicy *featurePolicy, unsigned int consumerUid)
{
    for (int k = 0; k < POLICY_SIZE; k++) {
        IpcPolicy policy = featurePolicy->policies[k];
        int ret = CheckPolicy(&policy, consumerUid);
        if (ret == AUTH_ERRORCODE_SUCCESS) {
            return ret;
        }
    }
    return AUTH_ERRORCODE_ACCESS_DENIED;
}

static int CheckSvcPolicies(const PolicySetting policySetting[], int policySize, const AuthParams *params)
{
    for (int i = 0; i < policySize; i++) {
        if (strcmp(policySetting[i].service, params->providerService) != 0) {
            continue;
        }
        for (int j = 0; j < policySetting[i].featureNum; j++) {
            FeaturePolicy *featurePolicy = (FeaturePolicy *)policySetting[i].features + j;
            char *s1 = featurePolicy->feature;
            char *s2 = params->providerfeature;
            if (StrcmpWithNull(s1, s2) != 0) {
                continue;
            }
            int ret = CheckFeaturePolicies(featurePolicy, params->consumerUid);
            if (ret == AUTH_ERRORCODE_SUCCESS) {
                return ret;
            }
            break;
        }
        break;
    }
    return AUTH_ERRORCODE_ACCESS_DENIED;
}

int IsCommunicationAllowed(AuthParams params)
{
    if (CheckSvcPolicies(g_presetPolicies, g_presetPolicySize, &params) == AUTH_ERRORCODE_SUCCESS) {
        return AUTH_ERRORCODE_SUCCESS;
    }
#if  POLICY_PRODUCT
    if (CheckSvcPolicies(g_productPolicies, g_productPolicySize, &params) == AUTH_ERRORCODE_SUCCESS) {
        return AUTH_ERRORCODE_SUCCESS;
    }
#endif
    if (CheckSvcPolicies(g_registryPolicies, g_regPoliciesSize, &params) == AUTH_ERRORCODE_SUCCESS) {
        return AUTH_ERRORCODE_SUCCESS;
    }

    HILOG_ERROR(HILOG_MODULE_APP,
        "Access denied, [consumerUid: %d][consumerPid: %d][providerUid: %d][providerPid: %d][line: %d]",
        params.consumerUid, params.consumerPid, params.providerUid, params.providerPid, __LINE__);
    return AUTH_ERRORCODE_ACCESS_DENIED;
}
