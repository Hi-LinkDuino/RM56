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

#ifndef OHOS_DM_CALLBACK_H
#define OHOS_DM_CALLBACK_H

#include <cstdint>
#include <string>

#include "dm_device_info.h"

namespace OHOS {
namespace DistributedHardware {
class DmInitCallback {
public:
    virtual ~DmInitCallback()
    {
    }
    virtual void OnRemoteDied() = 0;
};

class DeviceStateCallback {
public:
    virtual ~DeviceStateCallback()
    {
    }
    virtual void OnDeviceOnline(const DmDeviceInfo &deviceInfo) = 0;
    virtual void OnDeviceOffline(const DmDeviceInfo &deviceInfo) = 0;
    virtual void OnDeviceChanged(const DmDeviceInfo &deviceInfo) = 0;
    virtual void OnDeviceReady(const DmDeviceInfo &deviceInfo) = 0;
};

class DiscoveryCallback {
public:
    virtual ~DiscoveryCallback()
    {
    }
    virtual void OnDiscoverySuccess(uint16_t subscribeId) = 0;
    virtual void OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason) = 0;
    virtual void OnDeviceFound(uint16_t subscribeId, const DmDeviceInfo &deviceInfo) = 0;
};

class AuthenticateCallback {
public:
    virtual ~AuthenticateCallback()
    {
    }
    virtual void OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status,
                              int32_t reason) = 0;
};

class VerifyAuthCallback {
public:
    virtual ~VerifyAuthCallback()
    {
    }
    virtual void OnVerifyAuthResult(const std::string &deviceId, int32_t resultCode, int32_t flag) = 0;
};
class DeviceManagerFaCallback {
public:
    virtual ~DeviceManagerFaCallback()
    {
    }
    virtual void OnCall(const std::string &paramJson) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_CALLBACK_H
