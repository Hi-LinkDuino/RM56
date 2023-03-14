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

#ifndef OHOS_DM_NOTIFY_H
#define OHOS_DM_NOTIFY_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "device_manager_callback.h"
#include "dm_device_info.h"
#include "dm_subscribe_info.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class DeviceManagerNotify {
    DECLARE_SINGLE_INSTANCE(DeviceManagerNotify);

public:
    void RegisterDeathRecipientCallback(const std::string &pkgName, std::shared_ptr<DmInitCallback> dmInitCallback);
    void UnRegisterDeathRecipientCallback(const std::string &pkgName);
    void RegisterDeviceStateCallback(const std::string &pkgName, std::shared_ptr<DeviceStateCallback> callback);
    void UnRegisterDeviceStateCallback(const std::string &pkgName);
    void RegisterDiscoveryCallback(const std::string &pkgName, uint16_t subscribeId,
                                   std::shared_ptr<DiscoveryCallback> callback);
    void UnRegisterDiscoveryCallback(const std::string &pkgName, uint16_t subscribeId);
    void RegisterAuthenticateCallback(const std::string &pkgName, const std::string &deviceId,
                                      std::shared_ptr<AuthenticateCallback> callback);
    void UnRegisterAuthenticateCallback(const std::string &pkgName, const std::string &deviceId);
    void UnRegisterPackageCallback(const std::string &pkgName);
    void RegisterVerifyAuthenticationCallback(const std::string &pkgName, const std::string &authPara,
                                              std::shared_ptr<VerifyAuthCallback> callback);
    void UnRegisterVerifyAuthenticationCallback(const std::string &pkgName);
    void RegisterDeviceManagerFaCallback(const std::string &pkgName, std::shared_ptr<DeviceManagerFaCallback> callback);
    void UnRegisterDeviceManagerFaCallback(const std::string &pkgName);

public:
    void OnRemoteDied();
    void OnDeviceOnline(const std::string &pkgName, const DmDeviceInfo &deviceInfo);
    void OnDeviceOffline(const std::string &pkgName, const DmDeviceInfo &deviceInfo);
    void OnDeviceChanged(const std::string &pkgName, const DmDeviceInfo &deviceInfo);
    void OnDeviceReady(const std::string &pkgName, const DmDeviceInfo &deviceInfo);
    void OnDeviceFound(const std::string &pkgName, uint16_t subscribeId, const DmDeviceInfo &deviceInfo);
    void OnDiscoveryFailed(const std::string &pkgName, uint16_t subscribeId, int32_t failedReason);
    void OnDiscoverySuccess(const std::string &pkgName, uint16_t subscribeId);
    void OnAuthResult(const std::string &pkgName, const std::string &deviceId, const std::string &token,
                      uint32_t status, uint32_t reason);
    void OnVerifyAuthResult(const std::string &pkgName, const std::string &deviceId, int32_t resultCode, int32_t flag);
    void OnFaCall(std::string &pkgName, std::string &paramJson);

private:
#if !defined(__LITEOS_M__)
    std::mutex lock_;
#endif
    std::map<std::string, std::shared_ptr<DeviceStateCallback>> deviceStateCallback_;
    std::map<std::string, std::map<uint16_t, std::shared_ptr<DiscoveryCallback>>> deviceDiscoveryCallbacks_;
    std::map<std::string, std::map<std::string, std::shared_ptr<AuthenticateCallback>>> authenticateCallback_;
    std::map<std::string, std::shared_ptr<VerifyAuthCallback>> verifyAuthCallback_;
    std::map<std::string, std::shared_ptr<DmInitCallback>> dmInitCallback_;
    std::map<std::string, std::shared_ptr<DeviceManagerFaCallback>> dmFaCallback_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_NOTIFY_H
