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
#include <ohos_init.h>
#include <pthread.h>
#include <string.h>

#include "log.h"

#include "feature.h"
#include "iproxy_client.h"
#include "iproxy_server.h"
#include "iunknown.h"
#include "liteipc_adapter.h"
#include "samgr_lite.h"
#include "service.h"

#include "pms.h"
#include "pms_common.h"
#include "pms_types.h"

typedef struct InnerPermLiteApi {
    INHERIT_SERVER_IPROXY;
    int (*CheckPermission)(int uid, const char *permissionName);
    int (*GrantPermission)(const char *identifier, const char *permName);
    int (*RevokePermission)(const char *identifier, const char *permName);
    int (*GrantRuntimePermission)(int uid, const char *permissionName);
    int (*RevokeRuntimePermission)(int uid, const char *permissionName);
    int (*UpdatePermissionFlags)(const char *identifier, const char *permissionName, const int flags);
} InnerPermLiteApi;

typedef struct InnerPermLite {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(InnerPermLiteApi);
    Identity identity;
} InnerPermLite;

enum INNERFUNCID {
    ID_CHECK = 10,
    ID_GRANT,
    ID_REVOKE,
    ID_GRANT_RUNTIME,
    ID_REVOKE_RUNTIME,
    ID_UPDATE_PERMS_FLAGS,
};

static void Init();
static const char *GetName(Feature *feature);
static void OnInitialize(Feature *feature, Service *parent, Identity identity);
static void OnStop(Feature *feature, Identity identity);
static BOOL OnMessage(Feature *feature, Request *request);
static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply);

static InnerPermLite g_permlite = {
    .GetName = GetName,
    .OnInitialize = OnInitialize,
    .OnStop = OnStop,
    .OnMessage = OnMessage,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    .CheckPermission = CheckPermissionStat,
    .GrantPermission = GrantPermission,
    .RevokePermission = RevokePermission,
    .GrantRuntimePermission = GrantRuntimePermission,
    .RevokeRuntimePermission = RevokeRuntimePermission,
    .UpdatePermissionFlags = UpdatePermissionFlags,
    IPROXY_END,
    .identity = {-1, -1, NULL},
};

static void Init()
{
    SAMGR_GetInstance()->RegisterFeature(PERMISSION_SERVICE, (Feature *)&g_permlite);
    SAMGR_GetInstance()->RegisterFeatureApi(PERMISSION_SERVICE, PERM_INNER_FEATURE, GET_IUNKNOWN(g_permlite));
    HILOG_INFO(HILOG_MODULE_APP, "Init pms lite inner feature success");
}
APP_FEATURE_INIT(Init);

static const char *GetName(Feature *feature)
{
    (void)feature;
    return PERM_INNER_FEATURE;
}

static void OnInitialize(Feature *feature, Service *parent, Identity identity)
{
    (void)parent;
    if (feature == NULL) {
        return;
    }
    InnerPermLite *permlite = (InnerPermLite *)feature;
    permlite->identity = identity;
    HILOG_INFO(HILOG_MODULE_APP, "onInitialize pms lite inner feature");
}

static void OnStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
}

static BOOL OnMessage(Feature *feature, Request *request)
{
    if (feature == NULL || request == NULL) {
        return FALSE;
    }
    // call func
    return TRUE;
}

static void ReplyCheckPermission(const void *origin, IpcIo *req, IpcIo *reply, InnerPermLiteApi* api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_CHECK, [callerPid: %d][callerUid: %u]", callingPid, callingUid);

    size_t permLen = 0;
    int64_t uid = IpcIoPopInt64(req);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t ret = api->CheckPermission(uid, permName);
    HILOG_INFO(HILOG_MODULE_APP, "check permission, [uid: %lld][perm: %s][ret: %d]", uid, permName, ret);
    IpcIoPushInt32(reply, ret);
}

static void ReplyGrantPermission(const void *origin, IpcIo *req, IpcIo *reply, InnerPermLiteApi* api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_GRANT, [callerPid: %d][callerUid: %u]", callingPid, callingUid);
    size_t permLen = 0;
    size_t idLen = 0;
    char *identifier = (char *)IpcIoPopString(req, &idLen);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t ret = api->GrantPermission(identifier, permName);
    HILOG_INFO(HILOG_MODULE_APP, "grant permission, [id: %s][perm: %s][ret: %d]", identifier, permName, ret);
    IpcIoPushInt32(reply, ret);
}

static void ReplyRevokePermission(const void *origin, IpcIo *req, IpcIo *reply, InnerPermLiteApi* api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_REVOKE, [callerPid: %d][callerUid: %u]", callingPid, callingUid);
    size_t permLen = 0;
    size_t idLen = 0;
    char *identifier = (char *)IpcIoPopString(req, &idLen);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t ret = api->RevokePermission(identifier, permName);
    HILOG_INFO(HILOG_MODULE_APP, "revoke permission, [id: %s][perm: %s][ret: %d]", identifier, permName, ret);
    IpcIoPushInt32(reply, ret);
}

static void ReplyGrantRuntimePermission(const void *origin, IpcIo *req, IpcIo *reply, InnerPermLiteApi* api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_GRANTRUNTIME, [callerPid: %d][callerUid: %u]", callingPid, callingUid);
    size_t permLen = 0;
    int64_t uid = IpcIoPopInt64(req);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t ret = api->GrantRuntimePermission(uid, permName);
    HILOG_INFO(HILOG_MODULE_APP, "grant runtime permission, [uid: %lld][perm: %s][ret: %d]", uid, permName, ret);
    IpcIoPushInt32(reply, ret);
}

static void ReplyRevokeRuntimePermission(const void *origin, IpcIo *req, IpcIo *reply, InnerPermLiteApi* api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_REVOKERUNTIME, [callerPid: %d][callerUid: %u]", callingPid, callingUid);
    size_t permLen = 0;
    int64_t uid = IpcIoPopInt64(req);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t ret = api->RevokeRuntimePermission(uid, permName);
    HILOG_INFO(HILOG_MODULE_APP, "revoke runtime permission, [uid: %lld][perm: %s][ret: %d]", uid, permName, ret);
    IpcIoPushInt32(reply, ret);
}

static void ReplyUpdatePermissionFlags(const void *origin, IpcIo *req, IpcIo *reply, const InnerPermLiteApi *api)
{
    pid_t callingPid = GetCallingPid(origin);
    uid_t callingUid = GetCallingUid(origin);
    HILOG_INFO(HILOG_MODULE_APP, "Enter ID_UPDATE_PERMS_FLAGS, [callerPid: %d][callerUid: %u]", callingPid, callingUid);
    size_t permLen = 0;
    size_t idLen = 0;
    char *identifier = (char *)IpcIoPopString(req, &idLen);
    char *permName = (char *)IpcIoPopString(req, &permLen);
    int32_t flags = IpcIoPopInt32(req);
    int32_t ret = api->UpdatePermissionFlags(identifier, permName, flags);
    HILOG_INFO(HILOG_MODULE_APP, "update runtime permission flags, [identifier: %s][perm: %s][flags:%d][ret: %d]",
        identifier, permName, flags, ret);
    IpcIoPushInt32(reply, ret);
}

static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    InnerPermLiteApi *api = (InnerPermLiteApi *)iProxy;
    switch (funcId) {
        case ID_CHECK:
            ReplyCheckPermission(origin, req, reply, api);
            break;
        case ID_GRANT:
            ReplyGrantPermission(origin, req, reply, api);
            break;
        case ID_REVOKE:
            ReplyRevokePermission(origin, req, reply, api);
            break;
        case ID_GRANT_RUNTIME:
            ReplyGrantRuntimePermission(origin, req, reply, api);
            break;
        case ID_REVOKE_RUNTIME:
            ReplyRevokeRuntimePermission(origin, req, reply, api);
            break;
        case ID_UPDATE_PERMS_FLAGS:
            ReplyUpdatePermissionFlags(origin, req, reply, api);
            break;
        default:
            break;
    }
    return EC_SUCCESS;
}
