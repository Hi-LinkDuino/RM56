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

#include "bms_device_manager.h"

#include "app_log_wrapper.h"
#include "device_manager.h"
#include "distributed_data_storage.h"
#include "service_control.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    const std::string BUNDLE_NAME = "ohos.appexefwk.appexefwk_standard";
    const std::string SERVICES_NAME = "d-bms";
}

BmsDeviceManager::BmsDeviceManager()
{
    APP_LOGD("BmsDeviceManager instance is created");
}

bool BmsDeviceManager::InitDeviceManager()
{
    initCallback_ = std::make_shared<DeviceInitCallBack>();
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(BUNDLE_NAME, initCallback_);
    if (ret != 0) {
        APP_LOGE("init device manager failed, ret:%{public}d", ret);
        return false;
    }
    stateCallback_ = std::make_shared<BmsDeviceStateCallback>();
    ret =
        DistributedHardware::DeviceManager::GetInstance().RegisterDevStateCallback(BUNDLE_NAME, "", stateCallback_);
    if (ret != 0) {
        APP_LOGE("register devStateCallback failed, ret:%{public}d", ret);
        return false;
    }
    APP_LOGD("register device manager success");
    return true;
}

void BmsDeviceManager::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    APP_LOGD("OnAddSystemAbility systemAbilityId:%{public}d add!", systemAbilityId);
    if (DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID == systemAbilityId) {
        InitDeviceManager();
        if (GetTrustedDeviceListSize() > 0) {
            StartDynamicSystemProcess(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        }
    }
}

void BmsDeviceManager::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    APP_LOGD("OnRemoveSystemAbility systemAbilityId:%{public}d removed!", systemAbilityId);
    if (DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID == systemAbilityId) {
        if (GetTrustedDeviceListSize() > 0) {
            StartDynamicSystemProcess(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        }
    }
}

int BmsDeviceManager::GetTrustedDeviceListSize()
{
    std::vector<DistributedHardware::DmDeviceInfo> deviceList;
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().GetTrustedDeviceList(BUNDLE_NAME, "", deviceList);
    if (ret != 0) {
        APP_LOGE("GetTrustedDeviceList failed, ret:%{public}d", ret);
        return -1;
    }
    APP_LOGD("GetTrustedDeviceList size :%{public}ud", static_cast<uint32_t>(deviceList.size()));
    return deviceList.size();
}

void BmsDeviceManager::StartDynamicSystemProcess(int32_t systemAbilityId)
{
    APP_LOGD("StartDynamicSystemProcess systemAbilityId:%{public}d !", systemAbilityId);
    std::string strExtra = std::to_string(systemAbilityId);
    auto extraArgv = strExtra.c_str();
    int ret = ServiceControlWithExtra(SERVICES_NAME.c_str(), START, &extraArgv, 1);
    APP_LOGD("StartDynamicSystemProcess, ret:%{public}d", ret);
}

void BmsDeviceManager::StopDynamicSystemProcess(int32_t systemAbilityId)
{
    APP_LOGD("StopDynamicSystemProcess systemAbilityId:%{public}d !", systemAbilityId);
    std::string strExtra = std::to_string(systemAbilityId);
    auto extraArgv = strExtra.c_str();
    int ret = ServiceControlWithExtra(SERVICES_NAME.c_str(), STOP, &extraArgv, 1);
    APP_LOGD("StopDynamicSystemProcess, ret:%{public}d", ret);
}

int32_t BmsDeviceManager::GetUdidByNetworkId(const std::string &netWorkId, std::string &udid)
{
    APP_LOGD("GetUdidByNetworkId");
    return DistributedHardware::DeviceManager::GetInstance().GetUdidByNetworkId(BUNDLE_NAME, netWorkId, udid);
}

void BmsDeviceManager::DeviceInitCallBack::OnRemoteDied()
{
    APP_LOGD("DeviceInitCallBack OnRemoteDied");
}

void BmsDeviceManager::BmsDeviceStateCallback::OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    APP_LOGD("DeviceInitCallBack OnDeviceOnline");
    BmsDeviceManager::StartDynamicSystemProcess(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    std::vector<std::string> deviceList;
    deviceList.push_back(std::string(deviceInfo.networkId));
    DistributedDataStorage::GetInstance()->SyncDistributedData(deviceList);
}

void BmsDeviceManager::BmsDeviceStateCallback::OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    APP_LOGD("DeviceInitCallBack OnDeviceOffline");
    DistributedDataStorage::GetInstance()->RemoveDeviceData(std::string(deviceInfo.networkId));
    std::vector<DistributedHardware::DmDeviceInfo> deviceList;
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().GetTrustedDeviceList(BUNDLE_NAME, "", deviceList);
    if (ret != 0) {
        APP_LOGE("GetTrustedDeviceList failed, ret:%{public}d", ret);
        return;
    }
    APP_LOGD("OnDeviceOffline GetTrustedDeviceList size :%{public}u", static_cast<uint32_t>(deviceList.size()));
    if (deviceList.size() == 0) {
        BmsDeviceManager::StopDynamicSystemProcess(DISTRIBUTED_BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    }
}

void BmsDeviceManager::BmsDeviceStateCallback::OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    APP_LOGD("DeviceInitCallBack OnDeviceChanged");
}

void BmsDeviceManager::BmsDeviceStateCallback::OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    APP_LOGD("DeviceInitCallBack OnDeviceReady");
}
}
}