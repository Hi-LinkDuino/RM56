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

#ifndef OHOS_DM_DEVICE_STATE_MANAGER_H
#define OHOS_DM_DEVICE_STATE_MANAGER_H

#include <string>
#include <memory>
#if defined(__LITEOS_M__)
#include "dm_mutex.h"
#else
#include <mutex>
#endif
#include "device_manager_service_listener.h"
#include "dm_adapter_manager.h"
#include "softbus_connector.h"
#include "dm_timer.h"
#include "hichain_connector.h"

namespace OHOS {
namespace DistributedHardware {
#define OFFLINE_TIMEOUT 300
struct StateTimerInfo {
    std::string timerName;
    std::string netWorkId;
    std::string deviceId;
    std::shared_ptr<DmTimer> timer;
};

class DmDeviceStateManager final : public ISoftbusStateCallback,
                                   public std::enable_shared_from_this<DmDeviceStateManager> {
public:
    DmDeviceStateManager(std::shared_ptr<SoftbusConnector> softbusConnector,
                         std::shared_ptr<DeviceManagerServiceListener> listener,
                         std::shared_ptr<HiChainConnector> hiChainConnector);
    ~DmDeviceStateManager();
    int32_t RegisterProfileListener(const std::string &pkgName, const DmDeviceInfo &info);
    int32_t UnRegisterProfileListener(const std::string &pkgName, const DmDeviceInfo &info);
    void PostDeviceOnline(const std::string &pkgName, const DmDeviceInfo &info);
    void PostDeviceOffline(const std::string &pkgName, const DmDeviceInfo &info);
    void OnDeviceOnline(const std::string &pkgName, const DmDeviceInfo &info);
    void OnDeviceOffline(const std::string &pkgName, const DmDeviceInfo &info);
    void OnDeviceChanged(const std::string &pkgName, const DmDeviceInfo &info);
    void OnDeviceReady(const std::string &pkgName, const DmDeviceInfo &info);
    void OnProfileReady(const std::string &pkgName, const std::string &deviceId);
    int32_t RegisterSoftbusStateCallback();
    void RegisterOffLineTimer(const DmDeviceInfo &deviceInfo);
    void StartOffLineTimer(const DmDeviceInfo &deviceInfo);
    void DeleteTimeOutGroup(std::string stateTimer);
    void RegisterDevStateCallback(const std::string &pkgName, const std::string &extra);
    void UnRegisterDevStateCallback(const std::string &pkgName, const std::string &extra);

private:
    int32_t cumulativeQuantity_ = 0;
    std::string profileSoName_;
#if !defined(__LITEOS_M__)
    std::mutex timerMapMutex_;
    std::mutex remoteDeviceInfosMutex_;
#endif
    std::shared_ptr<SoftbusConnector> softbusConnector_;
    std::shared_ptr<DeviceManagerServiceListener> listener_;
    std::map<std::string, DmDeviceInfo> remoteDeviceInfos_;
    std::map<std::string, std::string> decisionInfos_;
    std::map<std::string, StateTimerInfo> stateTimerInfoMap_;
    std::shared_ptr<HiChainConnector> hiChainConnector_;
    std::string decisionSoName_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_DEVICE_STATE_MANAGER_H
