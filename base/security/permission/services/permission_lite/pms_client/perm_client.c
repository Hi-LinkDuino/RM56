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

#include <ohos_errno.h>
#include <pthread.h>
#include <securec.h>

#include "cJSON.h"
#include "iproxy_client.h"
#include "liteipc_adapter.h"
#include "log.h"
#include "pms_interface.h"
#include "pms_types.h"
#include "registry.h"
#include "samgr_lite.h"

#define PERMISSION_SERVICE "permissionms"
#define PERM_FEATURE "PmsFeature"
#define PERM_INNER_FEATURE "PmsInnerFeature"
#define MAX_DATA_LEN 0x100
#define FIELD_PERMISSION "permissions"
#define FIELD_NAME "name"
#define FIELD_DESC "desc"
#define FIELD_IS_GRANTED "isGranted"
#define FIELD_FLAGS "flags"
#define SYS_SVC_UID_MAX 99
#define SYS_APP_UID_MIN 100
#define SYS_APP_UID_MAX 999
#define PERMISSION_NUM_MAX 1000

enum FUNCID {
    ID_CHECK_SELF = 0,
    ID_QUERY,
    ID_CHECK = 10,
    ID_GRANT,
    ID_REVOKE,
    ID_GRANT_RUNTIME,
    ID_REVOKE_RUNTIME,
    ID_UPDATE_PERMS_FLAGS,
};

typedef struct ClientApi {
    INHERIT_CLIENT_IPROXY;
    int (*CheckSelfPermission)(const char *permissionName);
    int (*QueryPermission)(const char *identifier, PermissionSaved **permissions, int *permNum);
} ClientApi;

typedef struct ClientEntry {
    INHERIT_IUNKNOWNENTRY(ClientApi);
} ClientEntry;

typedef struct InnerClientApi {
    INHERIT_CLIENT_IPROXY;
    int (*CheckPermission)(int uid, const char *permissionName);
    int (*GrantPermission)(const char *identifier, const char *permName);
    int (*RevokePermission)(const char *identifier, const char *permName);
    int (*GrantRuntimePermission)(int uid, const char *permissionName);
    int (*RevokeRuntimePermission)(int uid, const char *permissionName);
    int (*UpdatePermissionFlags)(const char *identifier, const char *permissionName, int flags);
} InnerClientApi;

typedef struct ClientInnerEntry {
    INHERIT_IUNKNOWNENTRY(InnerClientApi);
} ClientInnerEntry;

typedef struct RetOfQueryPerms {
    int resultCode;
    int length;
    PermissionSaved *permission;
} RetOfQueryPerms;

typedef struct RetOfGetDevUdid {
    unsigned char *udid;
    int size;
    int result;
} RetOfGetDevUdid;

void *CreatClient(const char *service, const char *feature, uint32 size)
{
    (void)service;
    (void)feature;
    uint32 len = size + sizeof(ClientEntry);
    if ((len < size) || (len < sizeof(ClientEntry))) {
        return NULL;
    }
    uint8 *client = malloc(len);
    if (client == NULL) {
        return NULL;
    }
    (void)memset_s(client, len, 0, len);
    ClientEntry *entry = (ClientEntry *)&client[size];
    entry->ver = ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.CheckSelfPermission = CheckSelfPermission;
    entry->iUnknown.QueryPermission = QueryPermission;
    return client;
}

void DestroyClient(const char *service, const char *feature, void *iproxy)
{
    (void)service;
    (void)feature;
    free(iproxy);
}

static ClientApi *GetClientApi(void)
{
    SAMGR_RegisterFactory(PERMISSION_SERVICE, PERM_FEATURE, CreatClient, DestroyClient);
    ClientApi *clientApi = NULL;
    HILOG_INFO(HILOG_MODULE_APP, "[GetFeatureApi S:%s F:%s]: BEGIN\n", PERMISSION_SERVICE, PERM_FEATURE);
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(PERMISSION_SERVICE, PERM_FEATURE);
    if (iUnknown == NULL) {
        HILOG_INFO(HILOG_MODULE_APP, "[GetFeatureApi S:%s F:%s]: error is NULL\n", PERMISSION_SERVICE, PERM_FEATURE);
        return NULL;
    }

    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&clientApi);
    HILOG_INFO(HILOG_MODULE_APP, "[QueryInterface CLIENT_PROXY_VER S:%s, F:%s]\n",
               PERMISSION_SERVICE, PERM_FEATURE);
    return clientApi;
}

static void ReleaseClientApi(ClientApi *clientApi)
{
    if (clientApi == NULL) {
        return;
    }
    int32 ref = clientApi->Release((IUnknown *)clientApi);
    HILOG_INFO(HILOG_MODULE_APP, "[Release api S:%s, F:%s]: ref:%d\n",
               PERMISSION_SERVICE, PERM_FEATURE, ref);
}

void *CreatInnerClient(const char *service, const char *feature, uint32 size)
{
    (void)service;
    (void)feature;
    uint32 len = size + sizeof(ClientInnerEntry);
    if ((len < size) || (len < sizeof(ClientInnerEntry))) {
        return NULL;
    }
    uint8 *client = malloc(len);
    if (client == NULL) {
        return NULL;
    }
    (void)memset_s(client, len, 0, len);
    ClientInnerEntry *entry = (ClientInnerEntry *)&client[size];
    entry->ver = ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.CheckPermission = CheckPermission;
    entry->iUnknown.GrantPermission = GrantPermission;
    entry->iUnknown.RevokePermission = RevokePermission;
    entry->iUnknown.GrantRuntimePermission = GrantRuntimePermission;
    entry->iUnknown.RevokeRuntimePermission = RevokeRuntimePermission;
    entry->iUnknown.UpdatePermissionFlags = UpdatePermissionFlags;
    return client;
}

void DestroyInnerClient(const char *service, const char *feature, void *iproxy)
{
    (void)service;
    (void)feature;
    free(iproxy);
}

static InnerClientApi *GetInnerClientApi(void)
{
    SAMGR_RegisterFactory(PERMISSION_SERVICE, PERM_INNER_FEATURE, CreatInnerClient, DestroyInnerClient);
    InnerClientApi *clientApi = NULL;
    HILOG_INFO(HILOG_MODULE_APP, "[GetFeatureApi S:%s F:%s]: BEGIN\n", PERMISSION_SERVICE, PERM_INNER_FEATURE);
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(PERMISSION_SERVICE, PERM_INNER_FEATURE);
    if (iUnknown == NULL) {
        HILOG_INFO(HILOG_MODULE_APP, "[GetFeatureApi S:%s F:%s]: error is NULL\n", PERMISSION_SERVICE,
            PERM_INNER_FEATURE);
        return NULL;
    }
    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&clientApi);
    HILOG_INFO(HILOG_MODULE_APP, "[QueryInterface CLIENT_PROXY_VER S:%s, F:%s]\n",
               PERMISSION_SERVICE, PERM_INNER_FEATURE);
    return clientApi;
}

static void ReleaseInnerClientApi(InnerClientApi *clientApi)
{
    if (clientApi == NULL) {
        return;
    }
    int32 ref = clientApi->Release((IUnknown *)clientApi);
    HILOG_INFO(HILOG_MODULE_APP, "[Release api S:%s, F:%s]: ref:%d\n",
               PERMISSION_SERVICE, PERM_INNER_FEATURE, ref);
}

static int ParsePermissions(const char *jsonStr, PermissionSaved **perms, int *permNum)
{
    cJSON *root = cJSON_Parse(jsonStr);
    if (root == NULL) {
        return PERM_ERRORCODE_JSONPARSE_FAIL;
    }
    cJSON *array = cJSON_GetObjectItem(root, FIELD_PERMISSION);
    int pSize = cJSON_GetArraySize(array);
    if (pSize > PERMISSION_NUM_MAX) {
        return PERM_ERRORCODE_JSONPARSE_FAIL;
    }

    int allocSize = sizeof(PermissionSaved) * pSize;
    if (allocSize == 0) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_SUCCESS;
    }

    *perms = (PermissionSaved *) malloc(allocSize);
    if (*perms == NULL) {
        cJSON_Delete(root);
        return PERM_ERRORCODE_MALLOC_FAIL;
    }
    for (int i = 0; i < pSize; i++) {
        cJSON *object = cJSON_GetArrayItem(array, i);
        cJSON *itemName = cJSON_GetObjectItem(object, FIELD_NAME);
        cJSON *itemDesc = cJSON_GetObjectItem(object, FIELD_DESC);
        cJSON *itemGranted = cJSON_GetObjectItem(object, FIELD_IS_GRANTED);
        if (itemName == NULL || itemDesc == NULL || itemGranted == NULL) {
            cJSON_Delete(root);
            free(*perms);
            *perms = NULL;
            return PERM_ERRORCODE_JSONPARSE_FAIL;
        }
        if (strcpy_s((*perms + i)->name, PERM_NAME_LEN, itemName->valuestring) != EOK) {
            cJSON_Delete(root);
            free(*perms);
            *perms = NULL;
            return PERM_ERRORCODE_COPY_ERROR;
        }
        if (strcpy_s((*perms + i)->desc, PERM_DESC_LEN, itemDesc->valuestring) != EOK) {
            cJSON_Delete(root);
            free(*perms);
            *perms = NULL;
            return PERM_ERRORCODE_COPY_ERROR;
        }
        (*perms + i)->granted = (enum IsGranted) itemGranted->valueint;
    }
    *permNum = pSize;
    cJSON_Delete(root);
    return PERM_ERRORCODE_SUCCESS;
}

static int Notify(IOwner owner, int code, IpcIo *reply)
{
    if ((reply == NULL) || (owner == NULL)) {
        HILOG_ERROR(HILOG_MODULE_APP, "Lite Ipc reply or owner is NULL");
        return OHOS_FAILURE;
    }

    int32_t *ret = (int32_t *)owner;
    *ret = IpcIoPopInt32(reply);

    return EC_SUCCESS;
}

static int DealQueryReply(IOwner owner, int code, IpcIo *reply)
{
    if ((reply == NULL) || (owner == NULL)) {
        return OHOS_FAILURE;
    }
    int resultCode = IpcIoPopInt32(reply);
    RetOfQueryPerms *ret = (RetOfQueryPerms *)(owner);
    if (resultCode != PERM_ERRORCODE_SUCCESS) {
        ret->resultCode = resultCode;
        return resultCode;
    }
#ifndef __LINUX__
    BuffPtr *buff = IpcIoPopDataBuff(reply);
    if (buff == NULL) {
        ret->resultCode = OHOS_FAILURE;
        HILOG_ERROR(HILOG_MODULE_APP, "Permission string popped is empty!");
        return OHOS_FAILURE;
    }
    char *jsonStr = (char *)(buff->buff);
#else
    char *jsonStr = (char *)IpcIoPopString(reply, NULL);
#endif
    HILOG_INFO(HILOG_MODULE_APP, "[perms: %s]", jsonStr);
    int retCode = ParsePermissions(jsonStr, &(ret->permission), &(ret->length));
#ifndef __LINUX__
    FreeBuffer(NULL, buff->buff);
#endif
    ret->resultCode = retCode;
    return retCode;
}

int CheckSelfPermission(const char *permissionName)
{
    uid_t callingUid = getuid();
    if (callingUid <= SYS_APP_UID_MAX) {
        return GRANTED;
    }
    ClientApi *proxy = GetClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushString(&request, permissionName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_CHECK_SELF, &request, &ret, Notify);
    ReleaseClientApi(proxy);
    return ret;
}

int CheckPermission(int uid, const char *permissionName)
{
    uid_t callingUid = getuid();
    if (callingUid <= SYS_APP_UID_MAX) {
        return GRANTED;
    }
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushInt64(&request, uid);
    IpcIoPushString(&request, permissionName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_CHECK, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    return ret;
}

int QueryPermission(const char *identifier, PermissionSaved **permissions, int *permNum)
{
    ClientApi *proxy = GetClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    if (permissions == NULL || permNum == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushString(&request, identifier);
    RetOfQueryPerms ret = {
        .resultCode = 0,
        .length = 0,
        .permission = NULL
    };
    proxy->Invoke((IClientProxy *)proxy, ID_QUERY, &request, &ret, DealQueryReply);
    *permissions = ret.permission;
    *permNum = ret.length;
    ReleaseClientApi(proxy);
    return ret.resultCode;
}

int GrantPermission(const char *identifier, const char *permName)
{
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushString(&request, identifier);
    IpcIoPushString(&request, permName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_GRANT, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    HILOG_INFO(HILOG_MODULE_APP, "client grant[ret: %d]", ret);
    return ret;
}

int RevokePermission(const char *identifier, const char *permName)
{
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushString(&request, identifier);
    IpcIoPushString(&request, permName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_REVOKE, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    HILOG_INFO(HILOG_MODULE_APP, "client revoke[ret: %d]", ret);
    return ret;
}

int GrantRuntimePermission(int uid, const char *permissionName)
{
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushInt64(&request, uid);
    IpcIoPushString(&request, permissionName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_GRANT_RUNTIME, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    return ret;
}

int RevokeRuntimePermission(int uid, const char *permissionName)
{
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushInt64(&request, uid);
    IpcIoPushString(&request, permissionName);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_REVOKE_RUNTIME, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    return ret;
}

int UpdatePermissionFlags(const char *identifier, const char *permissionName, const int flags)
{
    InnerClientApi *proxy = GetInnerClientApi();
    if (proxy == NULL) {
        return OHOS_FAILURE;
    }
    IpcIo request;
    char data[MAX_DATA_LEN];
    IpcIoInit(&request, data, MAX_DATA_LEN, 0);
    IpcIoPushString(&request, identifier);
    IpcIoPushString(&request, permissionName);
    IpcIoPushInt32(&request, flags);
    int32_t ret = -1;
    proxy->Invoke((IClientProxy *)proxy, ID_UPDATE_PERMS_FLAGS, &request, &ret, Notify);
    ReleaseInnerClientApi(proxy);
    return ret;
}

