/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "wifi_permission_helper.h"
#include <map>
#include <mutex>
#include <string>
#include "wifi_log.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_MANAGER_PERMISSION_HELPER"

namespace OHOS {
namespace Wifi {
/* Supported permissions. */
static PermissionDef g_wifiPermissions[] = {
    {"ohos.permission.GET_WIFI_INFO", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.SET_WIFI_INFO", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.GET_WIFI_CONFIG", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.MANAGE_WIFI_CONNECTION", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.MANAGE_WIFI_HOTSPOT", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.MANAGE_ENHANCER_WIFI", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.GET_WIFI_LOCAL_MAC", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.LOCATION", USER_GRANT, NOT_RESTRICTED},
    {"ohos.permission.GET_P2P_DEVICE_LOCATION", USER_GRANT, NOT_RESTRICTED},
};

static int g_wifiPermissionSize = sizeof(g_wifiPermissions) / sizeof(PermissionDef);

using MAP_STRING_PERMISSION = std::map<std::string, IsGranted>;
using MAP_INT_PERMISSION_MAP = std::map<int, MAP_STRING_PERMISSION>;

/* pid Associate Permission Container. */
MAP_INT_PERMISSION_MAP g_pidPermissionMap;

/* uid Associate Permission Container. */
MAP_INT_PERMISSION_MAP g_uidPermissionMap;

std::mutex g_wifiPermissionMutex;
int WifiPermissionHelper::InitPermission(const int &pid, const int &uid)
{
    MAP_STRING_PERMISSION permissionMap;
    std::string permissionName = "";

    /* Initialize all permissions. */
    IsGranted granted = PERMISSION_GRANTED;
    for (int i = 0; i < g_wifiPermissionSize; i++) {
        permissionName = std::string(g_wifiPermissions[i].name);
        permissionMap[permissionName] = granted;
    }

    std::unique_lock<std::mutex> lock(g_wifiPermissionMutex);
    g_pidPermissionMap[pid] = permissionMap;
    g_uidPermissionMap[uid] = permissionMap;

    return 0;
}

int WifiPermissionHelper::ChangePermission(
    const std::map<std::string, int> &permissions, const int &pid, const int &uid)
{
    MAP_STRING_PERMISSION permissionMap;
    std::string permissionName = "";
    IsGranted granted = PERMISSION_GRANTED;

    std::map<std::string, int>::const_iterator iter = permissions.begin();
    while (iter != permissions.end()) {
        permissionName = iter->first;
        granted = (IsGranted)iter->second;
        permissionMap[permissionName] = granted;
        ++iter;
    }
    std::unique_lock<std::mutex> lock(g_wifiPermissionMutex);
    g_pidPermissionMap[pid] = permissionMap;
    g_uidPermissionMap[uid] = permissionMap;

    return 0;
}

int WifiPermissionHelper::ClearPermission(const int &pid, const int &uid)
{
    std::unique_lock<std::mutex> lock(g_wifiPermissionMutex);
    g_pidPermissionMap.erase(pid);
    g_uidPermissionMap.erase(uid);
    return 0;
}

IsGranted WifiPermissionHelper::MockVerifyPermission(const std::string &permissionName, const int &pid, const int &uid)
{
    std::unique_lock<std::mutex> lock(g_wifiPermissionMutex);

    /* Step 1: Verify whether the pid map is authorized. */
    MAP_INT_PERMISSION_MAP::iterator iter = g_pidPermissionMap.find(pid);
    if (iter == g_pidPermissionMap.end()) {
        /* Failed to find the pid permission list. */
        LOGD("can't find PID=%{public}d in g_pidPermissionMap!", pid);
        return PERMISSION_DENIED;
    }

    /* Verify whether the pid map is authorized. */
    MAP_STRING_PERMISSION pidPermissionMap = iter->second;
    MAP_STRING_PERMISSION::iterator itor = pidPermissionMap.find(permissionName);
    if (itor == pidPermissionMap.end()) {
        /* Failed to find the pid permission list. */
        LOGD("can't find permission_name=%{public}s pid permission list!", permissionName.c_str());
        return PERMISSION_DENIED;
    }

    /* Verify whether the pid map is authorized. */
    IsGranted granted = itor->second;
    if (granted == PERMISSION_DENIED) {
        /* pid map No authorization. */
        LOGD("find permission_name=%{public}s ,and there is really no permission in pid map", permissionName.c_str());
        return PERMISSION_DENIED;
    }

    /* Step 2: Verify whether the pid map has the permission. */
    iter = g_uidPermissionMap.find(uid);
    if (iter == g_uidPermissionMap.end()) {
        /* The uid permission list cannot be found. */
        LOGD("can't find UID=%{public}d in g_uidPermissionMap!", uid);
        return PERMISSION_DENIED;
    }

    /* Verify whether the uid map is authorized. */
    MAP_STRING_PERMISSION uidPermissionMap = iter->second;
    itor = uidPermissionMap.find(permissionName);
    if (itor == uidPermissionMap.end()) {
        /* Failed to find the pid permission list. */
        LOGD("can't find permission_name=%{public}s uid permission list!", permissionName.c_str());
        return PERMISSION_DENIED;
    }

    /* Verify whether authorization is granted in the uid map. */
    granted = itor->second;
    if (granted == PERMISSION_DENIED) {
        /* pid map No authorization. */
        LOGD("find permission_name=%{public}s ,and there is really no permission in uid "
             "map.",
            permissionName.c_str());
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyPermission(const std::string &permissionName, const int &pid, const int &uid)
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    auto tokenType = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        return PERMISSION_GRANTED;
    }

    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
        if (result == Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
            return PERMISSION_GRANTED;
        }
 
        LOGI("callerToken=0x%{public}x has no permission_name=%{public}s", pid, permissionName.c_str());
        return PERMISSION_DENIED;
    }

    LOGE("callerToken=0x%{public}x has invalid token type=%{public}d", pid, tokenType);
    return PERMISSION_DENIED;
}

int WifiPermissionHelper::VerifySetWifiInfoPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.SET_WIFI_INFO", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiInfoPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_WIFI_INFO", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifySetWifiConfigPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.SET_WIFI_CONFIG", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiConfigPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_WIFI_CONFIG", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetScanInfosPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.LOCATION", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiLocalMacPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_WIFI_LOCAL_MAC", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyWifiConnectionPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.MANAGE_WIFI_CONNECTION", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiDirectDevicePermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_P2P_DEVICE_LOCATION", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyManageWifiHotspotPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.MANAGE_WIFI_HOTSPOT", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiPeersMacPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_WIFI_PEERS_MAC", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

int WifiPermissionHelper::VerifyGetWifiInfoInternalPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.GET_WIFI_INFO_INTERNAL", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}
}  // namespace Wifi
}  // namespace OHOS