/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_SERVICE_LISTENER_H
#define OHOS_DM_SERVICE_LISTENER_H

#include <map>
#include <string>

#include "dm_device_info.h"
#if !defined(__LITEOS_M__)
#include "ipc_notify_dmfa_result_req.h"
#include "ipc_server_listener.h"
#endif


namespace OHOS {
namespace DistributedHardware {
class DeviceManagerServiceListener {
public:
    void OnDeviceStateChange(const std::string &pkgName, const DmDeviceState &state, const DmDeviceInfo &info);
    void OnDeviceFound(const std::string &pkgName, uint16_t subscribeId, const DmDeviceInfo &info);
    void OnDiscoveryFailed(const std::string &pkgName, uint16_t subscribeId, int32_t failedReason);
    void OnDiscoverySuccess(const std::string &pkgName, int32_t subscribeId);
    void OnAuthResult(const std::string &pkgName, const std::string &deviceId, const std::string &token, int32_t status,
                      int32_t reason);
    void OnVerifyAuthResult(const std::string &pkgName, const std::string &deviceId, int32_t resultCode,
                            const std::string &flag);
    void OnFaCall(std::string &pkgName, std::string &paramJson);

private:
#if !defined(__LITEOS_M__)
    IpcServerListener ipcServerListener_;
#endif
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_SERVICE_LISTENER_H
