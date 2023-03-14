/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "init/device_type_manager.h"

#include "common/hap_verify_log.h"
#include "init/trusted_root_ca.h"
#include "init/trusted_source_manager.h"

#ifdef SUPPORT_GET_DEVICE_TYPES
#include "security_device_mode.h"
#endif // SUPPORT_GET_DEVICE_TYPES

namespace OHOS {
namespace Security {
namespace Verify {
DeviceTypeManager& DeviceTypeManager::GetInstance()
{
    static DeviceTypeManager deviceTypeManager;
    return deviceTypeManager;
}

DeviceTypeManager::DeviceTypeManager() : deviceType(false), getDeviceTypeMtx()
{
}

DeviceTypeManager::~DeviceTypeManager()
{
}

bool DeviceTypeManager::GetDeviceType() const
{
#ifndef SUPPORT_GET_DEVICE_TYPES
    return false;
#else
    return InvokeIsDevelopmentMode();
#endif // SUPPORT_GET_DEVICE_TYPES
}

bool DeviceTypeManager::GetDeviceTypeInfo()
{
    bool currentDeviceType = GetDeviceType();
    HAPVERIFY_LOG_DEBUG(LABEL, "current device is type: %{public}d", static_cast<int>(currentDeviceType));

    if (currentDeviceType == deviceType) {
        return currentDeviceType;
    }

    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    TrustedSourceManager& trustedAppSourceManager = TrustedSourceManager::GetInstance();
    getDeviceTypeMtx.lock();
    if (currentDeviceType) {
        /* Device type change from  commercial to debugging */
        bool ret = rootCertsObj.EnableDebug() && trustedAppSourceManager.EnableDebug();
        if (!ret) {
            HAPVERIFY_LOG_ERROR(LABEL, "Enable debug failed");
            rootCertsObj.DisableDebug();
            trustedAppSourceManager.DisableDebug();
            getDeviceTypeMtx.unlock();
            return currentDeviceType;
        }
    } else {
        /* Device type change from debugging to commercial */
        rootCertsObj.DisableDebug();
        trustedAppSourceManager.DisableDebug();
    }
    deviceType = currentDeviceType;
    getDeviceTypeMtx.unlock();
    return currentDeviceType;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
