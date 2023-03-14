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

#ifndef OHOS_WIFI_PERMISSION_HELPER
#define OHOS_WIFI_PERMISSION_HELPER

#include <map>
#include "permission_def.h"

namespace OHOS {
namespace Wifi {
class WifiPermissionHelper {
public:
    /**
     * @Description : Initialization Permossion.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int InitPermission(const int &pid, const int &uid);

    /**
     * @Description : Clearing Permissions.
     *
     * @param pid - Process ID.
     * @param uid - User ID.
     * @return int
     */
    static int ClearPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Permission.
     *
     * @param permissions - the permission.[in]
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int ChangePermission(const std::map<std::string, int> &permissions, const int &pid, const int &uid);

    /**
     * @Description : Mock Check Permission.
     *
     * @param permissionName - Permission name.[in]
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return IsGranted
     */
    static IsGranted MockVerifyPermission(const std::string &permissionName, const int &pid, const int &uid);

    /**
     * @Description : Verify Permission.
     *
     * @param permissionName - Permission name.[in]
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyPermission(const std::string &permissionName, const int &pid, const int &uid);

    /**
     * @Description : Verify Set Wifi Information Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifySetWifiInfoPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Get Wifi Information Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiInfoPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Access Scan Info Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetScanInfosPermission(const int &pid, const int &uid);

    /**
     * @Description Verify Get Local Mac Address Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiLocalMacPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Wifi Connection Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyWifiConnectionPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Set Wifi Config Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifySetWifiConfigPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify Get Wifi Config Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiConfigPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify location information about nearby P2P devices Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiDirectDevicePermission(const int &pid, const int &uid);

    /**
     * @Description : Verify manage wifi hotspot Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyManageWifiHotspotPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify get wifi peers mac Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiPeersMacPermission(const int &pid, const int &uid);

    /**
     * @Description : Verify get internal wifi info Permission.
     *
     * @param pid - Process ID.[in]
     * @param uid - User ID.[in]
     * @return int
     */
    static int VerifyGetWifiInfoInternalPermission(const int &pid, const int &uid);
};
}  // namespace Wifi
}  // namespace OHOS
#endif