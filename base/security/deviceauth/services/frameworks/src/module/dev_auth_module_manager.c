/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "dev_auth_module_manager.h"
#include "common_defs.h"
#include "das_module.h"
#include "hc_log.h"
#include "hc_vector.h"
#include "account_module.h"
#include "version_util.h"

DECLARE_HC_VECTOR(AuthModuleVec, void *);
IMPLEMENT_HC_VECTOR(AuthModuleVec, void *, 1)

static AuthModuleVec g_authModuleVec;
static VersionStruct g_version;

int32_t CheckMsgRepeatability(const CJson *in, int moduleType)
{
    if (in == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    switch (moduleType) {
        case DAS_MODULE:
            return IsDasMsgNeedIgnore(in) ? HC_ERR_IGNORE_MSG : HC_SUCCESS;
        case ACCOUNT_MODULE:
            return CheckAccountMsgRepeatability(in);
        default:
            LOGE("Unsupported module type: %d.", moduleType);
            return HC_ERR_MODULE_NOT_FOUNT;
    }
    return HC_ERROR;
}

static AuthModuleBase *GetModule(int moduleType)
{
    uint32_t index;
    void **module = NULL;
    FOR_EACH_HC_VECTOR(g_authModuleVec, index, module) {
        if ((module != NULL) && (*module != NULL)) {
            if (moduleType == ((AuthModuleBase *)(*module))->moduleType) {
                return *module;
            }
        }
    }
    LOGE("There is no matched module, moduleType: %d.", moduleType);
    return NULL;
}

static bool IsParamsForDasTokenManagerValid(const char *pkgName, const char *serviceType, Uint8Buff *authId,
    int userType, int moduleType)
{
    if (moduleType != DAS_MODULE) {
        LOGE("Unsupported method in the module, moduleType: %d.", moduleType);
        return false;
    }
    if (pkgName == NULL || serviceType == NULL || authId == NULL || authId->val == NULL) {
        LOGE("Params is null.");
        return false;
    }

    if (HcStrlen(pkgName) == 0 || HcStrlen(serviceType) == 0 || authId->length == 0) {
        LOGE("The length of params is invalid!");
        return false;
    }
    if (userType < DEVICE_TYPE_ACCESSORY || userType > DEVICE_TYPE_PROXY) {
        LOGE("Invalid userType, userType: %d.", userType);
        return false;
    }
    return true;
}

int32_t RegisterLocalIdentity(const char *pkgName, const char *serviceType, Uint8Buff *authId, int userType,
    int moduleType)
{
    if (!IsParamsForDasTokenManagerValid(pkgName, serviceType, authId, userType, moduleType)) {
        LOGE("Params for RegisterLocalIdentity is invalid.");
        return HC_ERR_INVALID_PARAMS;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module for das.");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    DasAuthModule *dasModule = (DasAuthModule *)module;
    int32_t res = dasModule->registerLocalIdentity(pkgName, serviceType, authId, userType);
    if (res != HC_SUCCESS) {
        LOGE("Register local identity failed, res: %x.", res);
        return res;
    }
    return HC_SUCCESS;
}

int32_t UnregisterLocalIdentity(const char *pkgName, const char *serviceType, Uint8Buff *authId, int userType,
    int moduleType)
{
    if (!IsParamsForDasTokenManagerValid(pkgName, serviceType, authId, userType, moduleType)) {
        LOGE("Params for UnregisterLocalIdentity is invalid.");
        return HC_ERR_INVALID_PARAMS;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module for das.");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    DasAuthModule *dasModule = (DasAuthModule *)module;
    int32_t res = dasModule->unregisterLocalIdentity(pkgName, serviceType, authId, userType);
    if (res != HC_SUCCESS) {
        LOGE("Unregister local identity failed, res: %x.", res);
        return res;
    }
    return HC_SUCCESS;
}

int32_t DeletePeerAuthInfo(const char *pkgName, const char *serviceType, Uint8Buff *authId, int userType,
    int moduleType)
{
    if (!IsParamsForDasTokenManagerValid(pkgName, serviceType, authId, userType, moduleType)) {
        LOGE("Params for DeletePeerAuthInfo is invalid.");
        return HC_ERR_INVALID_PARAMS;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module for das.");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    DasAuthModule *dasModule = (DasAuthModule *)module;
    int32_t res = dasModule->deletePeerAuthInfo(pkgName, serviceType, authId, userType);
    if (res != HC_SUCCESS) {
        LOGE("Delete peer authInfo failed, res: %x.", res);
        return res;
    }
    return HC_SUCCESS;
}

int32_t GetPublicKey(int moduleType, AuthModuleParams *params, Uint8Buff *returnPk)
{
    if (params == NULL || returnPk == NULL ||
        !IsParamsForDasTokenManagerValid(params->pkgName, params->serviceType,
        params->authId, params->userType, moduleType)) {
        LOGE("Params for GetPublicKey is invalid.");
        return HC_ERR_INVALID_PARAMS;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module for das.");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    DasAuthModule *dasModule = (DasAuthModule *)module;
    int32_t res = dasModule->getPublicKey(params->pkgName, params->serviceType,
        params->authId, params->userType, returnPk);
    if (res != HC_SUCCESS) {
        LOGE("Get public key failed, res: %d", res);
        return res;
    }
    return HC_SUCCESS;
}

int32_t ProcessTask(int taskId, const CJson *in, CJson *out, int32_t *status, int moduleType)
{
    if (in == NULL || out == NULL || status == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module!");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    if (module->processTask == NULL) {
        LOGE("Unsupported method in the module, moduleType: %d.", moduleType);
        return HC_ERR_UNSUPPORTED_METHOD;
    }
    int32_t res = module->processTask(taskId, in, out, status);
    if (res != HC_SUCCESS) {
        LOGE("Process task failed, taskId: %d, moduleType: %d, res: %d.", taskId, moduleType, res);
        return res;
    }
    res = AddSingleVersionToJson(out, &g_version);
    if (res != HC_SUCCESS) {
        LOGE("AddSingleVersionToJson failed, res: %x.", res);
        return res;
    }
    LOGI("Process task success, taskId: %d, moduleType: %d.", taskId, moduleType);
    return res;
}

int32_t CreateTask(int32_t *taskId, const CJson *in, CJson *out, int moduleType)
{
    if (in == NULL || out == NULL || taskId == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        LOGE("Failed to get module!");
        return HC_ERR_MODULE_NOT_FOUNT;
    }
    if (module->createTask == NULL) {
        LOGE("Unsupported method in the module, moduleType: %d.", moduleType);
        return HC_ERR_UNSUPPORTED_METHOD;
    }
    int32_t res = module->createTask(taskId, in, out);
    if (res != HC_SUCCESS) {
        LOGE("Create task failed, taskId: %d, moduleType: %d, res: %d.", *taskId, moduleType, res);
        return res;
    }
    LOGI("Create task success, taskId: %d, moduleType: %d.", *taskId, moduleType);
    return HC_SUCCESS;
}

void DestroyTask(int taskId, int moduleType)
{
    AuthModuleBase *module = GetModule(moduleType);
    if (module == NULL) {
        return;
    }
    if (module->destroyTask == NULL) {
        LOGE("Unsupported method in the module, moduleType: %d.", moduleType);
        return;
    }
    module->destroyTask(taskId);
}

static int32_t InitDasModule(void)
{
    AuthModuleBase *das = CreateDasModule();
    if (das == NULL) {
        LOGE("Create das module failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    g_authModuleVec.pushBackT(&g_authModuleVec, (void *)das);
    return HC_SUCCESS;
}

static int32_t InitAccountModule(void)
{
    AuthModuleBase *accountModule = CreateAccountModule();
    if (accountModule == NULL) {
        LOGE("Create account module failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    g_authModuleVec.pushBackT(&g_authModuleVec, (void *)accountModule);
    return HC_SUCCESS;
}

int32_t InitModules(void)
{
    g_authModuleVec = CREATE_HC_VECTOR(AuthModuleVec);
    InitGroupAndModuleVersion(&g_version);
    int res;
    if (IsDasSupported()) {
        res = InitDasModule();
        if (res != HC_SUCCESS) {
            LOGE("Init das module failed, res: %x.", res);
            DestroyModules();
            return res;
        }
        g_version.third |= DAS_MODULE;
    }
    if (IsAccountSupported()) {
        res = InitAccountModule();
        if (res != HC_SUCCESS) {
            LOGE("Init account module failed, res: %x.", res);
            DestroyModules();
            return res;
        }
        g_version.third |= ACCOUNT_MODULE;
    }
    LOGI("Init modules success!");
    return HC_SUCCESS;
}

void DestroyModules(void)
{
    uint32_t index;
    void **module = NULL;
    FOR_EACH_HC_VECTOR(g_authModuleVec, index, module) {
        if ((module != NULL) && (*module != NULL)) {
            ((AuthModuleBase *)(*module))->destroyModule((AuthModuleBase *)*module);
        }
    }
    DESTROY_HC_VECTOR(AuthModuleVec, &g_authModuleVec);
    (void)memset_s(&g_version, sizeof(VersionStruct), 0, sizeof(VersionStruct));
}

int32_t ProcessCredentials(int credentialOpCode, const CJson *in, CJson *out, int moduleType)
{
    if (moduleType != ACCOUNT_MODULE) {
        LOGE("Unsupported method in the module, moduleType: %d.", moduleType);
        return HC_ERR_NOT_SUPPORT;
    }

    return ProcessAccountCredentials(credentialOpCode, in, out);
}
