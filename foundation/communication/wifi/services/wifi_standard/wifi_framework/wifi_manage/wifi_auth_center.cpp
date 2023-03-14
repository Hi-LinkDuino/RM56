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

#include "wifi_auth_center.h"
#include "wifi_permission_helper.h"

namespace OHOS {
namespace Wifi {
#ifdef PERMISSION_ALWAYS_GRANT
bool g_permissinAlwaysGrant = true;
#else
bool g_permissinAlwaysGrant = false;
#endif // PERMISSION_ALWAYS_GRANT

WifiAuthCenter &WifiAuthCenter::GetInstance()
{
    static WifiAuthCenter gWifiAuthCenter;
    return gWifiAuthCenter;
}

int WifiAuthCenter::Init()
{
    /* init system auth service client here */
    return 0;
}

int WifiAuthCenter::InitPermission(const int &pid, const int &uid)
{
    return WifiPermissionHelper::InitPermission(pid, uid);
}

int WifiAuthCenter::ChangePermission(const std::map<std::string, int> &permissions, const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::ChangePermission(permissions, pid, uid);
}

int WifiAuthCenter::VerifySetWifiInfoPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifySetWifiInfoPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiInfoPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiInfoPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetScanInfosPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetScanInfosPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiLocalMacPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiLocalMacPermission(pid, uid);
}

int WifiAuthCenter::VerifyWifiConnectionPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyWifiConnectionPermission(pid, uid);
}

int WifiAuthCenter::VerifySetWifiConfigPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifySetWifiConfigPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiConfigPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiConfigPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiDirectDevicePermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiDirectDevicePermission(pid, uid);
}

int WifiAuthCenter::VerifyManageWifiHotspotPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyManageWifiHotspotPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiPeersMacPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiPeersMacPermission(pid, uid);
}

int WifiAuthCenter::VerifyGetWifiInfoInternalPermission(const int &pid, const int &uid)
{
    if (g_permissinAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return WifiPermissionHelper::VerifyGetWifiInfoInternalPermission(pid, uid);
}
} // namespace Wifi
} // namespace OHOS