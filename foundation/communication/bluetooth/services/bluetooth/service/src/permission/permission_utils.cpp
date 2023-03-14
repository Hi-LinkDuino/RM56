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

#include "permission_utils.h"
#include "ipc_skeleton.h"
#include "auth_center.h"

namespace OHOS {
namespace bluetooth {
using namespace OHOS;

int PermissionUtils::VerifyUseBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyUseBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyDiscoverBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyDiscoverBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyManageBluetoothPermission()
{
    return AuthCenter::GetInstance().VerifyManageBluetoothPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyLocationPermission()
{
    return AuthCenter::GetInstance().VerifyLocationPermission(
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid());
}

int PermissionUtils::VerifyUseBluetoothPermission(const std::uint32_t  &tokenID)
{
    return AuthCenter::GetInstance().VerifyUseBluetoothPermission(tokenID);
}

int PermissionUtils::VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID)
{
    return AuthCenter::GetInstance().VerifyDiscoverBluetoothPermission(tokenID);
}
}  // namespace bluetooth
}  // namespace OHOS