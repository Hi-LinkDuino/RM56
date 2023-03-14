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

#ifndef GATT_CLIENT_SERVICE_H
#define GATT_CLIENT_SERVICE_H

#include <cstdint>
#include <list>
#include <memory>
#include <vector>
#include "base_def.h"
#include "context.h"
#include "bt_uuid.h"
#include "gatt_data.h"
#include "interface_profile_gatt_client.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class GattClientService : public IProfileGattClient, public utility::Context {
public:
    int RegisterApplication(IGattClientCallback &callback, const RawAddress &addr, uint8_t transport) override;
    int RegisterSharedApplication(IGattClientCallback &callback, const RawAddress &addr, uint8_t transport) override;
    int DeregisterApplication(int appId) override;
    int Connect(int appId, bool autoConnect = false) override;
    int Disconnect(int appId) override;
    int DiscoveryServices(int appId) override;
    int ReadCharacteristic(int appId, const Characteristic &characteristic) override;
    int ReadCharacteristicByUuid(int appId, const Uuid &uuid) override;
    int WriteCharacteristic(int appId, Characteristic &characteristic, bool withoutRespond = false) override;
    int SignedWriteCharacteristic(int appId, Characteristic &characteristic) override;
    int ReadDescriptor(int appId, const Descriptor &descriptor) override;
    int WriteDescriptor(int appId, Descriptor &descriptor) override;
    int RequestExchangeMtu(int appId, int mtu) override;
    std::vector<GattDevice> GetAllDevice() override;
    int RequestConnectionPriority(int appId, int connPriority) override;
    std::vector<Service> GetServices(int appId) override;

    GattClientService();
    ~GattClientService();

    /// Inherited from the parent class
    void Enable() override;
    void Disable() override;
    utility::Context *GetContext() override;

    int GetConnectState() override;
    int GetMaxConnectNum() override;
    std::list<RawAddress> GetConnectDevices() override;

    BT_DISALLOW_COPY_AND_ASSIGN(GattClientService);

private:
    int Connect(const RawAddress &device) override;
    int Disconnect(const RawAddress &device) override;
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_CLIENT_SERVICE_H
