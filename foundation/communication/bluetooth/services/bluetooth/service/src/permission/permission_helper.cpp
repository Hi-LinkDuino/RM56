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

#include "permission_helper.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
using namespace OHOS;
using namespace Security::AccessToken;

int PermissionHelper::VerifyPermission(const std::string &permissionName, const int &pid, const int &uid)
{
    auto callerToken = IPCSkeleton::GetCallingTokenID();
    int result;

    if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_NATIVE) {
        result = Security::AccessToken::PermissionState::PERMISSION_GRANTED;
    } else if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_SHELL) {
        result = Security::AccessToken::PermissionState::PERMISSION_GRANTED;
    } else if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_HAP) {
        result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    } else {
        LOG_INFO("callerToken=0x%{public}x is invalid token", pid);
        return PERMISSION_DENIED;
    }
    if (result == Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        return PERMISSION_GRANTED;
    } else {
        LOG_INFO("callerToken=0x%{public}x has no permission_name=%{public}s", pid, permissionName.c_str());
        return PERMISSION_DENIED;
    }
}

int PermissionHelper::VerifyPermission(const std::string &permissionName, const std::uint32_t &callerToken)
{
    auto  pid = IPCSkeleton::GetCallingPid();
    int result;

    if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_NATIVE) {
        result = Security::AccessToken::PermissionState::PERMISSION_GRANTED;
    } else if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_SHELL) {
        result = Security::AccessToken::PermissionState::PERMISSION_GRANTED;
    } else if (Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken) == TOKEN_HAP) {
        result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    } else {
        LOG_INFO("callerToken=0x%{public}x is invalid token", pid);
        return PERMISSION_DENIED;
    }
    if (result == Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        return PERMISSION_GRANTED;
    } else {
        LOG_INFO("callerToken=0x%{public}x has no permission_name=%{public}s", pid, permissionName.c_str());
        return PERMISSION_DENIED;
    }
}

int PermissionHelper::VerifyUseBluetoothPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.USE_BLUETOOTH", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int PermissionHelper::VerifyDiscoverBluetoothPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.DISCOVER_BLUETOOTH", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int PermissionHelper::VerifyManageBluetoothPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.MANAGE_BLUETOOTH", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int PermissionHelper::VerifyLocationPermission(const int &pid, const int &uid)
{
    if (VerifyPermission("ohos.permission.LOCATION", pid, uid) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int PermissionHelper::VerifyUseBluetoothPermission(const std::uint32_t  &tokenID)
{
    if (VerifyPermission("ohos.permission.USE_BLUETOOTH", tokenID) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}

int PermissionHelper::VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID)
{
    if (VerifyPermission("ohos.permission.DISCOVER_BLUETOOTH", tokenID) == PERMISSION_DENIED) {
        return PERMISSION_DENIED;
    }

    return PERMISSION_GRANTED;
}
}  // namespace bluetooth
}  // namespace OHOS
