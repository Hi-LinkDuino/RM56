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

#ifndef OHOS_DM_DISCOVERY_MANAGER_H
#define OHOS_DM_DISCOVERY_MANAGER_H

#include <queue>

#include "device_manager_service_listener.h"
#include "dm_timer.h"
#include "softbus_connector.h"
namespace OHOS {
namespace DistributedHardware {
typedef struct DmDiscoveryContext {
    std::string pkgName;
    std::string extra;
    uint16_t subscribeId;
} DmDiscoveryContext;

class DmDiscoveryManager final : public ISoftbusDiscoveryCallback,
                                 public std::enable_shared_from_this<DmDiscoveryManager> {
public:
    DmDiscoveryManager(std::shared_ptr<SoftbusConnector> softbusConnector,
                       std::shared_ptr<DeviceManagerServiceListener> listener);
    ~DmDiscoveryManager();
    int32_t StartDeviceDiscovery(const std::string &pkgName, const DmSubscribeInfo &subscribeInfo,
                                 const std::string &extra);
    int32_t StopDeviceDiscovery(const std::string &pkgName, uint16_t subscribeId);
    void OnDeviceFound(const std::string &pkgName, const DmDeviceInfo &info);
    void OnDiscoverySuccess(const std::string &pkgName, int32_t subscribeId);
    void OnDiscoveryFailed(const std::string &pkgName, int32_t subscribeId, int32_t failedReason);
    void HandleDiscoveryTimeout();

private:
    std::shared_ptr<SoftbusConnector> softbusConnector_;
    std::shared_ptr<DeviceManagerServiceListener> listener_;
    std::queue<std::string> discoveryQueue_;
    std::map<std::string, DmDiscoveryContext> discoveryContextMap_;
    std::shared_ptr<DmTimer> discoveryTimer_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_DISCOVERY_MANAGER_H
