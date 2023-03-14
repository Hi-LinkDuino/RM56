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

#ifndef GATT_SERVER_SERVICE_H
#define GATT_SERVER_SERVICE_H

#include <list>
#include <string>
#include "base_def.h"
#include "context.h"
#include "gatt_data.h"
#include "interface_profile_gatt_server.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class GattServerService : public IProfileGattServer, public utility::Context {
public:
    int RegisterApplication(std::shared_ptr<IGattServerCallback> callback) override;
    int DeregisterApplication(int appId) override;
    int AddService(int appId, Service &service) override;
    int RemoveService(int appId, const Service &service) override;
    int ClearServices(int appId) override;
    int NotifyClient(const GattDevice &device, Characteristic &characteristic, bool needConfirm = false) override;
    int RespondCharacteristicRead(const GattDevice &device, Characteristic &characteristic, int ret) override;
    int RespondCharacteristicReadByUuid(const GattDevice &device, Characteristic &characteristic, int ret) override;
    int RespondCharacteristicWrite(const GattDevice &device, const Characteristic &characteristic, int ret) override;
    int RespondDescriptorRead(const GattDevice &device, Descriptor &descriptor, int ret) override;
    int RespondDescriptorWrite(const GattDevice &device, const Descriptor &descriptor, int ret) override;
    int CancelConnection(const GattDevice &device) override;
    int SetCharacteristicValue(const Characteristic &characteristic);
    int SetCharacteristicPermission(const Characteristic &characteristic, uint8_t properties, uint8_t permission);

    int RegisterApplicationSync(std::shared_ptr<IGattServerCallback> callback) const;
    int DeregisterApplicationSync(int appId) const;
    int AddServiceSync(int appId, Service &service) const;
    const std::string GetDatabaseHash() const;

    GattServerService();
    ~GattServerService();

    /// Inherited from the parent class
    void Enable() override;
    void Disable() override;
    utility::Context *GetContext() override;

    std::list<RawAddress> GetConnectDevices() override;
    int GetConnectState() override;
    int GetMaxConnectNum() override;

    BT_DISALLOW_COPY_AND_ASSIGN(GattServerService);

private:
    int Connect(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;

    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_SERVER_SERVICE_H
