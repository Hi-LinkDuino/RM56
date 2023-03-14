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

#ifndef OHOS_DM_SOFTBUS_CONNECTOR_H
#define OHOS_DM_SOFTBUS_CONNECTOR_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "discovery_service.h"
#include "dm_device_info.h"
#include "dm_subscribe_info.h"
#include "softbus_bus_center.h"
#include "softbus_discovery_callback.h"
#include "softbus_session.h"
#include "softbus_state_callback.h"

namespace OHOS {
namespace DistributedHardware {
void DeviceOnLine(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> stateCallbackMap,
    DmDeviceInfo deviceInfo);
void DeviceOffLine(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> stateCallbackMap,
    DmDeviceInfo deviceInfo);
class SoftbusConnector {
public:
    static void OnPublishSuccess(int publishId);
    static void OnPublishFail(int publishId, PublishFailReason reason);
    static void OnSoftBusDeviceOnline(NodeBasicInfo *info);
    static void OnSoftbusDeviceOffline(NodeBasicInfo *info);
    static void OnSoftbusDeviceInfoChanged(NodeBasicInfoType type, NodeBasicInfo *info);
    static void OnSoftbusDeviceFound(const DeviceInfo *device);
    static void OnSoftbusDiscoveryFailed(int subscribeId, DiscoveryFailReason failReason);
    static void OnSoftbusDiscoverySuccess(int subscribeId);
    static void OnParameterChgCallback(const char *key, const char *value, void *context);
    static int32_t GetConnectionIpAddress(const std::string &deviceId, std::string &ipAddress);
    static ConnectionAddr *GetConnectAddr(const std::string &deviceId, std::string &connectAddr);
    static bool IsDeviceOnLine(const std::string &deviceId);
    static int32_t GetUdidByNetworkId(const char *networkId, std::string &udid);
    static int32_t GetUuidByNetworkId(const char *networkId, std::string &uuid);

public:
    SoftbusConnector();
    ~SoftbusConnector();
    int32_t RegisterSoftbusStateCallback(const std::string &pkgName,
                                         const std::shared_ptr<ISoftbusStateCallback> callback);
    int32_t UnRegisterSoftbusStateCallback(const std::string &pkgName);
    int32_t RegisterSoftbusDiscoveryCallback(const std::string &pkgName,
                                             const std::shared_ptr<ISoftbusDiscoveryCallback> callback);
    int32_t UnRegisterSoftbusDiscoveryCallback(const std::string &pkgName);
    int32_t GetTrustedDeviceList(std::vector<DmDeviceInfo> &deviceInfoList);
    int32_t GetLocalDeviceInfo(DmDeviceInfo &deviceInfo);
    int32_t StartDiscovery(const DmSubscribeInfo &subscribeInfo);
    int32_t StopDiscovery(uint16_t subscribeId);
    std::shared_ptr<SoftbusSession> GetSoftbusSession();
    bool HaveDeviceInMap(std::string deviceId);

private:
    int32_t Init();
    static int32_t CovertNodeBasicInfoToDmDevice(const NodeBasicInfo &nodeBasicInfo, DmDeviceInfo &dmDeviceInfo);
    static void CovertDeviceInfoToDmDevice(const DeviceInfo &deviceInfo, DmDeviceInfo &dmDeviceInfo);
    static ConnectionAddr *GetConnectAddrByType(DeviceInfo *deviceInfo, ConnectionAddrType type);

private:
    enum PulishStatus {
        STATUS_UNKNOWN = 0,
        ALLOW_BE_DISCOVERY = 1,
        NOT_ALLOW_BE_DISCOVERY = 2,
    };
    static PulishStatus publishStatus;
    static INodeStateCb softbusNodeStateCb_;
    static IDiscoveryCallback softbusDiscoveryCallback_;
    static IPublishCallback softbusPublishCallback_;
    std::shared_ptr<SoftbusSession> softbusSession_;
    static std::map<std::string, std::shared_ptr<DeviceInfo>> discoveryDeviceInfoMap_;
    static std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> stateCallbackMap_;
    static std::map<std::string, std::shared_ptr<ISoftbusDiscoveryCallback>> discoveryCallbackMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_SOFTBUS_CONNECTOR_H
