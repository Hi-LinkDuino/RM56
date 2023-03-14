/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "bluetooth_socket_server.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "bt_def.h"
#include "interface_profile_manager.h"
#include "interface_profile_socket.h"
#include "permission_utils.h"

using namespace OHOS::bluetooth;

namespace OHOS {
namespace Bluetooth {
int BluetoothSocketServer::Connect(std::string &addr, bluetooth::Uuid &uuid, int32_t securityFlag, int32_t type)
{
    HILOGI("addr: %{public}s, securityFlag: %{public}d, type: %{public}d",
        GetEncryptAddr(addr).c_str(), securityFlag, type);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return RET_NO_SUPPORT;
    }
    int ret = RET_NO_SUPPORT;
    IProfileSocket *socket = (IProfileSocket *)IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP);
    if (socket != nullptr) {
        ret = socket->Connect(addr, uuid, (int)securityFlag, (int)type);
    }

    return ret;
}

int BluetoothSocketServer::Listen(std::string &name, bluetooth::Uuid &uuid, int32_t securityFlag, int32_t type)
{
    HILOGI("name: %{public}s, securityFlag: %{public}d, type: %{public}d", name.c_str(), securityFlag, type);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return RET_NO_SUPPORT;
    }
    int ret = RET_NO_SUPPORT;
    IProfileSocket *socket = (IProfileSocket *)IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_SPP);
    if (socket != nullptr) {
        ret = socket->Listen(name, uuid, (int)securityFlag, (int)type);
    }

    return ret;
}
}  // namespace Bluetooth
}  // namespace OHOS