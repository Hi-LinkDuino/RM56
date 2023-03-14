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
#include <set>
#include <vector>

#include "__functional_base"
#include "__tree"
#include "array"

#include "bluetooth_def.h"
#include "bluetooth_gatt_client_proxy.h"
#include "bluetooth_gatt_device.h"
#include "bluetooth_gatt_manager.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_remote_device.h"
#include "i_bluetooth_host.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "memory"
#include "new"
#include "raw_address.h"
#include "refbase.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
struct GattManager::impl {
public:
    impl()
    {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

        sptr<BluetoothHostProxy> hostProxy = new BluetoothHostProxy(hostRemote);
        sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_GATT_CLIENT);

        clientProxy_ = new BluetoothGattClientProxy(remote);
    }
    ~impl()
    {}
    sptr<BluetoothGattClientProxy> clientProxy_;
};

GattManager::GattManager() : pimpl(new GattManager::impl())
{}

GattManager::~GattManager()
{}

std::vector<BluetoothRemoteDevice> GattManager::GetDevicesByStates(
    const std::array<int, GATT_CONNECTION_STATE_NUM> &states)
{
    std::set<int> stateSet;
    for (auto &state : states) {
        stateSet.emplace(state);
    }
    std::vector<BluetoothRemoteDevice> result;
    if (pimpl->clientProxy_ != nullptr) {
        std::vector<BluetoothGattDevice> devices;
        pimpl->clientProxy_->GetAllDevice(devices);
        for (auto &item : devices) {
            if (stateSet.find(item.connectState_) != stateSet.end()) {
                result.push_back(BluetoothRemoteDevice(item.addr_.GetAddress(),
                    item.transport_ == (GATT_TRANSPORT_TYPE_LE ? BT_TRANSPORT_BLE : BT_TRANSPORT_BREDR)));
            }
        }
    }
    return result;
}

std::vector<BluetoothRemoteDevice> GattManager::GetConnectedDevices()
{
    std::vector<BluetoothRemoteDevice> result;
    if (pimpl->clientProxy_ != nullptr) {
        std::vector<BluetoothGattDevice> device;
        pimpl->clientProxy_->GetAllDevice(device);
        for (auto &item : device) {
            if (item.connectState_ == static_cast<int>(BTConnectState::CONNECTED)) {
                result.push_back(BluetoothRemoteDevice(item.addr_.GetAddress(),
                    item.transport_ == (GATT_TRANSPORT_TYPE_LE ? BT_TRANSPORT_BLE : BT_TRANSPORT_BREDR)));
            }
        }
    }
    return result;
}
}  // namespace Bluetooth
}  // namespace OHOS
