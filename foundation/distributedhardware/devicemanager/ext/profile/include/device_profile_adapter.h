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

#ifndef OHOS_DM_ADAPTER_DEVICE_PROFILE_H
#define OHOS_DM_ADAPTER_DEVICE_PROFILE_H

#include <cstdint>
#include <mutex>
#include <string>

#include "profile_adapter.h"
#include "profile_connector.h"

namespace OHOS {
namespace DistributedHardware {
class DeviceProfileAdapter final : public IProfileAdapter, public IProfileConnectorCallback,
    public std::enable_shared_from_this<DeviceProfileAdapter> {
public:
    DeviceProfileAdapter();
    virtual ~DeviceProfileAdapter();
    int32_t RegisterProfileListener(const std::string &pkgName, const std::string &deviceId,
                                    std::shared_ptr<DmDeviceStateManager> callback) override;
    int32_t UnRegisterProfileListener(const std::string &pkgName) override;
    void OnProfileChanged(const std::string &pkgName, const std::string &deviceId) override;
    void OnProfileComplete(const std::string &pkgName, const std::string &deviceId) override;

private:
    std::mutex deviceProfileAdapterMutex_;
    std::shared_ptr<DmDeviceStateManager> deviceStateManager_;
    std::shared_ptr<ProfileConnector> profileConnector_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_ADAPTER_DEVICE_PROFILE_H
