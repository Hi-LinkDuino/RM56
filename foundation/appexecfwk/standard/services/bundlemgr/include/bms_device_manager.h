/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BMS_DEVICE_MANAGER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BMS_DEVICE_MANAGER_H

#include <memory>
#include <string>

#include "device_manager_callback.h"
#include "dm_device_info.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS {
namespace AppExecFwk {
class BmsDeviceManager {
public:
    BmsDeviceManager();
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    static int32_t GetUdidByNetworkId(const std::string &netWorkId, std::string &udid);
    
private:
    static void StartDynamicSystemProcess(int32_t systemAbilityId);
    static void StopDynamicSystemProcess(int32_t systemAbilityId);
    bool InitDeviceManager();
    int GetTrustedDeviceListSize();

    std::shared_ptr<DistributedHardware::DmInitCallback> initCallback_;
    std::shared_ptr<DistributedHardware::DeviceStateCallback> stateCallback_;

class DeviceInitCallBack : public DistributedHardware::DmInitCallback {
    void OnRemoteDied() override;
};

class BmsDeviceStateCallback : public DistributedHardware::DeviceStateCallback {
    void OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo) override;
};
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BMS_DEVICE_MANAGER_H