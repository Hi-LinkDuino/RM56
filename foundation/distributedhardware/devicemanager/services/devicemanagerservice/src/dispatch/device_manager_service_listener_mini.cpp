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

#include "device_manager_service_listener.h"

#include "dm_anonymous.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "device_manager_notify.h"
#include "command_dispatch.h"

namespace OHOS {
namespace DistributedHardware {
void DeviceManagerServiceListener::OnDeviceStateChange(const std::string &pkgName, const DmDeviceState &state,
                                                       const DmDeviceInfo &info)
{
    LOGI("call OnDeviceStateChange, state=%d", state);
    std::list<std::string> pkgNameList = CommandDispatch::GetInstance().GetPkgNameList();
    for (auto pkgName : pkgNameList) {
        DmDeviceState deviceState = static_cast<DmDeviceState>(state);
        if (pkgName == "") {
            LOGE("OnDeviceOnline, get para failed");
            return;
        }
        switch (deviceState) {
            case DEVICE_STATE_ONLINE:
                DeviceManagerNotify::GetInstance().OnDeviceOnline(pkgName, info);
                break;
            case DEVICE_STATE_OFFLINE:
                DeviceManagerNotify::GetInstance().OnDeviceOffline(pkgName, info);
                break;
            case DEVICE_INFO_CHANGED:
                DeviceManagerNotify::GetInstance().OnDeviceChanged(pkgName, info);
                break;
            default:
                LOGE("unknown device state:%d", deviceState);
                break;
        }
    }
}

void DeviceManagerServiceListener::OnDeviceFound(const std::string &pkgName, uint16_t subscribeId,
                                                 const DmDeviceInfo &info)
{
    LOGI("call OnDeviceFound for %s, originId %d, deviceId %s", pkgName.c_str(), subscribeId,
        GetAnonyString(std::string(info.deviceId)).c_str());
    DeviceManagerNotify::GetInstance().OnDeviceFound(pkgName, subscribeId, info);
}

void DeviceManagerServiceListener::OnDiscoveryFailed(const std::string &pkgName, uint16_t subscribeId,
                                                     int32_t failedReason)
{
    LOGI("call OnDiscoveryFailed");
    DeviceManagerNotify::GetInstance().OnDiscoveryFailed(pkgName, subscribeId, failedReason);
}

void DeviceManagerServiceListener::OnDiscoverySuccess(const std::string &pkgName, int32_t subscribeId)
{
    LOGI("call OnDiscoverySuccess");
    DeviceManagerNotify::GetInstance().OnDiscoverySuccess(pkgName, subscribeId);
}

void DeviceManagerServiceListener::OnAuthResult(const std::string &pkgName, const std::string &deviceId,
                                                const std::string &token, int32_t status, int32_t reason)
{
    LOGI("call package: %s, deviceId: %s", pkgName.c_str(), GetAnonyString(deviceId).c_str());
    DeviceManagerNotify::GetInstance().OnAuthResult(pkgName, deviceId, token, status, reason);
}

void DeviceManagerServiceListener::OnVerifyAuthResult(const std::string &pkgName, const std::string &deviceId,
                                                      int32_t resultCode, const std::string &flag)
{
    LOGI("call OnVerifyAuthResult");
    std::list<std::string> pkgNameList = CommandDispatch::GetInstance().GetPkgNameList();
    int32_t num = 0;
    ((resultCode == 0) && (!flag.empty())) ? (num = std::stoi(flag)) : (num = 0);

    for (auto pkgName : pkgNameList) {
        DeviceManagerNotify::GetInstance().OnVerifyAuthResult(pkgName, deviceId, resultCode, num);
    }
}

void DeviceManagerServiceListener::OnFaCall(std::string &pkgName, std::string &paramJson)
{
    LOGI("call OnFaCall in");
    DeviceManagerNotify::GetInstance().OnFaCall(pkgName, paramJson);
}
} // namespace DistributedHardware
} // namespace OHOS
