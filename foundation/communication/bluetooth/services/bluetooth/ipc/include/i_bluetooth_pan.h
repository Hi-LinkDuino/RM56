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
#ifndef OHOS_BLUETOOTH_IBLUETOOTHPAN_H
#define OHOS_BLUETOOTH_IBLUETOOTHPAN_H

#include <vector>
#include <string_ex.h>
#include <iremote_broker.h>
#include "i_bluetooth_pan_observer.h"

using OHOS::Bluetooth::IBluetoothPanObserver;

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;

class IBluetoothPan : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Bluetooth.IBluetoothPan");

    virtual ErrCode Disconnect(const BluetoothRawAddress &device, bool& result) = 0;
    virtual ErrCode GetDeviceState(const BluetoothRawAddress &device, int& result) = 0;
    virtual ErrCode GetDevicesByStates(const std::vector<int32_t> &states,
        std::vector<BluetoothRawAddress>& result) = 0;
    virtual ErrCode RegisterObserver(const sptr<IBluetoothPanObserver> observer) = 0;
    virtual ErrCode DeregisterObserver(const sptr<IBluetoothPanObserver> observer) = 0;
    virtual ErrCode SetTethering(const bool value, bool& result) = 0;
    virtual ErrCode IsTetheringOn(bool& result) = 0;

protected:
    static constexpr int COMMAND_DISCONNECT = MIN_TRANSACTION_ID + 0;
    static constexpr int COMMAND_GET_DEVICE_STATE = MIN_TRANSACTION_ID + 1;
    static constexpr int COMMAND_GET_DEVICES_BY_STATES = MIN_TRANSACTION_ID + 2;
    static constexpr int COMMAND_REGISTER_OBSERVER = MIN_TRANSACTION_ID + 3;
    static constexpr int COMMAND_DEREGISTER_OBSERVER = MIN_TRANSACTION_ID + 4;
    static constexpr int COMMAND_SET_TETHERING = MIN_TRANSACTION_ID + 5;
    static constexpr int COMMAND_IS_TETHERING_ON = MIN_TRANSACTION_ID + 6;
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_IBLUETOOTHPAN_H