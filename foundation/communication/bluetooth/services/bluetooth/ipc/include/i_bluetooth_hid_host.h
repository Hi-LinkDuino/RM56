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
#ifndef OHOS_BLUETOOTH_IBLUETOOTHHIDHOST_H
#define OHOS_BLUETOOTH_IBLUETOOTHHIDHOST_H

#include <vector>
#include <string_ex.h>
#include <iremote_broker.h>
#include "i_bluetooth_hid_host_observer.h"

using OHOS::Bluetooth::IBluetoothHidHostObserver;

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;

class IBluetoothHidHost : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Bluetooth.IBluetoothHidHost");

    virtual ErrCode Connect(const BluetoothRawAddress &device, bool& result) = 0;
    virtual ErrCode Disconnect(const BluetoothRawAddress &device, bool& result) = 0;
    virtual ErrCode GetDeviceState(const BluetoothRawAddress &device, int& result) = 0;
    virtual ErrCode GetDevicesByStates(const std::vector<int32_t> &states,
        std::vector<BluetoothRawAddress>& result) = 0;
    virtual ErrCode RegisterObserver(const sptr<IBluetoothHidHostObserver> observer) = 0;
    virtual ErrCode DeregisterObserver(const sptr<IBluetoothHidHostObserver> observer) = 0;
    virtual ErrCode HidHostVCUnplug(std::string &device,
        uint8_t &id, uint16_t &size, uint8_t &type, int& result) = 0;
    virtual ErrCode HidHostSendData(std::string &device,
        uint8_t &id, uint16_t &size, uint8_t &type, int& result) = 0;
    virtual ErrCode HidHostSetReport(std::string &device,
        uint8_t &type, uint16_t &size, uint8_t &report, int& reuslt) = 0;
    virtual ErrCode HidHostGetReport(std::string &device,
        uint8_t &id, uint16_t &size, uint8_t &type, int& result) = 0;

protected:
    static constexpr int COMMAND_CONNECT = MIN_TRANSACTION_ID + 0;
    static constexpr int COMMAND_DISCONNECT = MIN_TRANSACTION_ID + 1;
    static constexpr int COMMAND_GET_DEVICE_STATE = MIN_TRANSACTION_ID + 2;
    static constexpr int COMMAND_GET_DEVICES_BY_STATES = MIN_TRANSACTION_ID + 3;
    static constexpr int COMMAND_REGISTER_OBSERVER = MIN_TRANSACTION_ID + 4;
    static constexpr int COMMAND_DEREGISTER_OBSERVER = MIN_TRANSACTION_ID + 5;
    static constexpr int COMMAND_VCUN_PLUG = MIN_TRANSACTION_ID + 6;
    static constexpr int COMMAND_SEND_DATA = MIN_TRANSACTION_ID + 7;
    static constexpr int COMMAND_SET_REPORT = MIN_TRANSACTION_ID + 8;
    static constexpr int COMMAND_GET_REPORT = MIN_TRANSACTION_ID + 9;
};
} // Bluetooth
} // OHOS

#endif // OHOS_BLUETOOTH_IBLUETOOTHHIDHOST_H

