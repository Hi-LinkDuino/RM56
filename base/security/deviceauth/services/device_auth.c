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

#include "device_auth.h"

#include "alg_loader.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "common_defs.h"
#include "dev_auth_module_manager.h"
#include "group_auth_manager.h"
#include "group_manager.h"
#include "hc_init_protection.h"
#include "hc_log.h"
#include "json_utils.h"
#include "lcm_adapter.h"
#include "os_account_adapter.h"
#include "session_manager.h"
#include "task_manager.h"

static GroupAuthManager *g_groupAuthManager =  NULL;
static DeviceGroupManager *g_groupManagerInstance = NULL;

static void DestroyGroupAuthTask(HcTaskBase *task)
{
    AuthDeviceTask *realTask = (AuthDeviceTask *)task;
    FreeJson(realTask->authParams);
}

static bool InitAuthDeviceTask(int32_t osAccountId, AuthDeviceTask *task, int64_t authReqId, CJson *authParams,
    const DeviceAuthCallback *gaCallback)
{
    task->base.doAction = DoAuthDevice;
    task->base.destroy = DestroyGroupAuthTask;
    task->authReqId = authReqId;
    if (AddByteToJson(authParams, FIELD_REQUEST_ID, (const uint8_t*)&authReqId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add requestId to json!");
        return false;
    }
    if (AddIntToJson(authParams, FIELD_OS_ACCOUNT_ID, osAccountId) != HC_SUCCESS) {
        LOGE("Failed to add os accountId to json for auth device!");
        return false;
    }
    task->authParams = authParams;
    task->callback = gaCallback;
    if (task->callback == NULL) {
        LOGE("The input auth callback is null!");
        return false;
    }
    return true;
}

static bool InitProcessDataTask(AuthDeviceTask *task, int64_t authReqId,
    CJson *receivedData, const DeviceAuthCallback *gaCallback)
{
    task->base.doAction = DoProcessAuthData;
    task->base.destroy = DestroyGroupAuthTask;
    task->authReqId = authReqId;
    if (AddByteToJson(receivedData, FIELD_REQUEST_ID, (const uint8_t*)&authReqId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add requestId to json!");
        return false;
    }
    if (AddIntToJson(receivedData, FIELD_OPERATION_CODE, AUTHENTICATE) != HC_SUCCESS) {
        LOGE("Failed to add operation code to json!");
        return false;
    }
    task->authParams = receivedData;
    task->callback = gaCallback;
    if (task->callback == NULL) {
        LOGE("The input auth callback is null!");
        return false;
    }
    return true;
}

static int32_t AuthDevice(int32_t osAccountId, int64_t authReqId, const char *authParams,
    const DeviceAuthCallback *gaCallback)
{
    LOGI("Begin AuthDevice.");
    osAccountId = DevAuthGetRealOsAccountLocalId(osAccountId);
    if ((authParams == NULL) || (osAccountId == INVALID_OS_ACCOUNT)) {
        LOGE("The input auth params is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    CJson *jsonParams = CreateJsonFromString(authParams);
    if (jsonParams == NULL) {
        LOGE("Create json from params failed!");
        return HC_ERR_JSON_FAIL;
    }
    AuthDeviceTask *task = (AuthDeviceTask *)HcMalloc(sizeof(AuthDeviceTask), 0);
    if (task == NULL) {
        FreeJson(jsonParams);
        LOGE("Failed to allocate memory for task!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (!InitAuthDeviceTask(osAccountId, task, authReqId, jsonParams, gaCallback)) {
        LOGE("Failed to init task!");
        FreeJson(jsonParams);
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    if (PushTask((HcTaskBase*)task) != HC_SUCCESS) {
        FreeJson(jsonParams);
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("Push AuthDevice task successfully.");
    return HC_SUCCESS;
}

static int32_t ProcessData(int64_t authReqId, const uint8_t *data, uint32_t dataLen,
    const DeviceAuthCallback *gaCallback)
{
    LOGI("Begin ProcessData.");
    if ((data == NULL) || (dataLen > MAX_DATA_BUFFER_SIZE)) {
        LOGE("Invalid input for ProcessData!");
        return HC_ERR_INVALID_PARAMS;
    }
    CJson *receivedData = CreateJsonFromString((const char *)data);
    if (receivedData == NULL) {
        LOGE("Create Json for input data failed!");
        return HC_ERR_JSON_FAIL;
    }
    AuthDeviceTask *task = (AuthDeviceTask *)HcMalloc(sizeof(AuthDeviceTask), 0);
    if (task == NULL) {
        FreeJson(receivedData);
        LOGE("Failed to allocate memory for task!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (!InitProcessDataTask(task, authReqId, receivedData, gaCallback)) {
        LOGE("Failed to init task!");
        FreeJson(receivedData);
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    if (PushTask((HcTaskBase*)task) != HC_SUCCESS) {
        FreeJson(receivedData);
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("Push ProcessData task successfully.");
    return HC_SUCCESS;
}

static int32_t AllocGmAndGa(void)
{
    if (g_groupManagerInstance == NULL) {
        g_groupManagerInstance = (DeviceGroupManager *)HcMalloc(sizeof(DeviceGroupManager), 0);
        if (g_groupManagerInstance == NULL) {
            LOGE("Failed to allocate groupManager Instance memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
    }
    if (g_groupAuthManager == NULL) {
        g_groupAuthManager = (GroupAuthManager *)HcMalloc(sizeof(GroupAuthManager), 0);
        if (g_groupAuthManager == NULL) {
            LOGE("Failed to allocate groupAuth Instance memory!");
            HcFree(g_groupManagerInstance);
            g_groupManagerInstance = NULL;
            return HC_ERR_ALLOC_MEMORY;
        }
    }
    return HC_SUCCESS;
}

static void DestroyGmAndGa(void)
{
    if (g_groupAuthManager != NULL) {
        HcFree(g_groupAuthManager);
        g_groupAuthManager = NULL;
    }
    if (g_groupManagerInstance != NULL) {
        HcFree(g_groupManagerInstance);
        g_groupManagerInstance = NULL;
    }
}

static int32_t InitAllModules(void)
{
    int32_t res = GetLoaderInstance()->initAlg();
    if (res != HC_SUCCESS) {
        LOGE("[End]: [Service]: Failed to init algorithm module!");
        return res;
    }
    res = InitModules();
    if (res != HC_SUCCESS) {
        LOGE("[End]: [Service]: Failed to init all authenticator modules!");
        return res;
    }
    res = InitCallbackManager();
    if (res != HC_SUCCESS) {
        LOGE("[End]: [Service]: Failed to init callback manage module!");
        goto CLEAN_MODULE;
    }
    res = InitGroupManager();
    if (res != HC_SUCCESS) {
        goto CLEAN_CALLBACK;
    }
    InitSessionManager();
    res = InitTaskManager();
    if (res != HC_SUCCESS) {
        LOGE("[End]: [Service]: Failed to init worker thread!");
        goto CLEAN_ALL;
    }
    return res;
CLEAN_ALL:
    DestroySessionManager();
    DestroyGroupManager();
CLEAN_CALLBACK:
    DestroyCallbackManager();
CLEAN_MODULE:
    DestroyModules();
    return res;
}

DEVICE_AUTH_API_PUBLIC int InitDeviceAuthService(void)
{
    LOGI("[Service]: Start to init device auth service!");
    if (CheckInit() == FINISH_INIT) {
        LOGI("[End]: [Service]: Device auth service is running!");
        return HC_SUCCESS;
    }
    int32_t res = AllocGmAndGa();
    if (res != HC_SUCCESS) {
        return res;
    }
    res = InitAllModules();
    if (res != HC_SUCCESS) {
        DestroyGmAndGa();
        return res;
    }
    SetInitStatus();
    LOGI("[End]: [Service]: Init device auth service successfully!");
    return HC_SUCCESS;
}

DEVICE_AUTH_API_PUBLIC void DestroyDeviceAuthService(void)
{
    LOGI("[Service]: Start to destroy device auth service!");
    if (CheckDestroy() == FINISH_DESTROY) {
        LOGI("[End]: [Service]: The service has not been initialized!");
        return;
    }
    DestroyTaskManager();
    DestroyGroupManager();
    DestroySessionManager();
    DestroyGmAndGa();
    DestroyModules();
    DestroyChannelManager();
    DestroyCallbackManager();
    SetDeInitStatus();
    LOGI("[End]: [Service]: Destroy device auth service successfully!");
}

DEVICE_AUTH_API_PUBLIC const DeviceGroupManager *GetGmInstance(void)
{
    if (g_groupManagerInstance == NULL) {
        LOGE("Service not init.");
        return NULL;
    }

    g_groupManagerInstance->regCallback = RegGroupManagerCallback;
    g_groupManagerInstance->unRegCallback = UnRegGroupManagerCallback;
    g_groupManagerInstance->regDataChangeListener = RegListenerImpl;
    g_groupManagerInstance->unRegDataChangeListener = UnRegListenerImpl;
    g_groupManagerInstance->createGroup = CreateGroupImpl;
    g_groupManagerInstance->deleteGroup = DeleteGroupImpl;
    g_groupManagerInstance->addMemberToGroup = AddMemberToGroupImpl;
    g_groupManagerInstance->deleteMemberFromGroup = DeleteMemberFromGroupImpl;
    g_groupManagerInstance->processData = ProcessBindDataImpl;
    g_groupManagerInstance->processCredential = ProcessCredential;
    g_groupManagerInstance->getRegisterInfo = GetRegisterInfo;
    g_groupManagerInstance->checkAccessToGroup = CheckAccessToGroupImpl;
    g_groupManagerInstance->getPkInfoList = GetPkInfoListImpl;
    g_groupManagerInstance->getGroupInfoById = GetGroupInfoByIdImpl;
    g_groupManagerInstance->getGroupInfo = GetGroupInfoImpl;
    g_groupManagerInstance->getJoinedGroups = GetJoinedGroupsImpl;
    g_groupManagerInstance->getRelatedGroups = GetRelatedGroupsImpl;
    g_groupManagerInstance->getDeviceInfoById = GetDeviceInfoByIdImpl;
    g_groupManagerInstance->getTrustedDevices = GetTrustedDevicesImpl;
    g_groupManagerInstance->isDeviceInGroup = IsDeviceInGroupImpl;
    g_groupManagerInstance->destroyInfo = DestroyInfoImpl;
    return g_groupManagerInstance;
}

DEVICE_AUTH_API_PUBLIC const GroupAuthManager *GetGaInstance(void)
{
    if (g_groupAuthManager == NULL) {
        LOGE("Service not init.");
        return NULL;
    }

    g_groupAuthManager->processData = ProcessData;
    g_groupAuthManager->authDevice = AuthDevice;
    return g_groupAuthManager;
}
