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

#include "gatt_based_services_manager.h"
#include <condition_variable>
#include <mutex>
#include "bt_def.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
GattBasedServicesManager::GattBasedServicesManager(GattServerService &server, utility::Dispatcher &dispatcher)
    : gas_(std::make_unique<GenericAccessService>(server, dispatcher)),
    dis_(std::make_unique<DeviceInformationService>(server)),
    gatts_(std::make_unique<GenericAttributeService>(server, dispatcher))
{}

GattBasedServicesManager::~GattBasedServicesManager()
{}

int GattBasedServicesManager::Enable() const
{
    int result = gas_->RegisterService();
    if (result != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}s:%{public}d Register GAS failed!", __FILE__, __FUNCTION__, __LINE__);
    }

    result = dis_->RegisterService();
    if (result != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}s:%{public}d Register DIS failed!", __FILE__, __FUNCTION__, __LINE__);
    }

    if (dis_->RegisterSDP() != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}s:%{public}d Register DIS to SDP failed!", __FILE__, __FUNCTION__, __LINE__);
    }

    result = gatts_->RegisterService();
    if (result != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}s:%{public}d Register GATTS failed!", __FILE__, __FUNCTION__, __LINE__);
    }

    if (gatts_->RegisterSDP() != GattStatus::GATT_SUCCESS) {
        LOG_WARN("%{public}s:%{public}s:%{public}d Register GATTS to SDP failed!", __FILE__, __FUNCTION__, __LINE__);
    }

    return result;
}

void GattBasedServicesManager::Disable() const
{
    gatts_->DeregisterSDP();
    gatts_->DeregisterService();
    dis_->DeregisterSDP();
    dis_->DeregisterService();
    gas_->DeregisterService();
}
}  // namespace bluetooth
}  // namespace OHOS