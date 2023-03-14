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

#include "create_devicemanager_factory.h"


namespace OHOS {
namespace Camera {
    std::string deviceName = "MpiDeviceManager";
IDeviceManager::IDeviceManager() {}

IDeviceManager::~IDeviceManager() {}

std::shared_ptr<IDeviceManager> IDeviceManager::GetInstance()
{
    std::string deviceName = "MpiDeviceManager";
    static std::shared_ptr<IDeviceManager> deviceManger = nullptr;
    if (deviceManger == nullptr) {
        deviceManger = GetDeviceManagerFactory.GetDeviceManagerByName(deviceName);
    }
    return deviceManger;
}
} // namespace Camera
} // namespace OHOS
