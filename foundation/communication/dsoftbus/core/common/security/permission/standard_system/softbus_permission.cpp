/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "softbus_permission.h"

#include <sys/types.h>
#include <unistd.h>

#include "ipc_skeleton.h"
#include "permission_entry.h"
#include "softbus_adapter_mem.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "sys_mgr_client.h"
#include "system_ability_definition.h"

namespace {
    const std::string PERMISSION_JSON_FILE = "/system/etc/communication/softbus/softbus_trans_permission.json";
    const std::string DANGER_APP_PERMISSION = "ohos.permission.DISTRIBUTED_DATASYNC";
    const int32_t SYSTEM_UID = 1000;
    const int32_t MULTE_USER_RADIX = 100000;
}

int32_t TransPermissionInit(void)
{
    return LoadPermissionJson(PERMISSION_JSON_FILE.c_str());
}

void TransPermissionDeinit(void)
{
    DeinitPermissionJson();
}

int32_t CheckTransPermission(pid_t callingUid, pid_t callingPid,
    const char *pkgName, const char *sessionName, uint32_t actions)
{
    if (sessionName == nullptr || pkgName == nullptr) {
        return SOFTBUS_PERMISSION_DENIED;
    }
    SoftBusPermissionItem *pItem = CreatePermissionItem(NATIVE_APP, callingUid, callingPid, pkgName, actions);
    if (pItem == nullptr) {
        return SOFTBUS_MALLOC_ERR;
    }
    int32_t ret = CheckPermissionEntry(sessionName, pItem);
    SoftBusFree(pItem);
    if (ret >= SYSTEM_APP) {
        return SOFTBUS_OK;
    }
    return SOFTBUS_PERMISSION_DENIED;
}

int32_t CheckTransSecLevel(const char *mySessionName, const char *peerSessionName)
{
    if (mySessionName == nullptr || peerSessionName == nullptr) {
        return SOFTBUS_INVALID_PARAM;
    }
    if (strcmp(mySessionName, peerSessionName) == 0) {
        return SOFTBUS_OK;
    }
    if (!PermIsSecLevelPublic(mySessionName)) {
        return SOFTBUS_PERMISSION_DENIED;
    }
    if (!PermIsSecLevelPublic(peerSessionName)) {
        return SOFTBUS_PERMISSION_DENIED;
    }
    return SOFTBUS_OK;
}

bool CheckDiscPermission(pid_t callingUid, const char *pkgName)
{
    std::string pkg;
    if (pkgName != nullptr) {
        pkg = std::string(pkgName);
    } else {
        return false;
    }
    if (callingUid == SYSTEM_UID || callingUid % MULTE_USER_RADIX == SYSTEM_UID) {
        return true;
    }
    return false;
}

bool CheckBusCenterPermission(pid_t callingUid, const char *pkgName)
{
    std::string pkg;
    if (pkgName != nullptr) {
        pkg = std::string(pkgName);
    } else {
        return false;
    }
    if (callingUid == SYSTEM_UID || callingUid % MULTE_USER_RADIX == SYSTEM_UID) {
        return true;
    }
    return false;
}
