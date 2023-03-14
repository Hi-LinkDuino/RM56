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

#include "dm_device_state_manager.h"

#include "dm_adapter_manager.h"
#include "dm_anonymous.h"
#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
const int32_t SESSION_CANCEL_TIMEOUT = 0;

static void TimeOut(void *data, DmTimer& timer)
{
    LOGI("time out %s", timer.GetTimerName().c_str());
    if (data == nullptr || timer.GetTimerName().find(TIMER_PREFIX) != TIMER_DEFAULT) {
        LOGE("time out is not our timer");
        return;
    }

    DmDeviceStateManager *deviceStateMgr = (DmDeviceStateManager*)data;
    if (deviceStateMgr == nullptr) {
        LOGE("deviceStateMgr is nullptr");
        return;
    }

    deviceStateMgr->DeleteTimeOutGroup(timer.GetTimerName());
}

DmDeviceStateManager::DmDeviceStateManager(std::shared_ptr<SoftbusConnector> softbusConnector,
    std::shared_ptr<DeviceManagerServiceListener> listener, std::shared_ptr<HiChainConnector> hiChainConnector)
    : softbusConnector_(softbusConnector), listener_(listener), hiChainConnector_(hiChainConnector)
{
    profileSoName_ = "libdevicemanagerext_profile.z.so";
    decisionSoName_ = "libdevicemanagerext_decision.z.so";
    LOGI("DmDeviceStateManager constructor");
}

DmDeviceStateManager::~DmDeviceStateManager()
{
    LOGI("DmDeviceStateManager destructor");
    if (softbusConnector_ != nullptr) {
        softbusConnector_->UnRegisterSoftbusStateCallback("DM_PKG_NAME");
    }
}

int32_t DmDeviceStateManager::RegisterProfileListener(const std::string &pkgName, const DmDeviceInfo &info)
{
    DmAdapterManager &adapterMgrPtr = DmAdapterManager::GetInstance();
    std::shared_ptr<IProfileAdapter> profileAdapter = adapterMgrPtr.GetProfileAdapter(profileSoName_);
    if (profileAdapter != nullptr) {
        std::string deviceUdid;
        int32_t ret = SoftbusConnector::GetUdidByNetworkId(info.deviceId, deviceUdid);
        if (ret == DM_OK) {
            std::string uuid;
            DmDeviceInfo saveInfo = info;
            SoftbusConnector::GetUuidByNetworkId(info.deviceId, uuid);
            {
#if defined(__LITEOS_M__)
                DmMutex mutexLock;
#else
                std::lock_guard<std::mutex> mutexLock(remoteDeviceInfosMutex_);
#endif
                remoteDeviceInfos_[uuid] = saveInfo;
            }
            LOGI("RegisterProfileListener in, deviceId = %s, deviceUdid = %s, uuid = %s",
                 info.deviceId, deviceUdid.c_str(), uuid.c_str());
            profileAdapter->RegisterProfileListener(pkgName, deviceUdid, shared_from_this());
        }
    }
    return DM_OK;
}

int32_t DmDeviceStateManager::UnRegisterProfileListener(const std::string &pkgName, const DmDeviceInfo &info)
{
    DmAdapterManager &adapterMgrPtr = DmAdapterManager::GetInstance();
    std::shared_ptr<IProfileAdapter> profileAdapter = adapterMgrPtr.GetProfileAdapter(profileSoName_);
    if (profileAdapter != nullptr) {
        LOGI("UnRegister Profile Listener");
        profileAdapter->UnRegisterProfileListener(pkgName);
    }
    {
#if defined(__LITEOS_M__)
        DmMutex mutexLock;
#else
        std::lock_guard<std::mutex> mutexLock(remoteDeviceInfosMutex_);
#endif
        if (remoteDeviceInfos_.find(std::string(info.deviceId)) != remoteDeviceInfos_.end()) {
            remoteDeviceInfos_.erase(std::string(info.deviceId));
        }
    }
    return DM_OK;
}

void DmDeviceStateManager::PostDeviceOnline(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("DmDeviceStateManager::PostDeviceOnline in");
    if (listener_ != nullptr) {
        DmDeviceState state = DEVICE_STATE_ONLINE;
        listener_->OnDeviceStateChange(pkgName, state, info);
    }
    LOGI("DmDeviceStateManager::PostDeviceOnline out");
}

void DmDeviceStateManager::PostDeviceOffline(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("DmDeviceStateManager::PostDeviceOffline in");
    if (listener_ != nullptr) {
        DmDeviceState state = DEVICE_STATE_OFFLINE;
        listener_->OnDeviceStateChange(pkgName, state, info);
    }
    LOGI("DmDeviceStateManager::PostDeviceOffline out");
}

void DmDeviceStateManager::OnDeviceOnline(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("OnDeviceOnline function is called back with pkgName: %s", pkgName.c_str());
    RegisterOffLineTimer(info);
    RegisterProfileListener(pkgName, info);

    DmAdapterManager &adapterMgrPtr = DmAdapterManager::GetInstance();
    std::shared_ptr<IDecisionAdapter> decisionAdapter = adapterMgrPtr.GetDecisionAdapter(decisionSoName_);
    if (decisionAdapter == nullptr) {
        LOGE("OnDeviceOnline decision adapter is null");
        PostDeviceOnline(pkgName, info);
    } else if (decisionInfos_.size() == 0) {
        PostDeviceOnline(pkgName, info);
    } else {
        std::string extra;
        std::vector<DmDeviceInfo> infoList;
        LOGI("OnDeviceOnline decision decisionInfos_ size: %d", decisionInfos_.size());
        for (auto iter : decisionInfos_) {
            std::string listenerPkgName = iter.first;
            std::string extra = iter.second;
            infoList.clear();
            infoList.push_back(info);
            decisionAdapter->FilterDeviceList(infoList, extra);
            if (infoList.size() == 1) {
                PostDeviceOnline(listenerPkgName, info);
            }
        }
    }
    LOGI("DmDeviceStateManager::OnDeviceOnline out");
}

void DmDeviceStateManager::OnDeviceOffline(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("OnDeviceOnline function is called with pkgName: %s", pkgName.c_str());
    StartOffLineTimer(info);
    UnRegisterProfileListener(pkgName, info);

    DmAdapterManager &adapterMgrPtr = DmAdapterManager::GetInstance();
    std::shared_ptr<IDecisionAdapter> decisionAdapter = adapterMgrPtr.GetDecisionAdapter(decisionSoName_);
    if (decisionAdapter == nullptr) {
        LOGE("OnDeviceOnline decision adapter is null");
        PostDeviceOffline(pkgName, info);
    } else if (decisionInfos_.size() == 0) {
        PostDeviceOffline(pkgName, info);
    } else {
        std::string extra;
        std::vector<DmDeviceInfo> infoList;
        LOGI("OnDeviceOnline decision decisionInfos_ size: %d", decisionInfos_.size());
        for (auto iter : decisionInfos_) {
            std::string listenerPkgName = iter.first;
            std::string extra = iter.second;
            infoList.clear();
            infoList.push_back(info);
            decisionAdapter->FilterDeviceList(infoList, extra);
            if (infoList.size() == 1) {
                PostDeviceOffline(listenerPkgName, info);
            }
        }
    }
    LOGI("DmDeviceStateManager::OnDeviceOffline out");
}

void DmDeviceStateManager::OnDeviceChanged(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("OnDeviceChanged function is called back with pkgName: %s", pkgName.c_str());
}

void DmDeviceStateManager::OnDeviceReady(const std::string &pkgName, const DmDeviceInfo &info)
{
    LOGI("OnDeviceReady function is called back with pkgName: %s", pkgName.c_str());
}

void DmDeviceStateManager::OnProfileReady(const std::string &pkgName, const std::string &deviceId)
{
    LOGI("OnProfileReady function is called back");
    if (pkgName.empty() || deviceId.empty()) {
        LOGE("On profile ready pkgName is empty or deviceId is deviceId");
        return;
    }
    DmDeviceInfo saveInfo;
    {
#if defined(__LITEOS_M__)
        DmMutex mutexLock;
#else
        std::lock_guard<std::mutex> mutexLock(remoteDeviceInfosMutex_);
#endif
        auto iter = remoteDeviceInfos_.find(deviceId);
        if (iter == remoteDeviceInfos_.end()) {
            LOGE("OnProfileReady complete not find deviceId: %s", GetAnonyString(deviceId).c_str());
            return;
        }
        saveInfo = iter->second;
    }
    if (listener_ != nullptr) {
        DmDeviceState state = DEVICE_INFO_READY;
        listener_->OnDeviceStateChange(pkgName, state, saveInfo);
    }
}

int32_t DmDeviceStateManager::RegisterSoftbusStateCallback()
{
    if (softbusConnector_ != nullptr) {
        return softbusConnector_->RegisterSoftbusStateCallback(DM_PKG_NAME, shared_from_this());
    }
    return DM_OK;
}

void DmDeviceStateManager::RegisterDevStateCallback(const std::string &pkgName, const std::string &extra)
{
    LOGI("DmDeviceStateManager::RegisterDevStateCallback pkgName=%s, extra=%s", pkgName.c_str(), extra.c_str());
    if (pkgName != "") {
        decisionInfos_[pkgName] = extra;
    }
}

void DmDeviceStateManager::UnRegisterDevStateCallback(const std::string &pkgName, const std::string &extra)
{
    LOGI("DmDeviceStateManager::UnRegisterDevStateCallback pkgName=%s, extra=%s", pkgName.c_str(), extra.c_str());
    auto iter = decisionInfos_.find(pkgName);
    if (iter == decisionInfos_.end()) {
    } else {
        decisionInfos_.erase(pkgName);
    }
}

void DmDeviceStateManager::RegisterOffLineTimer(const DmDeviceInfo &deviceInfo)
{
    std::string deviceId;
    int32_t ret = softbusConnector_->GetUdidByNetworkId(deviceInfo.deviceId, deviceId);
    if (ret != DM_OK) {
        LOGE("fail to get udid by networkId");
        return;
    }
    LOGI("Register OffLine Timer with device: %s", GetAnonyString(deviceId).c_str());
#if defined(__LITEOS_M__)
    DmMutex mutexLock;
#else
    std::lock_guard<std::mutex> mutexLock(timerMapMutex_);
#endif
    for (auto &iter : stateTimerInfoMap_) {
        if (iter.second.netWorkId == deviceInfo.deviceId) {
            iter.second.timer->Stop(SESSION_CANCEL_TIMEOUT);
            return;
        }
    }

    std::string timerName = TIMER_PREFIX + STATE_TIMER_PREFIX + std::to_string(cumulativeQuantity_++);
    std::shared_ptr<DmTimer> offLineTimer = std::make_shared<DmTimer>(timerName);
    if (offLineTimer != nullptr) {
        StateTimerInfo stateTimer = {
            .timerName = timerName,
            .netWorkId = deviceInfo.deviceId,
            .deviceId = deviceId,
            .timer = offLineTimer
        };
        stateTimerInfoMap_[timerName] = stateTimer;
    }
}

void DmDeviceStateManager::StartOffLineTimer(const DmDeviceInfo &deviceInfo)
{
#if defined(__LITEOS_M__)
    DmMutex mutexLock;
#else
    std::lock_guard<std::mutex> mutexLock(timerMapMutex_);
#endif
    LOGI("start offline timer");
    for (auto &iter : stateTimerInfoMap_) {
        if (iter.second.netWorkId == deviceInfo.deviceId) {
            iter.second.timer->Start(OFFLINE_TIMEOUT, TimeOut, this);
        }
    }
}

void DmDeviceStateManager::DeleteTimeOutGroup(std::string stateTimer)
{
#if defined(__LITEOS_M__)
    DmMutex mutexLock;
#else
    std::lock_guard<std::mutex> mutexLock(timerMapMutex_);
#endif
    if (hiChainConnector_ != nullptr) {
        auto iter = stateTimerInfoMap_.find(stateTimer);
        if (iter != stateTimerInfoMap_.end()) {
            LOGI("remove hichain group with device: %s",
                GetAnonyString(stateTimerInfoMap_[stateTimer].deviceId).c_str());
            hiChainConnector_->DeleteTimeOutGroup(stateTimerInfoMap_[stateTimer].deviceId.c_str());
        }
    }
    stateTimerInfoMap_.erase(stateTimer);
}
} // namespace DistributedHardware
} // namespace OHOS