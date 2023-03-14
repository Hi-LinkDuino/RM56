/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "auth_center.h"
#include "stdbool.h"
#include "accesstoken_kit.h"
#include "permission_helper.h"


namespace OHOS {
namespace bluetooth {
#ifdef PERMISSION_ALWAYS_GRANT
bool g_permissionAlwaysGrant = true;
#else
bool g_permissionAlwaysGrant = false;
#endif

using namespace OHOS::Security::AccessToken;

AuthCenter &AuthCenter::GetInstance()
{
    static AuthCenter authCenter;
    return authCenter;
}

int AuthCenter::VerifyUseBluetoothPermission(const int &pid, const int &uid)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }

    return PermissionHelper::VerifyUseBluetoothPermission(pid, uid);
}

int AuthCenter::VerifyDiscoverBluetoothPermission(const int &pid, const int &uid)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return PermissionHelper::VerifyDiscoverBluetoothPermission(pid, uid);
}

int AuthCenter::VerifyManageBluetoothPermission(const int &pid, const int &uid)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return PermissionHelper::VerifyManageBluetoothPermission(pid, uid);
}

int AuthCenter::VerifyLocationPermission(const int &pid, const int &uid)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return PERMISSION_GRANTED;
}

int AuthCenter::VerifyUseBluetoothPermission(const std::uint32_t  &tokenID)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return PermissionHelper::VerifyUseBluetoothPermission(tokenID);
}

int AuthCenter::VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID)
{
    if (g_permissionAlwaysGrant) {
        return PERMISSION_GRANTED;
    }
    return PermissionHelper::VerifyDiscoverBluetoothPermission(tokenID);
}
}  // namespace bluetooth
}  // namespace OHOS