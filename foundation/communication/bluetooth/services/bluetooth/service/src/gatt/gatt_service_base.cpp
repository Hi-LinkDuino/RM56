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

#include "gatt_service_base.h"
#include <random>
#include "gatt_connection_manager.h"
#include "interface_profile.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const uint8_t GattServiceBase::MAXIMUM_NUMBER_APPLICATION = 0xFF;

int GattServiceBase::ConvertConnectionState(const std::string &state)
{
    BTConnectState newState = BTConnectState::DISCONNECTED;
    if (state.compare(GattConnectionManager::Device::STATE_CONNECTING) == 0) {
        newState = BTConnectState::CONNECTING;
    } else if (state.compare(GattConnectionManager::Device::STATE_CONNECTED) == 0) {
        newState = BTConnectState::CONNECTED;
    } else if (state.compare(GattConnectionManager::Device::STATE_DISCONNECTING) == 0) {
        newState = BTConnectState::DISCONNECTING;
    }

    return static_cast<int>(newState);
}

Buffer *GattServiceBase::BuildBuffer(const uint8_t *value, size_t length)
{
    Buffer *result = BufferMalloc(length);
    if (result != nullptr) {
        (void)memcpy_s(BufferPtr(result), length, value, length);
    }

    return result;
}

Buffer *GattServiceBase::BuildBuffer(const uint8_t *value, size_t offset, size_t length)
{
    Buffer *result = BufferMalloc(length);
    if (result != nullptr) {
        (void)memcpy_s(BufferPtr(result), length, value + offset, length);
    }

    return result;
}

GattValue GattServiceBase::MoveToGattValue(std::unique_ptr<uint8_t[]> &value)
{
    std::shared_ptr<std::unique_ptr<uint8_t[]>> sharedPtr
        = std::make_shared<std::unique_ptr<uint8_t[]>>(std::move(value));
    return sharedPtr;
}

GattValue GattServiceBase::BuildGattValue(const uint8_t *value, size_t length)
{
    std::shared_ptr<std::unique_ptr<uint8_t[]>> sharedPtr =
        std::make_shared<std::unique_ptr<uint8_t[]>>(std::make_unique<uint8_t[]>(length));
    (void)memcpy_s(sharedPtr->get(), length, value, length);
    return sharedPtr;
}

int GattServiceBase::GetApplicationId()
{
    // The random number is used for the application of GATT service internal identification registration,
    // and does not involve security calculations.
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0x01);

    return distribution(generator);
}
}  // namespace bluetooth
}  // namespace OHOS
