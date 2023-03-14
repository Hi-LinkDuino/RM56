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

#include "softbus_connector.h"

#include <securec.h>
#include <unistd.h>
#if defined(__LITEOS_M__)
#include "dm_mutex.h"
#include "dm_thread.h"
#else
#include <thread>
#include <mutex>
#endif

#include "dm_anonymous.h"
#include "dm_constants.h"
#include "dm_device_info.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace DistributedHardware {
SoftbusConnector::PulishStatus SoftbusConnector::publishStatus = SoftbusConnector::STATUS_UNKNOWN;
std::map<std::string, std::shared_ptr<DeviceInfo>> SoftbusConnector::discoveryDeviceInfoMap_ = {};
std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> SoftbusConnector::stateCallbackMap_ = {};
std::map<std::string, std::shared_ptr<ISoftbusDiscoveryCallback>> SoftbusConnector::discoveryCallbackMap_ = {};

IPublishCallback SoftbusConnector::softbusPublishCallback_ = {.OnPublishSuccess = SoftbusConnector::OnPublishSuccess,
                                                              .OnPublishFail = SoftbusConnector::OnPublishFail};

IDiscoveryCallback SoftbusConnector::softbusDiscoveryCallback_ = {
    .OnDeviceFound = SoftbusConnector::OnSoftbusDeviceFound,
    .OnDiscoverFailed = SoftbusConnector::OnSoftbusDiscoveryFailed,
    .OnDiscoverySuccess = SoftbusConnector::OnSoftbusDiscoverySuccess};

INodeStateCb SoftbusConnector::softbusNodeStateCb_ = {
    .events = EVENT_NODE_STATE_ONLINE | EVENT_NODE_STATE_OFFLINE | EVENT_NODE_STATE_INFO_CHANGED,
    .onNodeOnline = SoftbusConnector::OnSoftBusDeviceOnline,
    .onNodeOffline = SoftbusConnector::OnSoftbusDeviceOffline,
    .onNodeBasicInfoChanged = SoftbusConnector::OnSoftbusDeviceInfoChanged};

void DeviceOnLine(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> stateCallbackMap,
    DmDeviceInfo deviceInfo)
{
    LOGI("Device on line start");
#if defined(__LITEOS_M__)
    DmMutex lockDeviceOnLine;
#else
    std::mutex lockDeviceOnLine;
    std::lock_guard<std::mutex> lock(lockDeviceOnLine);
#endif
    for (auto &iter : stateCallbackMap) {
        iter.second->OnDeviceOnline(iter.first, deviceInfo);
    }
    LOGI("Device on line end");
}

void DeviceOffLine(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> stateCallbackMap,
    DmDeviceInfo deviceInfo)
{
    LOGI("Device off line start");
#if defined(__LITEOS_M__)
    DmMutex lockDeviceOffLine;
#else
    std::mutex lockDeviceOffLine;
    std::lock_guard<std::mutex> lock(lockDeviceOffLine);
#endif
    for (auto &iter : stateCallbackMap) {
        iter.second->OnDeviceOffline(iter.first, deviceInfo);
    }
    LOGI("Device off line end");
}

SoftbusConnector::SoftbusConnector()
{
    softbusSession_ = std::make_shared<SoftbusSession>();
    Init();
}

SoftbusConnector::~SoftbusConnector()
{
    LOGI("SoftbusConnector destructor");
}

int32_t SoftbusConnector::Init()
{
    int32_t ret;
    int32_t retryTimes = 0;
    do {
        ret = RegNodeDeviceStateCb(DM_PKG_NAME.c_str(), &softbusNodeStateCb_);
        if (ret != DM_OK) {
            ++retryTimes;
            LOGE("RegNodeDeviceStateCb failed with ret %d, retryTimes %d", ret, retryTimes);
            usleep(SOFTBUS_CHECK_INTERVAL);
        }
    } while (ret != DM_OK);

    PublishInfo dmPublishInfo;
    dmPublishInfo.publishId = DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID;
    dmPublishInfo.mode = DiscoverMode::DISCOVER_MODE_ACTIVE;
    dmPublishInfo.medium = ExchanageMedium::AUTO;
    dmPublishInfo.freq = ExchangeFreq::HIGH;
    dmPublishInfo.capability = DM_CAPABILITY_OSD;
    dmPublishInfo.capabilityData = nullptr;
    dmPublishInfo.dataLen = 0;
#if (defined(__LITEOS_M__) || defined(LITE_DEVICE))
    ret = PublishService(DM_PKG_NAME.c_str(), &dmPublishInfo, &softbusPublishCallback_);
    if (ret == DM_OK) {
        publishStatus = ALLOW_BE_DISCOVERY;
    }
#else
    char discoverStatus[DISCOVER_STATUS_LEN + 1] = {0};
    ret = GetParameter(DISCOVER_STATUS_KEY.c_str(), "not exist", discoverStatus, DISCOVER_STATUS_LEN);
    if (strcmp(discoverStatus, "not exist") == 0) {
        ret = SetParameter(DISCOVER_STATUS_KEY.c_str(), DISCOVER_STATUS_ON.c_str());
        LOGI("service set poatrameter result is : %d", ret);

        ret = PublishService(DM_PKG_NAME.c_str(), &dmPublishInfo, &softbusPublishCallback_);
        if (ret == DM_OK) {
            publishStatus = ALLOW_BE_DISCOVERY;
        }
        LOGI("service publish result is : %d", ret);
    } else if (ret >= 0 && strcmp(discoverStatus, DISCOVER_STATUS_ON.c_str()) == 0) {
        ret = PublishService(DM_PKG_NAME.c_str(), &dmPublishInfo, &softbusPublishCallback_);
        if (ret == DM_OK) {
            publishStatus = ALLOW_BE_DISCOVERY;
        }
        LOGI("service publish result is : %d", ret);
    } else if (ret >= 0 && strcmp(discoverStatus, DISCOVER_STATUS_OFF.c_str()) == 0) {
        ret = UnPublishService(DM_PKG_NAME.c_str(), DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
        if (ret == DM_OK) {
            publishStatus = NOT_ALLOW_BE_DISCOVERY;
        }
        LOGI("service unpublish result is : %d", ret);
    }

    ret = WatchParameter(DISCOVER_STATUS_KEY.c_str(), &SoftbusConnector::OnParameterChgCallback, nullptr);
#endif
    return ret;
}

int32_t SoftbusConnector::RegisterSoftbusDiscoveryCallback(const std::string &pkgName,
                                                           const std::shared_ptr<ISoftbusDiscoveryCallback> callback)
{
    discoveryCallbackMap_.emplace(pkgName, callback);
    return DM_OK;
}

int32_t SoftbusConnector::UnRegisterSoftbusDiscoveryCallback(const std::string &pkgName)
{
    discoveryCallbackMap_.erase(pkgName);
    return DM_OK;
}

int32_t SoftbusConnector::RegisterSoftbusStateCallback(const std::string &pkgName,
                                                       const std::shared_ptr<ISoftbusStateCallback> callback)
{
    stateCallbackMap_.emplace(pkgName, callback);
    return DM_OK;
}

int32_t SoftbusConnector::UnRegisterSoftbusStateCallback(const std::string &pkgName)
{
    stateCallbackMap_.erase(pkgName);
    return DM_OK;
}

int32_t SoftbusConnector::GetTrustedDeviceList(std::vector<DmDeviceInfo> &deviceInfoList)
{
    LOGI("SoftbusConnector::GetTrustDevices start");
    int32_t infoNum = 0;
    NodeBasicInfo *nodeInfo = nullptr;
    int32_t ret = GetAllNodeDeviceInfo(DM_PKG_NAME.c_str(), &nodeInfo, &infoNum);
    if (ret != 0) {
        LOGE("GetAllNodeDeviceInfo failed with ret %d", ret);
        return DM_FAILED;
    }
    DmDeviceInfo *info = (DmDeviceInfo *)malloc(sizeof(DmDeviceInfo) * (infoNum));
    if (info == nullptr) {
        FreeNodeInfo(nodeInfo);
        return DM_MALLOC_ERROR;
    }
    DmDeviceInfo **pInfoList = &info;
    for (int32_t i = 0; i < infoNum; ++i) {
        NodeBasicInfo *nodeBasicInfo = nodeInfo + i;
        DmDeviceInfo *deviceInfo = *pInfoList + i;
        CovertNodeBasicInfoToDmDevice(*nodeBasicInfo, *deviceInfo);
        deviceInfoList.push_back(*deviceInfo);
    }
    FreeNodeInfo(nodeInfo);
    free(info);
    LOGI("SoftbusConnector::GetTrustDevices success, deviceCount %d", infoNum);
    return DM_OK;
}

int32_t SoftbusConnector::GetLocalDeviceInfo(DmDeviceInfo &deviceInfo)
{
    LOGI("SoftbusConnector::GetLocalDeviceInfo start");
    NodeBasicInfo nodeBasicInfo;
    int32_t ret = GetLocalNodeDeviceInfo(DM_PKG_NAME.c_str(), &nodeBasicInfo);
    if (ret != 0) {
        LOGE("GetLocalNodeDeviceInfo failed with ret %d", ret);
        return DM_FAILED;
    }
    CovertNodeBasicInfoToDmDevice(nodeBasicInfo, deviceInfo);
    LOGI("SoftbusConnector::GetLocalDeviceInfo success");
    return DM_OK;
}

int32_t SoftbusConnector::StartDiscovery(const DmSubscribeInfo &dmSubscribeInfo)
{
    SubscribeInfo subscribeInfo;
    subscribeInfo.subscribeId = dmSubscribeInfo.subscribeId;
    subscribeInfo.mode = (DiscoverMode)dmSubscribeInfo.mode;
    subscribeInfo.medium = (ExchanageMedium)dmSubscribeInfo.medium;
    subscribeInfo.freq = (ExchangeFreq)dmSubscribeInfo.freq;
    subscribeInfo.isSameAccount = dmSubscribeInfo.isSameAccount;
    subscribeInfo.isWakeRemote = dmSubscribeInfo.isWakeRemote;
    subscribeInfo.capability = dmSubscribeInfo.capability;
    subscribeInfo.capabilityData = nullptr;
    subscribeInfo.dataLen = 0;
    int32_t ret = ::StartDiscovery(DM_PKG_NAME.c_str(), &subscribeInfo, &softbusDiscoveryCallback_);
    if (ret != 0) {
        LOGE("StartDiscovery failed with ret %d.", ret);
        return DM_DISCOVERY_FAILED;
    }
    return DM_OK;
}

int32_t SoftbusConnector::StopDiscovery(uint16_t subscribeId)
{
    LOGI("StopDiscovery begin, subscribeId:%d", (int32_t)subscribeId);
    int32_t ret = ::StopDiscovery(DM_PKG_NAME.c_str(), subscribeId);
    if (ret != 0) {
        LOGE("StopDiscovery failed with ret %d", ret);
        return ret;
    }
    LOGI("SoftbusConnector::StopDiscovery completed");
    return DM_OK;
}

int32_t SoftbusConnector::GetUdidByNetworkId(const char *networkId, std::string &udid)
{
    LOGI("GetUdidByNetworkId begin");
    uint8_t mUdid[UDID_BUF_LEN] = {0};
    int32_t ret =
        GetNodeKeyInfo(DM_PKG_NAME.c_str(), networkId, NodeDeviceInfoKey::NODE_KEY_UDID, mUdid, sizeof(mUdid));
    if (ret != DM_OK) {
        LOGE("GetUdidByNetworkId GetNodeKeyInfo failed");
        return DM_FAILED;
    }
    udid = (char *)mUdid;
    LOGI("SoftbusConnector::GetUdidByNetworkId completed");
    return DM_OK;
}

int32_t SoftbusConnector::GetUuidByNetworkId(const char *networkId, std::string &uuid)
{
    LOGI("GetUuidByNetworkId begin");
    uint8_t mUuid[UUID_BUF_LEN] = {0};
    int32_t ret =
        GetNodeKeyInfo(DM_PKG_NAME.c_str(), networkId, NodeDeviceInfoKey::NODE_KEY_UUID, mUuid, sizeof(mUuid));
    if (ret != DM_OK) {
        LOGE("GetUuidByNetworkId GetNodeKeyInfo failed");
        return DM_FAILED;
    }
    uuid = (char *)mUuid;
    LOGI("SoftbusConnector::GetUuidByNetworkId completed");
    return DM_OK;
}

bool SoftbusConnector::IsDeviceOnLine(const std::string &deviceId)
{
    NodeBasicInfo *info = nullptr;
    int32_t infoNum = 0;
    if (GetAllNodeDeviceInfo(DM_PKG_NAME.c_str(), &info, &infoNum) != DM_OK) {
        LOGE("GetAllNodeDeviceInfo failed");
        return false;
    }
    bool bDeviceOnline = false;
    for (int32_t i = 0; i < infoNum; ++i) {
        NodeBasicInfo *nodeBasicInfo = info + i;
        if (nodeBasicInfo == nullptr) {
            LOGE("nodeBasicInfo is empty for index %d, infoNum %d.", i, infoNum);
            continue;
        }
        std::string networkId = nodeBasicInfo->networkId;
        if (networkId == deviceId) {
            LOGI("DM_IsDeviceOnLine device %s online", GetAnonyString(deviceId).c_str());
            bDeviceOnline = true;
            break;
        }
        uint8_t udid[UDID_BUF_LEN] = {0};
        int32_t ret = GetNodeKeyInfo(DM_PKG_NAME.c_str(), networkId.c_str(), NodeDeviceInfoKey::NODE_KEY_UDID, udid,
                                     sizeof(udid));
        if (ret != DM_OK) {
            LOGE("DM_IsDeviceOnLine GetNodeKeyInfo failed");
            break;
        }
        if (strcmp((char *)udid, deviceId.c_str()) == 0) {
            LOGI("DM_IsDeviceOnLine device %s online", GetAnonyString(deviceId).c_str());
            bDeviceOnline = true;
            break;
        }
    }
    FreeNodeInfo(info);
    return bDeviceOnline;
}

std::shared_ptr<SoftbusSession> SoftbusConnector::GetSoftbusSession()
{
    return softbusSession_;
}

bool SoftbusConnector::HaveDeviceInMap(std::string deviceId)
{
    auto iter = discoveryDeviceInfoMap_.find(deviceId);
    if (iter == discoveryDeviceInfoMap_.end()) {
        LOGE("deviceInfo not found by deviceId %s", GetAnonyString(deviceId).c_str());
        return false;
    }
    return true;
}

int32_t SoftbusConnector::GetConnectionIpAddress(const std::string &deviceId, std::string &ipAddress)
{
    auto iter = discoveryDeviceInfoMap_.find(deviceId);
    if (iter == discoveryDeviceInfoMap_.end()) {
        LOGE("deviceInfo not found by deviceId %s", GetAnonyString(deviceId).c_str());
        return DM_FAILED;
    }
    DeviceInfo *deviceInfo = iter->second.get();
    if (deviceInfo->addrNum <= 0 || deviceInfo->addrNum >= CONNECTION_ADDR_MAX) {
        LOGE("deviceInfo address num not valid, addrNum %d", deviceInfo->addrNum);
        return DM_FAILED;
    }
    for (uint32_t i = 0; i < deviceInfo->addrNum; ++i) {
        // currently, only support CONNECT_ADDR_WLAN
        if (deviceInfo->addr[i].type != ConnectionAddrType::CONNECTION_ADDR_WLAN &&
            deviceInfo->addr[i].type != ConnectionAddrType::CONNECTION_ADDR_ETH) {
            continue;
        }
        ipAddress = deviceInfo->addr[i].info.ip.ip;
        LOGI("DM_GetConnectionIpAddr get ip ok.");
        return DM_OK;
    }
    LOGE("failed to get ipAddress for deviceId %s", GetAnonyString(deviceId).c_str());
    return DM_FAILED;
}

ConnectionAddr *SoftbusConnector::GetConnectAddrByType(DeviceInfo *deviceInfo, ConnectionAddrType type)
{
    if (deviceInfo == nullptr) {
        return nullptr;
    }
    for (uint32_t i = 0; i < deviceInfo->addrNum; ++i) {
        if (deviceInfo->addr[i].type == type) {
            return &deviceInfo->addr[i];
        }
    }
    return nullptr;
}

ConnectionAddr *SoftbusConnector::GetConnectAddr(const std::string &deviceId, std::string &connectAddr)
{
    auto iter = discoveryDeviceInfoMap_.find(deviceId);
    if (iter == discoveryDeviceInfoMap_.end()) {
        LOGE("deviceInfo not found by deviceId %s", GetAnonyString(deviceId).c_str());
        return nullptr;
    }
    DeviceInfo *deviceInfo = iter->second.get();
    if (deviceInfo->addrNum <= 0 || deviceInfo->addrNum >= CONNECTION_ADDR_MAX) {
        LOGE("deviceInfo addrNum not valid, addrNum %d", deviceInfo->addrNum);
        return nullptr;
    }
    nlohmann::json jsonPara;
    ConnectionAddr *addr = nullptr;
    addr = GetConnectAddrByType(deviceInfo, ConnectionAddrType::CONNECTION_ADDR_ETH);
    if (addr != nullptr) {
        LOGI("get ETH ConnectionAddr for deviceId %s", GetAnonyString(deviceId).c_str());
        jsonPara[ETH_IP] = addr->info.ip.ip;
        jsonPara[ETH_PORT] = addr->info.ip.port;
        connectAddr = jsonPara.dump();
        return addr;
    }
    addr = GetConnectAddrByType(deviceInfo, ConnectionAddrType::CONNECTION_ADDR_WLAN);
    if (addr != nullptr) {
        jsonPara[WIFI_IP] = addr->info.ip.ip;
        jsonPara[WIFI_PORT] = addr->info.ip.port;
        LOGI("get WLAN ConnectionAddr for deviceId %s", GetAnonyString(deviceId).c_str());
        connectAddr = jsonPara.dump();
        return addr;
    }
    addr = GetConnectAddrByType(deviceInfo, ConnectionAddrType::CONNECTION_ADDR_BLE);
    if (addr != nullptr) {
        jsonPara[BR_MAC] = addr->info.br.brMac;
        LOGI("get BLE ConnectionAddr for deviceId %s", GetAnonyString(deviceId).c_str());
        connectAddr = jsonPara.dump();
        return addr;
    }
    addr = GetConnectAddrByType(deviceInfo, ConnectionAddrType::CONNECTION_ADDR_BR);
    if (addr != nullptr) {
        jsonPara[BLE_MAC] = addr->info.ble.bleMac;
        LOGI("get BR ConnectionAddr for deviceId %s", GetAnonyString(deviceId).c_str());
        connectAddr = jsonPara.dump();
        return addr;
    }
    LOGE("failed to get ConnectionAddr for deviceId %s", GetAnonyString(deviceId).c_str());
    return nullptr;
}

int32_t SoftbusConnector::CovertNodeBasicInfoToDmDevice(const NodeBasicInfo &nodeBasicInfo, DmDeviceInfo &dmDeviceInfo)
{
    (void)memset_s(&dmDeviceInfo, sizeof(DmDeviceInfo), 0, sizeof(DmDeviceInfo));
    if (memcpy_s(dmDeviceInfo.deviceId, sizeof(dmDeviceInfo.deviceId), nodeBasicInfo.networkId,
                 std::min(sizeof(dmDeviceInfo.deviceId), sizeof(nodeBasicInfo.networkId))) != DM_OK) {
        LOGE("CovertNodeBasicInfoToDmDevice copy deviceId data failed");
    }

    if (memcpy_s(dmDeviceInfo.networkId, sizeof(dmDeviceInfo.networkId), nodeBasicInfo.networkId,
                 std::min(sizeof(dmDeviceInfo.networkId), sizeof(nodeBasicInfo.networkId))) != DM_OK) {
        LOGE("CovertNodeBasicInfoToDmDevice copy networkId data failed");
    }

    if (memcpy_s(dmDeviceInfo.deviceName, sizeof(dmDeviceInfo.deviceName), nodeBasicInfo.deviceName,
                 std::min(sizeof(dmDeviceInfo.deviceName), sizeof(nodeBasicInfo.deviceName))) != DM_OK) {
        LOGE("CovertNodeBasicInfoToDmDevice copy deviceName data failed");
    }
    dmDeviceInfo.deviceTypeId = nodeBasicInfo.deviceTypeId;
    return DM_OK;
}

void SoftbusConnector::CovertDeviceInfoToDmDevice(const DeviceInfo &deviceInfo, DmDeviceInfo &dmDeviceInfo)
{
    (void)memset_s(&dmDeviceInfo, sizeof(DmDeviceInfo), 0, sizeof(DmDeviceInfo));
    if (memcpy_s(dmDeviceInfo.deviceId, sizeof(dmDeviceInfo.deviceId), deviceInfo.devId,
                 std::min(sizeof(dmDeviceInfo.deviceId), sizeof(deviceInfo.devId))) != DM_OK) {
        LOGE("CovertDeviceInfoToDmDevice copy deviceId data failed");
    }

    if (memcpy_s(dmDeviceInfo.networkId, sizeof(dmDeviceInfo.networkId), 0,
                 sizeof(dmDeviceInfo.networkId)) != DM_OK) {
        LOGE("CovertDeviceInfoToDmDevice copy networkId data failed");
    }

    if (memcpy_s(dmDeviceInfo.deviceName, sizeof(dmDeviceInfo.deviceName), deviceInfo.devName,
                 std::min(sizeof(dmDeviceInfo.deviceName), sizeof(deviceInfo.devName))) != DM_OK) {
        LOGE("CovertDeviceInfoToDmDevice copy deviceName data failed");
    }
    dmDeviceInfo.deviceTypeId = deviceInfo.devType;
}

void SoftbusConnector::OnPublishSuccess(int publishId)
{
    LOGI("SoftbusConnector::OnPublishSuccess, publishId: %d", publishId);
}

void SoftbusConnector::OnPublishFail(int publishId, PublishFailReason reason)
{
    LOGI("SoftbusConnector::OnPublishFail failed, publishId: %d, reason: %d", publishId, reason);
}

void SoftbusConnector::OnSoftBusDeviceOnline(NodeBasicInfo *info)
{
    LOGI("device online");
    if (info == nullptr) {
        LOGE("SoftbusConnector::OnSoftbusDeviceOffline NodeBasicInfo is nullptr");
        return;
    }

    DmDeviceInfo dmDeviceInfo;
    CovertNodeBasicInfoToDmDevice(*info, dmDeviceInfo);
#if defined(__LITEOS_M__)
    DmThread deviceOnLine(DeviceOnLine, stateCallbackMap_, dmDeviceInfo);
    deviceOnLine.DmCreatThread();
#else
    std::thread deviceOnLine(DeviceOnLine, stateCallbackMap_, dmDeviceInfo);
    deviceOnLine.detach();
#endif

    if (discoveryDeviceInfoMap_.empty()) {
        return;
    }
    // remove the discovery node map
    uint8_t udid[UDID_BUF_LEN] = {0};
    int32_t ret =
        GetNodeKeyInfo(DM_PKG_NAME.c_str(), info->networkId, NodeDeviceInfoKey::NODE_KEY_UDID, udid, sizeof(udid));
    if (ret != DM_OK) {
        LOGE("GetNodeKeyInfo failed");
        return;
    }
    std::string deviceId = (char *)udid;
    LOGI("device online, deviceId: %s", GetAnonyString(deviceId).c_str());
    discoveryDeviceInfoMap_.erase(deviceId);
}

void SoftbusConnector::OnSoftbusDeviceOffline(NodeBasicInfo *info)
{
    if (info == nullptr) {
        LOGE("OnSoftbusDeviceOffline NodeBasicInfo is nullptr");
        return;
    }
    DmDeviceInfo dmDeviceInfo;
    CovertNodeBasicInfoToDmDevice(*info, dmDeviceInfo);
#if defined(__LITEOS_M__)
    DmThread deviceOffLine(DeviceOffLine, stateCallbackMap_, dmDeviceInfo);
    deviceOffLine.DmCreatThread();
#else
    std::thread deviceOffLine(DeviceOffLine, stateCallbackMap_, dmDeviceInfo);
    deviceOffLine.detach();
#endif
}

void SoftbusConnector::OnSoftbusDeviceInfoChanged(NodeBasicInfoType type, NodeBasicInfo *info)
{
    LOGI("SoftbusConnector::OnSoftbusDeviceInfoChanged.");
}

void SoftbusConnector::OnSoftbusDeviceFound(const DeviceInfo *device)
{
    if (device == nullptr) {
        LOGE("device is null");
        return;
    }
    std::string deviceId = device->devId;
    LOGI("SoftbusConnector::OnSoftbusDeviceFound device %s found.", GetAnonyString(deviceId).c_str());
    if (IsDeviceOnLine(deviceId)) {
        return;
    }
    std::shared_ptr<DeviceInfo> infoPtr = std::make_shared<DeviceInfo>();
    DeviceInfo *srcInfo = infoPtr.get();
    if (memcpy_s(srcInfo, sizeof(DeviceInfo), device, sizeof(DeviceInfo)) != 0) {
        LOGE("save discovery device info failed");
        return;
    }
    discoveryDeviceInfoMap_[deviceId] = infoPtr;
    // Remove the earliest element when reached the max size
    if (discoveryDeviceInfoMap_.size() == SOFTBUS_DISCOVER_DEVICE_INFO_MAX_SIZE) {
        auto iter = discoveryDeviceInfoMap_.begin();
        discoveryDeviceInfoMap_.erase(iter->second->devId);
    }
    DmDeviceInfo dmDeviceInfo;
    CovertDeviceInfoToDmDevice(*device, dmDeviceInfo);
    for (auto &iter : discoveryCallbackMap_) {
        iter.second->OnDeviceFound(iter.first, dmDeviceInfo);
    }
}

void SoftbusConnector::OnSoftbusDiscoveryFailed(int subscribeId, DiscoveryFailReason failReason)
{
    LOGI("In, subscribeId %d, failReason %d", subscribeId, (int32_t)failReason);
    uint16_t originId = (uint16_t)(((uint32_t)subscribeId) & SOFTBUS_SUBSCRIBE_ID_MASK);
    for (auto &iter : discoveryCallbackMap_) {
        iter.second->OnDiscoveryFailed(iter.first, originId, (int32_t)failReason);
    }
}

void SoftbusConnector::OnSoftbusDiscoverySuccess(int subscribeId)
{
    LOGI("In, subscribeId %d", subscribeId);
    uint16_t originId = (uint16_t)(((uint32_t)subscribeId) & SOFTBUS_SUBSCRIBE_ID_MASK);
    for (auto &iter : discoveryCallbackMap_) {
        iter.second->OnDiscoverySuccess(iter.first, originId);
    }
}

void SoftbusConnector::OnParameterChgCallback(const char *key, const char *value, void *context)
{
    if (strcmp(value, DISCOVER_STATUS_ON.c_str()) == 0 && publishStatus != ALLOW_BE_DISCOVERY) {
        PublishInfo dmPublishInfo;
        dmPublishInfo.publishId = DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID;
        dmPublishInfo.mode = DiscoverMode::DISCOVER_MODE_ACTIVE;
        dmPublishInfo.medium = ExchanageMedium::AUTO;
        dmPublishInfo.freq = ExchangeFreq::HIGH;
        dmPublishInfo.capability = DM_CAPABILITY_OSD;
        dmPublishInfo.capabilityData = nullptr;
        dmPublishInfo.dataLen = 0;
        int32_t ret = PublishService(DM_PKG_NAME.c_str(), &dmPublishInfo, &softbusPublishCallback_);
        if (ret == DM_OK) {
            publishStatus = ALLOW_BE_DISCOVERY;
        }
        LOGI("service publish result is : %d", ret);
    } else if (strcmp(value, DISCOVER_STATUS_OFF.c_str()) == 0 && publishStatus != NOT_ALLOW_BE_DISCOVERY) {
        int32_t ret = UnPublishService(DM_PKG_NAME.c_str(), DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
        if (ret == DM_OK) {
            publishStatus = NOT_ALLOW_BE_DISCOVERY;
        }
        LOGI("service unpublish result is : %d", ret);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
