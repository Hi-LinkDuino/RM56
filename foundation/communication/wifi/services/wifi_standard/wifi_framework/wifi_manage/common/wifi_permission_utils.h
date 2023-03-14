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
#ifndef OHOS_WIFI_PERMISSION_UTILS_H
#define OHOS_WIFI_PERMISSION_UTILS_H

#include <map>
#include "permission_def.h"

namespace OHOS {
namespace Wifi {
class WifiPermissionUtils {
public:
    static int VerifySetWifiInfoPermission();
    static int VerifyGetWifiInfoPermission();
    static int VerifyWifiConnectionPermission();
    static int VerifyGetScanInfosPermission();
    static int VerifyGetWifiLocalMacPermission();
    static int VerifySetWifiConfigPermission();
    static int VerifyGetWifiConfigPermission();
    static int VerifyGetWifiDirectDevicePermission();
    static int VerifyManageWifiHotspotPermission();
    static int VerifyGetWifiPeersMacPermission();
    static int VerifyGetWifiInfoInternalPermission();
};
}  // namespace Wifi
}  // namespace OHOS
#endif