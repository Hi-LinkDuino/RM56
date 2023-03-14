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

#include "device_profile_adapter.h"

#include "dm_constants.h"
#include "dm_device_state_manager.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
DeviceProfileAdapter::DeviceProfileAdapter()
{
    LOGI("DeviceProfileAdapter construct");
}

DeviceProfileAdapter::~DeviceProfileAdapter()
{
    LOGI("DeviceProfileAdapter Destructor");
}

int32_t DeviceProfileAdapter::RegisterProfileListener(const std::string &pkgName, const std::string &deviceId,
                                                      std::shared_ptr<DmDeviceStateManager> callback)
{
    if (pkgName.empty() || deviceId.empty() ||  callback == nullptr) {
        LOGE("Not a reasonable function argument");
        return ERR_DM_INPUT_PARA_INVALID;
    }
    LOGI("register profile listener with pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> mutexLock(deviceProfileAdapterMutex_);
    deviceStateManager_ = callback;
    if (profileConnector_ == nullptr) {
        profileConnector_ = std::make_shared<ProfileConnector>();
    }
    if (profileConnector_ != nullptr) {
        profileConnector_->RegisterProfileCallback(pkgName, deviceId, this);
    }
    return DM_OK;
}

int32_t DeviceProfileAdapter::UnRegisterProfileListener(const std::string &pkgName)
{
    if (pkgName.empty()) {
        LOGE("not a reasonable function argument");
        return ERR_DM_INPUT_PARA_INVALID;
    }
    LOGI("unregister profile listener with pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> mutexLock(deviceProfileAdapterMutex_);
    deviceStateManager_ = nullptr;
    if (profileConnector_ != nullptr) {
        profileConnector_->UnRegisterProfileCallback(pkgName);
    }
    return DM_OK;
}

void DeviceProfileAdapter::OnProfileChanged(const std::string &pkgName, const std::string &deviceId)
{
    LOGI("on profile changed with pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> mutexLock(deviceProfileAdapterMutex_);
    if (deviceStateManager_ == nullptr) {
        LOGE("deviceStateManager_ is nullptr");
        return;
    }
    deviceStateManager_->OnProfileReady(pkgName, deviceId);
}

void DeviceProfileAdapter::OnProfileComplete(const std::string &pkgName, const std::string &deviceId)
{
    LOGI("on profile complete with pkgName: %s", pkgName.c_str());
    std::lock_guard<std::mutex> mutexLock(deviceProfileAdapterMutex_);
    if (deviceStateManager_ == nullptr) {
        LOGE("deviceStateManager_ is nullptr");
        return;
    }
    deviceStateManager_->OnProfileReady(pkgName, deviceId);
}

extern "C" IProfileAdapter *CreateDeviceProfileObject(void)
{
    return new DeviceProfileAdapter;
}
} // namespace DistributedHardware
} // namespace OHOS
