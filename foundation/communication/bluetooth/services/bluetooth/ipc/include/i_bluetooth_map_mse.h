/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MSE_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MSE_INTERFACE_H

#include <string>
#include <vector>
#include "../parcel/bluetooth_map_mce_parameter.h"
#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"
#include "i_bluetooth_map_mse_observer.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothMapMse : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothMapMse");

    enum Code {
        MSE_REGISTER_OBSERVER = 0,
        MSE_DE_REGISTER_OBSERVER,
        MSE_GET_STATE,
        MSE_DISCONNECT,
        MSE_ISCONNECTED,
        MSE_GET_CONNECTED_DEVICES,
        MSE_GET_DEVICES_BY_STATES,
        MSE_GET_CONNECTION_STATE,
        MSE_SET_CONNECTION_STRATEGY,
        MSE_GET_CONNECTION_STRATEGY,
        MSE_GRANT_PERMISSION,
    };

    // IBluetoothMapMse:
    virtual void RegisterObserver(
        const sptr<IBluetoothMapMseObserver> &observer) = 0;
    virtual void DeregisterObserver(
        const sptr<IBluetoothMapMseObserver> &observer) = 0;
    virtual void GetState(int32_t &ret) = 0;
    virtual void Disconnect(
        const BluetoothRawAddress &device, int32_t &ret) = 0;
    virtual void IsConnected(
        const BluetoothRawAddress &device, bool &ret) = 0;
    virtual void GetConnectedDevices(
        std::vector<BluetoothRawAddress> &devices) = 0;
    virtual void GetDevicesByStates(const std::vector<int32_t> &states,
        std::vector<BluetoothRawAddress> &devices) = 0;
    virtual void GetConnectionState(
        const BluetoothRawAddress &device, int32_t &ret) = 0;
    virtual void SetConnectionStrategy(
        const BluetoothRawAddress &device, int32_t strategy, bool &ret) = 0;
    virtual void GetConnectionStrategy(
        const BluetoothRawAddress &device, int32_t &ret) = 0;
    virtual void GrantPermission(
        const BluetoothRawAddress &device, bool allow, bool save) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MSE_INTERFACE_H
