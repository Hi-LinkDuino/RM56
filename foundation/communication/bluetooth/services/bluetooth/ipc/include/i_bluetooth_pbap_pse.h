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
#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PSE_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PSE_INTERFACE_H

#include "../parcel/bluetooth_pbap_pce_parameter.h"
#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"
#include "pbap_pce_parameter.h"
#include "i_bluetooth_pbap_pse_observer.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothPbapPse : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothPbapPse");
    enum Code {
        PBAP_PSE_GET_DEVICE_STATE = 0,
        PBAP_PSE_GET_DEVICES_BY_STATES,
        PBAP_PSE_DISCONNECT,
        PBAP_PSE_SET_CONNECTION_STRATEGY,
        PBAP_PSE_GET_CONNECTION_STRATEGY,
        PBAP_PSE_GRANT_PERMISSION,
        PBAP_PSE_SET_DEVICE_PASSWORD,
        PBAP_PSE_REGISTER_OBSERVER,
        PBAP_PSE_DEREGISTER_OBSERVER,
    };

    virtual int GetDeviceState(const BluetoothRawAddress& device) = 0;
    virtual void GetDevicesByStates(
                const ::std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices) = 0;
    virtual int Disconnect(const BluetoothRawAddress& device) = 0;
    virtual int SetConnectionStrategy(const BluetoothRawAddress& device, int32_t strategy) = 0;
    virtual int GetConnectionStrategy(const BluetoothRawAddress& device) = 0;
    virtual void GrantPermission(const BluetoothRawAddress& device, bool allow, bool save) = 0;
    virtual int SetDevicePassword(
        const BluetoothRawAddress& device, const std::string &password, const std::string &userId) = 0;
    virtual void RegisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) = 0;
    virtual std::vector<BluetoothRawAddress> GetConnectedDevices() = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PSE_INTERFACE_H