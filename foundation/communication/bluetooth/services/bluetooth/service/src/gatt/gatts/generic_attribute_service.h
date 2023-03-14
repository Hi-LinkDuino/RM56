/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef GENERIC_ATTRIBUTE_SERVICE_H
#define GENERIC_ATTRIBUTE_SERVICE_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include "../gatt_connection.h"
#include "../gatt_server_service.h"
#include "base_def.h"
#include "dispatcher.h"
#include "gatt_data.h"
#include "gatt_service_over_bredr.h"

namespace OHOS {
namespace bluetooth {
class GenericAttributeService {
public:
    GenericAttributeService(GattServerService &service, utility::Dispatcher &dispatcher);
    ~GenericAttributeService();

    int RegisterService();
    void DeregisterService() const;

    int RegisterSDP();
    void DeregisterSDP() const;

    BT_DISALLOW_COPY_AND_ASSIGN(GenericAttributeService);

private:
    static const size_t CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH;
    static const size_t SERVICE_CHANGED_VALUE_LENGTH;
    static const uint8_t CLIENT_CHARACTERISTIC_CONFIGURATION_DEFAULT_VALUE[2];
    static const uint8_t SERVICE_CHANGED_DEFAULT_VALUE[4];

    class GattServerCallbackImpl;
    struct NotifyInformation {
        bool isNotified = true;
        uint16_t startHandle_ = 0;
        uint16_t endHandle_ = 0;
    };

    GattServerService &serverService_;
    utility::Dispatcher &dispatcher_;
    int appId_ = -1;
    std::unique_ptr<GattServiceOverBredrInterface> sdpRegister_ = {nullptr};
    std::unique_ptr<Service> instance_ = {nullptr};
    std::shared_ptr<GattServerCallbackImpl> serviceCallback_;
    std::map<GattDevice, NotifyInformation> devices_ = {};

    void StoreNotifyInformation() const;
    void LoadNotifyInformation();
    void ComfirmNotify(GattDevice device);
    std::unique_ptr<Service> BuildService();
    void NotifyAllDevices(uint16_t startHandle, uint16_t endHandle);
    void NotifyServiceChanged(const GattDevice& device, Characteristic& characteristic) const;
    void ConnectIncoming(const GattDevice& device);
    void Disconnected(const GattDevice& device);
    std::unique_ptr<uint8_t[]> BuildValue(uint16_t startHandle, uint16_t endHandle) const;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif // GENERIC_ATTRIBUTE_SERVICE_H